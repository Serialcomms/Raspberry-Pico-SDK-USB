/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "hardware/regs/usb.h"             
#include "hardware/structs/usb.h"           
#include "include/time_stamp.h"
#include "include/usb_endpoints.h"
#include "include/show_registers.h"

void show_usb_registers() {

volatile uint32_t DPRAM_EP0_BUF_CRTL_IN     = usb_dpram->ep_buf_ctrl[0].in;
volatile uint32_t DPRAM_EP0_BUF_CRTL_OUT    = usb_dpram->ep_buf_ctrl[0].out;  
volatile uint32_t USBHW_SIE_CONTROL         = usb_hw->sie_ctrl;
volatile uint32_t USBHW_SIE_STATUS          = usb_hw->sie_status;
volatile uint32_t USBHW_BUFFER_STATUS       = usb_hw->buf_status;
volatile uint32_t USBHW_INTERRUPT_RAW       = usb_hw->intr;
volatile uint32_t USBHW_INTERRUPT_STATUS    = usb_hw->ints;
volatile uint32_t USBHW_INTERRUPT_ENABLED   = usb_hw->inte;   
volatile uint32_t USBHW_EP_NAK_STALL        = usb_hw->ep_nak_stall_status;

TIMESTAMP(); printf("EP0:  Buffer Control     \tEP0 In       \t   Register = %08X\n", DPRAM_EP0_BUF_CRTL_IN);
TIMESTAMP(); printf("EP0:  Buffer Control     \tEP0 Out      \t   Register = %08X\n", DPRAM_EP0_BUF_CRTL_OUT);  
TIMESTAMP(); printf("SIE:  Serial Interface   \tSIE Control  \t   Register = %08X\n", USBHW_SIE_CONTROL);
TIMESTAMP(); printf("SIE:  Serial Interface   \tBuffer Status\t   Register = %08X\n", USBHW_BUFFER_STATUS);
TIMESTAMP(); printf("USB:  Interrupts raw     \tINTR         \t   Register = %08X\n", USBHW_INTERRUPT_RAW);
TIMESTAMP(); printf("USB:  Interrupts enabled \tINTE         \t   Register = %08X\n", USBHW_INTERRUPT_ENABLED);
TIMESTAMP(); printf("USB:  Interrupts status  \tINTS         \t   Register = %08X\n", USBHW_INTERRUPT_STATUS);
TIMESTAMP(); printf("USB:  EP Status Stall    \tNAK          \t   Register = %08X\n", USBHW_EP_NAK_STALL);

}

void show_dpram_to_host(uint8_t EP_NUMBER, uint8_t max_bytes) {

    show_dpram(EP_NUMBER, max_bytes, host_endpoint[EP_NUMBER].dpram_address);

}

void show_dpram_to_pico(uint8_t EP_NUMBER, uint8_t max_bytes) {

    show_dpram(EP_NUMBER, max_bytes, pico_endpoint[EP_NUMBER].dpram_address);
}

void show_dpram(uint8_t EP_NUMBER, uint8_t max_bytes, uint8_t *usb_dpram_data) {

if (max_bytes > 0) {

    TIMESTAMP(); printf("USB:  USB DPRAM EP%d Bytes \t01-08 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", EP_NUMBER, 
    usb_dpram_data[0x00], usb_dpram_data[0x01], usb_dpram_data[0x02], usb_dpram_data[0x03], 
    usb_dpram_data[0x04], usb_dpram_data[0x05], usb_dpram_data[0x06], usb_dpram_data[0x07]);
    }

if (max_bytes > 8) {

    TIMESTAMP(); printf("USB:  USB DPRAM EP%d Bytes \t09-16 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", EP_NUMBER, 
    usb_dpram_data[0x08], usb_dpram_data[0x09], usb_dpram_data[0x0a], usb_dpram_data[0x0b], 
    usb_dpram_data[0x0c], usb_dpram_data[0x0d], usb_dpram_data[0x0e], usb_dpram_data[0x0f]);    
    }

if (max_bytes > 16) {

    TIMESTAMP(); printf("USB:  USB DPRAM EP%d Bytes \t17-24 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", EP_NUMBER, 
    usb_dpram_data[0x10], usb_dpram_data[0x11], usb_dpram_data[0x12], usb_dpram_data[0x13], 
    usb_dpram_data[0x14], usb_dpram_data[0x15], usb_dpram_data[0x16], usb_dpram_data[0x17]);
    }

if (max_bytes > 24) {

    TIMESTAMP(); printf("USB:  USB DPRAM EP%d Bytes \t25-32 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", EP_NUMBER, 
    usb_dpram_data[0x18], usb_dpram_data[0x19], usb_dpram_data[0x1a], usb_dpram_data[0x1b], 
    usb_dpram_data[0x1c], usb_dpram_data[0x1d], usb_dpram_data[0x1e], usb_dpram_data[0x1f]);
    }

if (max_bytes > 32) {

    TIMESTAMP(); printf("USB:  USB DPRAM EP%d Bytes \t33-40 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", EP_NUMBER, 
    usb_dpram_data[0x20], usb_dpram_data[0x21], usb_dpram_data[0x22], usb_dpram_data[0x23], 
    usb_dpram_data[0x24], usb_dpram_data[0x25], usb_dpram_data[0x26], usb_dpram_data[0x27]);
    }

if (max_bytes > 40) {

    TIMESTAMP(); printf("USB:  USB DPRAM EP%d Bytes \t41-48 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", EP_NUMBER, 
    usb_dpram_data[0x28], usb_dpram_data[0x29], usb_dpram_data[0x2a], usb_dpram_data[0x2b], 
    usb_dpram_data[0x2c], usb_dpram_data[0x2d], usb_dpram_data[0x2e], usb_dpram_data[0x2f]);
    }


if (max_bytes > 48) {

    TIMESTAMP(); printf("USB:  USB DPRAM EP%d Bytes \t49-56 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", EP_NUMBER, 
    usb_dpram_data[0x30], usb_dpram_data[0x31], usb_dpram_data[0x32], usb_dpram_data[0x33], 
    usb_dpram_data[0x34], usb_dpram_data[0x35], usb_dpram_data[0x36], usb_dpram_data[0x37]);
    }


if (max_bytes > 56) {

    TIMESTAMP(); printf("USB:  USB DPRAM EP%d Bytes \t57-64 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", EP_NUMBER, 
    usb_dpram_data[0x38], usb_dpram_data[0x39], usb_dpram_data[0x3a], usb_dpram_data[0x3b], 
    usb_dpram_data[0x3c], usb_dpram_data[0x3d], usb_dpram_data[0x3e], usb_dpram_data[0x3f]);
    }

}
  