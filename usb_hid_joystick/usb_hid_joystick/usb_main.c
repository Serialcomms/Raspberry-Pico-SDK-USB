
#include "include/usb_debug.h"
#include "include/usb_dev_init.h"
#include "include/usb_joystick.h"
#include "include/usb_endpoints.h"
#include "include/status_screen.h"
#include "include/usb_protocol.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

int main(void) {

    USB_CONFIGURED = false;

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
   
    init_debug_critical_section();
    
    irq_set_enabled(UART0_IRQ, true);
    uart_set_fifo_enabled(uart0, true);

    stdio_uart_init();

    busy_wait_ms(100);

    clear_status_screen();

    start_status_screen();

    usb_setup_default_endpoint_0();

    usb_device_init();

    busy_wait_ms(3000);

    if (USB_CONFIGURED) {

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

        while (1) {  

        busy_wait_ms(50);

        gpio_put(PICO_DEFAULT_LED_PIN, 1);

        busy_wait_ms(50);

        gpio_put(PICO_DEFAULT_LED_PIN, 0);

    }


    }

 
}
