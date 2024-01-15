
typedef bool boolean;

typedef void (*buffer_completion_handler)(uint8_t EP_NUMBER);

struct endpoint_profile {

    boolean     async_mode;
    boolean     double_buffered;
    boolean     buffer_complete;
    boolean     transaction_complete;
    uint32_t    packet_id;
    uint16_t    max_packet_size;
    uint16_t    last_packet_size;
    uint16_t    async_bytes_pending;
    uint16_t    transfer_bytes;
    uint32_t    transfer_duration;
    uint16_t    bytes_transferred;
    uint32_t    address_base_offset;
    uint16_t    source_buffer_offset;
    void        *dpram_address; 
    void        *source_buffer_address;

    absolute_time_t start_time_now;
    absolute_time_t start_time_end;
     
    buffer_completion_handler completion_handler;

} ;

extern bool valid_host_endpoint[15];
extern bool valid_pico_endpoint[15];

extern struct endpoint_profile host_endpoint[15];
extern struct endpoint_profile pico_endpoint[15];

extern buffer_completion_handler buffer_completion_handler_pico[15];
extern buffer_completion_handler buffer_completion_handler_host[15];


