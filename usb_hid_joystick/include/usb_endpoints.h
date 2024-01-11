#include "pico/stdlib.h" 

typedef bool boolean;

void usb_setup_endpoint_0();

void usb_setup_function_endpoints();

extern bool valid_host_endpoint[16];
extern bool valid_pico_endpoint[16];

typedef void (*buffer_completion_handler)(uint8_t EP_NUMBER);

extern buffer_completion_handler buffer_completion_handler_pico[16];
extern buffer_completion_handler buffer_completion_handler_host[16];

struct endpoint_profile {

    boolean     async_mode;
    boolean     double_buffered;
    uint8_t     packet_size; 
    uint8_t     last_packet_size;
    uint16_t    async_bytes;
    uint32_t    packet_id;
    uint32_t    control_register;
    uint32_t    address_base_offset;
    uint16_t    transfer_bytes;
    uint32_t    transfer_duration;
    uint16_t    source_buffer_offset;
    void        *dpram_address; 
    void        *source_buffer_address;
    absolute_time_t start_time_now;
    absolute_time_t start_time_end;
     
    buffer_completion_handler completion_handler;

} ;

extern struct endpoint_profile host_endpoint[16];
extern struct endpoint_profile pico_endpoint[16];
