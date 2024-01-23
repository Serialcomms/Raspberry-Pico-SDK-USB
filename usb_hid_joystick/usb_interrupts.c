
/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "pico/types.h"
#include "include/usb_debug.h"
#include "include/usb_reset.h"
#include "include/setup_packet.h"
#include "include/setup_device.h"
#include "include/show_registers.h"
#include "include/usb_interrupts.h"
#include "include/usb_endpoints.h"
#include "include/ep0_handlers.h"
#include "include/epx_handlers.h"
#include "include/usb_common.h"

#include "hardware/irq.h"                 

extern uint8_t *DEBUG_TEXT;

void usb_handle_buffer_status_host (uint8_t EP_NUMBER) {

    void *handler_address = host_endpoint[EP_NUMBER].completion_handler;

    buffer_completion_handler buffer_completion_function = handler_address;

    host_endpoint[EP_NUMBER].buffer_complete = true;

    if (handler_address) {

        buffer_completion_function(EP_NUMBER);

    } else {

        buffer_completion_default(EP_NUMBER, 0);
    }

}

void usb_handle_buffer_status_pico (uint8_t EP_NUMBER) {

    void *handler_address = pico_endpoint[EP_NUMBER].completion_handler;

    buffer_completion_handler buffer_completion_function = handler_address;

    pico_endpoint[EP_NUMBER].buffer_complete = true;

    if (handler_address) {

        buffer_completion_function(EP_NUMBER);

    } else {

        buffer_completion_default(EP_NUMBER, 1);
    }
}

void buffer_completion_default (uint8_t EP_NUMBER, uint8_t direction_bit) {

    uint32_t buffer_status_bit = 1 << ((2 * EP_NUMBER) + direction_bit);

    usb_hardware_clear->buf_status = buffer_status_bit;
   
    DEBUG_TEXT = "Buffer Status Handler \tNo Completion Handler for Endpoint Number=%d";
    DEBUG_SHOW ("IRQ", EP_NUMBER);

    DEBUG_TEXT = "Buffer Status Handler \tCleared Buffer Status Bit=%08x";
    DEBUG_SHOW ("IRQ", buffer_status_bit);

}

void usb_handle_buffer_status() {

    uint8_t  endpoint_buffer_number = 0;
    io_rw_32 buffer_status_register = usb_hw->buf_status;
    uint32_t endpoint_buffer_status = buffer_status_register;
   
    do {

        DEBUG_TEXT = "Buffer Status Handler \tProcessing Endpoint Number=%d, IRQ=%s";
        DEBUG_SHOW ("IRQ", endpoint_buffer_number, endpoint_irq_pending(endpoint_buffer_status));

        if (endpoint_buffer_status & 0x1) {

            usb_handle_buffer_status_host(endpoint_buffer_number);
        }

        if (endpoint_buffer_status & 0x2) {

            usb_handle_buffer_status_pico(endpoint_buffer_number);
        }

        endpoint_buffer_status >>= 2;

    } while (++endpoint_buffer_number < 16 && endpoint_buffer_status);

}


void isr_usbctrl() {           // USB interrupt handler IRQ5
   
    uint32_t IRQ_HANDLED = 0;
    io_rw_32 IRQ_STATUS = usb_hw->ints;
    io_rw_32 SIE_STATUS = usb_hw->sie_status;
    io_rw_32 BUF_STATUS = usb_hw->buf_status;
    //io_rw_32 BUF_HANDLE = usb_hw->buf_cpu_should_handle;

    if (IRQ_STATUS & USB_INTS_BUS_RESET_BITS) {          

        DEBUG_TEXT = "USB BUS RESET \t\tUSB Bus Reset command received from host";
        DEBUG_SHOW ("IRQ");
   
        usb_hardware_clear->sie_status = USB_SIE_STATUS_BUS_RESET_BITS;

        IRQ_HANDLED |= USB_INTS_BUS_RESET_BITS;

        usb_bus_reset();

    }
   
    if (IRQ_STATUS & USB_INTS_SETUP_REQ_BITS) {

        DEBUG_TEXT = "USB Device Setup\tSetup command received from host";
        DEBUG_SHOW ("IRQ");

        usb_hardware_clear->sie_status = USB_SIE_STATUS_SETUP_REC_BITS;

        IRQ_HANDLED |= USB_INTS_SETUP_REQ_BITS;
        
        usb_handle_setup_packet();

    }
    
    if (IRQ_STATUS & USB_INTS_BUFF_STATUS_BITS) { 

        DEBUG_TEXT = "USB Buffer Completion\tBuffer Status Register=%08x";
        DEBUG_SHOW ("IRQ", BUF_STATUS);
        
        IRQ_HANDLED |= USB_INTS_BUFF_STATUS_BITS;

        usb_handle_buffer_status();
     
    }

    if (IRQ_STATUS & USB_INTS_TRANS_COMPLETE_BITS) {

        DEBUG_TEXT = "USB Transaction Completion\tSIE Register=%08x";
        DEBUG_SHOW ("IRQ", SIE_STATUS);

        usb_hardware_clear->sie_status = USB_INTS_TRANS_COMPLETE_BITS;
        
        IRQ_HANDLED |= USB_INTS_TRANS_COMPLETE_BITS;

    }

    if (IRQ_STATUS ^ IRQ_HANDLED) {

        DEBUG_TEXT = "Unhandled Interrupt\t IRQ 0x%08x";
        DEBUG_SHOW ("IRQ", (uint) (IRQ_STATUS ^ IRQ_HANDLED));
        busy_wait_ms(1000);
            
    }
}

uint8_t *endpoint_irq_pending (uint32_t buffer_status) {

    return buffer_status & 0x03 ? "TRUE" : "FALSE" ;   
}

