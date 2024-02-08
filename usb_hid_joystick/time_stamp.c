/*
 * Copyright (c) 2024 Serialcomms (GitHub).
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

#include <time.h>
#include <stdio.h>
#include "pico/types.h"
#include "pico/stdlib.h"
#include "hardware/divider.h"
#include "include/time_stamp.h"

void __time_critical_func(TIMESTAMP()) {

    static uint64_t TIMENOW64;
    static uint32_t TIMENOW32;
    static uint32_t Hours, minutes, Minutes, Seconds, Milliseconds, Microseconds;

    static char TIMESTAMP_STRING[18] ;

    static const uint32_t ONE_THOUSAND =  1000   ;
    static const uint32_t ONE_MILLION =  1000000  ;
    static const uint32_t DIVISOR_HOURS = 60 * 60 ;
 
    TIMENOW64 = time_us_64();
 
    TIMENOW32 = (uint32_t) (TIMENOW64 / ONE_MILLION);

    Hours = hw_divider_u32_quotient_inlined(TIMENOW32, DIVISOR_HOURS);

    minutes = hw_divider_u32_quotient_inlined(TIMENOW32, 60);

    Minutes = hw_divider_u32_remainder_inlined(minutes, 60);

    Seconds = hw_divider_u32_remainder_inlined(TIMENOW32, 60);

    Microseconds = TIMENOW64 % ONE_MILLION;

    Milliseconds = hw_divider_u32_quotient_inlined(Microseconds, ONE_THOUSAND); 

    sprintf(TIMESTAMP_STRING, "%03d:%02d:%02d.%3.3d", Hours, Minutes, Seconds, Milliseconds);

    printf("%s\t", TIMESTAMP_STRING);

}

