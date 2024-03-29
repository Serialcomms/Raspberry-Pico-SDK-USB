/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_receive.h"
#include "include/usb_transmit.h"
#include "include/usb_endpoints.h"
#include "include/show_registers.h"
#include "include/epx_handlers.h"

extern bool DEBUG_HIDE;

extern uint8_t *DEBUG_TEXT;

void ep1_handler_to_host (uint8_t EP_NUMBER) {

    DEBUG_HIDE = false;
 
    if (host_endpoint[1].async_bytes_pending) {

        send_async_packet(1);

    } else {

        usb_wait_for_buffer_completion_pico_to_host(1, true);
    }

    usb_hardware_clear->buf_status = USB_BUFF_STATUS_EP1_IN_BITS;

    DEBUG_TEXT = "====================\t===============================================";
    DEBUG_SHOW ("===", 1);  

    DEBUG_TEXT = "Buffer Status Handler \tCompletion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", 0, EP_NUMBER);

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status, Bit Mask=%08x";
    DEBUG_SHOW ("IRQ", 0, USB_BUFF_STATUS_EP1_IN_BITS);

}

void ep2_handler_to_host (uint8_t EP_NUMBER) {
 
    if (host_endpoint[1].async_bytes_pending) {

        send_async_packet(1);

    } else {

        wait_for_transaction_completion(true);
    }

    usb_hardware_clear->buf_status = USB_BUFF_STATUS_EP2_IN_BITS;

    DEBUG_TEXT = "Buffer Status Handler \tCompletion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", 0, EP_NUMBER);

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status, Bit Mask=%08x";
    DEBUG_SHOW ("IRQ", 0, USB_BUFF_STATUS_EP2_IN_BITS);

}


void ep2_handler_to_pico (uint8_t EP_NUMBER) {
 
    usb_hardware_clear->buf_status = USB_BUFF_STATUS_EP2_OUT_BITS;

    DEBUG_TEXT = "Buffer Status Handler \tCompletion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", 0, EP_NUMBER);

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status, Bit Mask=%08x";
    DEBUG_SHOW ("IRQ", 0, USB_BUFF_STATUS_EP1_OUT_BITS);

}
