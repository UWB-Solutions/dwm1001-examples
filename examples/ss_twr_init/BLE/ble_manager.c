/**
 * @file ble_manager.c
 * @brief BLE Manager cho DWM1001 - Nordic UART Service (NUS)
 *
 * Dùng nRF SDK 14.2 với SoftDevice S132 v5.0.
 * API: nrf_sdh (mới), không dùng softdevice_handler cũ.
 *
 * Luồng dữ liệu:
 *   ss_init_main.c tính distance
 *       → ble_manager_send("DIST:1.23\r\n")
 *           → ble_nus_data_send()
 *               → Raspberry Pi nhận BLE notification
 */

#include "ble_manager.h"

/* Nordic SDK */
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_freertos.h"
#include "nrf_ble_gatt.h"

/* BLE */
#include "ble.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_nus.h"

/* Libraries */
#include "app_timer.h"
#include "app_util_platform.h"

/* Log */
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/* -----------------------------------------------------------------------
 * Cấu hình BLE - chỉnh sửa các giá trị này nếu cần
 * ----------------------------------------------------------------------- */
#define DEVICE_NAME             "DWM1001_UWB"       /**< Tên BLE hiển thị khi scan */
#define NUS_SERVICE_UUID_TYPE   BLE_UUID_TYPE_VENDOR_BEGIN  /**< UUID type cho NUS */

#define APP_BLE_OBSERVER_PRIO   3                   /**< Priority của BLE observer */
#define APP_BLE_CONN_CFG_TAG    1                   /**< Tag cho SoftDevice BLE config */

/* Khoảng advertising */
#define APP_ADV_INTERVAL        64                  /**< Advertising interval (x 0.625ms = 40ms) */
#define APP_ADV_TIMEOUT_IN_SECONDS  0               /**< 0 = quảng bá mãi mãi */

/* Connection parameters */
#define MIN_CONN_INTERVAL       MSEC_TO_UNITS(20, UNIT_1_25_MS)   /**< 20ms */
#define MAX_CONN_INTERVAL       MSEC_TO_UNITS(75, UNIT_1_25_MS)   /**< 75ms */
#define SLAVE_LATENCY           0
#define CONN_SUP_TIMEOUT        MSEC_TO_UNITS(4000, UNIT_10_MS)   /**< 4 giây */

/* Connection parameters negotiation */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)
#define MAX_CONN_PARAMS_UPDATE_COUNT    3

/* -----------------------------------------------------------------------
 * Biến toàn cục nội bộ
 * ----------------------------------------------------------------------- */
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);  /**< NUS instance */
NRF_BLE_GATT_DEF(m_gatt);                           /**< GATT instance */
BLE_ADVERTISING_DEF(m_advertising);                 /**< Advertising instance */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;  /**< Handle kết nối hiện tại */
static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3; /**< MTU khả dụng */

/* -----------------------------------------------------------------------
 * Callback nội bộ
 * ----------------------------------------------------------------------- */

/**
 * @brief Xử lý dữ liệu nhận từ client qua NUS TX characteristic.
 *        (Raspberry Pi gửi lệnh xuống DWM1001)
 */
static void nus_data_handler(ble_nus_evt_t * p_evt)
{
    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {
        NRF_LOG_INFO("Received %d bytes from BLE client", p_evt->params.rx_data.length);
        /* TODO: xử lý lệnh từ Raspberry Pi nếu cần */
    }
}

/**
 * @brief BLE event handler - quản lý kết nối/ngắt kết nối.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("BLE: Client connected");
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("BLE: Client disconnected (reason: 0x%02X)",
                         p_ble_evt->evt.gap_evt.params.disconnected.reason);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            /* Tự động quảng bá lại để Raspberry Pi có thể kết nối lại */
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            ble_gap_phys_t const phys = {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            /* Không dùng pairing/bonding */
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                                   NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            break;
    }
}

/* Đăng ký BLE observer */
NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);

/**
 * @brief Callback khi có lỗi connection parameters.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * @brief Callback khi advertising kết thúc (timeout).
 *        Với timeout = 0, callback này không được gọi.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("BLE: Advertising started");
            break;
        case BLE_ADV_EVT_IDLE:
            /* Bắt đầu lại advertising nếu hết timeout */
            ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
            break;
        default:
            break;
    }
}

/**
 * @brief Callback khi GATT MTU thay đổi (Data Length Extension).
 */
static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) &&
        (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRF_LOG_INFO("BLE GATT MTU updated: data len = %d", m_ble_nus_max_data_len);
    }
}

/* -----------------------------------------------------------------------
 * Các hàm khởi tạo nội bộ
 * ----------------------------------------------------------------------- */

/**
 * @brief Khởi tạo SoftDevice và BLE stack.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    /* Bật SoftDevice */
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    /* Cấu hình BLE stack với RAM mặc định */
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    /* Bật BLE stack */
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Cấu hình GAP: tên thiết bị, appearance, connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    /* Security mode: Open link, no protection */
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    /* Đặt tên thiết bị */
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* Cấu hình connection parameters */
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Khởi tạo GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Khởi tạo Nordic UART Service.
 */
static void services_init(void)
{
    uint32_t           err_code;
    ble_nus_init_t     nus_init;

    memset(&nus_init, 0, sizeof(nus_init));
    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Khởi tạo Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));
    cp_init.p_conn_params                  = NULL;  /* Dùng PPCP đã set trong GAP */
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/* UUID của NUS service dùng trong scan response */
static ble_uuid_t m_adv_uuids[] =
{
    {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}
};

/**
 * @brief Cấu hình và bắt đầu advertising.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    /* Advertising data: flags + UUID của NUS */
    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    /* Scan response: UUID đầy đủ của NUS để client nhận diện */
    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;
    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/* -----------------------------------------------------------------------
 * Public API
 * ----------------------------------------------------------------------- */

void ble_manager_init(void)
{
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();

    /* Bắt đầu quảng bá BLE */
    ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("BLE Manager initialized. Advertising as \"%s\"", DEVICE_NAME);
}

void ble_manager_send(const uint8_t * p_data, uint16_t length)
{
    uint32_t err_code;

    /* Không gửi nếu chưa kết nối */
    if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return;
    }

    /* Chia nhỏ dữ liệu nếu vượt quá MTU */
    uint16_t remaining = length;
    const uint8_t * p_ptr = p_data;

    while (remaining > 0)
    {
        uint16_t chunk = remaining < m_ble_nus_max_data_len ? remaining : m_ble_nus_max_data_len;
        uint16_t send_len = chunk;  /* ble_nus_data_send cần con trỏ uint16_t */

        err_code = ble_nus_data_send(&m_nus, (uint8_t *)p_ptr, &send_len, m_conn_handle);

        if ((err_code != NRF_ERROR_INVALID_STATE) &&
            (err_code != NRF_ERROR_RESOURCES) &&
            (err_code != NRF_ERROR_NOT_FOUND))
        {
            APP_ERROR_CHECK(err_code);
        }

        if (err_code == NRF_SUCCESS)
        {
            p_ptr     += send_len;
            remaining -= send_len;
        }
        else
        {
            /* Buffer đầy, bỏ qua phần còn lại lần này */
            break;
        }
    }
}

bool ble_manager_is_connected(void)
{
    return (m_conn_handle != BLE_CONN_HANDLE_INVALID);
}
