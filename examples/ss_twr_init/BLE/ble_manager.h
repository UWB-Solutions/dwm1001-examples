/**
 * @file ble_manager.h
 * @brief BLE Manager cho DWM1001 - Nordic UART Service (NUS)
 *
 * Module này bọc toàn bộ BLE stack (SoftDevice S132 + GATT + NUS)
 * và cung cấp API đơn giản để gửi kết quả đo UWB qua BLE.
 *
 * Raspberry Pi (hoặc bất kỳ thiết bị BLE nào) kết nối vào và nhận
 * notifications qua NUS RX characteristic.
 */

#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Khởi tạo toàn bộ BLE stack và Nordic UART Service.
 *
 * Gọi hàm này một lần trong main() trước ranging loop.
 * Sau khi gọi, DWM1001 sẽ bắt đầu quảng bá BLE với tên "DWM1001_UWB".
 */
void ble_manager_init(void);

/**
 * @brief Gửi dữ liệu qua BLE NUS (giống UART nhưng không dây).
 *
 * Nếu chưa có client kết nối, hàm này không làm gì (không lỗi).
 *
 * @param[in] p_data  Con trỏ tới dữ liệu cần gửi.
 * @param[in] length  Số byte cần gửi (tối đa 20 byte mỗi lần với BLE 4.2,
 *                    hoặc tới 244 byte nếu dùng Data Length Extension).
 */
void ble_manager_send(const uint8_t * p_data, uint16_t length);

/**
 * @brief Kiểm tra có client BLE đang kết nối không.
 *
 * @return true  Có client đang kết nối và có thể gửi dữ liệu.
 * @return false Chưa có client kết nối.
 */
bool ble_manager_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* BLE_MANAGER_H */
