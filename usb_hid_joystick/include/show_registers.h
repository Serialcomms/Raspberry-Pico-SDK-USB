
void show_usb_registers();

void show_dpram_to_host(uint8_t EP_NUMBER, uint8_t max_bytes);
void show_dpram_to_pico(uint8_t EP_NUMBER, uint8_t max_bytes);

void show_dpram(uint8_t EP_NUMBER, uint8_t max_bytes, uint8_t *usb_dpram_data);
