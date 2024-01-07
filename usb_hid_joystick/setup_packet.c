
#include <stdio.h>
#include "pico/stdlib.h" 
#include "include/usb_debug.h"
#include "include/sie_errors.h"                   
//#include "include/time_stamp.h"
#include "include/setup_packet.h"
#include "include/setup_device.h"
#include "include/usb_transfer.h"
#include "include/usb_interrupts.h"
#include "include/usb_endpoints.h"
#include "include/setup_interface.h"
#include "hardware/regs/usb.h"
#include "hardware/structs/usb.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void usb_handle_setup_packet() {

    struct usb_setup_command setup_command;

    volatile bool sie_errors = check_sie_errors();

    DEBUG_TEXT = "Setup Packet Handler\tPico Device Address = %d";
    DEBUG_SHOW (1, "USB", DEBUG_TEXT, get_device_address());

    setup_command = prepare_setup_packet();

    endpoint_packet_id_to_host[0] = USB_BUF_CTRL_DATA1_PID;
    endpoint_packet_id_to_pico[0] = USB_BUF_CTRL_DATA1_PID; 

    switch (setup_command.recipient) {

        case 0:     usb_setup_device(&setup_command);               break;      // device
        case 1:     usb_setup_interface(&setup_command);            break;      // interface
    //  case 2:                                                     break;      // endpoint
    //  case 3:                                                     break;      // other
        default:    usb_setup_unknown(setup_command.recipient);     break;
        
    } 
}

struct usb_setup_command prepare_setup_packet() {

    struct usb_setup_command setup_command;

    volatile uint8_t *setup_packet = usb_dpram->setup_packet;

    setup_command.request = setup_packet[1];
    setup_command.direction = setup_packet[0] & 0x80;
    setup_command.recipient = setup_packet[0] & 0x1F;
    setup_command.descriptor_type = setup_packet[3];
    setup_command.descriptor_index = setup_packet[2];
    setup_command.request_type = (setup_packet[0] & 0x60) >> 5;

    setup_command.value =  setup_packet[3] << 8 | setup_packet[2];
    setup_command.index =  setup_packet[5] << 8 | setup_packet[4];
    setup_command.length = setup_packet[7] << 8 | setup_packet[6];

    DEBUG_TEXT = "Setup Packet Handler\tSetup Bytes = %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X";
    DEBUG_SHOW (1, "USB", DEBUG_TEXT,
    setup_packet[0], setup_packet[1], setup_packet[2], setup_packet[3], 
    setup_packet[4], setup_packet[5], setup_packet[6], setup_packet[7]);

    DEBUG_TEXT = "Setup Packet Handler\t%s, Recipient=%02X, Request=%02X, Length=%d";
    DEBUG_SHOW (1, "USB", DEBUG_TEXT, setup_command.direction? "Pico > Host":"Host > Pico" ,
    setup_command.recipient, setup_command.request, setup_command.length);

    DEBUG_TEXT = "Setup Packet Handler\tDesc. Index=%02X, Type=%04X, Value=%04X, Index=%04X";
    DEBUG_SHOW (1, "USB", DEBUG_TEXT, 
    setup_command.descriptor_index, setup_command.descriptor_type, setup_command.value, setup_command.index);

    return setup_command;

}

void usb_setup_device(struct usb_setup_command *setup_command) {

    if (setup_command->direction) {

        usb_setup_device_respond_to_host(setup_command);

    } else {

        usb_setup_device_request_to_pico(setup_command);

    }
}

void usb_setup_interface(struct usb_setup_command *setup_command) {

    if (setup_command->direction) {

        usb_setup_interface_respond_to_host(setup_command);

    } else {

        usb_setup_interface_request_to_pico(setup_command);

    }
}

void usb_setup_unknown(uint8_t recipient) {

    DEBUG_TEXT = "Unhandled device setup request, recipient=%02X";
    DEBUG_SHOW (9, "ERR", DEBUG_TEXT, recipient);

}