#include "pico/stdlib.h" 

void usb_setup_endpoint_0();

void usb_setup_function_endpoints();

extern uint32_t endpoint_packet_id_to_host[16];
extern uint32_t endpoint_packet_id_to_pico[16];

extern uint8_t *endpoint_data_buffer_to_host[16];
extern uint8_t *endpoint_data_buffer_to_pico[16];

typedef void (*buffer_completion_handler)(uint8_t EP_NUMBER);

extern buffer_completion_handler buffer_completion_handler_pico[16];
extern buffer_completion_handler buffer_completion_handler_host[16];