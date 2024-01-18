
#include "pico/stdlib.h"

#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_sie_errors.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

extern uint8_t *DEBUG_TEXT;

void usb_wait_for_buffer_completion(uint8_t EP_NUMBER, uint32_t buffer_mask, bool buffer_status_clear) {

    volatile bool sie_errors;
    volatile bool wait_timeout;
    
    volatile uint32_t buffer_status;
    volatile uint32_t buffer_complete;
   
    uint64_t wait_duration = 0;
    
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(100000);

    buffer_status = usb_hw->buf_status;

    DEBUG_TEXT = "Serial Interface Engine\tBuffer Wait,\tMask=%08X, Register=%08X";
    DEBUG_SHOW ("SIE", wait_duration, buffer_mask, buffer_status);
    
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
        DEBUG_TEXT = "Serial Interface Engine\tWaited %lld µs for Buffer\tMask=%08X,\t Register=%08X";
        DEBUG_SHOW ("SIE", wait_duration, buffer_mask, buffer_status);
       
    }

    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tBuffer Completion Wait Timeout, Duration=%lld µs";
        DEBUG_SHOW ("TIM", wait_duration);

        DEBUG_TEXT = "Wait Timeout Error\tBuffer Mask=%08X,\t Buffer Status=%08X";
        DEBUG_SHOW ("TIM", buffer_mask, buffer_status);

        DEBUG_TEXT = "Wait Timeout Error\tSIE Error Status=%08X";
        DEBUG_SHOW ("TIM", sie_errors);


    } else {

        if (buffer_status_clear) {

            DEBUG_TEXT = "Buffer Status\t\tClearing,\tMask=%08X, Register=%08X";
            DEBUG_SHOW ("USB", DEBUG_TEXT , buffer_mask, buffer_status);

            usb_hardware_clear->buf_status = buffer_mask;

            buffer_status = usb_hw->buf_status;

            DEBUG_TEXT = "Buffer Status\t\tCleared, \tMask=%08X, Register=%08X";
            DEBUG_SHOW ("USB", buffer_mask, buffer_status);

        }

        DEBUG_TEXT = "Buffer Status\t\tComplete, \tBuffer Status Wait Duration=%lldµs";
        DEBUG_SHOW ("USB", wait_duration);

    }
    
}

void wait_for_transaction_completion(bool clear_transaction) {

    volatile bool sie_errors;
    volatile bool wait_timeout;
    volatile bool transaction_complete;
    
    volatile uint32_t buffer_status;
   
    uint64_t wait_duration = 0;
    uint32_t wait_microseconds = 100000;
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_us(wait_microseconds);

    transaction_complete = usb_hw->sie_status & USB_SIE_STATUS_TRANS_COMPLETE_BITS;

    DEBUG_TEXT = "Serial Interface Engine\tWaiting Max. %d µs for Transaction Completion";
    DEBUG_SHOW ("SIE", wait_microseconds);
    
    do { 

        busy_wait_at_least_cycles(8);

        sie_errors = check_sie_errors();

        wait_timeout = time_reached(wait_time_end);

        transaction_complete = usb_hw->sie_status & USB_SIE_STATUS_TRANS_COMPLETE_BITS;

    } while (!sie_errors && !wait_timeout && !transaction_complete );

    wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());

    if (sie_errors) {

        DEBUG_TEXT = "Serial Interface Engine\tSIE Error Status=%08X";
        DEBUG_SHOW ("SIE", sie_errors);
        
        sie_status_error_handler();

    }

    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tTransaction Completion Wait Timeout, Duration=%lld µs";
        DEBUG_SHOW ("TIM", wait_duration);

    } else {

        if (clear_transaction) {

            usb_hardware_clear->sie_status = USB_SIE_STATUS_TRANS_COMPLETE_BITS;

            DEBUG_TEXT = "Serial Interface Engine\tCleared Transaction Complete Bits";
            DEBUG_SHOW ("SIE");

        }

        DEBUG_TEXT = "Serial Interface Engine\tTransaction Complete, Wait Duration=%lldµs";
        DEBUG_SHOW ("USB", wait_duration);

    }
    
}


void __not_in_flash_func (clear_buffer_status)(uint32_t buffer_status_bits) {

    usb_hardware_clear->buf_status = buffer_status_bits;

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status, Bit Mask=%08x";
    DEBUG_SHOW ("IRQ", buffer_status_bits);

}

uint32_t toggle_data_pid(uint32_t data_pid) {

    uint32_t new_pid = data_pid ? USB_BUF_CTRL_DATA0_PID : USB_BUF_CTRL_DATA1_PID;

    DEBUG_TEXT = "USB Data Transfer \tToggling Old PID=%d > New PID=%d";
    DEBUG_SHOW ("USB", data_pid/8192, new_pid/8192);

    return new_pid;

}

uint8_t get_device_address() {

    return usb_hw->dev_addr_ctrl & 0x7f;

}

void set_ep0_buffer_status(bool enable_interrupts) {

    if (enable_interrupts) {

        usb_hardware_set->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS;

    } else {

        usb_hardware_clear->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS;

    }

    // 0x20000000, set bit in BUFF_STATUS for every EP0 buffer completion
}

volatile uint16_t __not_in_flash_func (get_buffer_bytes_to_host)(uint8_t EP_NUMBER) {

   return (uint16_t) usb_dpram->ep_buf_ctrl[EP_NUMBER].in & 0x01FF;
}

volatile uint16_t __not_in_flash_func (get_buffer_bytes_to_pico)(uint8_t EP_NUMBER) {

   return (uint16_t) usb_dpram->ep_buf_ctrl[EP_NUMBER].out & 0x01FF;
}

