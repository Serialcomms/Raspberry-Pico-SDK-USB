/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "include/usb_reset.h"
#include "include/usb_debug.h"
#include "include/setup_device.h"
#include "include/show_registers.h"
#include "include/setup_packet.h"
#include "include/usb_protocol.h"
#include "include/usb_endpoints.h"
#include "include/usb_transmit.h"
#include "include/usb_receive.h"
#include "include/usb_common.h"
#include "include/pico_device.h"

#include "include/usb_descriptors.h"
#include "hardware/resets.h"

extern uint8_t *DEBUG_TEXT;

const io_rw_32 CLEAR_ALL_BITS = 0xFFFFFFFF; 

void usb_bus_reset() { 

  uint8_t DEVICE_ADDRESS = get_device_address();

  DEBUG_TEXT = "Bus Reset \t\tServicing USB Bus Reset, Pico Device Address=%d ";
  DEBUG_SHOW   ("USB", 1, DEVICE_ADDRESS);

  reset_endpoints();

  reset_usb_device();

  clear_sie_status_register();
  
  clear_buffer_status_register();
  
  if (device_address_is_set()) {

    disable_setup_interrupts();

    set_ep0_buffer_status(false);

  } else {

    enable_setup_interrupts();

  }

  busy_wait_ms(2);

}

void reset_endpoints() {

  uint8_t ep_number;

    do {

      host_endpoint[ep_number].async_bytes_pending = 0;
      pico_endpoint[ep_number].async_bytes_pending = 0;
      
      host_endpoint[ep_number].async_mode = false;
      pico_endpoint[ep_number].async_mode = false;

      host_endpoint[ep_number].double_buffered = false;
      pico_endpoint[ep_number].double_buffered = false;

      host_endpoint[ep_number].transaction_complete = false;
      pico_endpoint[ep_number].transaction_complete = false;

      host_endpoint[ep_number].transfer_bytes = 0;
      pico_endpoint[ep_number].transfer_bytes = 0;

      host_endpoint[ep_number].transaction_duration = 0;
      pico_endpoint[ep_number].transaction_duration = 0;

      host_endpoint[ep_number].start_time_now = nil_time;
      pico_endpoint[ep_number].start_time_now = nil_time;

      host_endpoint[ep_number].start_time_end = nil_time;
      pico_endpoint[ep_number].start_time_end = nil_time;

      host_endpoint[ep_number].last_packet_size = 0;
      pico_endpoint[ep_number].last_packet_size = 0;

      host_endpoint[ep_number].source_buffer_offset = 0;
      pico_endpoint[ep_number].source_buffer_offset = 0;

      host_endpoint[ep_number].packet_id = USB_BUF_CTRL_DATA0_PID;
      pico_endpoint[ep_number].packet_id = USB_BUF_CTRL_DATA0_PID;

    } while (++ep_number < 16);

}

void enable_setup_interrupts() {

  usb_hardware_set->inte = USB_INTS_SETUP_REQ_BITS;
  
  DEBUG_TEXT = "Bus Reset \t\tEnabling Setup Interrupt Register=%08X";
  DEBUG_SHOW ("USB", 1, usb_hw->inte);

}

void disable_setup_interrupts() {

  usb_hardware_clear->inte = USB_INTS_SETUP_REQ_BITS;
  
  DEBUG_TEXT = "Bus Reset \t\tDisabling Setup Interrupt Register=%08X";
  DEBUG_SHOW ("USB", 1, usb_hw->inte);

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