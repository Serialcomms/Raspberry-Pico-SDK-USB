/**
 * Copyright (c) 2024 SerialComms
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
             
#include "include/usb_debug.h"
#include "include/setup_device.h"
#include "include/usb_transfer.h"
#include "include/usb_descriptors.h"

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void set_pico_device_address(uint16_t DEVICE_ADDRESS) {

  send_ack_handshake_to_host(0, true); 
  
  set_device_address(DEVICE_ADDRESS);

  DEBUG_TEXT = "Setup Device Handler\tDevice Address Changed, Pico New Address=%d";
  DEBUG_SHOW ("DEV", DEBUG_TEXT, get_device_address());

}

void send_device_descriptor_to_host(uint16_t request_packet_size) { 

  uint8_t *device_descriptor = pico_device_descriptor;
  uint8_t  descriptor_length = pico_device_descriptor[0];
  uint8_t  descriptor_bytes = get_device_address() ? descriptor_length : 8; 
  // send first 8 descriptor bytes only before device is addressed

  DEBUG_TEXT = "Pico Device Descriptor\tSend to Host, Packet Size=%d, Bytes=%d/%d ";
  DEBUG_SHOW ("EP0", DEBUG_TEXT, ep0_packet_size(), descriptor_bytes, descriptor_length); 
  
  synchronous_transfer_to_host(0, device_descriptor, descriptor_bytes);

  receive_status_transaction_from_host(0, true);

}