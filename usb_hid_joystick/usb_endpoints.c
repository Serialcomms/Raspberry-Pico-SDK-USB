
/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "include/usb_endpoints.h"

//struct endpoint_profile __not_in_flash("endpoints") flash_endpoint[1];

//struct endpoint_profile __not_in_flash("endpoints") host_endpoint[15];
//struct endpoint_profile __not_in_flash("endpoints") pico_endpoint[15];

struct endpoint_profile host_endpoint[15];
struct endpoint_profile pico_endpoint[15];

