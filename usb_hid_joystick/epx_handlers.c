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

extern uint8_t *DEBUG_TEXT;

void  __not_in_flash_func (ep1_handler_to_host)(uint8_t EP_NUMBER) {
 
    if (host_endpoint[1].async_bytes_pending) {

        send_async_packet(1);

    } else {

        wait_for_transaction_completion(true);
    }

    usb_hardware_clear->buf_status = USB_BUFF_STATUS_EP1_IN_BITS;

    DEBUG_TEXT = "Buffer Status Handler \tCompletion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", EP_NUMBER);

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status, Bit Mask=%08x";
    DEBUG_SHOW ("IRQ", USB_BUFF_STATUS_EP1_IN_BITS);

}