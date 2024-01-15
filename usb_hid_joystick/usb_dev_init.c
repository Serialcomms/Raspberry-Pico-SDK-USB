
#include <string.h>                         // For memset
#include "pico/stdlib.h"                   
#include "hardware/irq.h"                   // For interrupt enable and numbers
#include "hardware/resets.h"                // For USB controller reset
#include "hardware/regs/usb.h"              // USB register definitions from pico-sdk
#include "hardware/structs/usb.h"           // USB hardware struct definitions from pico-sdk
#include "include/usb_dev_init.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

void usb_device_init() {                                  // Reset usb controller
   
io_rw_32 SIE_REGISTER_VALUE = 0;
io_rw_32 IRQ_REGISTER_VALUE = 0;
io_rw_32 PWR_REGISTER_VALUE = 0;
io_rw_32 MUX_REGISTER_VALUE = 0;
io_rw_32 USB_REGISTER_VALUE = 0;
    
reset_block(RESETS_RESET_USBCTRL_BITS);

unreset_block_wait(RESETS_RESET_USBCTRL_BITS);

memset(usb_dpram, 0, sizeof(*usb_dpram));               // Clear down dpram to known state
    
USB_REGISTER_VALUE |= USB_MAIN_CTRL_CONTROLLER_EN_BITS; // Enable the USB controller in device mode 
MUX_REGISTER_VALUE |= USB_USB_MUXING_TO_PHY_BITS;
MUX_REGISTER_VALUE |= USB_USB_MUXING_SOFTCON_BITS;
IRQ_REGISTER_VALUE |= USB_INTS_BUS_RESET_BITS;          // bus reset IRQ only until reset complete
//IRQ_REGISTER_VALUE |= USB_INTS_TRANS_COMPLETE_BITS;
SIE_REGISTER_VALUE |= USB_SIE_CTRL_PULLUP_EN_BITS;      // Present full speed device by enabling pull up on DP
SIE_REGISTER_VALUE |= USB_SIE_STATUS_DATA_SEQ_ERROR_BITS;
PWR_REGISTER_VALUE |= USB_USB_PWR_VBUS_DETECT_BITS;
PWR_REGISTER_VALUE |= USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_BITS; 
    
usb_hw->main_ctrl = USB_REGISTER_VALUE;   
usb_hw->sie_ctrl = SIE_REGISTER_VALUE;
usb_hw->muxing = MUX_REGISTER_VALUE;
usb_hw->inte = IRQ_REGISTER_VALUE;
usb_hw->pwr = PWR_REGISTER_VALUE;                       

busy_wait_ms(101);

irq_set_enabled(USBCTRL_IRQ, true);                     // Enable USB interrupt at processor

}
