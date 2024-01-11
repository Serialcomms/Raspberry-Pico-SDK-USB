#include <stdio.h>
#include <string.h>                       
#include <malloc.h>
#include "pico/unique_id.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"

#include "include/usb_debug.h"
#include "include/time_stamp.h"
#include "include/usb_transfer.h"
#include "include/setup_strings.h"
#include "include/show_registers.h"
#include "include/usb_descriptors.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

uint32_t PICO_CLOCK_SYS;
uint32_t PICO_CLOCK_USB;

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

uint8_t *pico_string_descriptors[] = {

    (uint8_t[]) {0x04, 0x03, 0x09, 0x04},               // Language bytes
    (uint8_t *) "Raspberry Pi",                         // Vendor
    (uint8_t *) "Pico SDK Joystick",                    // Product
    (uint8_t *) "Pico HID Interface",                   // Interface
    (uint8_t *) "Spare String",                         //
    NULL

} ;

uint8_t string_buffer[64];
static uint8_t serial_number[32];

uint8_t *rp2040_rom_versions[] =  { "?", "V1", "V2", "V3" };
uint8_t *rp2040_chip_versions[] = { "?", "B0/B1", "B2" };

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
        string_length = generate_serial_number_string(false);

      }

      usb_start_string_transfer(string_descriptor, string_length, string_index) ;

    } else {

      DEBUG_TEXT = "Pico Strings Handler \tString Array Empty";
      DEBUG_SHOW ("ERR", DEBUG_TEXT);

    }

}

uint8_t build_string_descriptor(uint8_t *ascii_string) {

    uint8_t string_length = MIN(strlen(ascii_string), 62);
    uint8_t buffer_length = 2 + (2 * string_length);
    uint8_t i = 1;  // for ++pre-increment
    uint8_t ascii_char;

    DEBUG_TEXT = "String Descriptor \t%s (%d ASCII Bytes)";
    DEBUG_SHOW ("USB", DEBUG_TEXT, ascii_string, string_length);

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
          DEBUG_SHOW ("EP0", DEBUG_TEXT, pico_string_descriptors[string_index], string_length-2, string_length );
        
        } else {

          DEBUG_TEXT = "USB String Transfer\tString Length = %d Bytes";
          DEBUG_SHOW ("EP0", DEBUG_TEXT, string_length) ;
    
        }

        synchronous_transfer_to_host(0, ep0_packet_size(), string_descriptor, string_length);

        receive_status_transaction_from_host(0, true);

    } else {

    DEBUG_TEXT = "Pico Request Handler \tGET PICO STRINGS, Empty String" ;
    DEBUG_SHOW ("USB", DEBUG_TEXT);

    }

}

uint8_t generate_serial_number_string(bool show_string_length) {
  
  snprintf(serial_number, 32, "%s%s%s%s%s%s", 
  "CHIP_", read_rp2040_chip_version(), 
  "_ROM_", read_rp2040_rom_version(), 
  "_ID_",  read_rp2040_board_id());
  
  uint8_t serial_number_length = build_string_descriptor(serial_number);

  if (show_string_length) {

    DEBUG_TEXT = "Pico Serial String \tSerial Number Length = %d" ;
    DEBUG_SHOW ("USB", DEBUG_TEXT, serial_number_length);

  }

  return serial_number_length;

}


void show_serial_number_string() {

  uint8_t *serial_string = serial_number;

  DEBUG_TEXT = "Pico Serial String \tSerial Number = %s" ;
  DEBUG_SHOW ("USB", DEBUG_TEXT, serial_string);

}

uint8_t *read_rp2040_chip_version() {

  uint8_t chip_version = rp2040_chip_version() ;
 
  if ((chip_version > 0) && (chip_version < count_of(rp2040_chip_versions) + 1)) {

    return rp2040_chip_versions[chip_version];
    
  } else {

    return rp2040_chip_versions[0];

  }
  
}

uint8_t *read_rp2040_rom_version() {

  uint8_t rom_version = rp2040_rom_version() ;
  
  if ((rom_version > 0) && (rom_version < count_of(rp2040_rom_versions) + 1)) {

    return rp2040_rom_versions[rom_version];
    
  } else {

    return rp2040_rom_versions[0];

  }
  
}

uint8_t *read_rp2040_board_id() {

  static uint8_t board_id[32];

  pico_get_unique_board_id_string(board_id, 16);

  return board_id;

}

uint32_t read_total_heap() {

  extern char __StackLimit, __bss_end__;

  return &__StackLimit  - &__bss_end__;

}

uint32_t read_free_heap(void) {

  struct mallinfo m = mallinfo();

  return read_total_heap() - m.uordblks;

}

void show_free_total_heap() {

  DEBUG_TEXT = "Pico Memory Heap \tTotal Bytes=%d, Free Bytes=%d" ;
  DEBUG_SHOW ("MEM", DEBUG_TEXT, read_total_heap(), read_free_heap());

}

void show_pico_clocks() {

  DEBUG_TEXT = "Pico Clocks (MHz)\tOscillator=%d, System=%d, Peripheral=%d, USB=%d" ;
  DEBUG_SHOW ("CLK", DEBUG_TEXT, read_clock_osc(), read_clock_sys(), read_clock_per(), read_clock_usb());

}

uint16_t read_clock_sys() {

  return frequency_count_mhz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);

}

uint16_t read_clock_usb() {

  return frequency_count_mhz(CLOCKS_FC0_SRC_VALUE_CLK_USB);

}

uint16_t read_clock_per() {

  return frequency_count_mhz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);

}

uint16_t read_clock_osc() {

  return frequency_count_mhz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);

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

