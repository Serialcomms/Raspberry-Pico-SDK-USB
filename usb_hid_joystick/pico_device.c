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

struct pico_usb_profile pico;

static uint8_t show_device_text[40];

void reset_usb_device() {

    pico.usb.device.address_set = false;
    pico.usb.descriptors.device = false;
    pico.usb.descriptors.config = false;
    pico.usb.functions.joystick.set_idle = false;
    pico.usb.functions.joystick.hid_report = false;

    pico.usb.device.last_transaction.done = false;
    pico.usb.device.last_transaction.wait = 2000000;
    pico.usb.device.last_transaction.time = at_the_end_of_time;

}

bool usb_device_enumerated() {

    pico.usb.device.last_transaction.done = last_transaction_done() ;

    bool enumerated = 

    pico.usb.device.address_set &
    pico.usb.descriptors.device &
    pico.usb.descriptors.config &
    pico.usb.functions.joystick.hid_report &
    pico.usb.functions.joystick.set_idle &
    pico.usb.device.last_transaction.done ;

    return enumerated;

}

bool last_transaction_done() {

    absolute_time_t earliest_time = absolute_time_min(pico.usb.device.last_transaction.time, get_absolute_time());

    uint64_t last_transaction_us = absolute_time_diff_us(earliest_time, get_absolute_time());
 
    return (last_transaction_us >pico.usb.device.last_transaction.wait) ? true : false ;
  
}

void show_device_enumerated() {

    DEBUG_TEXT = "Device Enumeration\tDEVICE ADDRESS SET \t = %s";
    DEBUG_SHOW ("DEV", pico.usb.device.address_set ? "Y" : "N"); 

    DEBUG_TEXT = "Device Enumeration\tDEVICE DESCRIPTOR SENT \t = %s";
    DEBUG_SHOW ("DEV", pico.usb.descriptors.device ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tDEVICE CONFIGURATION SENT \t = %s";
    DEBUG_SHOW ("DEV", pico.usb.descriptors.config ? "Y" : "N"); 

    DEBUG_TEXT = "Device Enumeration\tHID REPORT DESCRIPTOR SENT \t = %s";
    DEBUG_SHOW ("DEV", pico.usb.functions.joystick.hid_report ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tHID SET IDLE RECEIVED \t = %s";
    DEBUG_SHOW ("DEV", pico.usb.functions.joystick.set_idle ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tENUMERATION COMPLETE \t = %s";
    DEBUG_SHOW ("DEV", usb_device_enumerated() ? "TRUE" : "FALSE");

}

void set_binary_declarations() {     // for use with Picotool

    bi_decl(bi_program_description("Pico USB Joystick"));
    bi_decl(bi_program_feature("PicoProbe / UART Debug"));
    bi_decl(bi_program_version_string("SDK Demo Version"));
}