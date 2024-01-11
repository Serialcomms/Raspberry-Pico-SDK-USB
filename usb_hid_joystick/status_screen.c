/**
 * Copyright (c) 2023 SerialComms (github) 
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#include <stdio.h>
#include "pico/stdlib.h"
#include "include/pico_info.h"
#include "include/usb_debug.h"
#include "include/setup_strings.h"
#include "include/status_screen.h"

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

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

DEBUG_TEXT = "Pico SDK/USB \t\tJoystick Simulation Starting";
DEBUG_SHOW ("SDK", DEBUG_TEXT);

generate_serial_number_string(false);

show_serial_number_string();

show_pico_clocks();

show_free_total_heap();

busy_wait_ms(5000);

}

