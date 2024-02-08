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
#include "include/pico_device.h"
#include "include/setup_config.h"

extern uint8_t *DEBUG_TEXT;

extern struct pico_profile pico;

void send_device_configuration_to_host(uint16_t command_length) { 

  pico.usb.descriptors.config = false;

  uint8_t *config_descriptor = pico_config_descriptor;
  uint16_t descriptor_bytes = pico_config_descriptor[0];
  bool send_full_descriptor = (command_length == config_total_length());
  
  if (send_full_descriptor) descriptor_bytes = config_total_length();

  DEBUG_TEXT = "USB Device Config \tConfiguration Descriptor, Sending %d/%d Bytes";
  DEBUG_SHOW ("EP0", 1, descriptor_bytes, config_total_length());

  synchronous_transfer_to_host(0, config_descriptor, descriptor_bytes);

  DEBUG_TEXT = "Device Configuration \tReceive Status Transaction ACK from host";
  DEBUG_SHOW ("EP0", 1);

  ack_pico_to_host_control_transfer();

  pico.usb.descriptors.config = true;
 
}