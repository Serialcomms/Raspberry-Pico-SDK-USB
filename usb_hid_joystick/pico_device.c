
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "include/usb_debug.h"
#include "include/pico_device.h"

extern uint8_t *DEBUG_TEXT;

struct pico_device_profile pico_usb_device;

static uint8_t show_device_text[40];

void reset_usb_device() {

    pico_usb_device.ADDRESS_SET = false;
    pico_usb_device.DEVICE_DESCRIPTOR_SENT = false;
    pico_usb_device.DEVICE_CONFIGURATION_SENT = false;
    pico_usb_device.HID_REPORT_DESCRIPTOR_SENT = false;
    pico_usb_device.HID_SET_IDLE_RECEIVED = false;

}

bool usb_device_enumerated() {

    bool enumerated = 
    pico_usb_device.ADDRESS_SET &
    pico_usb_device.DEVICE_DESCRIPTOR_SENT & 
    pico_usb_device.DEVICE_CONFIGURATION_SENT &
    pico_usb_device.HID_REPORT_DESCRIPTOR_SENT &
    pico_usb_device.HID_SET_IDLE_RECEIVED;

    return enumerated;

}

void show_device_enumerated() {

    DEBUG_TEXT = "Device Enumeration\tDEVICE ADDRESS SET = %s";
    DEBUG_SHOW ("DEV", pico_usb_device.ADDRESS_SET ? "Y" : "N"); 

    DEBUG_TEXT = "Device Enumeration\tDEVICE DESCRIPTOR SENT= %s";
    DEBUG_SHOW ("DEV", pico_usb_device.DEVICE_DESCRIPTOR_SENT ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tDEVICE CONFIGURATION SENT = %s";
    DEBUG_SHOW ("DEV", pico_usb_device.DEVICE_CONFIGURATION_SENT ? "Y" : "N"); 

    DEBUG_TEXT = "Device Enumeration\tHID REPORT DESCRIPTOR SENT = %s";
    DEBUG_SHOW ("DEV", pico_usb_device.HID_REPORT_DESCRIPTOR_SENT ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tHID SET IDLE RECEIVED = %s";
    DEBUG_SHOW ("DEV", pico_usb_device.HID_SET_IDLE_RECEIVED ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tENUMERATION COMPLETE = %s";
    DEBUG_SHOW ("DEV", usb_device_enumerated() ? "TRUE" : "FALSE");

}

//set_binary_declarations();

void set_binary_declarations() {     // for use with Picotool

    bi_decl(bi_program_description("Pico USB Joystick"));
    bi_decl(bi_program_feature("PicoProbe / UART Debug"));
    bi_decl(bi_program_version_string("SDK Demo Version"));
}