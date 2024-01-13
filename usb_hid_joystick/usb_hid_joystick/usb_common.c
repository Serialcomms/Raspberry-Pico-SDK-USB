#include "include/usb_debug.h"
#include "include/usb_common.h"
 
static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

uint32_t toggle_data_pid(uint32_t data_pid) {

    uint32_t new_pid = data_pid ? USB_BUF_CTRL_DATA0_PID : USB_BUF_CTRL_DATA1_PID;

    DEBUG_TEXT = "USB Data Transfer \tToggling Old PID=%d > New PID=%d";
    DEBUG_SHOW ("USB", DEBUG_TEXT , data_pid/8192, new_pid/8192);

    return new_pid;

}


uint8_t get_device_address() {

    return usb_hw->dev_addr_ctrl & 0x7f;

}