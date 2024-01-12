
#include "pico/stdlib.h" 

unsigned char *config_dt_to_string(uint8_t descriptor_type);

void error_unknown_descriptor_type(uint8_t descriptor_type);

extern bool USB_CONFIGURED ;
