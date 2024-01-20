/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
     
#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_receive.h"
#include "include/usb_transmit.h"
#include "include/usb_descriptors.h"
#include "include/setup_device.h"
#include "include/pico_device.h"

extern uint8_t *DEBUG_TEXT;

void set_pico_device_address(uint16_t DEVICE_ADDRESS) {

  send_ack_handshake_to_host(0, true); 

  busy_wait_ms(1);

  usb_hardware_set->dev_addr_ctrl = DEVICE_ADDRESS & 0x7f;

  DEBUG_TEXT = "Setup Device Handler\tDevice Address Changed, Pico New Address=%d";
  DEBUG_SHOW ("DEV", get_device_address());

  pico_usb_device.ADDRESS_SET = true;

}

void send_device_descriptor_to_host(uint16_t request_packet_size) { 

  pico_usb_device.DEVICE_DESCRIPTOR_SENT = false;

  uint8_t *device_descriptor = pico_device_descriptor;
  uint8_t  descriptor_length = pico_device_descriptor[0];
  uint8_t  descriptor_bytes = get_device_address() ? descriptor_length : 8; 
  // send first 8 descriptor bytes only before device is addressed

  DEBUG_TEXT = "Pico Device Descriptor\tSend to Host, Packet Size=%d, Bytes=%d/%d ";
  DEBUG_SHOW ("EP0", ep0_packet_size(), descriptor_bytes, descriptor_length); 
  
  synchronous_transfer_to_host(0, device_descriptor, descriptor_bytes);

  receive_status_transaction_from_host(0, true);

  wait_for_transaction_completion(true);
  
  pico_usb_device.DEVICE_DESCRIPTOR_SENT = true;

}