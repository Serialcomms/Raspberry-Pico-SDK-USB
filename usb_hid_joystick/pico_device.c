
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "include/usb_debug.h"
#include "include/pico_device.h"

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

struct pico_device_profile pico_usb_device;

static uint8_t show_device_text[40];

void reset_usb_device() {

    pico_usb_device.ADDRESS_SET = false;
    pico_usb_device.DEVICE_DESCRIPTOR_SENT = false;
    pico_usb_device.DEVICE_CONFIGURATION_SENT = false;
    pico_usb_device.HID_REPORT_DESCRIPTOR_SENT = false;
    pico_usb_device.HID_SET_IDLE_RECEIVED = false;

}

bool usb_device_enumerated() {

    bool enumerated = 
    pico_usb_device.ADDRESS_SET &
    pico_usb_device.DEVICE_DESCRIPTOR_SENT & 
    pico_usb_device.DEVICE_CONFIGURATION_SENT &
    pico_usb_device.HID_REPORT_DESCRIPTOR_SENT &
    pico_usb_device.HID_SET_IDLE_RECEIVED;

    return enumerated;

}

void show_device_enumerated() {

    DEBUG_TEXT = "Device Enumeration\tDEVICE ADDRESS SET = %s";
    DEBUG_SHOW ("DEV", DEBUG_TEXT,  pico_usb_device.ADDRESS_SET ? "Y" : "N"); 

    DEBUG_TEXT = "Device Enumeration\tDEVICE DESCRIPTOR SENT= %s";
    DEBUG_SHOW ("DEV", DEBUG_TEXT,  pico_usb_device.DEVICE_DESCRIPTOR_SENT ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tDEVICE CONFIGURATION SENT = %s";
    DEBUG_SHOW ("DEV", DEBUG_TEXT,  pico_usb_device.DEVICE_CONFIGURATION_SENT ? "Y" : "N"); 

    DEBUG_TEXT = "Device Enumeration\tHID REPORT DESCRIPTOR SENT = %s";
    DEBUG_SHOW ("DEV", DEBUG_TEXT,   pico_usb_device.HID_REPORT_DESCRIPTOR_SENT ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tHID SET IDLE RECEIVED = %s";
    DEBUG_SHOW ("DEV", DEBUG_TEXT,  pico_usb_device.HID_SET_IDLE_RECEIVED ? "Y" : "N");

    DEBUG_TEXT = "Device Enumeration\tENUMERATION COMPLETE = %s";
    DEBUG_SHOW ("DEV", DEBUG_TEXT,   usb_device_enumerated() ? "TRUE" : "FALSE");

}

uint8_t *unused() {

uint8_t *enumeration_text = show_device_text;      
  
  snprintf(show_device_text, 40, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s", 

  "DEVICE ADDRESS SET=",     pico_usb_device.ADDRESS_SET ? "Y" : "N" ,
  "DEVICE DESCRIPTOR=",      pico_usb_device.DEVICE_DESCRIPTOR_SENT ? "Y" : "N" ,
  "CONFIG DESCRIPTOR=",      pico_usb_device.DEVICE_CONFIGURATION_SENT ? "Y" : "N" , 
  "REPORT DESCRIPTOR=",      pico_usb_device.HID_REPORT_DESCRIPTOR_SENT ? "Y" : "N" , 
  "HID SET IDLE=     ",      pico_usb_device.HID_SET_IDLE_RECEIVED ? "Y" : "N",
  "DEVICE ENUMERATED =",     device_enumerated() ? "TRUE" : "FAIL");
  
   return enumeration_text;


}