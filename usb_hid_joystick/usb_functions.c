
#include <stdio.h>
#include <string.h>                         
#include "include/usb_debug.h"
#include "include/usb_functions.h"
#include "include/usb_transfer.h"
#include "include/usb_endpoints.h"
#include "include/show_registers.h"

#include "hardware/regs/usb.h"              
#include "hardware/structs/usb.h"       

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void __not_in_flash_func (send_async_packet)(uint8_t EP_NUMBER) {

    uint8_t offset = 0;
    uint8_t *dpram_buffer = host_endpoint[EP_NUMBER].dpram_address;
    uint8_t *source_buffer = host_endpoint[EP_NUMBER].source_buffer_address;
    uint16_t async_bytes = host_endpoint[EP_NUMBER].async_bytes;
    uint16_t transfer_bytes = host_endpoint[EP_NUMBER].transfer_bytes;
    uint8_t  ep_packet_size = host_endpoint[EP_NUMBER].packet_size;
    uint8_t  async_packet_size = MIN(host_endpoint[EP_NUMBER].packet_size, async_bytes);
    uint16_t source_buffer_offset = host_endpoint[EP_NUMBER].source_buffer_offset;

    bool last_packet = (async_packet_size <= ep_packet_size) ? true : false;

    do {  

        dpram_buffer[offset] = source_buffer[source_buffer_offset + offset];

    } while (++offset < async_packet_size);

    host_endpoint[EP_NUMBER].async_bytes -= offset;
    host_endpoint[EP_NUMBER].source_buffer_offset += offset;

    send_data_packet(EP_NUMBER, async_packet_size, false, true, last_packet);

}

void  __not_in_flash_func (ep_handler_to_host_ep1)(uint8_t EP_NUMBER) {
 
    if (host_endpoint[1].async_bytes) {

        send_async_packet(1);

    } else {

        usb_wait_for_transaction_completion(1, false);
    }

    usb_hardware_clear->buf_status = USB_BUFF_STATUS_EP1_IN_BITS;

    DEBUG_TEXT = "Buffer Status Handler \tNew Completion Handler for Endpoint Number=%d";
    DEBUG_SHOW (1, "IRQ", DEBUG_TEXT, EP_NUMBER);

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status, Bit Mask=%08x";
    DEBUG_SHOW (1, "IRQ", DEBUG_TEXT, USB_BUFF_STATUS_EP1_IN_BITS);

}