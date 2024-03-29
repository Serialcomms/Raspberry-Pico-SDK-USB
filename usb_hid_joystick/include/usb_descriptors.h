/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//extern uint8_t pico_device_descriptor[];
//extern uint8_t pico_config_descriptor[];
//extern uint8_t pico_hid_report_descriptor[];

uint8_t ep0_packet_size();
uint16_t config_total_length();
uint16_t hid_report_descriptor_size();

uint8_t ep0_packet_size();
uint16_t usb_vendor_id();
uint16_t usb_product_id();
uint16_t config_total_length();
uint16_t hid_report_descriptor_size();

float usb_device_version();
float usb_device_release();

float bcd_convert(uint8_t major, uint8_t minor);

// bmRequestType bit definitions
#define USB_REQ_TYPE_STANDARD 0x00u
#define USB_REQ_TYPE_TYPE_MASK 0x60u
#define USB_REQ_TYPE_TYPE_CLASS 0x20u
#define USB_REQ_TYPE_TYPE_VENDOR 0x40u

#define USB_REQ_TYPE_RECIPIENT_MASK 0x1fu
#define USB_REQ_TYPE_RECIPIENT_DEVICE 0x00u
#define USB_REQ_TYPE_RECIPIENT_INTERFACE 0x01u
#define USB_REQ_TYPE_RECIPIENT_ENDPOINT 0x02u

#define USB_DIR_OUT 0x00u
#define USB_DIR_IN 0x80u

#define USB_TRANSFER_TYPE_CONTROL 0x0
#define USB_TRANSFER_TYPE_ISOCHRONOUS 0x1
#define USB_TRANSFER_TYPE_BULK 0x2
#define USB_TRANSFER_TYPE_INTERRUPT 0x3
#define USB_TRANSFER_TYPE_BITS 0x3

// Descriptor types
#define USB_DT_DEVICE 0x01
#define USB_DT_CONFIG 0x02
#define USB_DT_STRING 0x03
#define USB_DT_INTERFACE 0x04
#define USB_DT_ENDPOINT 0x05

#define USB_REQUEST_GET_STATUS 0x0
#define USB_REQUEST_CLEAR_FEATURE 0x01
#define USB_REQUEST_SET_FEATURE 0x03
#define USB_REQUEST_SET_ADDRESS 0x05
#define USB_REQUEST_GET_DESCRIPTOR 0x06
#define USB_REQUEST_SET_DESCRIPTOR 0x07
#define USB_REQUEST_GET_CONFIGURATION 0x08
#define USB_REQUEST_SET_CONFIGURATION 0x09
#define USB_REQUEST_GET_INTERFACE 0x0a
#define USB_REQUEST_SET_INTERFACE 0x0b
#define USB_REQUEST_SYNC_FRAME 0x0c

#define USB_REQUEST_MSC_GET_MAX_LUN 0xfe
#define USB_REQUEST_MSC_RESET 0xff

#define USB_FEAT_ENDPOINT_HALT 0x00
#define USB_FEAT_DEVICE_REMOTE_WAKEUP 0x01
#define USB_FEAT_TEST_MODE 0x02

#define USB_DESCRIPTOR_TYPE_ENDPOINT 0x05

