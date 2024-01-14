
#include "pico/stdlib.h"
#include "hardware/divider.h"               
#include "hardware/regs/usb.h"             
#include "hardware/structs/usb.h"  
#include "include/usb_debug.h"
#include "include/sie_errors.h"
#include "include/usb_common.h"  
#include "include/usb_receive.h"
#include "include/usb_endpoints.h"
#include "include/show_registers.h"

//#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
//#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void receive_status_transaction_from_host(uint8_t EP_NUMBER, bool clear_buffer_status) {

    // bit 0 = EP0_IN
    // bit 1 = EP0_OUT
    // bit 2 = EP1_IN
    // bit 3 = EP1_OUT

    uint32_t buffer_status = 0;
    uint32_t buffer_control = 0;
    uint32_t buffer_dispatch = 0;
    uint16_t ZER0_LENGTH_PACKET = 0;
    
    uint8_t shift_left_bits = (2 * EP_NUMBER) + 1;

    uint32_t buffer_status_mask = 1 << shift_left_bits;
    
    buffer_dispatch = ZER0_LENGTH_PACKET | USB_BUF_CTRL_DATA1_PID;      // PID1 expected from host
    
    usb_dpram->ep_buf_ctrl[EP_NUMBER].out = buffer_dispatch;

    busy_wait_at_least_cycles(3);

    usb_dpram->ep_buf_ctrl[EP_NUMBER].out = buffer_dispatch | USB_BUF_CTRL_AVAIL;
    
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

void usb_start_transfer_host_to_pico(uint8_t EP_NUMBER, uint16_t buffer_length) {

    uint32_t buffer_dispatch;

    uint32_t DATA_PID = pico_endpoint[EP_NUMBER].packet_id;

    pico_endpoint[EP_NUMBER].packet_id = toggle_data_pid(DATA_PID);

    buffer_dispatch = DATA_PID | USB_BUF_CTRL_AVAIL;

    usb_dpram->ep_buf_ctrl[EP_NUMBER].out = buffer_dispatch;

    DEBUG_TEXT = "USB Data Transfer \tHost to Pico, PID=%08X, Length=%d";
    DEBUG_SHOW ("USB", DEBUG_TEXT, DATA_PID, buffer_length);

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

void usb_wait_for_buffer_completion_host_to_pico(uint8_t EP_NUMBER, bool buffer_status_clear) {

    uint32_t buffer_mask = (1 << (EP_NUMBER * 2u) + 1); 

    usb_wait_for_buffer_completion(EP_NUMBER, buffer_mask, buffer_status_clear);
    
}
