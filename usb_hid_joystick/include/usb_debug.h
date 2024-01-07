
#include "pico/stdlib.h" 
void show_buffer_control(uint8_t EP);

unsigned char *sie_trans_complete();
unsigned char *sie_status_any_error();
unsigned char *sie_status_seq_error();
unsigned char *sie_status_ack_received();

unsigned char *buffer_status_ep0_in();
unsigned char *buffer_status_ep0_out();

unsigned char *buffer_status_in(uint8_t EP);
unsigned char *buffer_status_out(uint8_t EP);

unsigned char *buffer_control_out_pid(uint8_t EP);
unsigned char *buffer_control_out_full(uint8_t EP);
unsigned char *buffer_control_out_last(uint8_t EP);
unsigned char *buffer_control_out_avail(uint8_t EP);

unsigned char *buffer_control_in_pid(uint8_t EP);
unsigned char *buffer_control_in_full(uint8_t EP);
unsigned char *buffer_control_in_last(uint8_t EP);
unsigned char *buffer_control_in_avail(uint8_t EP);

unsigned char *ep_text(uint8_t EP_NUMBER);

unsigned char *concatenate(uint8_t *string1, uint8_t *string2);

extern uint8_t DEBUG_STRING_BUFFER[100];

void DEBUG_PRINT(uint8_t *debug_text, ...);

void DEBUG_SHOW(uint8_t debug_level, uint8_t *prefix_text, uint8_t *debug_text, ...);

//void DEBUG_SHOW2(uint8_t debug_level, uint8_t *prefix_text, ...) ;
