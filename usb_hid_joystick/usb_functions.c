
#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_transmit.h"
#include "include/usb_receive.h"
#include "include/usb_common.h"
#include "include/usb_functions.h"
#include "include/usb_endpoints.h"
#include "include/show_registers.h"

#include "hardware/regs/usb.h"              
#include "hardware/structs/usb.h"       

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void  __not_in_flash_func (ep_handler_to_host_ep0)(uint8_t EP_NUMBER) {

    DEBUG_TEXT = "Buffer Status Handler \tStarting Completion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, EP_NUMBER);

    if (host_endpoint[0].async_mode) {

        DEBUG_TEXT = "Buffer Status Handler \tAsync Mode=True, Async Bytes=%d";
        DEBUG_SHOW ("IRQ", DEBUG_TEXT, host_endpoint[0].async_bytes );

        if (host_endpoint[0].async_bytes) {

            usb_wait_for_buffer_completion_pico_to_host(0, false);

            send_async_packet(0);

           // usb_wait_for_buffer_completion_pico_to_host(0, true);

        } else {

            DEBUG_TEXT = "Buffer Status Handler \tTransfer Complete, Async Bytes=%d";
            DEBUG_SHOW ("IRQ", DEBUG_TEXT, host_endpoint[0].async_bytes );

            receive_status_transaction_from_host(0, true);

            host_endpoint[0].transfer_complete = true;
       
            //busy_wait_ms(10000);
    }

    } else {

        DEBUG_TEXT = "Buffer Status Handler \tAsync Mode=False, Waiting for Buffer";
        DEBUG_SHOW ("IRQ", DEBUG_TEXT);

        usb_wait_for_buffer_completion_pico_to_host(0, true);
    }
 

    usb_hardware_clear->buf_status = USB_BUFF_STATUS_EP0_IN_BITS;

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status, Bit Mask=%08x";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, USB_BUFF_STATUS_EP0_IN_BITS);

}

void  __not_in_flash_func (ep_handler_to_host_ep1)(uint8_t EP_NUMBER) {
 
    if (host_endpoint[1].async_bytes) {

        send_async_packet(1);

    } else {

        usb_wait_for_buffer_completion_pico_to_host(1, true);
    }

    usb_hardware_clear->buf_status = USB_BUFF_STATUS_EP1_IN_BITS;

    DEBUG_TEXT = "Buffer Status Handler \tCompletion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, EP_NUMBER);

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status, Bit Mask=%08x";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, USB_BUFF_STATUS_EP1_IN_BITS);

}