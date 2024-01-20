
/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "include/usb_endpoints.h"

bool valid_host_endpoint[15];
bool valid_pico_endpoint[15];

struct endpoint_profile host_endpoint[15];
struct endpoint_profile pico_endpoint[15];

