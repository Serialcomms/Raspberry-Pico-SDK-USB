
void usb_handle_buffer_status_host(uint8_t EP_NUMBER);
void usb_handle_buffer_status_pico(uint8_t EP_NUMBER);

void buffer_completion_default(uint8_t EP_NUMBER, uint8_t status_bit);

volatile uint8_t *endpoint_irq_pending(uint32_t buffer_status);
