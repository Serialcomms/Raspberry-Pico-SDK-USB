
add_executable(usb_hid_joystick

        usb_main.c        
        usb_debug.c
        usb_reset.c
        usb_dev_init.c     
       
        pico_info.c
        pico_device.c

        setup_device.c
        setup_config.c
        setup_strings.c
        setup_endpoints.c
        setup_interface.c

        usb_common.c
        usb_receive.c
        usb_protocol.c
        usb_transmit.c
        usb_joystick.c
        usb_endpoints.c 
        usb_interrupts.c
        usb_descriptors.c 
        usb_setup_packet.c
        usb_sie_errors.c 
        
        ep0_handlers.c
        epx_handlers.c

        time_stamp.c 
        status_screen.c
        show_registers.c
       
        )

target_link_libraries(usb_hid_joystick PRIVATE 

        pico_stdlib 
        pico_unique_id
        hardware_irq
        hardware_resets
)

add_compile_options("-fstack-usage")

add_compile_definitions(PICO_USE_STACK_GUARDS=0)
add_compile_definitions(PICO_STDIO_DEADLOCK_TIMEOUT_MS=1500)

target_compile_definitions(usb_hid_joystick PRIVATE PICO_STACK_SIZE=0x1000)

pico_add_extra_outputs(usb_hid_joystick)

pico_enable_stdio_usb(usb_hid_joystick 0)
pico_enable_stdio_uart(usb_hid_joystick 1)

# add url via pico_set_program_url
# example_auto_set_url(dev_lowlevel)
