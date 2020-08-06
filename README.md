# EasyTelemetry

A simple telemetry board designed to measure many useful things on an Eco-Marathon car. This board will log quantities to an SD card for later analysis, in addition to providing useful information to the driver on an LCD. This design is hopefully easy to extend.

## Features
* 15v-60v operation
* 20A continuous current, 40A burst (more with heatsinking)
* All through-hole design. Easy to solder!
* Socket for automotive fuse
* Automatic hall sensor identification
* Extremely simple code. <200 lines with comments
* Duty-cycle control
* Teensy 3.2 microcontroller
* L6387E gate drivers
* IRFB7730 MOSFETS
* BOM cost of 70 USD
* Totally open source

## Schematic

![Schematic](/board/sch.png)

## Board

![Board](/board/board.png)

## Software Used

* PCB layout: [Eagle](https://www.autodesk.com/products/eagle/free-download)
* Compiler: [Arduino](https://www.arduino.cc/) with [PJRC Teensyduino add-on](https://www.pjrc.com/teensy/td_download.html)

## Assembly

This respository includes Gerber files which can be directly submitted to PCB manufacturers. We recommend Chinese manufacturers such as [AllPCB](https://www.allpcb.com/) or [JLCPCB](https://jlcpcb.com/). The repository also includes a Bill of Materials (BOM) with Digikey part numbers to order from.

Assembly is straightforward. However, it is recommended to cut the VIN-VUSB trace on the [underside of the Teensy](https://www.pjrc.com/teensy/card7b_rev1.pdf). Cutting the trace means that the Teensy can no longer be powered over USB. This is a safety procaution to prevent the controller feeding power backwards into your computer's USB port. 

## Contact Us

This controller was designed by Patrick Grady from Duke Electric Vehicles. If you need help or clarification, please [send us a message on our website.](http://www.duke-ev.org/dev-contact-us)