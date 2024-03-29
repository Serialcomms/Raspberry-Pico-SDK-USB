/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct usb_setup_command {

    uint8_t descriptor_type;
    uint8_t descriptor_index;
    uint8_t request_type;
    uint8_t direction;
    uint8_t recipient;
    uint8_t request;
    uint16_t length;
    uint16_t value;
    uint16_t index;
    uint16_t stage;

} __packed;

typedef struct usb_setup_command usb_setup_t;
 
void usb_handle_setup_packet();

static inline void usb_setup_device();

static inline void usb_setup_interface();

static inline void setup_interface_class();

static inline void setup_interface_standard();

static inline void prepare_setup_packet();

static inline void usb_setup_unknown();

static inline void show_setup_details();

static inline void show_device_decode();

static inline void show_interface_decode();

static inline void show_recipient_decode();

static inline void show_descriptor_decode();
