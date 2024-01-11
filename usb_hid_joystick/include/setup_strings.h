
void show_string_buffer();
void clear_string_buffer();

void send_device_string_to_host(uint8_t string_index);
void usb_start_string_transfer(uint8_t *string_descriptor, uint8_t string_length, uint8_t string_index);

uint8_t build_string_descriptor(uint8_t *ascii_string);
uint8_t generate_serial_number_string(bool show_string_length);

