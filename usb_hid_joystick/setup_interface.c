/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_transmit.h"
#include "include/usb_receive.h"
#include "include/setup_interface.h"
#include "include/setup_packet.h"
#include "include/usb_descriptors.h"
#include "include/usb_endpoints.h"
#include "include/pico_device.h"

extern uint8_t *DEBUG_TEXT;

extern usb_setup_t *setup;

void usb_setup_interface_request_to_pico() {

DEBUG_TEXT = "Setup Interface \tRequest=%d, Type=%d";
DEBUG_SHOW ("HID", setup->request, setup->request_type);

switch (setup->request) {

    case 0x0A:

        DEBUG_TEXT = "Setup Interface \tSetting Idle, Request=%d";
        DEBUG_SHOW ("HID", setup->request);

        pico_usb_joystick.RECEIVED_HID_SET_IDLE = true;

    break;

    default:

        DEBUG_TEXT = "Setup Interface \tUnknown Request=%d";
        DEBUG_SHOW ("HID", setup->request);

        send_ack_handshake_to_host(0, true);

    break;

    }

}

void usb_setup_interface_respond_to_host() {

    DEBUG_TEXT = "Pico HID Interface \tSend HID Report Descriptor to Host";
    DEBUG_SHOW ("HID");

    send_hid_descriptors_to_host();

}

static void send_hid_descriptors_to_host() {

    const bool ASYNC_MODE_TRANSFER = true;

    uint16_t  report_length = hid_report_descriptor_size();
    uint8_t  *report_descriptor = pico_hid_report_descriptor;

    DEBUG_TEXT = "Pico HID Report \tSend HID Descriptor to Host,\tTotal Bytes=%d";
    DEBUG_SHOW ("HID", report_length);

    if (ASYNC_MODE_TRANSFER) { // aysnc mode test

        start_async_transfer_to_host(0, report_descriptor, report_length);

        pico_usb_joystick.DESCRIPTOR.HID_REPORT = true;

    } else {

        synchronous_transfer_to_host(0, report_descriptor, report_length);

        receive_status_transaction_from_host(0, true);

        wait_for_transaction_completion(true);

        pico_usb_joystick.DESCRIPTOR.HID_REPORT = true;

    }

}
