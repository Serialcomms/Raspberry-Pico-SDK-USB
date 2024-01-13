#include "pico/stdlib.h"

#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/sie_errors.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

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

uint32_t toggle_data_pid(uint32_t data_pid) {

    uint32_t new_pid = data_pid ? USB_BUF_CTRL_DATA0_PID : USB_BUF_CTRL_DATA1_PID;

    DEBUG_TEXT = "USB Data Transfer \tToggling Old PID=%d > New PID=%d";
    DEBUG_SHOW ("USB", DEBUG_TEXT , data_pid/8192, new_pid/8192);

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