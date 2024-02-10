/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "include/usb_descriptors.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

extern uint8_t *DEBUG_TEXT;

uint8_t pico_device_descriptor[] = {

0x12,        // bLength // 18 bytes
0x01,        // bDescriptorType (Device)
0x10, 0x01,  // bcdUSB 1.1
0x00,        // bDeviceClass (Use class information in the Interface Descriptors)
0x00,        // bDeviceSubClass 
0x00,        // bDeviceProtocol 
0x40,        // bMaxPacketSize0 64
0x8A, 0x2E,  // idVendor 0x2E8A (Raspberry Pi)
0xAA, 0xAA,  // idProduct 0xAAAA
0x00, 0x01,  // bcdDevice 2.00
0x01,        // iManufacturer (String Index)
0x02,        // iProduct (String Index)
0x00,        // iSerialNumber (String Index)
0x01,        // bNumConfigurations 1

};

uint8_t pico_config_descriptor[] = {

0x09,        //   bLength
0x02,        //   bDescriptorType (Configuration)
0x22, 0x00,  //   wTotalLength 34
0x01,        //   bNumInterfaces 1
0x01,        //   bConfigurationValue
0x00,        //   iConfiguration (String Index)
0xA0,        //   bmAttributes Remote Wakeup
0x32,        //   bMaxPower 100mA

0x09,        //   bLength
0x04,        //   bDescriptorType (Interface)
0x00,        //   bInterfaceNumber 0
0x00,        //   bAlternateSetting
0x01,        //   bNumEndpoints 1
0x03,        //   bInterfaceClass
0x00,        //   bInterfaceSubClass
0x04,        //   bInterfaceProtocol
0x00,        //   iInterface (String Index)

0x09,        //   bLength
0x21,        //   bDescriptorType (HID)
0x11, 0x01,  //   bcdHID 1.11
0x00,        //   bCountryCode
0x01,        //   bNumDescriptors
0x22,        //   bDescriptorType[0] (HID)
0x59, 0x00,  //   wDescriptorLength[0] 79 // 48 was 0x30

0x07,        //   bLength
0x05,        //   bDescriptorType (Endpoint)
0x81,        //   bEndpointAddress (IN/D2H)
0x03,        //   bmAttributes (Interrupt)
0x08, 0x00,  //   wMaxPacketSize 8     
0x0A,        //   bInterval 10 (unit depends on device speed)

// 34 bytes

} ;


uint8_t pico_hid_report_descriptor[] = {

0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x04,        // Usage (Joystick)
0xA1, 0x01,        // Collection (Application)

0x05, 0x02,        //   Usage Page (Sim Ctrls)
0x09, 0xBA,        //   Usage (Rudder)
0x09, 0xBB,        //   Usage (Throttle)
0x15, 0x81,        //   Logical Minimum (-127)
0x25, 0x7F,        //   Logical Maximum (127)
0x75, 0x08,        //   Report Size (8)
0x95, 0x02,        //   Report Count (2)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
0x09, 0x01,        //   Usage (Pointer)
0xA1, 0x00,        //   Collection (Physical)
0x09, 0x30,        //     Usage (X)
0x09, 0x31,        //     Usage (Y)
0x15, 0x81,        //     Logical Minimum (-127)
0x25, 0x7F,        //     Logical Maximum (127)
0x75, 0x08,        //     Report Size (8)
0x95, 0x02,        //     Report Count (2)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection

0x09, 0x39,        //   Usage (Hat switch)
0x15, 0x00,        //   Logical Minimum (0)
0x25, 0x07,        //   Logical Maximum (7)
0x35, 0x00,        //   Physical Minimum (0)
0x46, 0x3B, 0x01,  //   Physical Maximum (315)
0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
0x75, 0x04,        //   Report Size (4)
0x95, 0x01,        //   Report Count (1)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

0x75, 0x01,        //   Report Size (1)
0x95, 0x04,        //   Report Count (4)
0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

0x05, 0x09,        //   Usage Page (Button)
0x19, 0x01,        //   Usage Minimum (0x01)
0x29, 0x08,        //   Usage Maximum (0x08)
0x15, 0x00,        //   Logical Minimum (0)
0x25, 0x01,        //   Logical Maximum (1)
0x75, 0x01,        //   Report Size (1)
0x95, 0x08,        //   Report Count (8)
0x55, 0x00,        //   Unit Exponent (0)
0x65, 0x00,        //   Unit (None)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection

// 89 bytes = 0x59

};

uint16_t hid_report_descriptor_size() {

    return sizeof(pico_hid_report_descriptor);

}

uint16_t config_total_length() {

   return pico_config_descriptor[3] << 8 | pico_config_descriptor[2] ;

}

uint8_t ep0_packet_size() {

    if (pico_device_descriptor[7]==16 | 
        pico_device_descriptor[7]==32 | 
        pico_device_descriptor[7]==64 ) 

    {
        return pico_device_descriptor[7];
    
    } else {

        return 8;
    }
}

uint16_t usb_product_id() {

  return pico_device_descriptor[9] << 8 | pico_device_descriptor[8];

}

uint16_t usb_vendor_id() {

  return pico_device_descriptor[11] << 8 | pico_device_descriptor[10];

}

float usb_device_version() {

  return bcd_convert(pico_device_descriptor[3] , pico_device_descriptor[2]);

}

float usb_device_release() {

  return bcd_convert(pico_device_descriptor[13] , pico_device_descriptor[12]);

}

float bcd_convert(uint8_t major, uint8_t minor) {

  const float TEN = 10;
 
  uint8_t major_tens = MIN(major >> 4, 9);
  uint8_t major_units = MIN(major & 0xF, 9);

  uint8_t minor_tens = MIN(minor >> 4, 9);
  uint8_t minor_units = MIN(minor & 0xF, 9);

  float integer_part = major_tens * TEN + major_units;

  float fractional_part = minor_tens * TEN + minor_units;

  return integer_part + fractional_part / (TEN * TEN);

}
