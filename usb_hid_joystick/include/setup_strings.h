
void show_string_buffer();
void clear_string_buffer();

uint8_t *read_rp2040_board_id();
uint8_t *read_rp2040_rom_version();
uint8_t *read_rp2040_chip_version();

uint32_t read_clock_osc();
uint32_t read_clock_sys();
uint32_t read_clock_usb();
uint32_t read_clock_per();

uint32_t read_free_heap();
uint32_t read_total_heap();

void show_pico_clocks();
void show_free_total_heap();
void show_serial_number_string();

uint8_t build_string_descriptor(uint8_t *ascii_string);
uint8_t generate_serial_number_string(bool show_string_length);

void send_device_string_to_host(uint8_t string_index);
void usb_start_string_transfer(uint8_t *string_descriptor, uint8_t string_length, uint8_t string_index);



