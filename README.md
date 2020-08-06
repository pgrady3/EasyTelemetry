# EasyTelemetry

A simple telemetry board designed to measure many useful things on an Eco-Marathon car. This board will log quantities to an SD card for later analysis, in addition to providing useful information to the driver on an LCD. This design is hopefully easy to extend.

## Features
* 15v-60v operation
* Based on Teensy 3.2 microcontroller
* Uses INA226 for measuring voltage and low-side current
* Measures vehicle speed using hall gear-tooth sensor
* Measures vehicle position using cheap Chinese Ublox NEO-6M GPS modules
* Integrates instataneous power into total energy (joules) consumed
* Compatible with cheap Chinese 20x4 displays with I2C "LCD backpack"

## Output File

The software creates log files in the following format

``39.542 2.804 110.87 5.53 294087.16 5380.27 1407101 4.6546335 114.4687347
39.511 3.111 122.93 5.51 294099.44 5380.78 1407201 4.6546288 114.4687347
39.437 3.854 151.98 5.49 294114.66 5381.30 1407301 4.6546235 114.4687347
39.437 3.854 151.98 5.48 294129.84 5381.82 1407401 4.6546183 114.4687347
39.092 7.068 276.28 5.48 294157.47 5382.34 1407501 4.6546135 114.4687347
39.020 7.721 301.29 5.49 294187.59 5382.85 1407601 4.6546082 114.4687347
``

corresponding to
``voltage(V) current(A) power(w) velocity(m/s) energy(J) distance(m) elapsed time(ms) latitude(DD.dddd) longitude(DD.dddd)``

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