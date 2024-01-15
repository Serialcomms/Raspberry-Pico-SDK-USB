#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_receive.h"
#include "include/usb_transmit.h"
#include "include/usb_endpoints.h"
#include "include/show_registers.h"
#include "include/ep0_handlers.h"
     
static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void  __not_in_flash_func (ep0_handler_to_host_async)() {

    DEBUG_TEXT = "Buffer Status Handler \tAsync Mode=True, Async Bytes Remaining=%d";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, host_endpoint[0].async_bytes_pending);

    host_endpoint[0].bytes_transferred += get_buffer_bytes_to_host(0);

    if (host_endpoint[0].async_bytes_pending) {

            clear_buffer_status(USB_BUFF_STATUS_EP0_IN_BITS); // re-check

            send_async_packet(0);

    } else {

            DEBUG_TEXT = "Buffer Status Handler \tTransfer Complete, Async Bytes Transfered=%d";
            DEBUG_SHOW ("IRQ", DEBUG_TEXT, host_endpoint[0].bytes_transferred);

            receive_status_transaction_from_host(0, true);

            host_endpoint[0].transfer_complete = true;

    }
}

void  __not_in_flash_func (ep0_handler_to_host)(uint8_t EP_NUMBER) {

    DEBUG_TEXT = "Buffer Status Handler \tStarting Completion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, EP_NUMBER);

    if (host_endpoint[0].async_mode) {

        ep0_handler_to_host_async;
            
    } else {

        DEBUG_TEXT = "Buffer Status Handler \tAsync Mode=False, Clearing Buffer Status";
        DEBUG_SHOW ("IRQ", DEBUG_TEXT);

        clear_buffer_status(USB_BUFF_STATUS_EP0_IN_BITS);

    }
 
}

