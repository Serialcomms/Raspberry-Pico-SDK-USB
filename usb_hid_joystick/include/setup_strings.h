
void show_string_buffer();
void clear_string_buffer();

uint8_t *read_rp2040_rom_version();
uint8_t *read_rp2040_chip_version();
uint8_t generate_serial_number_string();
uint8_t build_string_descriptor(uint8_t *ascii_string);

void show_serial_number_string();

void send_device_string_to_host(uint8_t string_index);
void usb_start_string_transfer(uint8_t *string_descriptor, uint8_t string_length, uint8_t string_index);



