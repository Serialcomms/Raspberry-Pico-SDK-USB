/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"                   
#include "hardware/irq.h"                   // Interrupt enable and numbers
#include "hardware/resets.h"                // USB controller reset
#include "hardware/regs/usb.h"              // USB register definitions
#include "hardware/structs/usb.h"           // USB hardware struct definitions
#include "include/usb_dev_init.h"
#include "include/usb_debug.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

extern uint8_t *DEBUG_TEXT;

void usb_device_init() {                                  
   
io_rw_32 SIE_REGISTER_VALUE = 0;
io_rw_32 IRQ_REGISTER_VALUE = 0;
io_rw_32 PWR_REGISTER_VALUE = 0;
io_rw_32 MUX_REGISTER_VALUE = 0;
io_rw_32 USB_REGISTER_VALUE = 0;
    
reset_block(RESETS_RESET_USBCTRL_BITS);

unreset_block_wait(RESETS_RESET_USBCTRL_BITS);

clear_usb_dpsram();
    
USB_REGISTER_VALUE |= USB_MAIN_CTRL_CONTROLLER_EN_BITS; // Enable Pico USB controller = device mode 
MUX_REGISTER_VALUE |= USB_USB_MUXING_TO_PHY_BITS; 
IRQ_REGISTER_VALUE |= USB_INTS_BUS_RESET_BITS;          // bus reset IRQ only until bus reset complete
SIE_REGISTER_VALUE |= USB_SIE_STATUS_DATA_SEQ_ERROR_BITS;
PWR_REGISTER_VALUE |= USB_USB_PWR_VBUS_DETECT_BITS;
PWR_REGISTER_VALUE |= USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_BITS; 
    
usb_hw->main_ctrl = USB_REGISTER_VALUE;   
usb_hw->sie_ctrl = SIE_REGISTER_VALUE;
usb_hw->muxing = MUX_REGISTER_VALUE;
usb_hw->inte = IRQ_REGISTER_VALUE;
usb_hw->pwr = PWR_REGISTER_VALUE;                       

busy_wait_ms(101);

//irq_set_enabled(USBCTRL_IRQ, true);                     // Enable USB interrupt at processor

}

void usb_insert_device() {

    io_rw_32 SIE_REGISTER_VALUE;

    SIE_REGISTER_VALUE = usb_hw->sie_ctrl;

    SIE_REGISTER_VALUE |= USB_SIE_CTRL_PULLUP_EN_BITS;

    usb_hw->sie_ctrl = SIE_REGISTER_VALUE;

    irq_set_enabled(USBCTRL_IRQ, true);                     // Enable USB interrupt at processor

}

void usb_remove_device() {

    io_rw_32 SIE_REGISTER_VALUE;

    SIE_REGISTER_VALUE = usb_hw->sie_ctrl;

    SIE_REGISTER_VALUE &= ~USB_SIE_CTRL_PULLUP_EN_BITS;

    usb_hw->sie_ctrl = SIE_REGISTER_VALUE;

    irq_set_enabled(USBCTRL_IRQ, false);                     // Enable USB interrupt at processor

}

void clear_usb_dpsram() {

    uint16_t offset = 0;
    void *dpram_address = usb_dpram;
    uint8_t *dpram_data = dpram_address;
    uint16_t dpram_size = sizeof(*usb_dpram);

    DEBUG_TEXT = "Device Initialisation\tDPRAM Start Address=%08X, Size=%d Bytes";
    DEBUG_SHOW ("USB", dpram_address, dpram_size);
   
    do {

        dpram_data[offset] = 0;

    } while (++offset < dpram_size);

}