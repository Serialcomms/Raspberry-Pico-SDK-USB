# Raspberry Pico native SDK-based Device USB stack

#### Proof-of-Concept development to help understand USB device stack operation in a Pico SDK/Hardware context.

This repository has one objective only, which is to understand the viability of developing an alternative USB Device stack.

The 'stack' [^1] is written in C using native SDK functions only, and is derived from the low-level device example provided by Raspberry.

Extensive debug/log reporting via UART0 has been provided in lieu of an external hardware USB protocol analyser.

Synchronous (blocking) and Asynchronous (interrupt-driven) transmission routined are provided. 

The example provided simulates a continually-running joystick.

No attempt to optimise memory usage, code size, style or performance has been made at this stage.

The repository may be extensively modified or made unavailable at any time without notice.

Some unresolved issues (possibly related to stack usage ?) remain in the endpoint handler and debug routines.

### Usage

Once compiled, a


<details><summary>Acknowledgements</summary>  
<p>

* [Microsoft USB Device Enumeration](https://techcommunity.microsoft.com/t5/microsoft-usb-blog/how-does-usb-stack-enumerate-a-device/ba-p/270685)
* [Microsoft USB Control Transfer](https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/usb-control-transfer)
* [USB Descriptor and Request Parser](https://eleccelerator.com/usbdescreqparser/)
* [Thesycon USB Descriptor Dumper](https://www.thesycon.de/eng/usb_descriptordumper.shtml)
* [BUSDOG USB Analyser](https://github.com/djpnewton/busdog)

   
</p>
</details>

[^1]: The 'stack' is more of a 'controller' at this stage of development.
There are no published APIs or documentation available.
