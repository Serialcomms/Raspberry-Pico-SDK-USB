
#include "pico/stdlib.h"
#include "pico/types.h"
#include "include/usb_debug.h"
#include "include/usb_reset.h"
#include "include/setup_packet.h"
#include "include/setup_device.h"
#include "include/show_registers.h"
#include "include/usb_interrupts.h"
#include "include/usb_endpoints.h"
#include "include/usb_functions.h"

#include "hardware/irq.h"                   // For interrupt enable and numbers
#include "hardware/resets.h"                // For resetting the USB controller
#include "hardware/regs/usb.h"              // USB register definitions from pico-sdk
#include "hardware/structs/usb.h"           // USB hardware struct definitions from pico-sdk

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

void  __not_in_flash_func (usb_handle_buffer_status_host)(uint8_t EP_NUMBER) {

    void *handler_address = host_endpoint[EP_NUMBER].completion_handler;     

    buffer_completion_handler buffer_completion_function = handler_address;

    if (handler_address) {

        buffer_completion_function(EP_NUMBER);

    } else {

        buffer_completion_default(EP_NUMBER, 0);
    }

}

void __not_in_flash_func (usb_handle_buffer_status_pico)(uint8_t EP_NUMBER) {

    void *handler_address = pico_endpoint[EP_NUMBER].completion_handler;

    buffer_completion_handler buffer_completion_function = handler_address;

    if (handler_address) {

        buffer_completion_function(EP_NUMBER);

    } else {

        buffer_completion_default(EP_NUMBER, 1);
    }
}

void __not_in_flash_func (buffer_completion_default)(uint8_t EP_NUMBER, uint8_t direction_bit) {

    uint32_t buffer_status_bit = 1 << ((2 * EP_NUMBER) + direction_bit);

    usb_hardware_clear->buf_status = buffer_status_bit;
   
    DEBUG_TEXT = "Buffer Status Handler \tNo Completion Handler for Endpoint Number=%d";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, EP_NUMBER);

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status Bit=%08x";
    DEBUG_SHOW ("IRQ", DEBUG_TEXT, buffer_status_bit);

}

void __not_in_flash_func (usb_handle_buffer_status()) {

    uint8_t  endpoint_buffer_number = 0;
    io_rw_32 buffer_status_register = usb_hw->buf_status;
    uint32_t endpoint_buffer_status = buffer_status_register;
   
    do {

        DEBUG_TEXT = "Buffer Status Handler \tProcessing Endpoint Number=%d, IRQ=%s";
        DEBUG_SHOW ("IRQ", DEBUG_TEXT, endpoint_buffer_number, endpoint_irq_pending(endpoint_buffer_status));

        if (endpoint_buffer_status & 0x1) {

            usb_handle_buffer_status_host(endpoint_buffer_number);
        }

        if (endpoint_buffer_status & 0x2) {

            usb_handle_buffer_status_pico(endpoint_buffer_number);
        }

        endpoint_buffer_status >>= 2;

    } while (++endpoint_buffer_number < 16 && endpoint_buffer_status);

}

void __not_in_flash_func (isr_usbctrl()) {           // USB interrupt handler IRQ5
   
    uint32_t IRQ_HANDLED = 0;
    io_rw_32 IRQ_STATUS = usb_hw->ints;
    io_rw_32 SIE_STATUS = usb_hw->sie_status;
    io_rw_32 BUF_STATUS = usb_hw->buf_status;
    io_rw_32 BUF_HANDLE = usb_hw->buf_cpu_should_handle;

    if (IRQ_STATUS & USB_INTS_BUS_RESET_BITS) {          

        DEBUG_TEXT = "USB BUS RESET \t\tUSB Bus Reset command received from host";
        DEBUG_SHOW ("IRQ", DEBUG_TEXT);
   
        usb_hardware_clear->sie_status = USB_SIE_STATUS_BUS_RESET_BITS;

        IRQ_HANDLED |= USB_INTS_BUS_RESET_BITS;

        usb_bus_reset();

    }
   
    if (IRQ_STATUS & USB_INTS_SETUP_REQ_BITS) {

        DEBUG_TEXT = "USB Device Setup\tSetup command received from host";
        DEBUG_SHOW ("IRQ", DEBUG_TEXT);

        usb_hardware_clear->sie_status = USB_SIE_STATUS_SETUP_REC_BITS;

        IRQ_HANDLED |= USB_INTS_SETUP_REQ_BITS;
        
        usb_handle_setup_packet();

    }
    
    if (IRQ_STATUS & USB_INTS_BUFF_STATUS_BITS) { 

        DEBUG_TEXT = "USB Buffer Completion\tBuffer Status Register=%08x";
        DEBUG_SHOW ("IRQ", DEBUG_TEXT, BUF_STATUS);
        
        IRQ_HANDLED |= USB_INTS_BUFF_STATUS_BITS;

        usb_handle_buffer_status();
     
    }

     if (IRQ_STATUS & USB_INTS_TRANS_COMPLETE_BITS) {

        DEBUG_TEXT = "USB Protocol\t\tTransaction Complete, Register = %08X";
        DEBUG_SHOW ("IRQ", DEBUG_TEXT , SIE_STATUS);

        usb_hardware_clear->sie_status = USB_SIE_STATUS_TRANS_COMPLETE_BITS;

        IRQ_HANDLED |= USB_INTS_TRANS_COMPLETE_BITS;

        // to do

    }

    if (IRQ_STATUS ^  IRQ_HANDLED) {

        DEBUG_TEXT = "Unhandled IRQ 0x%08x";
        DEBUG_SHOW ("IRQ", DEBUG_TEXT , (uint) (IRQ_STATUS ^ IRQ_HANDLED));
            
    }
}

volatile uint8_t *endpoint_irq_pending(uint32_t buffer_status) {

    return buffer_status & 0x03 ? "TRUE" : "FALSE" ;   
}
