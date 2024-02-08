/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

extern struct pico_profile pico;

void wait_for_device_enumeration();

static inline void device_enumeration_pass();

static inline void device_enumeration_fail();
