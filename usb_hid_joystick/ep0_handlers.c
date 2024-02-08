/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico.h"
#include <stdio.h>
#include "pico/time.h"
#include "pico/sync.h" 
#include "pico/stdlib.h"
#include "include/time_stamp.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_receive.h"
#include "include/usb_transmit.h"
#include "include/usb_endpoints.h"
#include "include/show_registers.h"
#include "include/ep0_handlers.h"

extern uint8_t *DEBUG_TEXT;

uint8_t EP0_BUFFER[64];

static uint8_t *mini_buffer = EP0_BUFFER;

static struct critical_section ep0_handler_critical_section;

static inline void __force_inline(ep0_handler_to_host_async()) {

    DEBUG_TEXT = "Buffer Status Handler \tStarting ASYNC Completion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", 1);

    uint32_t transaction_duration = 0;

    host_endpoint[0].bytes_transferred += get_buffer_bytes_to_host(0);

    DEBUG_TEXT = "Buffer Status Handler \tCleared EP0_IN Buffer Status ?";
    DEBUG_SHOW ("IRQ", 1);

    if (host_endpoint[0].async_bytes_pending) {

        send_async_packet(0);

    } else {

        ack_pico_to_host_control_transfer();

        transaction_duration = absolute_time_diff_us(host_endpoint[0].start_time_now, get_absolute_time());

        host_endpoint[0].transaction_duration = transaction_duration;

        host_endpoint[0].transaction_complete = true;

    }
}

void ep0_handler_to_host (uint8_t EP_NUMBER) {

    critical_section_enter_blocking(&ep0_handler_critical_section);

    uint32_t transaction_duration = 0;

    const bool USE_ASYNC_HANDLER = false;  // does not call async properly when = true;

    DEBUG_TEXT = "Buffer Status Handler \tStarting Completion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", 1, EP_NUMBER);

    if (host_endpoint[0].async_mode) {

        DEBUG_TEXT = "Buffer Status Handler \tStarting ASYNC Completion Handler for Endpoint %d";
        DEBUG_SHOW ("IRQ", 1, EP_NUMBER);

        if (USE_ASYNC_HANDLER) {

            clear_buffer_status(USB_BUFF_STATUS_EP0_IN_BITS); 

            ep0_handler_to_host_async;

        } else {

            DEBUG_TEXT = "Buffer Status Handler \tASYNC Bytes Transferred=%d, Bytes Remaining=%d";
            DEBUG_SHOW ("IRQ", 1, get_buffer_bytes_to_host(0), host_endpoint[0].async_bytes_pending );
            
            host_endpoint[0].bytes_transferred += get_buffer_bytes_to_host(0);

            clear_buffer_status(USB_BUFF_STATUS_EP0_IN_BITS); 

            if (host_endpoint[0].async_bytes_pending) {

            send_async_packet(0);

    } else {

        DEBUG_TEXT = "Buffer Status Handler \tTransfer Complete, Async Bytes Transfered=%d";
        DEBUG_SHOW ("IRQ", 1, host_endpoint[0].bytes_transferred);

        ack_pico_to_host_control_transfer();

        transaction_duration = absolute_time_diff_us(host_endpoint[0].start_time_now, get_absolute_time());

        host_endpoint[0].transaction_duration = transaction_duration;

        host_endpoint[0].transaction_complete = true;

        }

    }        

    } else {

        DEBUG_TEXT = "Buffer Status Handler \tPico > Host, Async Mode=False, Clearing IRQ Only";
        DEBUG_SHOW ("IRQ", 1);

        clear_buffer_status(USB_BUFF_STATUS_EP0_IN_BITS); 

    }

    critical_section_exit(&ep0_handler_critical_section);

}

void ep0_handler_to_pico (uint8_t EP_NUMBER) {

    DEBUG_TEXT = "Buffer Status Handler \tEP0 Handler to Pico";
    DEBUG_SHOW ("IRQ", 1);

    copy_ep0_data_packet(mini_buffer);

    TIMESTAMP(); printf("EP0:\tUSB Mini Buffer, EP%d \tBytes 01-08 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", 0, 
    mini_buffer[0x00], mini_buffer[0x01], mini_buffer[0x02], mini_buffer[0x03], 
    mini_buffer[0x04], mini_buffer[0x05], mini_buffer[0x06], mini_buffer[0x07]);

    send_ack_handshake_to_host(0, true);

    clear_buffer_status(USB_BUFF_STATUS_EP0_OUT_BITS); 


}

void init_ep0_handler_critical_section() {

    critical_section_init(&ep0_handler_critical_section);

}