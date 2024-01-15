#include "pico/stdlib.h" 
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/ep0_handlers.h"
#include "include/epx_handlers.h"
#include "include/usb_endpoints.h"
#include "include/usb_descriptors.h"
#include "include/setup_endpoints.h"
#include "include/setup_device.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

const uint16_t usb_dpram_size = sizeof(*usb_dpram);
const uint16_t usb_dpram_epx_size = usb_dpram_size - 128; // check

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

void setup_host_endpoint_0(void *completion_handler_address) {

  host_endpoint[0].async_mode = false;
  host_endpoint[0].async_bytes_pending = 0;
  host_endpoint[0].buffer_complete = false;
  pico_endpoint[0].transfer_complete = false;
  host_endpoint[0].transaction_complete = false;
  host_endpoint[0].packet_id = USB_BUF_CTRL_DATA0_PID;
  host_endpoint[0].max_packet_size = ep0_packet_size();
  host_endpoint[0].dpram_address = &usb_dpram->ep0_buf_a[0];
  host_endpoint[0].completion_handler = completion_handler_address;

}

void setup_pico_endpoint_0(void *completion_handler_address) {

  
  pico_endpoint[0].async_mode = false;
  pico_endpoint[0].async_bytes_pending = 0;
  pico_endpoint[0].buffer_complete = false;
  pico_endpoint[0].transfer_complete = false;
  pico_endpoint[0].transaction_complete = false;
  pico_endpoint[0].packet_id = USB_BUF_CTRL_DATA0_PID;
  pico_endpoint[0].max_packet_size = ep0_packet_size();
  pico_endpoint[0].dpram_address = &usb_dpram->ep0_buf_a[0];
  pico_endpoint[0].completion_handler = completion_handler_address;

}


void usb_setup_host_endpoint(uint8_t EP_NUMBER, uint16_t TRANSFER_TYPE, void *completion_handler_address) {

  uint8_t EP_OFFSET = EP_NUMBER - 1;

  valid_host_endpoint[EP_NUMBER] = true; 

  void *usb_dpram_address =  &usb_dpram->epx_data[(64 * 2 * EP_OFFSET) + 0x0000];  // first half of available dpram

  host_endpoint[EP_NUMBER].async_mode = false;
  host_endpoint[EP_NUMBER].async_bytes_pending = 0;
  host_endpoint[EP_NUMBER].buffer_complete = false;
  host_endpoint[EP_NUMBER].transfer_complete = false;
  host_endpoint[EP_NUMBER].transaction_complete = false;
  host_endpoint[EP_NUMBER].max_packet_size = 8;
  host_endpoint[EP_NUMBER].packet_id = USB_BUF_CTRL_DATA0_PID;
  host_endpoint[EP_NUMBER].dpram_address = usb_dpram_address;
  host_endpoint[EP_NUMBER].completion_handler = completion_handler_address;

  uint32_t address_base_offset = usb_buffer_offset(host_endpoint[EP_NUMBER].dpram_address); 
  uint32_t ep_control_register = endpoint_base_config(TRANSFER_TYPE, address_base_offset);

  usb_dpram->ep_ctrl[EP_OFFSET].in = ep_control_register;

  DEBUG_TEXT = "Configure Endpoint\tConfigure and Enable Endpoint %d to Host";
  DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, EP_NUMBER);

  DEBUG_TEXT = "Endpoint Control In\tRegister = %08X, Offset = %04X";
  DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, usb_dpram->ep_ctrl[EP_OFFSET].in, address_base_offset);

}

void usb_setup_pico_endpoint(uint8_t EP_NUMBER, uint16_t TRANSFER_TYPE, void *completion_handler_address) {

  uint8_t EP_OFFSET = EP_NUMBER - 1;

  valid_pico_endpoint[EP_NUMBER] = true; 

  void *usb_dpram_address = &usb_dpram->epx_data[(64 * 2 * EP_OFFSET) + 0x0780];  // second half of available dpram 6CC ??

  pico_endpoint[EP_NUMBER].async_mode = false;
  pico_endpoint[EP_NUMBER].async_bytes_pending = 0;
  pico_endpoint[EP_NUMBER].buffer_complete = false;
  pico_endpoint[EP_NUMBER].transfer_complete = false;
  pico_endpoint[EP_NUMBER].transaction_complete = false;
  pico_endpoint[EP_NUMBER].max_packet_size = 8;
  pico_endpoint[EP_NUMBER].packet_id = USB_BUF_CTRL_DATA0_PID;
  pico_endpoint[EP_NUMBER].dpram_address = usb_dpram_address;
  pico_endpoint[EP_NUMBER].completion_handler = completion_handler_address;

  uint32_t address_base_offset = usb_buffer_offset(pico_endpoint[EP_NUMBER].dpram_address); 
  uint32_t ep_control_register = endpoint_base_config(TRANSFER_TYPE, address_base_offset);

  usb_dpram->ep_ctrl[EP_OFFSET].out = ep_control_register;

  DEBUG_TEXT = "Configure Endpoint\tConfigure and Enable Endpoint %d to Pico";
  DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, EP_NUMBER);

  DEBUG_TEXT = "Endpoint Control Out\tRegister = %08X, Offset = %04X";
  DEBUG_SHOW (ep_text(EP_NUMBER), DEBUG_TEXT, usb_dpram->ep_ctrl[EP_OFFSET].out, address_base_offset);

}

// 3840 dspram bytes available for buffers
// = 64 * 2 bytes for each EP number > 0


void usb_setup_default_endpoint_0() {

  setup_host_endpoint_0(&ep0_handler_to_host);
  setup_pico_endpoint_0(NULL);

}

void usb_setup_function_endpoints() {

  usb_setup_host_endpoint(1, USB_TRANSFER_TYPE_INTERRUPT, &ep1_handler_to_host);

  usb_hw->inte |= USB_INTE_BUFF_STATUS_BITS;

  DEBUG_TEXT = "Pico Request Handler \tEnabling Buffer Status Completion IRQ=%08X";
  DEBUG_SHOW ("EPx", DEBUG_TEXT, usb_hw->inte);

}