/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "include/usb_transmit.h"
#include "include/usb_receive.h"
#include "include/usb_debug.h"
#include "include/pico_device.h"
#include "include/usb_common.h"
#include "include/usb_sie_errors.h"
#include "include/usb_endpoints.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

extern uint8_t *DEBUG_TEXT;

extern struct pico_profile pico;

void usb_wait_for_buffer_completion(uint8_t EP_NUMBER, uint32_t buffer_mask, bool buffer_status_clear) {

    volatile bool sie_errors;
    volatile bool wait_timeout;
    
    io_rw_32 buffer_status;
    io_rw_32 buffer_complete;
    
    uint32_t wait_duration=0;
    
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(100000);

    buffer_status = usb_hw->buf_status;

    DEBUG_TEXT = "Serial Interface Engine\tBuffer Wait     Mask=%08X, Register=%08X";
    DEBUG_SHOW ("SIE", 1, buffer_mask, buffer_status);

    do { 

        busy_wait_at_least_cycles(8);

        sie_errors = check_sie_errors();

        wait_timeout = time_reached(wait_time_end);

        buffer_complete = usb_hw->buf_status & buffer_mask;

    } while (!sie_errors && !wait_timeout && !buffer_complete);

    buffer_status = usb_hw->buf_status;

    wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());

    if (sie_errors) {

        sie_status_error_handler();
        DEBUG_TEXT = "Serial Interface Engine\tWaited %ld µs\tMask=%08X, Register=%08X";
        DEBUG_SHOW ("SIE", 9, wait_duration, buffer_mask, buffer_status);
       
    }

    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tBuffer Completion Wait Timeout, Duration=%ld µs";
        DEBUG_SHOW ("TIM", 9, wait_duration);

        DEBUG_TEXT = "Wait Timeout Error\tBuffer Mask=%08X,\t Buffer Status=%08X";
        DEBUG_SHOW ("TIM", 9, buffer_mask, buffer_status);

        DEBUG_TEXT = "Wait Timeout Error\tSIE Error Status=%08X";
        DEBUG_SHOW ("TIM", 9, sie_errors);


    } else {

        if (buffer_status_clear) {

            clear_buffer_status(buffer_mask);           

        }

        DEBUG_TEXT = "Buffer Wait Complete\tDuration=%ldµs";
        DEBUG_SHOW ("USB", 1, wait_duration);

    }
    
}

void wait_for_transaction_completion(bool clear_transaction) {

    volatile bool sie_errors;
    volatile bool wait_timeout;
    volatile bool transaction_complete;
    volatile uint32_t buffer_status;
    
    uint32_t wait_duration=0;
    uint32_t wait_microseconds = 100000;
    
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(wait_microseconds);

    transaction_complete = usb_hw->sie_status & USB_SIE_STATUS_TRANS_COMPLETE_BITS;

    DEBUG_TEXT = "Serial Interface Engine\tMax. Wait %ld µs for Transaction Completion";
    DEBUG_SHOW ("SIE", 1, wait_microseconds);
    
    do { 

        busy_wait_at_least_cycles(8);

        sie_errors = check_sie_errors();

        wait_timeout = time_reached(wait_time_end);

        transaction_complete = usb_hw->sie_status & USB_SIE_STATUS_TRANS_COMPLETE_BITS;

    } while (!sie_errors && !wait_timeout && !transaction_complete );

    wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());

    if (sie_errors) {

        DEBUG_TEXT = "Serial Interface Engine\tSIE Error Status=%08X";
        DEBUG_SHOW ("SIE", 9, sie_errors);
        
        sie_status_error_handler();

    }

    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tTransaction Completion Wait Timeout, Duration=%ld µs";
        DEBUG_SHOW ("TIM", 1, wait_duration);

    } else {

        if (clear_transaction) {

            usb_hardware_clear->sie_status = USB_SIE_STATUS_TRANS_COMPLETE_BITS;

            DEBUG_TEXT = "Serial Interface Engine\tCleared Transaction Complete Bits";
            DEBUG_SHOW ("SIE", 1);

        }

        DEBUG_TEXT = "Serial Interface Engine\tTransaction Complete, Wait Duration=%ldµs";
        DEBUG_SHOW ("USB", 1, wait_duration);

        pico.usb.device.last_transaction.time = get_absolute_time();

    }
    
}

void clear_buffer_status (uint32_t buffer_mask) {

    io_rw_32 buffer_status = usb_hw->buf_status;

    DEBUG_TEXT = "Buffer Status\t\tClearing \tMask=%08X, Register=%08X";
    DEBUG_SHOW ("USB", 1, buffer_mask, buffer_status);

    usb_hardware_clear->buf_status = buffer_mask;

    buffer_status = usb_hw->buf_status;

    DEBUG_TEXT = "Buffer Status\t\tCleared  \tMask=%08X, Register=%08X";
    DEBUG_SHOW ("USB", 1, buffer_mask, buffer_status);
}

uint32_t toggle_data_pid(uint8_t EP_NUMBER, bool setup_direction, uint32_t data_pid) {

    uint32_t new_pid = data_pid ? USB_BUF_CTRL_DATA0_PID : USB_BUF_CTRL_DATA1_PID;

    DEBUG_TEXT = "Toggle Packet ID EP%d\t%s\tOld PID=%d > New PID=%d";

    if (setup_direction) {

        DEBUG_SHOW ("PID", 1, EP_NUMBER, "Pico > Host", data_pid/8192, new_pid/8192);

    } else {

        DEBUG_SHOW ("PID", 1, EP_NUMBER, "Host > Pico", data_pid/8192, new_pid/8192); 

    }

    return new_pid;

}

void prepare_control_transfer() {
    
    host_endpoint[0].packet_id = USB_BUF_CTRL_DATA1_PID;
    pico_endpoint[0].packet_id = USB_BUF_CTRL_DATA1_PID;

    DEBUG_TEXT = "Setup Control Status\tPreparing Control Transfer, Set Packet IDs=1";
    DEBUG_SHOW ("USB", 1);

    pico.usb.device.control_transfer_stage = 2;

}

void ack_pico_to_host_control_transfer() {

    uint16_t bytes_received = 0;
    uint32_t buffer_dispatch = 0;
    uint16_t EXPECT_PACKET_ID = pico_endpoint[0].packet_id;  // set by prepare_control_transfer

    buffer_dispatch |= EXPECT_PACKET_ID;

    DEBUG_TEXT = "ACK Handshake (Host)\tReceiving ACK from Host, PID=%d";
    DEBUG_SHOW ("ACK", 1, EXPECT_PACKET_ID/8192);
    
    usb_dpram->ep_buf_ctrl[0].out = buffer_dispatch | USB_BUF_CTRL_AVAIL;

    pico_endpoint[0].packet_id = toggle_data_pid(0, false, EXPECT_PACKET_ID);

    usb_wait_for_buffer_completion_host_to_pico(0, true);

      if (bytes_received == 0) {

        DEBUG_TEXT = "Status Transaction\tReceived Zero Length Packet (Bytes=%d) from host";
        DEBUG_SHOW ("ZLP", 1, bytes_received);

    }

    show_ep0_buffers_complete();

    pico.usb.device.control_transfer_stage = 2;

}

void ack_host_to_pico_control_transfer() {

    uint32_t buffer_dispatch = 0;
    uint16_t ZERO_LENGTH_PACKET = 0;
    uint16_t SEND_PACKET_ID = host_endpoint[0].packet_id;  // set by prepare_control_transfer

    SEND_PACKET_ID = USB_BUF_CTRL_DATA1_PID;  // set by prepare_control_transfer
    buffer_dispatch |= SEND_PACKET_ID;
    buffer_dispatch |= ZERO_LENGTH_PACKET;
    buffer_dispatch |= USB_BUF_CTRL_LAST;
    buffer_dispatch |= USB_BUF_CTRL_FULL;
   
    DEBUG_TEXT = "ACK Handshake (Pico)\tSending ACK to Host, PID=%d";
    DEBUG_SHOW ("ACK", 1, SEND_PACKET_ID/8192);

    usb_dpram->ep_buf_ctrl[0].in = buffer_dispatch ;
    
    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[0].in = buffer_dispatch | USB_BUF_CTRL_AVAIL;

    pico_endpoint[0].packet_id = toggle_data_pid(0, false, SEND_PACKET_ID);

    usb_hardware_clear->buf_status = (uint32_t) 1;

    //usb_wait_for_buffer_completion_pico_to_host(0, true);

    show_ep0_buffers_complete();


}

void send_ack_handshake_to_host(uint8_t EP_NUMBER, bool clear_buffer_status) {

    uint8_t shift_left_bits = (2 * EP_NUMBER) + 0;

    uint32_t buffer_control = 0;
    uint32_t buffer_dispatch = 0;
    uint32_t buffer_status_mask = 1 << shift_left_bits;

    uint16_t ZERO_LENGTH_PACKET = 0;

    buffer_dispatch |= ZERO_LENGTH_PACKET;
    buffer_dispatch |= USB_BUF_CTRL_LAST;
    buffer_dispatch |= USB_BUF_CTRL_FULL;
    buffer_dispatch |= USB_BUF_CTRL_DATA1_PID;
    
    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch;

    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch | USB_BUF_CTRL_AVAIL;

    if (clear_buffer_status) usb_hardware_clear->buf_status = buffer_status_mask;

    DEBUG_TEXT = "ACK Handshake (Host)\tSending ACK to Host, PID=%d";
    DEBUG_SHOW ("ACK", 1, USB_BUF_CTRL_DATA1_PID/8192);

    pico.usb.device.control_transfer_stage = 2;

}

uint8_t get_device_address() {

    return usb_hw->dev_addr_ctrl & 0x7f;
}

volatile bool device_address_is_set() {

    return (usb_hw->dev_addr_ctrl & 0x7f) ? true : false;
}

void build_ep0_data_packet(uint8_t *source_buffer, uint8_t transfer_bytes) {

    uint8_t offset = 0; 
    uint8_t ep0_buffer_bytes = MIN(transfer_bytes, 64);

    do {  

       usb_dpram->ep0_buf_a[offset] = source_buffer[offset];

    } while (++offset < ep0_buffer_bytes);

}

void copy_ep0_data_packet(uint8_t *copy_buffer) {

    uint8_t offset = 0; 

    do {  

       copy_buffer[offset] = usb_dpram->ep0_buf_a[offset];

    } while (++offset < 64);

}

void set_ep0_buffer_status(bool enable_interrupts) {

    if (enable_interrupts) {

        usb_hardware_set->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS;

    } else {

        usb_hardware_clear->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS;

    }

}

volatile uint16_t get_buffer_bytes_to_host (uint8_t EP_NUMBER) {

   return (uint16_t) usb_dpram->ep_buf_ctrl[EP_NUMBER].in & USB_BUF_CTRL_LEN_MASK;
}

volatile uint16_t get_buffer_bytes_to_pico (uint8_t EP_NUMBER) {

   return (uint16_t) usb_dpram->ep_buf_ctrl[EP_NUMBER].out & USB_BUF_CTRL_LEN_MASK;
}

uint8_t *last_packet_text (bool last_packet) {

    return (last_packet) ? "TRUE" : "FALSE" ; 
}
