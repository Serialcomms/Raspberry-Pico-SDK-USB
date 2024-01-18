#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_protocol.h"
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

usb_setup_t setup_command;

usb_setup_t *setup = &setup_command;

static void prepare_setup_packet() {

    volatile uint8_t *setup_packet = usb_dpram->setup_packet;

    setup->request = setup_packet[1];
    setup->direction = setup_packet[0] & 0x80;
    setup->recipient = setup_packet[0] & 0x1F;
    setup->descriptor_type = setup_packet[3];
    setup->descriptor_index = setup_packet[2];
    setup->request_type = (setup_packet[0] & 0x60) >> 5;

    setup->value =  setup_packet[3] << 8 | setup_packet[2];
    setup->index =  setup_packet[5] << 8 | setup_packet[4];
    setup->length = setup_packet[7] << 8 | setup_packet[6];

    DEBUG_TEXT = "Setup Packet Handler\tSetup Bytes = %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X";
    DEBUG_SHOW ("USB",
    setup_packet[0], setup_packet[1], setup_packet[2], setup_packet[3], 
    setup_packet[4], setup_packet[5], setup_packet[6], setup_packet[7]);

    DEBUG_TEXT = "Setup Packet Handler\t%s, Recipient=%02X, Request=%02X, Length=%d";
    DEBUG_SHOW ("USB", setup->direction? "Pico > Host":"Host > Pico" ,
    setup_command.recipient, setup->request, setup->length);

    DEBUG_TEXT = "Setup Packet Handler\tDesc. Index=%02X, Type=%04X, Value=%04X, Index=%04X";
    DEBUG_SHOW ("USB", 
    setup->descriptor_index, setup->descriptor_type, setup->value, setup->index);

}

void usb_handle_setup_packet() {

    volatile bool sie_errors = check_sie_errors();

    DEBUG_TEXT = "Setup Packet Handler\tPico Device Address = %d";
    DEBUG_SHOW ("USB", get_device_address());

    prepare_setup_packet();

    host_endpoint[0].packet_id = USB_BUF_CTRL_DATA1_PID;
    pico_endpoint[0].packet_id = USB_BUF_CTRL_DATA1_PID;

    DEBUG_TEXT = "Setup Packet Handler\tRecipient = %d";
    DEBUG_SHOW ("USB", setup->recipient);

    switch (setup->recipient) {

        case 0:     usb_setup_device();              break;      // device
        case 1:     usb_setup_interface();           break;      // interface
    //  case 2:                                                    break;      // endpoint
    //  case 3:                                                    break;      // other
        default:    usb_setup_unknown(setup->recipient);           break;
        
    } 
}

void usb_setup_device() {

    if (setup->direction) {

     usb_setup_device_respond_to_host();

    } else {

     usb_setup_device_request_to_pico();

    }
}

void usb_setup_interface() {

    if (setup->direction) {

        usb_setup_interface_respond_to_host();

    } else {

        usb_setup_interface_request_to_pico();

    }
}

static void usb_setup_unknown(uint8_t recipient) {

    DEBUG_TEXT = "Unhandled device setup request, recipient=%02X";
    DEBUG_SHOW ("ERR", recipient);

}