	.cpu cortex-m4
	.arch armv7e-m
	.fpu fpv4-sp-d16
	.eabi_attribute 27, 1
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 38, 1
	.eabi_attribute 18, 4
	.file	"ble_nus.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.file 1 "D:\\252_DATN\\Cuong-LeDuc\\dwm1001-examples\\nRF5_SDK_14.2.0\\components\\ble\\ble_services\\ble_nus\\ble_nus.c"
.Letext0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x91
	.2byte	0x4
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF16
	.byte	0xc
	.4byte	.LASF17
	.4byte	.LASF18
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.4byte	.LASF0
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.4byte	.LASF1
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.4byte	.LASF2
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.4byte	.LASF3
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.4byte	.LASF4
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.4byte	.LASF5
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.4byte	.LASF6
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.4byte	.LASF8
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.4byte	.LASF9
	.uleb128 0x3
	.byte	0x8
	.byte	0x4
	.4byte	.LASF10
	.uleb128 0x3
	.byte	0x4
	.byte	0x4
	.4byte	.LASF11
	.uleb128 0x3
	.byte	0x8
	.byte	0x3
	.4byte	.LASF12
	.uleb128 0x3
	.byte	0x10
	.byte	0x3
	.4byte	.LASF13
	.uleb128 0x3
	.byte	0x8
	.byte	0x4
	.4byte	.LASF14
	.uleb128 0x3
	.byte	0x10
	.byte	0x3
	.4byte	.LASF15
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x2134
	.uleb128 0x19
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_pubnames,"",%progbits
	.4byte	0xe
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x95
	.4byte	0
	.section	.debug_pubtypes,"",%progbits
	.4byte	0x129
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x95
	.4byte	0x1d
	.ascii	"int\000"
	.4byte	0x24
	.ascii	"char\000"
	.4byte	0x2b
	.ascii	"unsigned int\000"
	.4byte	0x32
	.ascii	"signed char\000"
	.4byte	0x39
	.ascii	"unsigned char\000"
	.4byte	0x40
	.ascii	"short int\000"
	.4byte	0x47
	.ascii	"short unsigned int\000"
	.4byte	0x4e
	.ascii	"long int\000"
	.4byte	0x55
	.ascii	"long unsigned int\000"
	.4byte	0x5c
	.ascii	"long long int\000"
	.4byte	0x63
	.ascii	"long long unsigned int\000"
	.4byte	0x6a
	.ascii	"double\000"
	.4byte	0x71
	.ascii	"float\000"
	.4byte	0x78
	.ascii	"complex float\000"
	.4byte	0x7f
	.ascii	"complex double\000"
	.4byte	0x86
	.ascii	"long double\000"
	.4byte	0x8d
	.ascii	"complex long double\000"
	.4byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0x14
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	0
	.4byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF8:
	.ascii	"long long int\000"
.LASF1:
	.ascii	"unsigned int\000"
.LASF15:
	.ascii	"complex long double\000"
.LASF13:
	.ascii	"complex double\000"
.LASF7:
	.ascii	"long unsigned int\000"
.LASF9:
	.ascii	"long long unsigned int\000"
.LASF12:
	.ascii	"complex float\000"
.LASF17:
	.ascii	"D:\\252_DATN\\Cuong-LeDuc\\dwm1001-examples\\nRF5_S"
	.ascii	"DK_14.2.0\\components\\ble\\ble_services\\ble_nus\\"
	.ascii	"ble_nus.c\000"
.LASF18:
	.ascii	"D:\\252_DATN\\Cuong-LeDuc\\dwm1001-examples\\exampl"
	.ascii	"es\\ss_twr_init\\SES\000"
.LASF3:
	.ascii	"unsigned char\000"
.LASF0:
	.ascii	"char\000"
.LASF6:
	.ascii	"long int\000"
.LASF10:
	.ascii	"double\000"
.LASF5:
	.ascii	"short unsigned int\000"
.LASF2:
	.ascii	"signed char\000"
.LASF14:
	.ascii	"long double\000"
.LASF11:
	.ascii	"float\000"
.LASF4:
	.ascii	"short int\000"
.LASF16:
	.ascii	"GNU C99 14.2.1 20241119 -fmessage-length=0 -std=gnu"
	.ascii	"99 -mcpu=cortex-m4 -mlittle-endian -mfloat-abi=hard"
	.ascii	" -mabi=aapcs -mfpu=fpv4-sp-d16 -mthumb -munaligned-"
	.ascii	"access -mtp=soft -mfp16-format=ieee -gdwarf-4 -g2 -"
	.ascii	"gpubnames -fomit-frame-pointer -ffunction-sections "
	.ascii	"-fdata-sections -fshort-enums -fno-common\000"
	.ident	"GCC: (based on arm-14.2.Rel1 source release) 14.2.1 20241119"
