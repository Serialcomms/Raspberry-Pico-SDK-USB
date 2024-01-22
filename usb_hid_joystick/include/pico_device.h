/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


struct usb_descriptor_sent {

    bool DEVICE;
    bool CONFIG;
    bool HID_REPORT;

};

struct pico_device_profile {

    bool SET_ADDRESS;
    bool RECEIVED_HID_SET_IDLE;
    bool LAST_TRANSACTION_DONE;
    uint32_t LAST_TRANSACTION_WAIT;
    absolute_time_t LAST_TRANSACTION_TIME;
    struct usb_descriptor_sent DESCRIPTOR;

};

extern struct pico_device_profile pico_usb_joystick;

void reset_usb_device();

bool last_transaction_done();

bool usb_device_enumerated();

void show_device_enumerated();

void set_binary_declarations();
