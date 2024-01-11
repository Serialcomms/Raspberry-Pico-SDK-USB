
#include "include/usb_debug.h"
#include "include/usb_transfer.h"
#include "include/usb_functions.h"
#include "include/usb_endpoints.h"
#include "include/show_registers.h"

#include "hardware/regs/usb.h"              
#include "hardware/structs/usb.h"       

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void  __not_in_flash_func (ep_handler_to_host_ep1)(uint8_t EP_NUMBER) {
 
    if (host_endpoint[1].async_bytes) {

        send_async_packet(1);

    } else {

        usb_wait_for_transaction_completion(1, false);
    }

    usb_hardware_clear->buf_status = USB_BUFF_STATUS_EP1_IN_BITS;

    DEBUG_TEXT = "Buffer Status Handler \tCompletion Handler for Endpoint %d";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, EP_NUMBER);

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status, Bit Mask=%08x";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, USB_BUFF_STATUS_EP1_IN_BITS);

}