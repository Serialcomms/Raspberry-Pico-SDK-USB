/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/divider.h"               
#include "include/usb_debug.h"
#include "include/usb_common.h"  
#include "include/usb_receive.h"
#include "include/usb_endpoints.h"
#include "include/usb_sie_errors.h"
#include "include/show_registers.h"
#include "include/pico_device.h"

extern uint8_t *DEBUG_TEXT;

extern struct pico_profile pico;

uint16_t usb_start_transfer_host_to_pico(uint8_t EP_NUMBER, uint8_t *mini_buffer, uint16_t buffer_length) {

    uint32_t buffer_dispatch;
    uint16_t buffer_offset = 0;
    uint16_t bytes_received = 0;
    uint8_t *usb_buffer = pico_endpoint[EP_NUMBER].dpram_address;
    uint32_t EXPECT_PACKET_ID = pico_endpoint[EP_NUMBER].packet_id;
    
    DEBUG_TEXT = "USB Data Transfer \tHost to Pico, Expected PID=%d, Expected Bytes=%d";
    DEBUG_SHOW ("USB", 1, EXPECT_PACKET_ID/8192, buffer_length);
 
    buffer_dispatch = EXPECT_PACKET_ID | USB_BUF_CTRL_AVAIL;

    usb_dpram->ep_buf_ctrl[EP_NUMBER].out = buffer_dispatch;

    usb_wait_for_buffer_completion_host_to_pico(EP_NUMBER, true);

    bytes_received = get_buffer_bytes_to_pico(EP_NUMBER);

    if (bytes_received) {

        do {

            mini_buffer[buffer_offset] = usb_buffer[buffer_offset];

        } while (++buffer_offset < 64);


         pico_endpoint[EP_NUMBER].packet_id = toggle_data_pid(EP_NUMBER, false, EXPECT_PACKET_ID);
    }

    uint32_t RECEIVE_PACKET_ID = usb_dpram->ep_buf_ctrl[EP_NUMBER].out & USB_BUF_CTRL_DATA1_PID;

    DEBUG_TEXT = "USB Data Transfer \tHost to Pico, Received PID=%d, Received Bytes=%d";
    DEBUG_SHOW ("USB", 1, RECEIVE_PACKET_ID/8192, bytes_received);

    return bytes_received;
  
}

void usb_wait_for_buffer_completion_host_to_pico(uint8_t EP_NUMBER, bool buffer_status_clear) {

    // bit 0 = EP0_IN
    // bit 1 = EP0_OUT
    // bit 2 = EP1_IN
    // bit 3 = EP1_OUT

    uint8_t shift_left_bits = (2u * EP_NUMBER) + 1;
    
    uint32_t buffer_mask = 1 << shift_left_bits;

    usb_wait_for_buffer_completion(EP_NUMBER, buffer_mask, buffer_status_clear);
    
}

void receive_zlp_from_host (uint8_t EP_NUMBER, bool expect_pid_1, bool wait_buffer_complete) {

    uint32_t EXPECT_PACKET_ID;
    uint32_t RECEIVE_PACKET_ID;
    uint32_t buffer_dispatch = 0;
    
    if (expect_pid_1) {
        
        EXPECT_PACKET_ID = USB_BUF_CTRL_DATA1_PID;
        
        pico_endpoint[EP_NUMBER].packet_id = USB_BUF_CTRL_DATA1_PID;

    } else {

        EXPECT_PACKET_ID = pico_endpoint[EP_NUMBER].packet_id;
    }
    
    buffer_dispatch = pico_endpoint[EP_NUMBER].packet_id;

    usb_dpram->ep_buf_ctrl[EP_NUMBER].out = buffer_dispatch;

    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].out = buffer_dispatch | USB_BUF_CTRL_AVAIL;

    RECEIVE_PACKET_ID = usb_dpram->ep_buf_ctrl[EP_NUMBER].out & USB_BUF_CTRL_DATA1_PID;

    if (wait_buffer_complete) usb_wait_for_buffer_completion_host_to_pico(EP_NUMBER, true);

    pico_endpoint[EP_NUMBER].packet_id = toggle_data_pid(EP_NUMBER, false, RECEIVE_PACKET_ID);

    DEBUG_TEXT = "ACK Handshake (Host)\tReceive Host ZLP, PID Expected/Received=%d/%d, Bytes=%d";
    DEBUG_SHOW ("ZLP", 1, EXPECT_PACKET_ID/8192, RECEIVE_PACKET_ID/8192, get_buffer_bytes_to_pico(EP_NUMBER));

}

