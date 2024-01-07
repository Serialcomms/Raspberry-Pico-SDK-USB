#include <stdio.h>
#include "hardware/regs/usb.h"
#include "hardware/structs/usb.h"
#include "include/usb_debug.h"
#include "include/setup_device.h"
#include "include/usb_endpoints.h"
#include "include/usb_functions.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

uint32_t endpoint_packet_id_to_host[16]; 
uint32_t endpoint_packet_id_to_pico[16]; 

uint8_t *endpoint_data_buffer_to_host[16]; 
uint8_t *endpoint_data_buffer_to_pico[16]; 

buffer_completion_handler buffer_completion_handler_pico[16];
buffer_completion_handler buffer_completion_handler_host[16];

static inline uint32_t usb_buffer_offset(volatile uint8_t *buffer) {  // from pico-examples

  return (uint32_t) buffer ^ (uint32_t) usb_dpram;

}

static inline uint32_t endpoint_base_config(uint8_t TRANSFER_TYPE, uint32_t address_base) {

  uint32_t base_config = address_base;

  base_config |= EP_CTRL_ENABLE_BITS;
  base_config |= EP_CTRL_INTERRUPT_PER_BUFFER;
  base_config |= TRANSFER_TYPE << EP_CTRL_BUFFER_TYPE_LSB;

  return base_config;

}

void usb_setup_endpoint_to_host(uint8_t EP_NUMBER, uint16_t TRANSFER_TYPE, void *handler_address) {

  uint8_t *data_buffer_address;
  uint32_t control_register = 0;
  uint32_t address_base_offset = 0;
  uint8_t  EP_OFFSET = EP_NUMBER - 1;

  data_buffer_address = &usb_dpram->epx_data[64 * 2 * EP_OFFSET];

  endpoint_packet_id_to_host[EP_NUMBER] = USB_BUF_CTRL_DATA0_PID;

  endpoint_data_buffer_to_host[EP_NUMBER] = data_buffer_address;

  buffer_completion_handler_host[EP_NUMBER] = handler_address;   

  address_base_offset = usb_buffer_offset(data_buffer_address); 

  control_register = endpoint_base_config(TRANSFER_TYPE, address_base_offset);

  usb_dpram->ep_ctrl[EP_OFFSET].in = control_register;

  DEBUG_TEXT = "Configure Endpoint\tConfigure and Enable Endpoint %d to Host";
  DEBUG_SHOW (1, ep_text(EP_NUMBER), DEBUG_TEXT, EP_NUMBER);

  DEBUG_TEXT = "Endpoint Control In\tRegister = %08X, Offset = %04X";
  DEBUG_SHOW (1, ep_text(EP_NUMBER), DEBUG_TEXT, control_register, address_base_offset);

}

// 3840 dspram bytes available for buffers
// = 64 * 2 bytes for each EP number > 0

void usb_setup_endpoint_to_pico(uint8_t EP_NUMBER, uint16_t TRANSFER_TYPE, void *handler_address) {

  uint8_t *data_buffer_address;
  uint32_t control_register = 0;
  uint32_t address_base_offset = 0;
  uint8_t  EP_OFFSET = EP_NUMBER - 1;

  data_buffer_address = &usb_dpram->epx_data[(64 * 2 * EP_OFFSET) + 0x0780]; // second half of available dpram

  endpoint_packet_id_to_pico[EP_NUMBER] = USB_BUF_CTRL_DATA0_PID;

  endpoint_data_buffer_to_pico[EP_NUMBER] = data_buffer_address;

  buffer_completion_handler_pico[EP_NUMBER] = handler_address;  

  address_base_offset = usb_buffer_offset(data_buffer_address); 

  control_register = endpoint_base_config(TRANSFER_TYPE, address_base_offset);

  usb_dpram->ep_ctrl[EP_OFFSET].out = control_register;

  DEBUG_TEXT = "Configure Endpoint\tConfigure and Enable Endpoint %d to Pico";
  DEBUG_SHOW (1, ep_text(EP_NUMBER), DEBUG_TEXT, EP_NUMBER);

  DEBUG_TEXT = "Endpoint Control Out\tRegister = %08X, Offset = %04X";
  DEBUG_SHOW (1, ep_text(EP_NUMBER), DEBUG_TEXT, control_register, address_base_offset);


}

void usb_setup_endpoint_0() {

  endpoint_data_buffer_to_pico[0] = &usb_dpram->ep0_buf_a[0];
  endpoint_data_buffer_to_host[0] = &usb_dpram->ep0_buf_a[0];

}

void usb_setup_function_endpoints() {

  usb_setup_endpoint_to_host(1, USB_TRANSFER_TYPE_INTERRUPT, &ep_handler_to_host_ep1);

  usb_setup_endpoint_to_host(4, USB_TRANSFER_TYPE_INTERRUPT, NULL);

}