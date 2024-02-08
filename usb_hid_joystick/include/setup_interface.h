/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

static void send_hid_descriptors_to_host();

void usb_setup_interface_request_to_pico();

void usb_setup_interface_respond_to_host();

//extern uint8_t pico_device_descriptor[];
//extern uint8_t pico_config_descriptor[];
extern uint8_t pico_hid_report_descriptor[];
