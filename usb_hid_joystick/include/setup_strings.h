
uint16_t read_clock_osc();
uint16_t read_clock_sys();
uint16_t read_clock_usb();
uint16_t read_clock_per();

uint32_t read_free_heap();
uint32_t read_total_heap();

uint8_t *read_rp2040_board_id();
uint8_t *read_rp2040_rom_version();
uint8_t *read_rp2040_chip_version();

void show_string_buffer();
void clear_string_buffer();

void show_pico_clocks();
void show_free_total_heap();
void show_serial_number_string();

void send_device_string_to_host(uint8_t string_index);
void usb_start_string_transfer(uint8_t *string_descriptor, uint8_t string_length, uint8_t string_index);

uint8_t build_string_descriptor(uint8_t *ascii_string);
uint8_t generate_serial_number_string(bool show_string_length);

