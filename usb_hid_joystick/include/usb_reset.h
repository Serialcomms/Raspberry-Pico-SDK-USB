/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

void usb_bus_reset();

void reset_endpoints();

void enable_setup_interrupts();

void disable_setup_interrupts();

void clear_sie_status_register();

void clear_buffer_status_register();
