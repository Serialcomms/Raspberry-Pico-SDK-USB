
struct pico_device_profile {

    bool ADDRESS_SET;
    bool DEVICE_DESCRIPTOR_SENT;
    bool DEVICE_CONFIGURATION_SENT;
    bool HID_REPORT_DESCRIPTOR_SENT;
    bool HID_SET_IDLE_RECEIVED;

};

extern struct pico_device_profile pico_usb_device;

void reset_usb_device();

bool usb_device_enumerated();

void show_device_enumerated();

void set_binary_declarations();
