
#include <stdio.h>
#include <stdlib.h>
#include <string.h>                         // For strlen
#include "pico/stdlib.h"                    // for printf
#include "include/sie_errors.h"
#include "include/time_stamp.h"

#include "hardware/regs/usb.h"              // USB register definitions from pico-sdk
#include "hardware/structs/usb.h"           // USB hardware struct definitions from pico-sdk

#define usb_hardware_set   ((usb_hw_t *)hw_set_alias_untyped(usb_hw))
#define usb_hardware_clear ((usb_hw_t *)hw_clear_alias_untyped(usb_hw))

volatile static uint32_t sie_errors;

volatile bool check_sie_errors() {

    const uint32_t sie_error_mask = 0x8F000000;

    sie_errors = usb_hw->sie_status & sie_error_mask;

    if (sie_errors) {

        TIMESTAMP(); printf("SIE:\t SIE Error\tSerial Interface Engine, Error = %08X\n", sie_errors);

        return true;

    } else {

        return false;

    }
}

void sie_status_error_handler() {

    if (sie_errors & USB_SIE_STATUS_DATA_SEQ_ERROR_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_DATA_SEQ_ERROR_BITS;

        TIMESTAMP(); printf("SIE:\t Data Sequence Error \tSIE Register=%08X\n", sie_errors);

    }

    if (sie_errors & USB_SIE_STATUS_RX_TIMEOUT_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_RX_TIMEOUT_BITS;

        TIMESTAMP(); printf("SIE:\t ACK Wait Timeout\tTimeout waiting for ACK\t SIE Register=%08X\n", sie_errors);

    }

    if (sie_errors & USB_SIE_STATUS_RX_OVERFLOW_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_RX_OVERFLOW_BITS;

        TIMESTAMP(); printf("SIE:\t SIE Status Error\tReceive Overflow \t SIE Register=%08X\n", sie_errors);

    }
        
    if (sie_errors & USB_SIE_STATUS_BIT_STUFF_ERROR_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_BIT_STUFF_ERROR_BITS;

        TIMESTAMP(); printf("SIE:\t SIE Status Error\t Bit Stuff Error \t SIE Register=%08X\n", sie_errors);

    }

     if (sie_errors & USB_SIE_STATUS_CRC_ERROR_BITS) {

        usb_hardware_clear->sie_status = USB_SIE_STATUS_CRC_ERROR_BITS;

        TIMESTAMP(); printf("SIE:\t SIE Status Error\t CRC Error \t SIE Register=%08X\n", sie_errors);

    }

}