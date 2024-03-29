/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

uint8_t get_debug_threshold();

void init_debug_critical_section();

uint8_t *boolean_text(bool boolean_value);

volatile void show_ep0_buffers_complete();
volatile void show_buffer_control(uint8_t EP);

volatile uint8_t *sie_trans_complete();
volatile uint8_t *sie_status_any_error();
volatile uint8_t *sie_status_seq_error();
volatile uint8_t *sie_status_ack_received();

volatile uint8_t *buffer_status_ep0_in();
volatile uint8_t *buffer_status_ep0_out();

volatile uint8_t *buffer_status_complete_in(uint8_t EP);
volatile uint8_t *buffer_status_complete_out(uint8_t EP);

volatile uint8_t *buffer_control_out_pid(uint8_t EP);
volatile uint8_t *buffer_control_out_full(uint8_t EP);
volatile uint8_t *buffer_control_out_last(uint8_t EP);
volatile uint8_t *buffer_control_out_avail(uint8_t EP);
volatile uint16_t buffer_control_out_bytes(uint8_t EP);

volatile uint8_t *buffer_control_in_pid(uint8_t EP);
volatile uint8_t *buffer_control_in_full(uint8_t EP);
volatile uint8_t *buffer_control_in_last(uint8_t EP);
volatile uint8_t *buffer_control_in_avail(uint8_t EP);
volatile uint16_t buffer_control_in_bytes(uint8_t EP);

unsigned char *ep_text(uint8_t EP_NUMBER);

unsigned char *concatenate(uint8_t *string1, uint8_t *string2);

extern uint8_t DEBUG_STRING_BUFFER[100];

extern bool DEBUG_HIDE;

void __time_critical_func (DEBUG_SHOW)(uint8_t *prefix_text, uint8_t debug_level, ...);