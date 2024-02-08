/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

void receive_zlp_from_host (uint8_t EP_NUMBER, bool expect_pid_1, bool wait_buffer_complete);

//void receive_status_transaction_from_host(uint8_t EP_NUMBER, bool clear_buffer_status);

void usb_wait_for_buffer_completion_host_to_pico(uint8_t EP_NUMBER, bool buffer_status_clear);

uint16_t usb_start_transfer_host_to_pico(uint8_t EP_NUMBER, uint8_t *mini_buffer, uint16_t buffer_length);
