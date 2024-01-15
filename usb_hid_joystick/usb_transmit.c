
#include "pico/stdlib.h"
#include "hardware/divider.h"               
#include "include/usb_debug.h"
#include "include/usb_common.h" 
#include "include/usb_transmit.h"
#include "include/usb_endpoints.h"
#include "include/usb_sie_errors.h"
#include "include/show_registers.h"
        
static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void send_sync_packet(uint8_t EP_NUMBER, uint8_t data_packet_size, bool last_packet) {

    uint32_t DATA_PID = host_endpoint[EP_NUMBER].packet_id;
    uint32_t buffer_dispatch = data_packet_size | DATA_PID | USB_BUF_CTRL_FULL; 

    host_endpoint[EP_NUMBER].buffer_complete = false;

    if (last_packet) buffer_dispatch |= USB_BUF_CTRL_LAST;

    DEBUG_TEXT = "Sending Data Packet \tSync Data Packet Size=%d Bytes,      Packet ID (PID)=%d" ;
    DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, data_packet_size, DATA_PID/8192);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch;
    
    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch | USB_BUF_CTRL_AVAIL;

    host_endpoint[EP_NUMBER].packet_id = toggle_data_pid(DATA_PID);

    usb_wait_for_buffer_available_to_host(EP_NUMBER);

}

void send_async_packet(uint8_t EP_NUMBER) {

    uint8_t offset = 0;
    uint16_t async_bytes = host_endpoint[EP_NUMBER].async_bytes_pending;
    uint8_t *dpram_buffer = host_endpoint[EP_NUMBER].dpram_address;
    uint8_t *source_buffer = host_endpoint[EP_NUMBER].source_buffer_address;
    uint8_t  max_packet_size = host_endpoint[EP_NUMBER].max_packet_size;
    uint8_t  async_packet_size = MIN(max_packet_size, async_bytes);
    uint16_t source_buffer_offset = host_endpoint[EP_NUMBER].source_buffer_offset;

    bool last_packet = (async_packet_size <= max_packet_size) ? true : false;

    do {  

        dpram_buffer[offset] = source_buffer[source_buffer_offset + offset];

    } while (++offset < async_packet_size);

    DEBUG_TEXT = "Sending Async Packet \tBuffer Offset=%d, Async Packet Bytes=%d" ;
    DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, source_buffer_offset, async_bytes);

    host_endpoint[EP_NUMBER].async_bytes_pending -= offset;
    host_endpoint[EP_NUMBER].source_buffer_offset += offset;

    send_data_packet(EP_NUMBER, async_packet_size, false, last_packet);

}

void send_data_packet(uint8_t EP_NUMBER, uint8_t data_packet_size, bool wait_for_buffers, bool last_packet) {

    uint32_t DATA_PID = host_endpoint[EP_NUMBER].packet_id;

    uint32_t buffer_dispatch = data_packet_size | DATA_PID | USB_BUF_CTRL_FULL; 

    host_endpoint[EP_NUMBER].buffer_complete = false;

    if (last_packet) buffer_dispatch |= USB_BUF_CTRL_LAST;

    DEBUG_TEXT = "Sending Data Packet \tData Packet Size=%d Bytes,      Packet ID (PID)=%d" ;
    DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, data_packet_size, DATA_PID/8192);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch;
    
    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch | USB_BUF_CTRL_AVAIL;
    
    host_endpoint[EP_NUMBER].packet_id = toggle_data_pid(DATA_PID);

    if (wait_for_buffers) {

        usb_wait_for_buffer_completion_pico_to_host(EP_NUMBER, true);
    } 
       
} 

void synchronous_transfer_to_host(uint8_t EP_NUMBER, uint8_t *buffer_data, uint16_t transfer_bytes, uint8_t transfer_id) {

    uint8_t  dpram_offset       = 0;
    uint32_t buffer_offset      = 0;
    uint64_t transfer_duration  = 0;
    uint16_t full_packet_size   = MIN(host_endpoint[EP_NUMBER].max_packet_size, 64);   
    uint16_t full_packets       = transfer_bytes / full_packet_size;
    uint16_t last_packet_size   = transfer_bytes - (full_packets * full_packet_size);
    uint8_t  *usb_dpram_data    = host_endpoint[EP_NUMBER].dpram_address;

    DEBUG_TEXT = "Synchronous Transfer \tFull (%d Byte) Packets to Send=%d, Last Packet Size=%d";
    DEBUG_SHOW ("USB", DEBUG_TEXT, full_packet_size, full_packets, last_packet_size);

    bool last_packet = (transfer_bytes == full_packet_size) ? true : false;

    absolute_time_t start_time_now = get_absolute_time();

    host_endpoint[EP_NUMBER].async_bytes_pending = 0;
    host_endpoint[EP_NUMBER].async_mode = false;
    host_endpoint[EP_NUMBER].transfer_id = transfer_id;
    host_endpoint[EP_NUMBER].transfer_complete = false;


    do {  

        dpram_offset = buffer_offset % full_packet_size;

        usb_dpram_data[dpram_offset] = buffer_data[buffer_offset];

        if (dpram_offset == full_packet_size - 1) { 

            send_sync_packet(EP_NUMBER, full_packet_size, last_packet);
        
        }  

    } while (++buffer_offset < transfer_bytes);
    
    if (last_packet_size) {

        DEBUG_TEXT = "Synchronous Transfer \tDPRAM Data Remaining, Bytes=%d";
        DEBUG_SHOW ("USB", DEBUG_TEXT, dpram_offset + 1);

        send_sync_packet(EP_NUMBER, last_packet_size, last_packet);

    }

    transfer_duration = absolute_time_diff_us(start_time_now, get_absolute_time());

    DEBUG_TEXT = "Synchronous Transfer \tBuffer Offset=%d, Transfer Duration=%lldµs";
    DEBUG_SHOW ("USB", DEBUG_TEXT, buffer_offset, transfer_duration);

}

void start_async_transfer_to_host(uint8_t EP_NUMBER, void *source_buffer_address, uint16_t transfer_bytes, uint8_t transfer_id) {

    uint8_t  offset = 0;
    uint16_t full_packet_size   = MIN(host_endpoint[EP_NUMBER].max_packet_size, 64);   
    uint16_t first_packet_size  = MIN(transfer_bytes, full_packet_size);
    uint16_t async_bytes        = transfer_bytes - first_packet_size;
    uint16_t full_async_packets = async_bytes / full_packet_size;
    uint16_t last_packet_size   = async_bytes - (full_async_packets * full_packet_size);
    uint8_t *source_buffer      = source_buffer_address;
    uint8_t *dpram_buffer       = host_endpoint[EP_NUMBER].dpram_address;
    bool     last_packet        = (transfer_bytes <= full_packet_size) ? true : false;

    host_endpoint[EP_NUMBER].async_mode = true;
    host_endpoint[EP_NUMBER].async_bytes_pending = async_bytes;
    host_endpoint[EP_NUMBER].transfer_id = transfer_id;
    host_endpoint[EP_NUMBER].transfer_bytes = transfer_bytes;
    host_endpoint[EP_NUMBER].transfer_complete = false;
    host_endpoint[EP_NUMBER].bytes_transferred = 0;
    host_endpoint[EP_NUMBER].source_buffer_address = source_buffer_address;
    host_endpoint[EP_NUMBER].last_packet_size = last_packet_size;
    host_endpoint[EP_NUMBER].transfer_duration = 0;
    host_endpoint[EP_NUMBER].start_time_now = get_absolute_time();
          
    DEBUG_TEXT = "Start Async Transfer \tSending First %d/%d Bytes";  
    DEBUG_SHOW ("USB", DEBUG_TEXT, first_packet_size, transfer_bytes);

    do {  

        dpram_buffer[offset] = source_buffer[offset];

    } while (++offset < first_packet_size);

    host_endpoint[EP_NUMBER].source_buffer_offset = offset; 

    send_data_packet(EP_NUMBER, first_packet_size, false, last_packet);
 
}

void send_ack_handshake_to_host(uint8_t EP_NUMBER, bool clear_buffer_status) {

    // bit 0 = EP0_IN
    // bit 1 = EP0_OUT
    // bit 2 = EP1_IN
    // bit 3 = EP1_OUT

    // uint32_t buffer_status_mask = (1 << (EP_NUMBER * 2u));

    uint8_t shift_left_bits = (2u * EP_NUMBER) + 0;

    uint32_t buffer_control = 0;
    uint32_t buffer_dispatch = 0;
    uint32_t buffer_status_mask = 1u << shift_left_bits;

    uint16_t ZERO_LENGTH_PACKET = 0;

    buffer_dispatch |= ZERO_LENGTH_PACKET;
    buffer_dispatch |= USB_BUF_CTRL_LAST;
    buffer_dispatch |= USB_BUF_CTRL_FULL;
    buffer_dispatch |= USB_BUF_CTRL_DATA1_PID;
    
    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch;

    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch | USB_BUF_CTRL_AVAIL;

    buffer_control = usb_dpram->ep_buf_ctrl[EP_NUMBER].in;

    if (clear_buffer_status) usb_hardware_clear->buf_status = buffer_status_mask;

    DEBUG_TEXT = "ACK Handshake (Host)\tSending ACK to Host, PID=%d, Register=%08X";
    DEBUG_SHOW ("ACK", DEBUG_TEXT, USB_BUF_CTRL_DATA1_PID/8192, buffer_control);

}

void usb_wait_for_buffer_available_to_host(uint8_t EP_NUMBER) {
    
    volatile bool sie_errors;
    volatile bool wait_timeout;
    volatile uint32_t buffer_unavailable;
   
    uint64_t wait_duration = 0;
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

        DEBUG_TEXT = "Buffer Wait Timeout\tWaited %d µs for buffer available to CPU";
        DEBUG_SHOW ("TIM", DEBUG_TEXT, wait_duration);

    } else {

        DEBUG_TEXT = "Buffer Wait Complete\tWaited %d µs for buffer available to CPU";
        DEBUG_SHOW ("TIM", DEBUG_TEXT, wait_duration);

    }

}

void usb_wait_for_buffer_completion_pico_to_host(uint8_t EP_NUMBER, bool buffer_status_clear) {

    // bit 0 = EP0_IN
    // bit 1 = EP0_OUT
    // bit 2 = EP1_IN
    // bit 3 = EP1_OUT

    uint32_t buffer_bits = (2 << EP_NUMBER);

    uint8_t shift_left_bits = (2 * EP_NUMBER) + 0;
    
    uint32_t buffer_mask = 1 << shift_left_bits;

    usb_wait_for_buffer_completion(EP_NUMBER, buffer_mask, buffer_status_clear);

}

void usb_wait_for_last_packet_to_host(uint8_t EP_NUMBER) {

    uint64_t wait_duration = 0;
    volatile bool wait_timeout;  
    volatile bool last_received;
  
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(100000);

    do { 

        wait_timeout = time_reached(wait_time_end);

        last_received = usb_dpram->ep_buf_ctrl[EP_NUMBER].in & USB_BUF_CTRL_LAST;
       
    } while (!wait_timeout && !last_received);

        wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());


    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tLast Packet Wait Timeout, Duration=%lld µs";
        DEBUG_SHOW ("USB", DEBUG_TEXT , wait_duration);


    } else {

        DEBUG_TEXT = "Wait Last Packet\tWait Duration=%lld µs";
        DEBUG_SHOW ("SIE", DEBUG_TEXT , wait_duration);

    }

}
