/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stdio.h"
#include "pico/stdlib.h"
#include "include/usb_main.h"
#include "include/usb_debug.h"
#include "include/usb_dev_init.h"
#include "include/usb_joystick.h"
#include "include/usb_endpoints.h"
#include "include/setup_endpoints.h"
#include "include/status_screen.h"
#include "include/usb_protocol.h"
#include "include/usb_common.h"
#include "include/usb_transmit.h"
#include "include/pico_device.h"
#include "include/pico_info.h"
#include "include/usb_sie_errors.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

extern uint8_t *DEBUG_TEXT;

extern struct pico_profile pico;

static inline uint16_t build_data_packet(uint8_t *source_buffer, uint8_t *destination_buffer);

int main(void) {

    init_debug_critical_section();

    gpio_init(PICO_DEFAULT_LED_PIN);

    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    gpio_init(16);

    gpio_set_dir(16, GPIO_OUT);
   
    stdio_uart_init();

    busy_wait_ms(100);

    clear_status_screen();

    start_status_screen();

    usb_setup_default_endpoint_0();

    usb_device_init();

    busy_wait_ms(1000);
    
    gpio_put(16, 1); 

    wait_for_device_enumeration();

    show_free_total_heap();

    if (usb_device_enumerated()) {

        device_enumeration_pass();

    } else {

        device_enumeration_fail();        
    }

}

void wait_for_device_enumeration() {

    volatile bool sie_errors;
    volatile bool wait_timeout;
    volatile bool device_enumerated;
   
    uint8_t wait_seconds = 20;
    uint64_t wait_duration = 0;

    absolute_time_t wait_time_now;
    absolute_time_t wait_time_end = make_timeout_time_ms(wait_seconds * 1000);
    absolute_time_t wait_enumeration = make_timeout_time_ms(15000);

    pico.usb.device.last_transaction.wait = 2000000;

    device_enumerated = false;

    DEBUG_TEXT = "Device Insertion\tInserting USB device now ...";
    DEBUG_SHOW ("DEV", 0);

    usb_insert_device();

    busy_wait_ms(1500);

    DEBUG_TEXT = "Device Enumeration\tAllowing %d seconds max. for USB Device Enumeration";
    DEBUG_SHOW ("DEV", 0, wait_seconds);

    busy_wait_ms(2500);

    wait_time_now = get_absolute_time(); 

    do { 

        busy_wait_ms(333);

        sie_errors = check_sie_errors();

        wait_timeout = time_reached(wait_time_end);

        device_enumerated = usb_device_enumerated();

    } while (!sie_errors && !device_enumerated && !wait_timeout);
    
    if (device_enumerated) {
        
        const float ONE_THOUSAND = 1000;

        uint64_t wait_microseconds = absolute_time_diff_us(wait_time_now, get_absolute_time());

        uint32_t wait_milliseconds = MIN(us_to_ms(wait_microseconds), 3600 * 1000);  
                                                                       
        float wait_seconds = (float) wait_milliseconds / ONE_THOUSAND;

        DEBUG_TEXT = "Device Enumerated\t%3.3f seconds for USB Device Enumeration";
        DEBUG_SHOW ("DEV", 0, wait_seconds);

        busy_wait_ms(1000);

      }

}

static inline void device_enumeration_pass() {

        gpio_put(PICO_DEFAULT_LED_PIN, 1); 

        DEBUG_TEXT = "====================\t===============================================";
        DEBUG_SHOW ("===", 1);       

        DEBUG_TEXT = "Pico SDK/USB \t\tJoystick Simulation Starting";
        DEBUG_SHOW ("SDK", 1);

        DEBUG_TEXT = "Simulation Starting\tUSB Joystick with controls and buttons";
        DEBUG_SHOW ("JOY", 1);

        DEBUG_TEXT = "====================\t===============================================";
        DEBUG_SHOW ("===", 1);   

        busy_wait_ms(2500);

    while (1) {  

        busy_wait_ms(333);

        send_joystick_movement(false);

    }

}

static inline void device_enumeration_fail() {

    DEBUG_TEXT = "USB Device Failure\tError Enumerating USB Device";
    DEBUG_SHOW ("ERR", 9);

    busy_wait_ms(5000);

    usb_remove_device();

    show_device_enumerated();

    while (1) {  

        busy_wait_ms(50);

        gpio_put(PICO_DEFAULT_LED_PIN, 1);

        busy_wait_ms(50);

        gpio_put(PICO_DEFAULT_LED_PIN, 0);

    }

}

static inline uint16_t build_data_packet(uint8_t *source_buffer, uint8_t *destination_buffer) {

    uint8_t offset = 0;
   
    do {  
     
       destination_buffer[offset] = source_buffer[offset];

    } while (++offset < 64 && source_buffer[offset]);

    return offset;

}