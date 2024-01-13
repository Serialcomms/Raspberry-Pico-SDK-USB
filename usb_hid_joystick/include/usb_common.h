#include "hardware/regs/usb.h"             
#include "hardware/structs/usb.h" 

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

uint8_t get_device_address();

volatile bool get_ep0_buffer_status();

uint32_t toggle_data_pid(uint32_t data_pid);

void set_ep0_buffer_status(bool enable_interrupts);

void usb_wait_for_buffer_completion(uint8_t EP_NUMBER, uint32_t buffer_mask, bool buffer_status_clear);

