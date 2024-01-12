
#include "hardware/divider.h"               
#include "hardware/regs/usb.h"             
#include "hardware/structs/usb.h"  

#include "include/usb_debug.h"
#include "include/sie_errors.h" 
#include "include/usb_transfer.h"
#include "include/usb_endpoints.h"
#include "include/show_registers.h"
        
#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;


void send_sync_packet(uint8_t EP_NUMBER, uint8_t data_packet_size, bool last_packet) {

    uint32_t DATA_PID = host_endpoint[EP_NUMBER].packet_id;
    uint32_t buffer_dispatch = DATA_PID | USB_BUF_CTRL_AVAIL | USB_BUF_CTRL_FULL; 

    host_endpoint[EP_NUMBER].buffer_complete = false;

    if (last_packet) buffer_dispatch |= USB_BUF_CTRL_LAST;

    DEBUG_TEXT = "Sending Data Packet \tSync Data Packet Size=%d Bytes,      Packet ID (PID)=%d" ;
    DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, data_packet_size, DATA_PID/8192);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = data_packet_size | buffer_dispatch;

    host_endpoint[EP_NUMBER].packet_id = toggle_data_pid(DATA_PID);

    usb_wait_for_buffer_empty_to_host(EP_NUMBER);

}


void send_async_packet(uint8_t EP_NUMBER) {

    uint8_t offset = 0;
    uint16_t async_bytes = host_endpoint[EP_NUMBER].async_bytes;
    uint8_t *dpram_buffer = host_endpoint[EP_NUMBER].dpram_address;
    uint8_t *source_buffer = host_endpoint[EP_NUMBER].source_buffer_address;
    uint8_t  max_packet_size = host_endpoint[EP_NUMBER].max_packet_size;
    uint8_t  async_packet_size = MIN(max_packet_size, async_bytes);
    uint16_t source_buffer_offset = host_endpoint[EP_NUMBER].source_buffer_offset;

    bool last_packet = (async_packet_size <= max_packet_size) ? true : false;

    do {  

        dpram_buffer[offset] = source_buffer[source_buffer_offset + offset];

    } while (++offset < async_packet_size);

    DEBUG_TEXT = "Sending Async Packet \tBuffer Offset=%d, Async Bytes=%d" ;
    DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, source_buffer_offset, async_bytes);

    host_endpoint[EP_NUMBER].async_bytes -= offset;
    host_endpoint[EP_NUMBER].source_buffer_offset += offset;

    send_data_packet(EP_NUMBER, async_packet_size, false, last_packet);

}

void send_data_packet(uint8_t EP_NUMBER, uint8_t data_packet_size, bool wait_for_buffers, bool last_packet) {

    uint32_t DATA_PID = host_endpoint[EP_NUMBER].packet_id;
    uint32_t buffer_dispatch = DATA_PID | USB_BUF_CTRL_AVAIL | USB_BUF_CTRL_FULL; 

    host_endpoint[EP_NUMBER].buffer_complete = false;

    if (last_packet) buffer_dispatch |= USB_BUF_CTRL_LAST;

    DEBUG_TEXT = "Sending Data Packet \tData Packet Size=%d Bytes,      Packet ID (PID)=%d" ;
    DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, data_packet_size, DATA_PID/8192);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = data_packet_size | buffer_dispatch;

    host_endpoint[EP_NUMBER].packet_id = toggle_data_pid(DATA_PID);

    if (wait_for_buffers) {

        usb_wait_for_buffer_completion_pico_to_host(EP_NUMBER, true);
    } 
       
} 

void synchronous_transfer_to_host(uint8_t EP_NUMBER, uint8_t *buffer_data, uint16_t transfer_bytes) {

    uint8_t  dpram_offset       = 0;
    uint32_t buffer_offset      = 0;
    uint64_t transfer_duration  = 0;
    uint16_t full_packet_size   = MIN(host_endpoint[EP_NUMBER].max_packet_size, 64);   
    uint16_t full_packets       = transfer_bytes / full_packet_size;
    uint16_t last_packet_size   = transfer_bytes - (full_packets * full_packet_size);
    uint8_t  *usb_dpram_data    = host_endpoint[EP_NUMBER].dpram_address;

    DEBUG_TEXT = "Synchronous Transfer \tFull (%d Byte) Packets to Send=%d, Remainder=%d";
    DEBUG_SHOW ("USB", DEBUG_TEXT, full_packet_size, full_packets, last_packet_size);

    bool last_packet = (transfer_bytes == full_packet_size) ? true : false;

    absolute_time_t start_time_now = get_absolute_time();

    host_endpoint[EP_NUMBER].transaction_complete = false;
    host_endpoint[EP_NUMBER].async_mode = false;
    host_endpoint[EP_NUMBER].async_bytes = 0;

    do {  

        dpram_offset = buffer_offset % full_packet_size;

        usb_dpram_data[dpram_offset] = buffer_data[buffer_offset];

        if (dpram_offset == full_packet_size - 1) { 

            //send_data_packet(EP_NUMBER, full_packet_size, true, last_packet);

            send_sync_packet(EP_NUMBER, full_packet_size, last_packet);
        
        }  

    } while (++buffer_offset < transfer_bytes);
    
    if (last_packet_size) {

        DEBUG_TEXT = "Synchronous Transfer \tDPRAM Data Remaining, bytes=%d";
        DEBUG_SHOW ("USB", DEBUG_TEXT, dpram_offset + 1);

        send_data_packet(EP_NUMBER, last_packet_size, true, true);

    }

   // DEBUG_TEXT = "Synchronous Transfer \tWaiting for Transaction Completion";
    DEBUG_TEXT = "Synchronous Transfer \tWaiting for Last Packet";
    DEBUG_SHOW ("USB", DEBUG_TEXT);

    //usb_wait_for_transaction_completion(EP_NUMBER, true);

    usb_wait_for_last_packet_to_host(EP_NUMBER);

    transfer_duration = absolute_time_diff_us(start_time_now, get_absolute_time());

    DEBUG_TEXT = "Synchronous Transfer \tBuffer Offset=%d, Transfer Duration=%lldµs";
    DEBUG_SHOW ("USB", DEBUG_TEXT, buffer_offset, transfer_duration);

}

void start_async_transfer_to_host(uint8_t EP_NUMBER, void *source_buffer_address, uint16_t transfer_bytes) {

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
    host_endpoint[EP_NUMBER].async_bytes = async_bytes;
    host_endpoint[EP_NUMBER].transfer_bytes = transfer_bytes;
    host_endpoint[EP_NUMBER].source_buffer_address = source_buffer_address;
    host_endpoint[EP_NUMBER].last_packet_size = last_packet_size;
    host_endpoint[EP_NUMBER].transfer_duration = 0;
    host_endpoint[EP_NUMBER].transaction_complete = false;
    host_endpoint[EP_NUMBER].start_time_now = get_absolute_time();
          
    DEBUG_TEXT = "Start Async Transfer \tSending First %d/%d Bytes";  
    DEBUG_SHOW ("USB", DEBUG_TEXT, first_packet_size, transfer_bytes);

    do {  

        dpram_buffer[offset] = source_buffer[offset];

    } while (++offset < first_packet_size);

    host_endpoint[EP_NUMBER].source_buffer_offset = offset; 

    send_data_packet(EP_NUMBER, first_packet_size, false, last_packet);
 
}

void usb_start_transfer_host_to_pico(uint8_t EP_NUMBER, uint16_t buffer_length) {

    uint32_t buffer_dispatch;

    uint32_t DATA_PID = pico_endpoint[EP_NUMBER].packet_id;

    pico_endpoint[EP_NUMBER].packet_id = toggle_data_pid(DATA_PID);

    buffer_dispatch = DATA_PID | USB_BUF_CTRL_AVAIL;

    usb_dpram->ep_buf_ctrl[EP_NUMBER].out = buffer_dispatch;

    DEBUG_TEXT = "USB Data Transfer \tHost to Pico, PID=%08X, Length=%d";
    DEBUG_SHOW ("USB", DEBUG_TEXT, DATA_PID, buffer_length);

}

void receive_status_transaction_from_host(uint8_t EP_NUMBER, bool clear_buffer_status) {

    uint32_t buffer_status = 0;
    uint32_t buffer_control = 0;
    uint32_t buffer_dispatch = 0;
    
    uint32_t buffer_status_mask = (1 << (EP_NUMBER * 2u) + 1);
    
    buffer_dispatch |= USB_BUF_CTRL_AVAIL;
    buffer_dispatch |= USB_BUF_CTRL_DATA1_PID;      // PID1 expected from host
    
    usb_dpram->ep_buf_ctrl[EP_NUMBER].out = buffer_dispatch;

    buffer_control = usb_dpram->ep_buf_ctrl[EP_NUMBER].out;

    if (clear_buffer_status) {

       usb_hardware_clear->buf_status = buffer_status_mask;

       buffer_status =  usb_hw->buf_status;
    }

    DEBUG_TEXT = "USB Data Transfer \tReceive Status Transaction,\tRegister=%08X";
    DEBUG_SHOW ("USB", DEBUG_TEXT, buffer_control);

    DEBUG_TEXT = "USB Data Transfer \tBuffer Mask=%08X,\t Status Register=%08X";
    DEBUG_SHOW ("USB", DEBUG_TEXT, buffer_status_mask, buffer_status);

}

void send_ack_handshake_to_host(uint8_t EP_NUMBER, bool clear_buffer_status) {

    uint32_t buffer_control = 0;
    uint32_t buffer_dispatch = 0;
    uint32_t buffer_status_mask = (1 << (EP_NUMBER * 2u));

    buffer_dispatch |= USB_BUF_CTRL_LAST;
    buffer_dispatch |= USB_BUF_CTRL_FULL;
    buffer_dispatch |= USB_BUF_CTRL_AVAIL;
    buffer_dispatch |= USB_BUF_CTRL_DATA1_PID;
    
    usb_dpram->ep_buf_ctrl[EP_NUMBER].in = buffer_dispatch;

    buffer_control = usb_dpram->ep_buf_ctrl[EP_NUMBER].in;

    if (clear_buffer_status) usb_hardware_clear->buf_status = buffer_status_mask;

    DEBUG_TEXT = "ACK Handshake (Host)\tSending ACK to Host, PID=%d, Register=%08X";
    DEBUG_SHOW ("ACK", DEBUG_TEXT, USB_BUF_CTRL_DATA1_PID/8192, buffer_control);

}

void usb_wait_for_buffer_empty_to_host(uint8_t EP_NUMBER) {

    volatile bool wait_timeout;
    volatile bool sie_errors;
    
    volatile uint32_t buffer_empty;
    volatile uint32_t buffer_status;
   
    uint64_t wait_duration = 0;
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(100000);
    
    do { 

        busy_wait_at_least_cycles(8);

        sie_errors = check_sie_errors();

        buffer_empty = !usb_dpram->ep_buf_ctrl[EP_NUMBER].in & USB_BUF_CTRL_FULL;

        wait_timeout = time_reached(wait_time_end);

    } while (!sie_errors && !buffer_empty && !wait_timeout);


}

uint32_t toggle_data_pid(uint32_t data_pid) {

    uint32_t new_pid = data_pid ? USB_BUF_CTRL_DATA0_PID : USB_BUF_CTRL_DATA1_PID;

    DEBUG_TEXT = "USB Data Transfer \tToggling Old PID=%d > New PID=%d";
    DEBUG_SHOW ("USB", DEBUG_TEXT , data_pid/8192, new_pid/8192);

    return new_pid;

}

void usb_wait_for_buffer_completion_pico_to_host(uint8_t EP_NUMBER, bool buffer_status_clear) {
    
    uint32_t buffer_mask = (1 << (EP_NUMBER * 2u) + 0);

    usb_wait_for_buffer_completion(EP_NUMBER, buffer_mask, buffer_status_clear);

}

void usb_wait_for_buffer_completion_host_to_pico(uint8_t EP_NUMBER, bool buffer_status_clear) {

    uint32_t buffer_mask = (1 << (EP_NUMBER * 2u) + 1); 

    usb_wait_for_buffer_completion(EP_NUMBER, buffer_mask, buffer_status_clear);
    
}

void usb_wait_for_buffer_completion(uint8_t EP_NUMBER, uint32_t buffer_mask, bool buffer_status_clear) {

    volatile bool sie_errors;
    volatile bool wait_timeout;
    
    volatile uint32_t buffer_done;
    volatile uint32_t buffer_status;
   
    uint64_t wait_duration = 0;
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(100000);
    
    do { 

        busy_wait_at_least_cycles(8);

        sie_errors = check_sie_errors();

        buffer_done = usb_hw->buf_status & buffer_mask;

        wait_timeout = time_reached(wait_time_end);

    } while (!sie_errors && !buffer_done && !wait_timeout);

    wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());

    if (sie_errors) {

        sie_status_error_handler();

        DEBUG_TEXT = "Serial Interface Engine\tWaited %lld µs for Buffer\tMask=%08X,\t Register=%08X";
        DEBUG_SHOW ("SIE", DEBUG_TEXT, wait_duration, buffer_mask, buffer_done);
       
    }

    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tBuffer Completion Wait Timeout, Duration=%lld µs";
        DEBUG_SHOW ("TIM", DEBUG_TEXT, wait_duration);

        DEBUG_TEXT = "Wait Timeout Error\tBuffer Mask=%08X,\t Buffer Status=%08X";
        DEBUG_SHOW ("TIM", DEBUG_TEXT, buffer_mask, buffer_done);

        DEBUG_TEXT = "Wait Timeout Error\tSIE Error Status=%08X";
        DEBUG_SHOW ("TIM", DEBUG_TEXT, sie_errors);


    } else {

        if (buffer_status_clear) {

            buffer_status = usb_hw->buf_status;

            DEBUG_TEXT = "Buffer Status\t\tClearing,\tMask=%08X, Register=%08X";
            DEBUG_SHOW ("USB", DEBUG_TEXT , buffer_mask, buffer_status);

            usb_hardware_clear->buf_status = buffer_mask;

            buffer_status = usb_hw->buf_status;

            DEBUG_TEXT = "Buffer Status\t\tCleared, \tMask=%08X, Register=%08X";
            DEBUG_SHOW ("USB", DEBUG_TEXT , buffer_mask, buffer_status);

        }

        DEBUG_TEXT = "Buffer Status\t\tComplete, \tBuffer Status Wait Duration=%lldµs";
        DEBUG_SHOW ("USB", DEBUG_TEXT ,wait_duration );

    }
    
}

void usb_wait_for_transaction_completion(uint8_t EP_NUMBER, bool completion_clear) {

    volatile bool wait_timeout;  
    volatile bool transaction_complete;
  
    uint64_t wait_duration = 0;
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(100000);
   
    do { 

        wait_timeout = time_reached(wait_time_end);

        transaction_complete = host_endpoint[EP_NUMBER].transaction_complete;
       
    } while (!wait_timeout && !transaction_complete);

    
    wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());


    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tTransaction Completion Wait Timeout, Duration=%lld µs";
        DEBUG_SHOW ("USB", DEBUG_TEXT , wait_duration);


    } else {

        DEBUG_TEXT = "Transaction Complete\tWait Duration=%lld µs";
        DEBUG_SHOW ("SIE", DEBUG_TEXT , wait_duration);

    }


    if (completion_clear) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_TRANS_COMPLETE_BITS;

        DEBUG_TEXT = "Transaction Complete\tClearing SIE Status";
        DEBUG_SHOW ("SIE", DEBUG_TEXT);

        usb_wait_for_buffer_completion_host_to_pico(EP_NUMBER, true);

    }

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

void usb_wait_for_host_ack(uint8_t EP_NUMBER) {

    uint64_t wait_duration = 0;
    volatile bool wait_timeout;  
    volatile bool ack_received;
    
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(100000);
   
    do { 

       // busy_wait_at_least_cycles(8);

        wait_timeout = time_reached(wait_time_end);

        //ack_received = usb_hw->sie_status & USB_SIE_STATUS_TRANS_COMPLETE_BITS;

        ack_received = usb_dpram->ep_buf_ctrl[EP_NUMBER].out & USB_BUF_CTRL_AVAIL;
       
    } while (!wait_timeout && !ack_received);

    
    wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());


    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tTransaction Completion Wait Timeout, Duration=%lld µs";
        DEBUG_SHOW ("USB", DEBUG_TEXT , wait_duration);


    } else {

        DEBUG_TEXT = "Transaction Complete\tWait Duration=%lld µs";
        DEBUG_SHOW ("SIE", DEBUG_TEXT , wait_duration);

    }

}

void set_device_address(uint8_t DEVICE_ADDRESS) {

    busy_wait_ms(1);

    usb_hardware_set->dev_addr_ctrl = DEVICE_ADDRESS & 0x7f;

}

uint8_t get_device_address() {

    return usb_hw->dev_addr_ctrl & 0x7f;

}

volatile bool get_ep0_buffer_status() {

    return usb_hardware_set->sie_ctrl & USB_SIE_CTRL_EP0_INT_1BUF_BITS;
}

void set_ep0_buffer_status(bool enable_interrupts) {

    if (enable_interrupts) {

        usb_hardware_set->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS;

    } else {

        usb_hardware_clear->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS;

    }

    // 0x20000000, set bit in BUFF_STATUS for every EP0 buffer completion
}

void set_transaction_complete_interrupts(bool enable_interrupts) {

    if (enable_interrupts) {

        usb_hardware_set->sie_ctrl = USB_SIE_STATUS_TRANS_COMPLETE_BITS;

    } else {

        usb_hardware_clear->sie_ctrl = USB_SIE_STATUS_TRANS_COMPLETE_BITS;

    }

}