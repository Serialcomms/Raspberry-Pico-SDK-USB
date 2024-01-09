#include "pico/stdlib.h" 

void usb_setup_endpoint_0();

void usb_setup_function_endpoints();

extern bool valid_host_endpoint[16];
extern bool valid_pico_endpoint[16];

extern uint32_t endpoint_packet_id_to_host[16];
extern uint32_t endpoint_packet_id_to_pico[16];

extern uint8_t *endpoint_data_buffer_to_host[16];
extern uint8_t *endpoint_data_buffer_to_pico[16];

typedef void (*buffer_completion_handler)(uint8_t EP_NUMBER);
typedef bool boolean;

extern buffer_completion_handler buffer_completion_handler_pico[16];
extern buffer_completion_handler buffer_completion_handler_host[16];

struct source_data_queue {


};

struct host_endpoint_profile {

    boolean     async_mode;
    boolean     double_buffered;
    uint8_t     packet_size; 
    uint8_t     full_async_packets;
    uint8_t     last_packet_size;
    uint16_t    async_bytes;
    uint32_t    packet_id;
    uint32_t    control_register;
    uint32_t    address_base_offset;
    uint16_t    transfer_bytes;
    uint32_t    transfer_duration;
    uint16_t    source_buffer_offset;
    void        *source_buffer_address;
    void        *dpram_address;  
    absolute_time_t start_time_now;
    absolute_time_t start_time_end;
     
    buffer_completion_handler completion_handler;

} ;

extern struct host_endpoint_profile host_endpoint[16];


/* Struct in which we keep the endpoint configuration
struct endpoint_profile_lowlevel {

    const struct usb_endpoint_descriptor *descriptor;
    
    usb_ep_handler handler;

    // Pointers to endpoint + buffer control registers in the USB controller DPSRAM
 
    volatile uint32_t *endpoint_control;
    volatile uint32_t *buffer_control;
    volatile uint8_t *data_buffer;

    // Toggle after each packet (unless replying to a SETUP)
    uint8_t next_pid;
};
*/