/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


void usb_get_descriptor();

void usb_setup_device_respond_to_host();

void usb_setup_device_request_to_pico();

static inline void error_unknown_descriptor_type(uint8_t descriptor_type);

static inline uint8_t *config_dt_to_string(uint8_t descriptor_type);
