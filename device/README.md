# Tourox Mate - Device

## Introduction
As explained in the [parent document](../README.md), the goal of this device is to allow a tour guide to stay
in permanent contact with his group members during a trip. Each tourist wears a device with a lanyard.

The current version has the following communication capabilities:
* It can connect directly to the tour guide's smartphone via [BLE](https://en.wikipedia.org/wiki/Bluetooth_Low_Energy).
  This protocol allows the guide to configure a device, send text messages and measure the distance.
* It can use the standard GSM network to give phone calls (tour guide to tourist and vice versa).
* It can get its geographical position (latitude and longitude) thanks to the cellular network.
* It can connect to a server over internet via [GPRS](https://en.wikipedia.org/wiki/General_Packet_Radio_Service) in
  order to regularly send its location.

## Components
The following diagram shows the components and how they are connected:

![Components and wires](wiring/wiring.png?raw=true "Components and wires")

The brain of the system is a [nRF51822](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF51822), a
low-power SoC containing an ARM Cortex M0 CPU and a BLE module. It is located on the 
[RedBearLab BLE Nano](http://redbearlab.com/blenano/) board. For development I also used a
[nRF51 DK](https://www.nordicsemi.com/eng/Products/nRF51-DK) board, which is optional but more convenient at this stage.

The screen is a low-power [SHARP LCD display](https://www.adafruit.com/product/1393) packaged on a board produced
by AdaFruit.

The GSM module is an [AdaFruit FONA](https://www.adafruit.com/product/1946) powered with a
[SIM800](http://simcomm2m.com/En/module/detail.aspx?id=138) chip. It handles voice calls and provides an
internet connection. A typical [SIM card](https://en.wikipedia.org/wiki/Subscriber_identity_module) is necessary
(there is a socket under the board). In addition, this module can even recharge a LiPo battery.

Finally, the device also contains a [1200mAh LiPo battery](https://www.adafruit.com/product/258),
a [8 ohm 0.5W speaker](https://www.adafruit.com/product/1890),
an [electret microphone](https://www.adafruit.com/product/1064),
two [tactile buttons](https://www.adafruit.com/product/367),
and a [SPDT Slide Switch](https://www.adafruit.com/product/805).

## Building steps
Comming soon...