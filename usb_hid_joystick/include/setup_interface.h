
void send_hid_descriptors_to_host();

void usb_setup_interface_respond_to_host(struct usb_setup_command *setup_command);

void usb_setup_interface_request_to_pico(struct usb_setup_command *setup_command);

bool wait_for_synchronous_transfer_to_host(uint8_t EP_NUMBER);
