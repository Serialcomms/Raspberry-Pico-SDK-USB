
void send_data_packet(uint8_t EP_NUMBER, uint8_t packet_size, bool wait_for_buffer, bool last_packet);

void send_async_packet(uint8_t EP_NUMBER);

void synchronous_transfer_to_host(uint8_t EP_NUMBER, uint8_t *buffer_data, uint16_t buffer_length);
void start_async_transfer_to_host(uint8_t EP_NUMBER, void *source_buffer_address, uint16_t transfer_bytes);

void usb_wait_for_buffer_available_to_host(uint8_t EP_NUMBER);
void usb_wait_for_buffer_completion_pico_to_host(uint8_t EP_NUMBER, bool buffer_clear);

void send_ack_handshake_to_host(uint8_t EP_NUMBER, bool reset_buf_status);

void usb_wait_for_last_packet_to_host(uint8_t EP_NUMBER);