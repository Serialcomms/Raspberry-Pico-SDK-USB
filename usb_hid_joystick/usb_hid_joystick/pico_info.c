#include <malloc.h>
#include "pico/unique_id.h"
#include "include/pico_info.h"
#include "include/usb_debug.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"

#undef LIB_TINYUSB_HOST
#undef LIB_TINYUSB_DEVICE

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

uint8_t *rp2040_rom_versions[] =  { "?", "V1", "V2", "V3" };
uint8_t *rp2040_chip_versions[] = { "?", "B0/B1", "B2" };

uint8_t *read_rp2040_rom_version() {

    uint8_t rom_version = rp2040_rom_version();

    if (rom_version && rom_version < count_of(rp2040_rom_versions)) {

        return rp2040_rom_versions[rom_version];

    } else {

        return rp2040_rom_versions[0];

    }   

}

uint8_t *read_rp2040_chip_version() {

  uint8_t chip_version = rp2040_chip_version() ;
 
  if (chip_version && chip_version < count_of(rp2040_chip_versions)) {

    return rp2040_chip_versions[chip_version];
    
  } else {

    return rp2040_chip_versions[0];

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

uint32_t read_free_heap() {

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