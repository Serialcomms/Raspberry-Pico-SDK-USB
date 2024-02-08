/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct usb_joystick_profile {

    bool hid_report;
    bool set_idle;

};

struct cdc_line_encoding_profile {

    uint32_t baud;
    uint8_t format; // stop bits
    uint8_t parity; 
    uint8_t data;        

} __packed ;

struct cdc_line_control_profile {

    union {

        uint32_t line_status;
        bool dsr : 1;
        bool dcd : 1;

        };

};

struct usb_serial_profile {

    struct cdc_line_control_profile control;
    
    struct cdc_line_encoding_profile framing;
    
} ;

struct usb_functions_profile {

    struct usb_serial_profile cdc;
    struct usb_joystick_profile joystick;

} __packed ;

struct usb_descriptors_profile {

    bool device;
    bool config;

};

struct transaction_profile {

    bool done;
    uint32_t wait;
    uint16_t stage;
    uint32_t duration;
    absolute_time_t time;

};

struct usb_device_profile {

    bool address_set;

    bool cdc_state;

    uint8_t control_transfer_stage;

    struct transaction_profile last_transaction; 

};

struct usb_profile {

    struct usb_device_profile       device;
    struct usb_functions_profile    functions;
    struct usb_descriptors_profile  descriptors;

};

struct adc_profile {

    uint8_t channel[3];
};

struct pin_profile {

    uint8_t number[12];

};

struct pico_profile {

    struct usb_profile usb;
    
};

void reset_usb_device();

bool last_transaction_done();

bool usb_device_enumerated();

void show_device_enumerated();

void set_binary_declarations();
