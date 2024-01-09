
#include <stdio.h>
//#include <string.h>                         // For memcpy
#include "include/usb_reset.h"
#include "include/usb_debug.h"
//#include "include/time_stamp.h"
#include "include/setup_device.h"
#include "include/setup_packet.h"
#include "include/show_registers.h"
#include "include/usb_endpoints.h"
#include "include/usb_transfer.h"
#include "include/usb_descriptors.h"
#include "hardware/resets.h"                // For resetting the USB controller
#include "hardware/regs/usb.h"              // USB register definitions from pico-sdk
#include "hardware/structs/usb.h"           // USB hardware struct definitions from pico-sdk

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

const io_rw_32 CLEAR_ALL_BITS = 0xFFFFFFFF; 

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void usb_bus_reset() { 

  uint8_t DEVICE_ADDRESS = get_device_address();

  DEBUG_TEXT = "Bus Reset \t\tProcesssing USB Bus Reset,  Pico Device Address=%d ";
  DEBUG_SHOW   (1, "USB", DEBUG_TEXT, DEVICE_ADDRESS);

  reset_endpoint_pids();

  clear_sie_status_register();
  
  clear_buffer_status_register();
  
  if (!DEVICE_ADDRESS) {

    enable_setup_interrupts();

  }

  busy_wait_ms(2);

  receive_status_transaction_from_host(0, true);

}

void reset_endpoint_pids() {

  uint8_t ep_number;

    do {

      host_endpoint[ep_number].packet_id = 0x0000;
      pico_endpoint[ep_number].packet_id = 0x0000;

    } while (++ep_number < 16);

}

void enable_setup_interrupts() {

  usb_hardware_set->inte = USB_INTS_SETUP_REQ_BITS ;
  usb_hardware_set->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS; // 0x20000000, set bit in BUFF_STATUS for every EP0 buffer completion

  DEBUG_TEXT = "Bus Reset \t\tEnabling Setup Interrupt,\tRegister=%08X";
  DEBUG_SHOW (1, "USB", DEBUG_TEXT, usb_hw->inte);

}

void clear_sie_status_register() {

  usb_hardware_clear->sie_status = USB_SIE_STATUS_BUS_RESET_BITS;
  usb_hardware_clear->sie_status = USB_SIE_STATUS_DATA_SEQ_ERROR_BITS;
  usb_hardware_clear->sie_status = USB_SIE_STATUS_ACK_REC_BITS;
  usb_hardware_clear->sie_status = USB_SIE_STATUS_RX_TIMEOUT_BITS;
  usb_hardware_clear->sie_status = USB_SIE_STATUS_RX_OVERFLOW_BITS;
  usb_hardware_clear->sie_status = USB_SIE_STATUS_BIT_STUFF_ERROR_BITS;
  usb_hardware_clear->sie_status = USB_SIE_STATUS_CRC_ERROR_BITS;
  usb_hardware_clear->sie_status = USB_SIE_STATUS_TRANS_COMPLETE_BITS;
  usb_hardware_clear->sie_status = USB_SIE_STATUS_SETUP_REC_BITS;
  usb_hardware_clear->sie_status = USB_SIE_STATUS_SUSPENDED_BITS;
  
}

void clear_buffer_status_register() {

  usb_hardware_clear->buf_status = CLEAR_ALL_BITS;  

}