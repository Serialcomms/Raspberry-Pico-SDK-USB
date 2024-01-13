#include <stdio.h>
#include <stdarg.h>
#include <string.h>                        
#include "pico/sync.h" 
#include "pico/stdlib.h"  

#include "include/usb_debug.h"
#include "include/time_stamp.h"

#include "hardware/divider.h"
#include "hardware/regs/usb.h"              
#include "hardware/structs/usb.h"  

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

uint8_t DEBUG_STRING_BUFFER[100];

static struct critical_section debug_critical_section;

void __not_in_flash_func(DEBUG_SHOW)(uint8_t *prefix_text, uint8_t *debug_text, ...) {
 
    critical_section_enter_blocking(&debug_critical_section);
  
    fflush(stdout);

    TIMESTAMP();

    printf("%s%s", prefix_text, ":\t");
    
    va_list args;

    va_start(args, debug_text);
   
    vprintf(debug_text, args);
   
    va_end(args);

    printf("\n");

    fflush(stdout);

    critical_section_exit(&debug_critical_section);
  
}

void init_debug_critical_section() {

    critical_section_init(&debug_critical_section);

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



volatile uint8_t *sie_status_any_error() {

    return usb_hw->sie_status & 0x8F000200 ? "TRUE" : "FALSE" ;
}

volatile uint8_t *sie_status_seq_error() {

    return usb_hw->sie_status & USB_SIE_STATUS_DATA_SEQ_ERROR_BITS ? "TRUE" : "FALSE" ;
}

volatile uint8_t *sie_status_ack_received() {

    return usb_hw->sie_status & USB_SIE_STATUS_ACK_REC_BITS ? "TRUE" : "FALSE" ;   
}

volatile uint8_t *sie_trans_complete() {

    return usb_hw->sie_status & USB_SIE_STATUS_TRANS_COMPLETE_MSB ? "TRUE" : "FALSE" ; 
}

volatile uint8_t *buffer_status_ep0_in() {

    return usb_hw->buf_status & (uint32_t)(1 << 0) ? "Y" : "N" ;  
}

volatile uint8_t *buffer_status_ep0_out() {

    return usb_hw->buf_status & (uint32_t)(1 << 1) ? "Y" : "N" ;  
}

volatile uint8_t *buffer_status_in(uint8_t EP) {

    return usb_hw->buf_status & (uint32_t)(1 << EP) ? "Y" : "N" ;
}

volatile uint8_t *buffer_status_out(uint8_t EP) {

    return usb_hw->buf_status & (uint32_t)(1 << EP + 1) ? "Y" : "N" ;  
}

volatile uint8_t *buffer_control_in_pid(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].in & (uint32_t)(1 << 13) ? "1" : "0" ;
}

volatile uint8_t *buffer_control_out_pid(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].out & (uint32_t)(1 << 13) ? "1" : "0" ; 
}

volatile uint8_t *buffer_control_in_full(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].in & USB_BUF_CTRL_LAST ? "Y" : "N" ;
}

volatile uint8_t *buffer_control_out_full(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].out & USB_BUF_CTRL_FULL ? "Y" : "N" ;
}

volatile uint8_t *buffer_control_in_last(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].in & USB_BUF_CTRL_LAST ? "Y" : "N" ;
}

volatile uint8_t *buffer_control_out_last(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].out & USB_BUF_CTRL_LAST ? "Y" : "N" ;
}

volatile uint8_t *buffer_control_in_avail(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].in & USB_BUF_CTRL_AVAIL ? "Y" : "N" ;
}

volatile uint8_t *buffer_control_out_avail(uint8_t EP) {

    return usb_dpram->ep_buf_ctrl[EP].out & USB_BUF_CTRL_AVAIL ? "Y" : "N" ;
}

void show_buffer_control(uint8_t EP) {

    TIMESTAMP(); printf("USB:\tPico Device Config \tSEQ Error=%s, ACK Recd.=%s, Complete=%s\n", sie_status_seq_error(),  sie_status_ack_received(), sie_trans_complete());
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Full     \t Out = %s, In = %s\n", EP, buffer_control_out_full(EP),  buffer_control_in_full(EP));
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Last     \t Out = %s, In = %s\n", EP, buffer_control_out_last(EP),  buffer_control_in_last(EP));
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Available\t Out = %s, In = %s\n", EP, buffer_control_out_avail(EP), buffer_control_in_avail(EP));
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Packet ID\t Out = %s, In = %s\n", EP, buffer_control_out_pid(EP),   buffer_control_in_pid(EP));
    TIMESTAMP(); printf("USB:\tPico Device Config \tEndpoint %d Buffer Complete \t Out = %s, In = %s\n",  0, buffer_status_ep0_out(),      buffer_status_ep0_in());

}
