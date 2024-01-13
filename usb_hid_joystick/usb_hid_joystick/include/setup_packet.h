
//#include "pico/stdlib.h" 
//#include "usb_interrupts.h"

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

struct usb_setup_command prepare_setup_packet();

void usb_handle_setup_packet();

void usb_setup_device(struct usb_setup_command *setup_command);

void usb_setup_interface(struct usb_setup_command *setup_command);

void usb_setup_unknown(uint8_t recipient);

void usb_setup_device_request_to_pico(struct usb_setup_command*);

void usb_setup_device_respond_to_host(struct usb_setup_command*);



