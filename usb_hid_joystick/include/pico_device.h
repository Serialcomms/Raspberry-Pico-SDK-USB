/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct usb_joystick_profile {

    bool hid_report;
    bool set_idle;

};

struct usb_functions_profile {

  struct usb_joystick_profile   joystick;

};

struct usb_descriptors_profile {

    bool device;
    bool config;

};

struct transaction_profile {

    bool done;
    uint32_t wait;
    uint32_t duration;
    absolute_time_t time;

};

struct usb_device_profile {

    bool address_set;

    struct transaction_profile last_transaction; 

};

struct usb_profile {

    struct usb_device_profile       device;
    struct usb_functions_profile    functions;
    struct usb_descriptors_profile  descriptors;

};

struct pico_usb_profile {

    struct usb_profile usb;
    
};


extern struct pico_usb_profile pico;

void reset_usb_device();

bool last_transaction_done();

bool usb_device_enumerated();

void show_device_enumerated();

void set_binary_declarations();
