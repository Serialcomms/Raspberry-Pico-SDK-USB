#include "pico/stdlib.h"
#include "include/setup_packet.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_transmit.h"
#include "include/usb_receive.h"
#include "include/setup_interface.h"
#include "include/usb_descriptors.h"
#include "include/usb_endpoints.h"
#include "include/pico_device.h"

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void usb_setup_interface_request_to_pico(struct usb_setup_command setup_command);
void usb_setup_interface_respond_to_host(struct usb_setup_command setup_command);

void usb_setup_interface_request_to_pico(struct usb_setup_command setup_command) {

DEBUG_TEXT = "Setup Interface \tRequest=%d, Type=%d";
DEBUG_SHOW ("HID", DEBUG_TEXT, setup_command.request, setup_command.request_type);

switch (setup_command.request) {

    case 0x0A:

        DEBUG_TEXT = "Setup Interface \tSetting Idle, Request=%d";
        DEBUG_SHOW ("HID", DEBUG_TEXT, setup_command.request);

        pico_usb_device.HID_SET_IDLE_RECEIVED = true;

      // send_ack_handshake_to_host(0, true);
      // now done by completion handler. 

    break;

    default:

        DEBUG_TEXT = "Setup Interface \tUnknown Request=%d";
        DEBUG_SHOW ("HID", DEBUG_TEXT, setup_command.request);

        send_ack_handshake_to_host(0, true);

    break;

    }

}

void usb_setup_interface_respond_to_host(struct usb_setup_command setup_command) {

    DEBUG_TEXT = "Pico HID Interface \tSend HID Report Descriptor to Host";
    DEBUG_SHOW ("HID", DEBUG_TEXT );

    send_hid_descriptors_to_host();

}

void send_hid_descriptors_to_host() {

    uint16_t  report_length = hid_report_descriptor_size();
    uint8_t  *report_descriptor = pico_hid_report_descriptor;

    DEBUG_TEXT = "Pico HID Report \tSend HID Descriptor to Host,\tBytes=%d";
    DEBUG_SHOW ("HID", DEBUG_TEXT , report_length);

   // start_async_transfer_to_host(0, report_descriptor, report_length);

    synchronous_transfer_to_host(0, report_descriptor, report_length);

    receive_status_transaction_from_host(0, true);

    wait_for_transaction_completion(true);

    pico_usb_device.HID_REPORT_DESCRIPTOR_SENT = true;

}
