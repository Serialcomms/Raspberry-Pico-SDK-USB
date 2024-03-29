/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/setup_device.h"
#include "include/setup_config.h"
#include "include/setup_strings.h"
#include "include/show_registers.h"
#include "include/usb_transmit.h"
#include "include/usb_receive.h"
#include "include/usb_common.h"
#include "include/usb_protocol.h"
#include "include/setup_endpoints.h"
#include "include/usb_descriptors.h"
#include "include/setup_packet.h"

extern uint8_t *DEBUG_TEXT;

extern usb_setup_t *setup;

void usb_get_descriptor() {

    static uint16_t get_descriptor_count;

    DEBUG_TEXT = "Pico Request Handler \tGET DESCRIPTOR TYPE=%s, Count=%d";
    DEBUG_SHOW ("USB", 2, config_dt_to_string(setup->descriptor_type), ++get_descriptor_count);

    prepare_control_transfer();

    switch (setup->descriptor_type) {

        case USB_DT_DEVICE:     send_device_descriptor_to_host(setup->length);          break;
   
        case USB_DT_CONFIG:     send_device_configuration_to_host(setup->length);       break;
 
        case USB_DT_STRING:     send_device_string_to_host(setup->descriptor_index);    break;

        default:                error_unknown_descriptor_type(setup->descriptor_type);  break;
           
    }
}

void usb_protocol_setup_device_request_to_pico() {

    prepare_control_transfer();

    switch(setup->request) {

    //  case USB_REQUEST_CLEAR_FEATURE:   break;
    //  case USB_REQUEST_SET_FEATURE:     break;
    //  case USB_REQUEST_SET_DESCRIPTOR:  break;
          
        case USB_REQUEST_SET_ADDRESS:

            DEBUG_TEXT = "Setup Device Address\tNew Device Address Allocated=%d";
            DEBUG_SHOW ("USB", 2, setup->value);

           // ack_control_transfer_host_to_pico();

            send_ack_handshake_to_host(0, false); // VERY IMPORTANT, otherwise Host will issue hang / BUS RESET

            set_pico_device_address(setup->value);

            set_ep0_buffer_status(true);

        break;

        case USB_REQUEST_SET_CONFIGURATION:  // 9   // Expected to start CDC sequence

            DEBUG_TEXT = "Pico Request Handler \tSET CONFIGURATION REQUEST (HID), Value=%d";
            DEBUG_SHOW ("HID", 4, setup->value);

            ack_host_to_pico_control_transfer();        // VERY IMPORTANT, otherwise Host will issue hang / BUS RESET

            usb_setup_function_endpoints();

            busy_wait_ms(1);

        break;

        default:

            DEBUG_TEXT = "Pico Request Handler \tUNKNOWN USB SET REQUEST";
            DEBUG_SHOW ("ERR", 9);

          //  send_ack_handshake_to_host(0, true); 

        break;

    }

}

void usb_protocol_setup_device_respond_to_host() {

    static uint16_t get_status_count;
    static uint16_t get_unknown_count;
    static uint16_t get_descriptor_count;
    static uint16_t get_configuration_count;

    switch(setup->request) {

        case USB_REQUEST_GET_STATUS:

            DEBUG_TEXT = "Pico Request Handler \tGET PICO DEVICE STATUS, Count=%d";
            DEBUG_SHOW ("USB", 1, ++get_status_count);

        break;

        case USB_REQUEST_GET_DESCRIPTOR:

            DEBUG_TEXT = "Pico Request Handler \tGET PICO USB DESCRIPTOR, Count=%d";
            DEBUG_SHOW ("USB", 1, ++get_descriptor_count);

            usb_get_descriptor(setup);

        break;

        case USB_REQUEST_GET_CONFIGURATION:

            DEBUG_TEXT = "Pico Request Handler \tGET PICO USB CONFIGURATION, Count=%d";
            DEBUG_SHOW ("USB", 1, ++get_configuration_count);
  
        break;

        default:

            DEBUG_TEXT = "Pico Request Handler \tUNKNOWN USB GET REQUEST, Count=%d";
            DEBUG_SHOW ("ERR", 9, ++get_unknown_count);

        break;

    }
}


static inline void error_unknown_descriptor_type(uint8_t descriptor_type) {

    DEBUG_TEXT = "Pico Request Handler \tUNKNOWN REQUEST, Descriptor Type=%d";

    DEBUG_SHOW ("ERR", 9, descriptor_type);

} 

static inline uint8_t *config_dt_to_string(uint8_t descriptor_type) {

    switch (descriptor_type) {

        case USB_DT_DEVICE: return "DEVICE";
        case USB_DT_CONFIG: return "CONFIG";
        case USB_DT_STRING: return "STRING";
        
        default: return "UNKNOWN";

    }

}
