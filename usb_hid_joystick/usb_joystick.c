
#include <stdio.h>
#include <string.h>                        
#include <math.h>
#include "pico/stdlib.h"                
#include "include/usb_debug.h"
#include "include/time_stamp.h"
#include "include/usb_endpoints.h"
#include "include/usb_transfer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static uint8_t *DEBUG_TEXT = DEBUG_STRING_BUFFER;

void send_joystick_movement() {

static uint16_t i = 200;
static uint16_t angle;
static uint16_t radius = 120;

static uint8_t rudder;   
static uint8_t buttons;
static uint8_t throttle;
static uint8_t hat_switch;

static int8_t pointer_x;
static int8_t pointer_y;

const float radians = M_PI / (float) 180.0; 

//uint8_t joystick_report[6];

uint8_t *joystick_report = host_endpoint[1].usb_buffer;

//uint8_t *joystick_buffer = joystick_report;



++i;

i %= 16535;       
              
angle += 6;  

angle %= 360;

rudder = (i % 0xFF * 8) & 0xFF;               // values -127 to +128;
throttle = 255 - (i % 0xFF * 8) & 0xFF;       // values -127 to 128;
buttons = (int8_t) (1 << (i%8)) & 0xff;       // one bit per button 
hat_switch = (i) & 0x07;                      // hat switch value 
pointer_x = cosf(angle * radians) * radius ;  // values -127 to +128
pointer_y = sinf(angle * radians) * radius ;  // values -127 to +128

joystick_report[0] = rudder;
joystick_report[1] = throttle;
joystick_report[2] = pointer_x;
joystick_report[3] = pointer_y;
joystick_report[4] = hat_switch;
joystick_report[5] = buttons;

DEBUG_TEXT = "HID Report Bytes\tEPX Address=%08X,\tData = %02X, %02X, %02X, %02X, %02X, %02X";

DEBUG_SHOW (1, "HID", DEBUG_TEXT, joystick_report,  
joystick_report[0], joystick_report[1], joystick_report[2], joystick_report[3],
joystick_report[4], joystick_report[5], joystick_report[6], joystick_report[7]); 

DEBUG_TEXT = "HID Report Bytes\ti=%d, Angle=%d, Rudder=%d, Pointer x/y = %d/%d  ";

DEBUG_SHOW (1, "JOY" , DEBUG_TEXT, i, angle, rudder, pointer_x, pointer_y);

//send_data_packet(1, 6, false, true, true); // P4=true, must toggle PID

//joystick_data_packet(1, 6, false, true) ;

start_async_transfer_to_host(1, 8, joystick_report, 6);

gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));

}

