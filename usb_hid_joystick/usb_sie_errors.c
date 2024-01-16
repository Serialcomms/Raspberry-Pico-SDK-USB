
#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/usb_common.h"
#include "include/usb_sie_errors.h"

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

volatile static uint32_t sie_errors;

volatile bool check_sie_errors() {

    const uint32_t sie_error_mask = 0x8F000000;

    sie_errors = usb_hw->sie_status & sie_error_mask;

    if (sie_errors) {

        DEBUG_TEXT = "Serial Interface Engine\t, SIE Error = %08X";
        DEBUG_SHOW ("SIE", DEBUG_TEXT, sie_errors);

        return true;

    } else {

        return false;

    }
}

void sie_status_error_handler() {

    if (sie_errors & USB_SIE_STATUS_DATA_SEQ_ERROR_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_DATA_SEQ_ERROR_BITS;

        DEBUG_TEXT = "Data Sequence Error \tSIE Register=%08X";
        DEBUG_SHOW ("SIE", DEBUG_TEXT, sie_errors);

    }

    if (sie_errors & USB_SIE_STATUS_RX_TIMEOUT_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_RX_TIMEOUT_BITS;

        DEBUG_TEXT = "ACK Wait Timeout\tTimeout waiting for ACK\t SIE Register=%08X";
        DEBUG_SHOW ("SIE", DEBUG_TEXT, sie_errors);

    }

    if (sie_errors & USB_SIE_STATUS_RX_OVERFLOW_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_RX_OVERFLOW_BITS;

        DEBUG_TEXT = "SIE Status Error\tReceive Overflow \t SIE Register=%08X";
        DEBUG_SHOW ("SIE", DEBUG_TEXT, sie_errors);

    }
        
    if (sie_errors & USB_SIE_STATUS_BIT_STUFF_ERROR_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_BIT_STUFF_ERROR_BITS;

        DEBUG_TEXT = "SIE Status Error\t Bit Stuff Error \t SIE Register=%08X";
        DEBUG_SHOW ("SIE", DEBUG_TEXT, sie_errors);

    }

     if (sie_errors & USB_SIE_STATUS_CRC_ERROR_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_CRC_ERROR_BITS;

        DEBUG_TEXT = "SIE Status Error\t CRC Error \t SIE Register=%08X";
        DEBUG_SHOW ("SIE", DEBUG_TEXT, sie_errors);

    }

}