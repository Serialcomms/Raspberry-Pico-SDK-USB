/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "include/usb_debug.h"
#include "include/pico_device.h"

extern uint8_t *DEBUG_TEXT;

struct pico_device_profile pico_usb_joystick;

static uint8_t show_device_text[40];

void reset_usb_device() {

    pico_usb_joystick.SET_ADDRESS = false;
    pico_usb_joystick.DESCRIPTOR.DEVICE = false;
    pico_usb_joystick.DESCRIPTOR.CONFIG = false;
    pico_usb_joystick.DESCRIPTOR.HID_REPORT = false;
    pico_usb_joystick.RECEIVED_HID_SET_IDLE = false;
    pico_usb_joystick.LAST_TRANSACTION_DONE = false;
    pico_usb_joystick.LAST_TRANSACTION_WAIT = 2000000;
    pico_usb_joystick.LAST_TRANSACTION_TIME = at_the_end_of_time;

}

bool usb_device_enumerated() {

    pico_usb_joystick.LAST_TRANSACTION_DONE = last_transaction_done() ;

    bool enumerated = 
    pico_usb_joystick.SET_ADDRESS &
    pico_usb_joystick.DESCRIPTOR.DEVICE  & 
    pico_usb_joystick.DESCRIPTOR.CONFIG  &
    pico_usb_joystick.DESCRIPTOR.HID_REPORT &
    pico_usb_joystick.RECEIVED_HID_SET_IDLE &
    pico_usb_joystick.LAST_TRANSACTION_DONE ;

    return enumerated;

}

bool last_transaction_done() {

    absolute_time_t earliest_time = absolute_time_min(pico_usb_joystick.LAST_TRANSACTION_TIME, get_absolute_time());

    uint64_t last_transaction_us = absolute_time_diff_us(earliest_time, get_absolute_time());
 
    return (last_transaction_us > pico_usb_joystick.LAST_TRANSACTION_WAIT) ? true : false ;
  
}

void show_device_enumerated() {

    DEBUG_TEXT = "Device Enumeration\tDEVICE ADDRESS SET \t = %s";
    DEBUG_SHOW ("DEV", pico_usb_joystick.SET_ADDRESS ? "Y" : "N"); 

    DEBUG_TEXT = "Device Enumeration\tDEVICE DESCRIPTOR SENT \t = %s";
    DEBUG_SHOW ("DEV", pico_usb_joystick.DESCRIPTOR.DEVICE ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tDEVICE CONFIGURATION SENT \t = %s";
    DEBUG_SHOW ("DEV", pico_usb_joystick.DESCRIPTOR.CONFIG ? "Y" : "N"); 

    DEBUG_TEXT = "Device Enumeration\tHID REPORT DESCRIPTOR SENT \t = %s";
    DEBUG_SHOW ("DEV", pico_usb_joystick.DESCRIPTOR.HID_REPORT ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tHID SET IDLE RECEIVED \t = %s";
    DEBUG_SHOW ("DEV", pico_usb_joystick.DESCRIPTOR.HID_REPORT ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tENUMERATION COMPLETE \t = %s";
    DEBUG_SHOW ("DEV", usb_device_enumerated() ? "TRUE" : "FALSE");

}

void set_binary_declarations() {     // for use with Picotool

    bi_decl(bi_program_description("Pico USB Joystick"));
    bi_decl(bi_program_feature("PicoProbe / UART Debug"));
    bi_decl(bi_program_version_string("SDK Demo Version"));
}