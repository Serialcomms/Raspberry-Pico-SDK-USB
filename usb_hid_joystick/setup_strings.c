#include <stdio.h>
#include <string.h>                         // For strlen
#include "pico/stdlib.h"                    // for printf
//#include "pico/unique_id.h"
#include "include/usb_debug.h"
#include "include/time_stamp.h"
#include "include/usb_transfer.h"
#include "include/setup_strings.h"
#include "include/show_registers.h"
#include "include/usb_descriptors.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

unsigned char *pico_string_descriptors[] = {

    (uint8_t[]) {0x04, 0x03, 0x09, 0x04},               // Language bytes
    (uint8_t *) "Raspberry Pi",                         // Vendor
    (uint8_t *) "Pico SDK Gamepad",                     // Product
    (uint8_t *) "Pico HID Interface",                   // Interface
    (uint8_t *) "Spare String",                         //
    NULL

} ;

unsigned char string_buffer[64];
unsigned char serial_number[32];

unsigned char *ROM_VERSIONS[] =  { "?" , "1" , "2", "3" };
unsigned char *CHIP_VERSIONS[] = { "?" , "B0/B1" , "B2" };

void send_device_string_to_host(uint8_t string_index) {

    static uint8_t  string_length = 0;
    static uint8_t *string_descriptor;

    uint8_t string_entries = count_of(pico_string_descriptors);

    if (string_entries) {

      if (string_index < string_entries) {

        if (string_index == 0) {

          string_descriptor = pico_string_descriptors[0];
          string_length = sizeof(pico_string_descriptors[0]);

        } else {

          string_descriptor = string_buffer;
          string_length = build_string_descriptor(pico_string_descriptors[string_index]);

        }

      } else {  // string index not in array, generate serial number instead

        string_descriptor = serial_number;
        string_length = generate_serial_number_string();

      }

      usb_start_string_transfer(string_descriptor, string_length, string_index) ;

    } else {

      DEBUG_TEXT = "Pico Strings Handler \tString Array Empty";
      DEBUG_SHOW (9, "ERR", DEBUG_TEXT);

    }

}


uint8_t build_string_descriptor(uint8_t *ascii_string) {

    uint8_t string_length = MIN(strlen(ascii_string), 62);
    uint8_t buffer_length = 2 + (2 * string_length);
    uint8_t i = 1;  // for ++pre-increment
    uint8_t ascii_char;

    DEBUG_TEXT = "String Descriptor \t%s (%d ASCII Bytes)";
    DEBUG_SHOW (1, "USB", DEBUG_TEXT, ascii_string, string_length);

    clear_string_buffer();

    if (string_length) {

        string_buffer[0x00] = buffer_length;
        string_buffer[0x01] = 0x03;             // 03 = string descriptor

        do {

          ascii_char = *ascii_string++ ;

          string_buffer[++i] = ascii_char & 0x7f;
          string_buffer[++i] = 0x00;

        } while (--string_length);
     
        return buffer_length;

    } else {

      return 0;
    
    } 

}

void usb_start_string_transfer(uint8_t *string_descriptor, uint8_t string_length, uint8_t string_index) {

    if (string_length) {

        if (string_index) {

          DEBUG_TEXT = "USB String Transfer\t%s (%d Unicode Bytes)+2 Bytes=%d";
          DEBUG_SHOW (1, "EP0", DEBUG_TEXT, pico_string_descriptors[string_index], string_length-2, string_length );
        
        } else {

          DEBUG_TEXT = "USB String Transfer\tString Length = %d Bytes";
          DEBUG_SHOW (1, "EP0", DEBUG_TEXT, string_length) ;
    
        }

        usb_start_transfer_pico_to_host(0, ep0_packet_size(), string_descriptor, string_length, true);

        receive_status_transaction_from_host(0, true);

    } else {

     DEBUG_TEXT = "Pico Request Handler \tGET PICO STRINGS, Empty String" ;
     DEBUG_SHOW (1, "USB", DEBUG_TEXT);

    }

}

uint8_t generate_serial_number_string() {
  
  snprintf(serial_number, 32, ":, :, :, :", "Pico Chip=", read_rp2040_chip_version(), ", ROM=", read_rp2040_rom_version());

  uint8_t serial_number_length = build_string_descriptor(serial_number);

  return serial_number_length;

}

unsigned char *read_rp2040_chip_version() {

  uint8_t VERSION = rp2040_chip_version() ;
 
  if ((VERSION > 0 ) && (VERSION < count_of(CHIP_VERSIONS) + 1)) {

    return CHIP_VERSIONS[VERSION];
    
  } else {

    return CHIP_VERSIONS[0];

  }
  
}

unsigned char *read_rp2040_rom_version() {

  uint8_t VERSION = rp2040_rom_version() ;
  
  if ((VERSION > 0) && (VERSION < count_of(ROM_VERSIONS) + 1)) {

    return ROM_VERSIONS[VERSION];
    
  } else {

    return ROM_VERSIONS[0];

  }
  
}

void clear_string_buffer() {

  uint8_t zero = 0x00; 
  
  uint8_t buffer_length = 64;

  while (--buffer_length) {

    string_buffer[buffer_length] = zero;
      
  } 
    
}


void show_string_buffer() {
    
TIMESTAMP(); printf("USB:\t String Buffer Bytes \t01-08 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", string_buffer[0x00], string_buffer[0x01], string_buffer[0x02] , string_buffer[0x03] , string_buffer[0x04] , string_buffer[0x05] , string_buffer[0x06] , string_buffer[0x07]);    
TIMESTAMP(); printf("USB:\t String Buffer Bytes \t09-16 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", string_buffer[0x08], string_buffer[0x09], string_buffer[0x0a] , string_buffer[0x0b] , string_buffer[0x0c] , string_buffer[0x0d] , string_buffer[0x0e] , string_buffer[0x0f]);
TIMESTAMP(); printf("USB:\t String Buffer Bytes \t17-24 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", string_buffer[0x10], string_buffer[0x11], string_buffer[0x12] , string_buffer[0x13] , string_buffer[0x14] , string_buffer[0x15] , string_buffer[0x16] , string_buffer[0x17]);    
TIMESTAMP(); printf("USB:\t String Buffer Bytes \t25-32 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", string_buffer[0x18], string_buffer[0x19], string_buffer[0x1a] , string_buffer[0x1b] , string_buffer[0x1c] , string_buffer[0x1d] , string_buffer[0x1e] , string_buffer[0x1f]);    
TIMESTAMP(); printf("USB:\t String Buffer Bytes \t33-40 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", string_buffer[0x20], string_buffer[0x21], string_buffer[0x22] , string_buffer[0x23] , string_buffer[0x24] , string_buffer[0x25] , string_buffer[0x26] , string_buffer[0x27]);    
TIMESTAMP(); printf("USB:\t String Buffer Bytes \t41-48 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", string_buffer[0x28], string_buffer[0x29], string_buffer[0x2a] , string_buffer[0x2b] , string_buffer[0x2c] , string_buffer[0x2d] , string_buffer[0x2e] , string_buffer[0x2f]);
TIMESTAMP(); printf("USB:\t String Buffer Bytes \t49-56 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", string_buffer[0x30], string_buffer[0x31], string_buffer[0x32] , string_buffer[0x33] , string_buffer[0x34] , string_buffer[0x35] , string_buffer[0x36] , string_buffer[0x37]);    
TIMESTAMP(); printf("USB:\t String Buffer Bytes \t57-64 = %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x \n", string_buffer[0x38], string_buffer[0x39], string_buffer[0x3a] , string_buffer[0x3b] , string_buffer[0x3c] , string_buffer[0x3d] , string_buffer[0x3e] , string_buffer[0x3f]);    
}

