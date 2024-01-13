
void usb_wait_for_host_ack(uint8_t EP_NUMBER);

void usb_wait_for_buffer_completion_host_to_pico(uint8_t EP_NUMBER, bool buffer_status_clear);

void receive_status_transaction_from_host(uint8_t EP_NUMBER, bool clear_buffer_status);
