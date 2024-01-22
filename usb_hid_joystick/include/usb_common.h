/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hardware/regs/usb.h"             
#include "hardware/structs/usb.h" 

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

uint8_t get_device_address();

uint32_t toggle_data_pid(uint32_t data_pid);

void set_ep0_buffer_status(bool enable_interrupts);

void usb_wait_for_buffer_completion(uint8_t EP_NUMBER, uint32_t buffer_mask, bool buffer_status_clear);

void wait_for_transaction_completion(bool clear_transaction);

void build_ep0_data_packet(uint8_t *source_buffer, uint8_t transfer_bytes);

void __not_in_flash_func (clear_buffer_status)(uint32_t buffer_mask);

volatile uint16_t __not_in_flash_func (get_buffer_bytes_to_host)(uint8_t EP_NUMBER);
volatile uint16_t __not_in_flash_func (get_buffer_bytes_to_pico)(uint8_t EP_NUMBER);

uint8_t __not_in_flash_func (*last_packet_text)(bool last_packet);





