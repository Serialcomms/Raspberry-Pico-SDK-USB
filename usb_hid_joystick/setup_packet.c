/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "include/pico_device.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_protocol.h"
#include "include/usb_receive.h"
#include "include/usb_transmit.h"
#include "include/usb_endpoints.h"
#include "include/usb_interrupts.h"
#include "include/usb_sie_errors.h"
#include "include/setup_interface.h"
#include "include/setup_packet.h"
#include "hardware/regs/usb.h"
#include "hardware/structs/usb.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

extern uint8_t *DEBUG_TEXT;

extern struct pico_profile pico;

usb_setup_t setup_command;

usb_setup_t *setup = &setup_command;

uint8_t request_type_index = 0;
uint8_t request_recipient_index = 0;

uint8_t *request_type[]         =  { "Standard", "Class", "Vendor", "Reserved", "Unknown" };
uint8_t *request_action[]       =  { "Respond to Host", "Request to Pico", "Unknown" };
uint8_t *request_direction[]    =  { "Pico > Host", "Host > Pico", "Unknown" };
uint8_t *request_recipient[]    =  { "Device", "Interface", "Endpoint", "Other", "Unknown" };

volatile uint8_t *setup_packet = usb_dpram->setup_packet;

static inline void prepare_setup_packet() {

    setup->request = setup_packet[1];
    setup->direction = setup_packet[0] & 0x80;
    setup->recipient = setup_packet[0] & 0x1F;
    setup->descriptor_type = setup_packet[3];
    setup->descriptor_index = setup_packet[2];
    setup->request_type = (setup_packet[0] & 0x60) >> 5;

    setup->value =  setup_packet[3] << 8 | setup_packet[2];
    setup->index =  setup_packet[5] << 8 | setup_packet[4];
    setup->length = setup_packet[7] << 8 | setup_packet[6];

    pico.usb.device.control_transfer_stage = 1;

}

void usb_handle_setup_packet() {

    prepare_setup_packet();

    show_setup_details();

    switch (setup->recipient) {

        case 0:     usb_setup_device();     break;      // device
        case 1:     usb_setup_interface();  break;      // interface
        case 2:     usb_setup_unknown();    break;      // endpoint
        case 3:     usb_setup_unknown();    break;      // other
        default:    usb_setup_unknown();    break;
    } 

}

static inline void usb_setup_device() {

    if (setup->direction) {

        usb_protocol_setup_device_respond_to_host();

    } else {

        usb_protocol_setup_device_request_to_pico();

    }
}

static inline void usb_setup_interface() {

    switch(setup->request_type) {

        case 0:     setup_interface_standard();     break;        // standard
        case 1:     setup_interface_class();        break;        // class
        default:    usb_setup_unknown();            break;                

    }
}

static inline void setup_interface_standard() {

    if (setup->direction) {

        usb_setup_interface_respond_to_host();

    } else {

        usb_setup_interface_request_to_pico();

    }

}

static inline void setup_interface_class() {

    if (setup->direction) {

      //  usb_setup_class_respond_to_host();

    } else {

     //   usb_setup_class_request_to_pico();

    }

}

static inline void usb_setup_unknown() {

    DEBUG_TEXT = "Setup Packet Handler\tUnhandled device setup request, Recipient=%d";
    DEBUG_SHOW ("ERR", 9, setup->recipient);

}

static inline void show_recipient_decode() {

    switch (setup->recipient) {

        case 0:     show_device_decode();       break;
        case 1:     show_interface_decode();    break;
        case 2:                                 break;  // Endpoint                         
        default:                                break;  // other/reserved
    }

}

static inline void show_device_decode() {

    switch (setup->request) {

        case 2:

            DEBUG_TEXT = "Setup Packet Handler\tGet Interface Number=%d, Request Type=%s";
            DEBUG_SHOW ("GET", 2, setup->index, request_type[request_type_index]); 
                    
        break;

        case 5:

            DEBUG_TEXT = "Setup Packet Handler\tSetup Configure Device Address=%d";
            DEBUG_SHOW ("GET", 1, setup->value);

        break;            

        case 6:

            DEBUG_TEXT = "Setup Packet Handler\tSetup Get Descriptor Type=%d, Index=%d";
            DEBUG_SHOW ("GET", 2, setup->descriptor_type, setup->descriptor_index);

        break;

        case 8:

            DEBUG_TEXT = "Setup Packet Handler\tGet Configuration";
            DEBUG_SHOW ("GET", 1);
        
        break;   

        default: // other / reserved

            DEBUG_TEXT = "Setup Packet Handler\tUnhandled Device Request Type=%d";
            DEBUG_SHOW ("GET", 9, setup->request_type); 

        break;

    }         

}

static inline void show_interface_decode() {

    DEBUG_TEXT = "Setup Packet Handler\tSetup Control Stage=%d, Type=%s, Interface=%d ";
    DEBUG_SHOW ("GET", 2,  pico.usb.device.control_transfer_stage, request_type[request_type_index], setup->index);

}

static inline void show_setup_details() {

    request_type_index = MIN(setup->request_type, count_of(request_type));
    request_recipient_index = MIN(setup->recipient, count_of(request_recipient));

    DEBUG_TEXT = "Setup Packet Handler\tSetup Bytes = %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X";
    DEBUG_SHOW ("SET", 4,
    setup_packet[0], setup_packet[1], setup_packet[2], setup_packet[3], 
    setup_packet[4], setup_packet[5], setup_packet[6], setup_packet[7]);

    DEBUG_TEXT = "Setup Packet Handler\tSetup %s, Request=%d, Length=%d";
    DEBUG_SHOW ("SET", 4, setup->direction ? request_action[0] : request_action[1], setup->request, setup->length);

    DEBUG_TEXT = "Setup Packet Handler\tSetup Recipient=%s, Type=%s";
    DEBUG_SHOW ("SET", 4, request_recipient[request_recipient_index], request_type[request_type_index]);

    show_recipient_decode();

    show_ep0_buffers_complete();

}