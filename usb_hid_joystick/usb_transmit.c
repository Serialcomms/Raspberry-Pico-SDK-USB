/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/divider.h"               
#include "include/usb_debug.h"
#include "include/usb_common.h" 
#include "include/usb_transmit.h"
#include "include/usb_endpoints.h"
#include "include/usb_sie_errors.h"
#include "include/show_registers.h"
#include "include/pico_device.h"
        
extern uint8_t *DEBUG_TEXT;

extern struct pico_profile pico;

void send_sync_packet(uint8_t EP_NUMBER, uint8_t data_packet_size, bool last_packet) {

    uint32_t DATA_PID = host_endpoint[EP_NUMBER].packet_id;
    uint32_t buffer_dispatch = data_packet_size | DATA_PID | USB_BUF_CTRL_FULL; 

    host_endpoint[EP_NUMBER].buffer_complete = false;

    if (last_packet) buffer_dispatch |= USB_BUF_CTRL_LAST;

    DEBUG_TEXT = "Sending Data Packet \tSync Data Packet Size=%d Bytes, Packet ID=%d" ;
    DEBUG_SHOW (ep_text(EP_NUMBER), 1, data_packet_size, DATA_PID/8192);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch;
    
    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch | USB_BUF_CTRL_AVAIL;

    host_endpoint[EP_NUMBER].packet_id = toggle_data_pid(EP_NUMBER, true, DATA_PID);

    usb_wait_for_buffer_completion_pico_to_host(EP_NUMBER, true);
    //usb_wait_for_buffer_completion_host_to_pico(EP_NUMBER, true);

}

void send_async_packet(uint8_t EP_NUMBER) {

    uint8_t offset = 0;
    uint16_t async_bytes = host_endpoint[EP_NUMBER].async_bytes_pending;
    uint8_t *dpram_buffer = host_endpoint[EP_NUMBER].dpram_address;
    uint8_t *source_buffer = host_endpoint[EP_NUMBER].source_buffer_address;
    uint8_t  max_packet_size = host_endpoint[EP_NUMBER].max_packet_size;
    uint8_t  async_packet_size = MIN(max_packet_size, async_bytes);
    uint16_t source_buffer_offset = host_endpoint[EP_NUMBER].source_buffer_offset;

    bool last_packet = (max_packet_size >= async_packet_size) ? true : false;

    do {  

        dpram_buffer[offset] = source_buffer[source_buffer_offset + offset];

    } while (++offset < async_packet_size);

    DEBUG_TEXT = "Sending Async Packet \tBuffer Offset=%d, Bytes Pending=%d, Last=%s" ;
    DEBUG_SHOW (ep_text(EP_NUMBER), 1, source_buffer_offset, async_bytes, last_packet_text(last_packet));

    host_endpoint[EP_NUMBER].async_bytes_pending -= offset;
    host_endpoint[EP_NUMBER].source_buffer_offset += offset;

    send_data_packet(EP_NUMBER, async_packet_size, false, last_packet);

}

void send_data_packet(uint8_t EP_NUMBER, uint8_t data_packet_size, bool wait_for_buffers, bool last_packet) {

    uint32_t DATA_PID = host_endpoint[EP_NUMBER].packet_id;

    uint32_t buffer_dispatch = data_packet_size | DATA_PID | USB_BUF_CTRL_FULL; 

    host_endpoint[EP_NUMBER].buffer_complete = false;

    if (last_packet) buffer_dispatch |= USB_BUF_CTRL_LAST;

    DEBUG_TEXT = "Sending Data Packet \tData Packet Size=%d Bytes, Packet ID=%d" ;
    DEBUG_SHOW (ep_text(EP_NUMBER), 1, data_packet_size, DATA_PID/8192);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch;
    
    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch | USB_BUF_CTRL_AVAIL;
    
    host_endpoint[EP_NUMBER].packet_id = toggle_data_pid(EP_NUMBER, true, DATA_PID);

    if (wait_for_buffers) {

        usb_wait_for_buffer_completion_pico_to_host(EP_NUMBER, true);

        //usb_wait_for_buffer_completion_host_to_pico(EP_NUMBER, true);
    } 
       
} 

void synchronous_transfer_to_host(uint8_t EP_NUMBER, uint8_t *buffer_data, uint16_t transfer_bytes) {

    uint8_t  dpram_offset           = 0;
    uint32_t buffer_offset          = 0;
    uint16_t full_packet_size       = MIN(host_endpoint[EP_NUMBER].max_packet_size, 64);  
    uint16_t full_packet_offset     = full_packet_size - 1; 
    uint16_t full_packets           = transfer_bytes / full_packet_size;
    uint16_t final_packet_size      = transfer_bytes - (full_packets * full_packet_size);
    uint8_t  *usb_dpram_data        = host_endpoint[EP_NUMBER].dpram_address;
    bool     last_packet_full       = (transfer_bytes == full_packet_size);
    bool     last_packet_flag       = (transfer_bytes %  full_packet_size == 0);
    bool     last_packet_short      = (transfer_bytes  < full_packet_size);
    uint16_t ZERO_LENGTH_PACKET     = 0;

    DEBUG_TEXT = "Synchronous Transfer \tFull (%d Byte) Packets=%d, Final Packet Size=%d";
    DEBUG_SHOW ("USB", 1, full_packet_size, full_packets, final_packet_size);

    absolute_time_t start_time_now = get_absolute_time();

    host_endpoint[EP_NUMBER].async_mode = false;
    host_endpoint[EP_NUMBER].async_bytes_pending = 0;
    host_endpoint[EP_NUMBER].transaction_duration = 0;
    host_endpoint[EP_NUMBER].transaction_complete = false;
    host_endpoint[EP_NUMBER].start_time_now = start_time_now;

    usb_wait_for_buffer_available_to_host(EP_NUMBER);

    if (transfer_bytes) {

        do {  

            dpram_offset = buffer_offset % full_packet_size;

            usb_dpram_data[dpram_offset] = buffer_data[buffer_offset];

            if (dpram_offset == full_packet_offset) { 

                send_sync_packet(EP_NUMBER, full_packet_size, last_packet_short);
        
            }  

        } while (++buffer_offset < transfer_bytes);

       // DEBUG_TEXT = "Synchronous Transfer \tOn Exit, Buffer Offset=%d, dpram_offset=%d";
       // DEBUG_SHOW ("USB", 1, buffer_offset, dpram_offset);

        if (final_packet_size) {

            DEBUG_TEXT = "Synchronous Transfer \tSending Final Data Packet, Bytes=%d";
            DEBUG_SHOW ("USB", 1, final_packet_size);

            send_sync_packet(EP_NUMBER, final_packet_size, true);

        } else if (last_packet_flag) {

            DEBUG_TEXT = "Synchronous Transfer \tSending Final Zero Length Packet";
            DEBUG_SHOW ("USB", 1);

            send_sync_packet(EP_NUMBER, ZERO_LENGTH_PACKET, true);

        }
    
    } else {

        DEBUG_TEXT = "Synchronous Transfer \tSending Zero Length Packet, Bytes=%d";
        DEBUG_SHOW ("USB", 1, transfer_bytes);

        send_sync_packet(EP_NUMBER, ZERO_LENGTH_PACKET, true);

    }

    uint32_t transaction_duration = absolute_time_diff_us(start_time_now, get_absolute_time());
    
    host_endpoint[EP_NUMBER].start_time_end = get_absolute_time();

    host_endpoint[EP_NUMBER].transaction_duration = transaction_duration;

    DEBUG_TEXT = "Synchronous Transfer \tBuffer Offset=%d, Transfer Duration=%ldµs";
    DEBUG_SHOW ("USB", 1, buffer_offset, transaction_duration);

}

void start_async_transfer_to_host(uint8_t EP_NUMBER, void *source_buffer_address, uint16_t transfer_bytes) {

    uint8_t  offset = 0;
    uint16_t max_packet_size    = host_endpoint[EP_NUMBER].max_packet_size;
    uint16_t full_packet_size   = MIN(max_packet_size, 64);   
    uint16_t first_packet_size  = MIN(transfer_bytes, full_packet_size);
    uint16_t async_bytes        = transfer_bytes - first_packet_size;
    uint16_t full_async_packets = async_bytes / full_packet_size;
    uint16_t last_packet_size   = async_bytes - (full_async_packets * full_packet_size);
    uint8_t *source_buffer      = source_buffer_address;
    uint8_t *dpram_buffer       = host_endpoint[EP_NUMBER].dpram_address;
    bool     last_packet        = (transfer_bytes <= full_packet_size) ? true : false;

    host_endpoint[EP_NUMBER].async_mode = true;
    host_endpoint[EP_NUMBER].async_bytes_pending = async_bytes;
    host_endpoint[EP_NUMBER].transfer_bytes = transfer_bytes;
    host_endpoint[EP_NUMBER].transaction_duration = 0;
    host_endpoint[EP_NUMBER].transaction_complete = false;
    host_endpoint[EP_NUMBER].bytes_transferred = 0;
    host_endpoint[EP_NUMBER].last_packet_size = last_packet_size;
    host_endpoint[EP_NUMBER].source_buffer_address = source_buffer_address;
 
    host_endpoint[EP_NUMBER].start_time_now = get_absolute_time();

    usb_wait_for_buffer_available_to_host(EP_NUMBER);
          
    DEBUG_TEXT = "Start Async Transfer \tSending First %d/%d Bytes";  
    DEBUG_SHOW ("USB", 1, first_packet_size, transfer_bytes);

    do {  

        dpram_buffer[offset] = source_buffer[offset];

    } while (++offset < first_packet_size);

    host_endpoint[EP_NUMBER].source_buffer_offset = offset; 

    send_data_packet(EP_NUMBER, first_packet_size, false, last_packet);
 
}

void usb_wait_for_buffer_available_to_host(uint8_t EP_NUMBER) {
    
    volatile bool sie_errors;
    volatile bool wait_timeout;
    volatile uint32_t buffer_unavailable;
   
    uint32_t wait_duration = 0;
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(100000);
    
    do { 

        busy_wait_at_least_cycles(8);

        sie_errors = check_sie_errors();

        wait_timeout = time_reached(wait_time_end);

        buffer_unavailable = usb_dpram->ep_buf_ctrl[EP_NUMBER].in & USB_BUF_CTRL_AVAIL;

    } while (!sie_errors && !wait_timeout && buffer_unavailable);

        wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());

    if (wait_timeout) {

        DEBUG_TEXT = "Buffer Wait Timeout\tWaited %ld µs for buffer available to CPU";
        DEBUG_SHOW ("TIM", 9, wait_duration);

    } else {

        DEBUG_TEXT = "Buffer Wait Complete\tWaited %ld µs for buffer available to CPU";
        DEBUG_SHOW ("TIM", 1, wait_duration);

    }

}

void usb_wait_for_buffer_completion_pico_to_host(uint8_t EP_NUMBER, bool buffer_status_clear) {

    uint8_t shift_left_bits = (2 * EP_NUMBER) + 0;
    
    uint32_t buffer_mask = 1 << shift_left_bits;

    usb_wait_for_buffer_completion(EP_NUMBER, buffer_mask, buffer_status_clear);

}

void usb_wait_for_last_packet_to_host(uint8_t EP_NUMBER) {

    uint32_t wait_duration=0;
    volatile bool sie_errors;
    volatile bool wait_timeout;  
    volatile bool last_received;
  
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(100000);

    do { 

        sie_errors = check_sie_errors();
        
        wait_timeout = time_reached(wait_time_end);

        last_received = usb_dpram->ep_buf_ctrl[EP_NUMBER].in & USB_BUF_CTRL_LAST;
       
    } while (!wait_timeout && !sie_errors && !last_received);

        wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());

    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tLast Packet Wait Timeout, Duration=%ld µs";
        DEBUG_SHOW ("USB", 9, wait_duration);

    } else {

        DEBUG_TEXT = "Wait Last Packet\tWait Duration=%ld µs";
        DEBUG_SHOW ("SIE", 9, wait_duration);

    }

}

void send_zlp_to_host (uint8_t EP_NUMBER, bool send_pid_1, bool wait_for_buffer) {

    uint32_t PACKET_ID;
    uint32_t buffer_control = 0;
    uint32_t buffer_dispatch = 0;
 
    uint16_t ZERO_LENGTH_PACKET = 0;
 
    if (send_pid_1) host_endpoint[EP_NUMBER].packet_id = USB_BUF_CTRL_DATA1_PID;

    PACKET_ID = host_endpoint[EP_NUMBER].packet_id;

    buffer_dispatch =  PACKET_ID | USB_BUF_CTRL_FULL | ZERO_LENGTH_PACKET;

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch;

    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch | USB_BUF_CTRL_AVAIL;

    if (wait_for_buffer) usb_wait_for_buffer_completion_pico_to_host(0, true);

    DEBUG_TEXT = "ACK Handshake (Host)\tSend ZLP to Host, EP%d, PID=%d";
    DEBUG_SHOW ("ZLP", 1, EP_NUMBER, PACKET_ID/8192);

}
