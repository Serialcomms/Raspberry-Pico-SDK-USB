#

add_executable(usb_hid_joystick

        usb_debug.c
        usb_reset.c
        usb_main.c
        usb_dev_init.c     
        usb_endpoints.c
        usb_descriptors.c 
        pico_info.c
        pico_device.c
        time_stamp.c  
        sie_errors.c 
        setup_packet.c
        setup_device.c
        setup_config.c
        setup_strings.c
        setup_interface.c
        usb_common.c
        usb_protocol.c
        usb_transmit.c 
        usb_receive.c    
        usb_interrupts.c 
        usb_joystick.c
        ep0_handlers.c
        epx_handlers.c
        show_registers.c
        status_screen.c
        )

target_link_libraries(usb_hid_joystick PRIVATE pico_stdlib pico_unique_id hardware_resets hardware_irq)

pico_add_extra_outputs(usb_hid_joystick)

pico_enable_stdio_usb(usb_hid_joystick 0)
pico_enable_stdio_uart(usb_hid_joystick 1)

add_compile_definitions(PICO_USE_STACK_GUARDS=1)

# add url via pico_set_program_url
# example_auto_set_url(dev_lowlevel)
