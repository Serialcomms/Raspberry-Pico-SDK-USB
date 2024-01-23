/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//void __not_in_flash_func (usb_handle_buffer_status_host)(uint8_t EP_NUMBER);
//void __not_in_flash_func (usb_handle_buffer_status_pico)(uint8_t EP_NUMBER);
//void __not_in_flash_func (buffer_completion_default)(uint8_t EP_NUMBER, uint8_t direction_bit);

void usb_handle_buffer_status_host (uint8_t EP_NUMBER);
void usb_handle_buffer_status_pico (uint8_t EP_NUMBER);
void buffer_completion_default (uint8_t EP_NUMBER, uint8_t direction_bit);


//uint8_t __not_in_flash_func (*endpoint_irq_pending)(uint32_t buffer_status);

uint8_t *endpoint_irq_pending (uint32_t buffer_status);