/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

typedef bool boolean;

typedef void (*buffer_completion_handler)(uint8_t EP_NUMBER);

struct endpoint_profile {

    boolean     async_mode;
    boolean     double_buffered;
    boolean     buffer_complete;
    boolean     transaction_complete;

    uint32_t    packet_id;
    uint16_t    transfer_bytes;
    uint16_t    max_packet_size;
    uint16_t    last_packet_size;
    uint16_t    bytes_transferred;
    uint16_t    async_bytes_pending;
    uint32_t    transaction_duration;
    uint32_t    address_base_offset;
    uint16_t    source_buffer_offset;
    void        *dpram_address; 
    void        *source_buffer_address;

    absolute_time_t start_time_now;
    absolute_time_t start_time_end;
     
    buffer_completion_handler completion_handler;

};

extern struct endpoint_profile host_endpoint[15];
extern struct endpoint_profile pico_endpoint[15];
