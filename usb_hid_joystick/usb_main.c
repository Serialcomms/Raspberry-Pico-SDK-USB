//#include <stdio.h>
#include "include/usb_debug.h"
#include "include/usb_dev_init.h"
#include "include/status_screen.h"

#include "include/usb_endpoints.h"
#include "include/usb_joystick.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

int main(void) {

    irq_set_enabled(UART0_IRQ, true);
    uart_set_fifo_enabled(uart0, true);

    stdio_uart_init();

    busy_wait_ms(100);

    clear_status_screen();

    start_status_screen();

    DEBUG_TEXT = "Pico Initialising\tUSB Device - HID Joystick Device Function";
    DEBUG_SHOW (1 , "SDK", DEBUG_TEXT);
   
    usb_setup_endpoint_0();

    usb_device_init();

    busy_wait_ms(5000);

    DEBUG_TEXT = "Simulation Starting\tUSB Joystick with controls and buttons";
    DEBUG_SHOW (1 , "SDK" , DEBUG_TEXT);

    while (1) {  

        busy_wait_ms(333);

        send_joystick_movement();

    }
}
