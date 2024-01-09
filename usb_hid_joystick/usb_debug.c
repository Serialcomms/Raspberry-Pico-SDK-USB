#include <stdio.h>
#include <stdarg.h>
#include <string.h>                         // For memcpy
#include "pico/stdlib.h"                    // for printf
#include "pico/lock_core.h"                 

#include "include/usb_debug.h"
#include "include/time_stamp.h"

#include "hardware/divider.h"
#include "hardware/regs/usb.h"              // USB register definitions from pico-sdk
#include "hardware/structs/usb.h"           // USB hardware struct definitions from pico-sdk

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

uint8_t DEBUG_STRING_BUFFER[100];
static uint8_t *show_text = DEBUG_STRING_BUFFER;
static uint8_t string_pointer = 0;
static const uint8_t debug_threshold = 0;


void DEBUG_PRINT(uint8_t *debug_text, ...) {
   
   TIMESTAMP();
   
   va_list args;

   va_start(args, debug_text);
   
   vprintf(debug_text, args);
   
   va_end(args);

}

void  __not_in_flash_func (DEBUG_SHOW)(uint8_t debug_level, uint8_t *prefix_text, uint8_t *debug_text, ...) {

//wait_for_transmit_fifo_empty();

if (debug_level > debug_threshold) {

    TIMESTAMP();

    printf(prefix_text);
    
    printf(":\t");

    va_list args;

    va_start(args, debug_text);
   
    vprintf(debug_text, args);
   
    va_end(args);

    printf("\n");

    fflush(stdout);

    } else {

    va_list args;

    va_start(args, debug_text);

    va_copy(args, args);

    va_end(args);
    
    fflush(stdout);

      //  gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));

      //  printf("DBG:\t Debug Print\tDebug Threshold=%d, Level=%d\n", debug_threshold, debug_level);

    }
      
}

unsigned char *ep_text(uint8_t EP_NUMBER) {

static uint8_t ep_string[5];

if (16 - EP_NUMBER > 0) {

    snprintf(ep_string, 4, "EP%d", EP_NUMBER) ;

    } else {

    snprintf(ep_string, 3, "ERR") ;

    } 

return ep_string;  

}

unsigned char *concatenate(uint8_t *string1, uint8_t *string2) {

    static uint8_t strings[20];
    uint8_t string1_offset = 0;
    uint8_t string2_offset = 0;
    uint8_t strings_offset = 0;
    uint8_t string1_length = MIN(strlen(string1), 10);
    uint8_t string2_length = MIN(strlen(string2), 10);

    if (string1_length) {

        do {

            strings[strings_offset] = string1[string1_offset];

            ++ strings_offset; 
            ++ string1_offset;

        } while (--string1_length);

    }

    if (string2_length) {

        do {

            strings[strings_offset] = string2[string2_offset];

            ++ strings_offset; 
            ++ string2_offset;

        } while (--string2_length);

    }

    strings[++strings_offset] = 0;

    return strings;
}


unsigned char *sie_status_any_error() {

    return usb_hw->sie_status & 0x8F000200 ? "TRUE" : "FALSE" ;
}

unsigned char *sie_status_seq_error() {

    return usb_hw->sie_status & USB_SIE_STATUS_DATA_SEQ_ERROR_BITS ? "TRUE" : "FALSE" ;
}

unsigned char *sie_status_ack_received() {

    return usb_hw->sie_status & USB_SIE_STATUS_ACK_REC_BITS ? "TRUE" : "FALSE" ;   
}

unsigned char *sie_trans_complete() {

    return usb_hw->sie_status & USB_SIE_STATUS_TRANS_COMPLETE_MSB ? "TRUE" : "FALSE" ; 
}

unsigned char *buffer_status_ep0_in() {

    return usb_hw->buf_status & (uint32_t)(1 << 0) ? "Y" : "N" ;  
}

unsigned char *buffer_status_ep0_out() {

    return usb_hw->buf_status & (uint32_t)(1 << 1) ? "Y" : "N" ;  
}

unsigned char *buffer_status_in(uint8_t EP) {

    return usb_hw->buf_status & (uint32_t)(1 << EP) ? "Y" : "N" ;
}

unsigned char *buffer_status_out(uint8_t EP) {

    return usb_hw->buf_status & (uint32_t)(1 << EP + 1) ? "Y" : "N" ;  
}

unsigned char *buffer_control_in_pid(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].in & (uint32_t)(1 << 13) ? "1" : "0" ;
}

unsigned char *buffer_control_out_pid(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].out & (uint32_t)(1 << 13) ? "1" : "0" ; 
}

unsigned char *buffer_control_in_full(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].in & USB_BUF_CTRL_LAST ? "Y" : "N" ;
}

unsigned char *buffer_control_out_full(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].out & USB_BUF_CTRL_FULL ? "Y" : "N" ;
}

unsigned char *buffer_control_in_last(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].in & USB_BUF_CTRL_LAST ? "Y" : "N" ;
}

unsigned char *buffer_control_out_last(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].out & USB_BUF_CTRL_LAST ? "Y" : "N" ;
}

unsigned char *buffer_control_in_avail(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].in & USB_BUF_CTRL_AVAIL ? "Y" : "N" ;
}

unsigned char *buffer_control_out_avail(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].out & USB_BUF_CTRL_AVAIL ? "Y" : "N" ;
}

bool transmit_fifo_empty() {

    return uart_get_hw(uart0)->fr & 0x40;

}

void wait_for_transmit_fifo_empty() {

uint64_t wait_duration = 0;
volatile bool wait_timeout;
volatile bool transmit_fifo_empty;
absolute_time_t wait_time_now = get_absolute_time();
absolute_time_t wait_time_end = make_timeout_time_us(1000);
    
    do { 

        transmit_fifo_empty = uart_get_hw(uart0)->fr & 0x40;
   
        wait_timeout = time_reached(wait_time_end);

    } while (!transmit_fifo_empty && !wait_timeout);

}

void show_buffer_control(uint8_t EP) {

    TIMESTAMP(); printf("USB:\tPico Device Config \tSEQ Error=%s, ACK Recd.=%s, Complete=%s\n", sie_status_seq_error(),  sie_status_ack_received(), sie_trans_complete());
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Full     \t Out = %s, In = %s\n", EP, buffer_control_out_full(EP),  buffer_control_in_full(EP));
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Last     \t Out = %s, In = %s\n", EP, buffer_control_out_last(EP),  buffer_control_in_last(EP));
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Available\t Out = %s, In = %s\n", EP, buffer_control_out_avail(EP), buffer_control_in_avail(EP));
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Packet ID\t Out = %s, In = %s\n", EP, buffer_control_out_pid(EP),   buffer_control_in_pid(EP));
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Complete \t Out = %s, In = %s\n",  0, buffer_status_ep0_out(),      buffer_status_ep0_in());

}
