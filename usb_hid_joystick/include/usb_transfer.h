
#include "pico/stdlib.h" 

void set_device_address(uint8_t DEVICE_ADDRESS);

void set_ep0_buffer_interrupts(bool enable_interrupts);

void send_data_packet(uint8_t EP_NUMBER, uint8_t packet_size, bool wait_for_buffer, bool last_packet);

void send_async_packet(uint8_t EP_NUMBER);

void synchronous_transfer_to_host(uint8_t EP_NUMBER, uint8_t packet_size, uint8_t *buffer_data, uint16_t buffer_length);
void start_async_transfer_to_host(uint8_t EP_NUMBER, uint8_t packet_size, void *source_buffer_address, uint16_t transfer_bytes);

void usb_wait_for_buffer_completion_pico_to_host(uint8_t EP_NUMBER, bool buffer_clear);
void usb_wait_for_buffer_completion_host_to_pico(uint8_t EP_NUMBER, bool buffer_clear);
void usb_wait_for_buffer_completion(uint8_t EP_NUMBER, uint32_t buffer_mask, bool buffer_status_clear);

void usb_wait_for_transaction_completion(uint8_t EP_NUMBER, bool completion_clear);

void receive_status_transaction_from_host(uint8_t EP_NUMBER, bool clear_buffer_status);

void send_ack_handshake_to_host(uint8_t EP_NUMBER, bool reset_buf_status);

uint8_t get_device_address();

uint32_t toggle_data_pid(uint32_t data_pid);
