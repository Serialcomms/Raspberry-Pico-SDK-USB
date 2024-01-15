
#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_dev_init.h"
#include "include/usb_joystick.h"
#include "include/usb_endpoints.h"
#include "include/status_screen.h"
#include "include/usb_protocol.h"
#include "include/usb_common.h"
#include "include/pico_device.h"
#include "include/usb_main.h"

#include "include/sie_errors.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

int main(void) {

    init_debug_critical_section();

    gpio_init(PICO_DEFAULT_LED_PIN);

    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
   
    irq_set_enabled(UART0_IRQ, true);
   
    uart_set_fifo_enabled(uart0, true);

    stdio_uart_init();

    busy_wait_ms(100);

    clear_status_screen();

    start_status_screen();

    usb_setup_default_endpoint_0();

    usb_device_init();

    wait_for_device_enumeration();

    if (usb_device_enumerated()) {

        gpio_put(PICO_DEFAULT_LED_PIN, 1);        

        DEBUG_TEXT = "Pico SDK/USB \t\tJoystick Simulation Starting";
        DEBUG_SHOW ("SDK", DEBUG_TEXT);

        busy_wait_ms(2000);

        DEBUG_TEXT = "Simulation Starting\tUSB Joystick with controls and buttons";
        DEBUG_SHOW ("SDK" , DEBUG_TEXT);

        busy_wait_ms(1000);

    while (1) {  

        busy_wait_ms(333);

        send_joystick_movement(false);

    }


    } else {

    DEBUG_TEXT = "USB Device Failure\tError Initialising USB";
    DEBUG_SHOW ("ERR" , DEBUG_TEXT);

    show_device_enumerated();

    //show_free_total_heap();

        while (1) {  

        busy_wait_ms(50);

        gpio_put(PICO_DEFAULT_LED_PIN, 1);

        busy_wait_ms(50);

        gpio_put(PICO_DEFAULT_LED_PIN, 0);

        }

    }

}

void wait_for_device_enumeration() {

    volatile bool sie_errors;
    volatile bool wait_timeout;
    volatile bool device_enumerated;
   
    uint8_t wait_seconds = 11;
    uint64_t wait_duration = 0;
    absolute_time_t wait_time_now = get_absolute_time();
    absolute_time_t wait_time_end = make_timeout_time_ms(wait_seconds * 1000);

    device_enumerated = false;

    DEBUG_TEXT = "Device Enumeration\tWaiting %d seconds for USB Device Enumeration";
    DEBUG_SHOW ("DEV", DEBUG_TEXT, wait_seconds);
    
    do { 

        busy_wait_ms(500);

        sie_errors = check_sie_errors();

        wait_timeout = time_reached(wait_time_end);

        device_enumerated = usb_device_enumerated();

    } while (!sie_errors && !device_enumerated && !wait_timeout);

    wait_duration = absolute_time_diff_us(wait_time_now, get_absolute_time());

    if (device_enumerated) {

    DEBUG_TEXT = "Device Enumerated\tWaited %d seconds for USB Device Enumeration";
    DEBUG_SHOW ("DEV", DEBUG_TEXT, wait_duration/1000000);


    }

}