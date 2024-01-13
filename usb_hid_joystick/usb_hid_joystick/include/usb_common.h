#include "hardware/regs/usb.h"             
#include "hardware/structs/usb.h" 

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

uint8_t get_device_address();
uint32_t toggle_data_pid(uint32_t data_pid);



