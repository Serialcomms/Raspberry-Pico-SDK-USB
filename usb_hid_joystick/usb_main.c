#include <stdio.h>
//#include <string.h>                         // For memcpy
//#include "pico/stdlib.h"                    // for printf
//#include "include/time_stamp.h"

#include "include/usb_debug.h"
#include "include/usb_dev_init.h"
#include "include/status_screen.h"
#include "include/setup_strings.h"
#include "include/usb_endpoints.h"
#include "include/usb_joystick.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

int main(void) {

    stdio_init_all();

    busy_wait_ms(100);

    clear_status_screen();

    start_status_screen();

    DEBUG_TEXT = "Pico Initialising\tUSB Device - HID Joystick Device Function";
    DEBUG_SHOW (1 , "SDK", DEBUG_TEXT);

    generate_serial_number_string();

    show_serial_number_string();
   
    usb_setup_endpoint_0();

    usb_device_init();

    busy_wait_ms(5000);

    DEBUG_TEXT = "Emulation Starting\tUSB Joystick with controls and buttons";
    DEBUG_SHOW (1 , "SDK" , DEBUG_TEXT);

    while (1) {  

        busy_wait_ms(333);

        send_joypad_movement();

    }
}
