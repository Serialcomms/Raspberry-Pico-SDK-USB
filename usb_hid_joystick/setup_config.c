
#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_receive.h"
#include "include/usb_transmit.h"
#include "include/usb_descriptors.h"
#include "include/pico_device.h"
#include "include/setup_config.h"

extern uint8_t *DEBUG_TEXT;

void send_device_configuration_to_host(uint16_t command_length) { 

  pico_usb_device.DEVICE_CONFIGURATION_SENT = false;

  uint8_t  config_length = pico_config_descriptor[0];
  uint8_t *config_descriptor = pico_config_descriptor;
  bool     full_descriptor = (command_length > config_total_length());
  uint16_t descriptor_bytes = full_descriptor ? config_length : command_length;

  DEBUG_TEXT = "Pico Device Config \tConfiguration Descriptor, Sending %d/%d Bytes";
  DEBUG_SHOW ("EP0", descriptor_bytes, config_total_length());

  synchronous_transfer_to_host(0, config_descriptor, descriptor_bytes);

  DEBUG_TEXT = "Device Configuration \tReceive Status Transaction ACK from host";
  DEBUG_SHOW ("EP0");

  receive_status_transaction_from_host(0, true);

  wait_for_transaction_completion(true);

  //if (full_descriptor) 
  
  pico_usb_device.DEVICE_CONFIGURATION_SENT = true;


}