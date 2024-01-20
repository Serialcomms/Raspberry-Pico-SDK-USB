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

} __packed;

typedef struct usb_setup_command usb_setup_t;
 
void usb_handle_setup_packet();

static inline void usb_setup_device();

static inline void usb_setup_interface();

static inline void prepare_setup_packet();

static inline void usb_setup_unknown(uint8_t recipient);
