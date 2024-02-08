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

extern struct pico_profile pico;

void set_pico_device_address(uint16_t DEVICE_ADDRESS) {

  busy_wait_ms(1);

  usb_hardware_set->dev_addr_ctrl = DEVICE_ADDRESS & 0x7f;

  DEBUG_TEXT = "Setup Device Handler\tDevice Address Changed, Pico New Address=%d";
  DEBUG_SHOW ("DEV", 1, get_device_address());

  pico.usb.device.address_set = device_address_is_set();

}

void send_device_descriptor_to_host(uint16_t request_size) { 

  uint8_t  descriptor_bytes = 8;
  uint8_t *device_descriptor = pico_device_descriptor;
  uint8_t  descriptor_length = pico_device_descriptor[0];
 
  if (device_address_is_set()) {   

    descriptor_bytes = descriptor_length; 
    
    synchronous_transfer_to_host(0, device_descriptor, descriptor_bytes);

    ack_pico_to_host_control_transfer();

    pico.usb.descriptors.device = true;
  

  } else {  
    
    // host wants first 8 bytes only to determine EP0 max_packet_size
  
    build_ep0_data_packet(device_descriptor, descriptor_bytes);
    
    send_data_packet(0, descriptor_bytes, false, false);

    pico.usb.descriptors.device = false;

  }

  DEBUG_TEXT = "USB Device Descriptor\tSend to Host, Packet Size=%d, Bytes=%d/%d ";
  DEBUG_SHOW ("EP0", 1, ep0_packet_size(), descriptor_bytes, descriptor_length); 
  
}