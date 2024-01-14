
#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/pico_device.h"

struct pico_device_profile pico_usb_device;

void create_usb_device() {

    pico_usb_device.DEVICE_ADDRESSED = false;
    pico_usb_device.DEVICE_CONFIGURED = false;

}


