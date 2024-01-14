#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_endpoints.h"
#include "include/usb_common.h"
#include "include/usb_transmit.h"
#include "include/usb_receive.h"
#include "include/setup_packet.h"
#include "include/setup_interface.h"
#include "include/usb_descriptors.h"

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void usb_setup_interface_request_to_pico(struct usb_setup_command *setup_command) {

DEBUG_TEXT = "Setup Interface \tRequest=%d, Type=%d";
DEBUG_SHOW ("HID", DEBUG_TEXT, setup_command->request, setup_command->request_type);

switch (setup_command->request) {

    case 0x0A:

        DEBUG_TEXT = "Setup Interface \tSetting Idle, Request=%d";
        DEBUG_SHOW ("HID", DEBUG_TEXT, setup_command->request);

        send_ack_handshake_to_host(0, true);

    break;

    default:

        DEBUG_TEXT = "Setup Interface \tUnknown Request=%d";
        DEBUG_SHOW ("HID", DEBUG_TEXT, setup_command->request);

        send_ack_handshake_to_host(0, true);

    break;

    }

}

void usb_setup_interface_respond_to_host(struct usb_setup_command *setup_command) {

    DEBUG_TEXT = "Pico HID Interface \tSend HID Report Descriptor to Host";
    DEBUG_SHOW ("HID", DEBUG_TEXT );

    send_hid_descriptors_to_host();

}

void send_hid_descriptors_to_host() {

    uint16_t  report_length = hid_report_descriptor_size();
    uint8_t  *report_descriptor = pico_hid_report_descriptor;

    DEBUG_TEXT = "Pico HID Report \tSend Descriptor to Host,\tBytes=%d";
    DEBUG_SHOW ("HID", DEBUG_TEXT , report_length);

  //  start_async_transfer_to_host(0, report_descriptor, report_length);

    synchronous_transfer_to_host(0, report_descriptor, report_length);

    receive_status_transaction_from_host(0, true);

 //  if (wait_for_synchronous_transfer_to_host(0)) USB_DEVICE_CONFIGURED = true;

}

bool wait_for_synchronous_transfer_to_host(uint8_t EP_NUMBER) {

    uint64_t wait_duration = 0;
    volatile bool wait_timeout;  
    volatile bool transfer_complete;
  
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_ms(8000);

    do { 

        busy_wait_us(500);
        
        wait_timeout = time_reached(wait_time_end);

        transfer_complete = host_endpoint[EP_NUMBER].transfer_complete;
       
    } while (!wait_timeout && !transfer_complete);

        wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());


    if (wait_timeout) {

        DEBUG_TEXT = "Wait Timeout Error\tTransfer Complete Wait Timeout, Duration=%lld µs";
        DEBUG_SHOW ("USB", DEBUG_TEXT , wait_duration);

        return false;

    } else {

        return true;
    }

}