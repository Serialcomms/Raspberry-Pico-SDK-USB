/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

void usb_setup_default_endpoint_0();

void usb_setup_function_endpoints();

static inline void setup_pico_endpoint_0(void *completion_handler_address);

static inline void setup_host_endpoint_0(void *completion_handler_address);
