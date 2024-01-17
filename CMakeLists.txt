cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

#include(pico_extras_import_optional.cmake)

project(pico_usb C CXX ASM)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.5.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

#set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

set(SKIP_PICO_STDIO_USB 1)

# Initialize the SDK
pico_sdk_init()

#add_subdirectory(dev_lowlevel)

add_subdirectory(usb_hid_joystick)
#add_subdirectory(usb_joystick_GOOD)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
        
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

set(CMAKE_C_COMPILER_WORKS ON)
