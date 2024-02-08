/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//#include "pico/stdlib.h"

void ep0_handler_to_host (uint8_t EP_NUMBER);

void ep0_handler_to_pico (uint8_t EP_NUMBER);

static inline void ep0_handler_to_host_async();

void init_ep0_handler_critical_section();
