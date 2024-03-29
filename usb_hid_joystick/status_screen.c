/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#include <stdio.h>
#include "pico/stdlib.h"
#include "include/pico_info.h"
#include "include/usb_debug.h"
#include "include/usb_descriptors.h"
#include "include/setup_strings.h"
#include "include/status_screen.h"

extern bool DEBUG_HIDE;
extern uint8_t *DEBUG_TEXT;

void clear_status_screen() {

    printf("\033[H");        // cursor home
    printf("\033[2J");       // clear screen
    printf("\033[3J");       // clear scrollback
    printf("\033[1m");       // bold text
    printf("\033[36m");      // cyan text
    printf("\033[40m");      // black background
    printf("\033[?25l");     // hide cursor (reduce flicker)

}

void start_status_screen() {

clear_status_screen();

DEBUG_TEXT = "Pico USB Initialising\tUSB Device - HID Joystick Function";
DEBUG_SHOW ("SDK", 1);

DEBUG_TEXT = "Pico USB Initialising\tDebug Hide=%s, Show Threshold=%d";
DEBUG_SHOW ("DBG", 99, boolean_text(DEBUG_HIDE), get_debug_threshold());

DEBUG_TEXT = "Pico USB Initialising\tUSB Specification Version = %2.2f";
DEBUG_SHOW ("USB", 1, usb_device_version());

DEBUG_TEXT = "Pico USB Initialising\tUSB Device Release Number = %2.2f";
DEBUG_SHOW ("USB", 1, usb_device_release());

DEBUG_TEXT = "Pico USB Initialising\tUSB Device Vendor ID=%04X, Product ID=%04X";
DEBUG_SHOW ("USB", 1, usb_vendor_id(), usb_product_id());

generate_serial_number_string(false);

show_serial_number_string();

show_pico_clocks();

show_free_total_heap();

}

