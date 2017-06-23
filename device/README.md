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
internet connection. A typical [SIM card](https://en.wikipedia.org/wiki/Subscriber_identity_module)
(there is a socket under the board) and a [uFL GSM Antenna](https://www.adafruit.com/product/1991) are necessary.
In addition, this module can even recharge a LiPo battery.

Finally, the device also contains a [1200mAh LiPo battery](https://www.adafruit.com/product/258),
a [8 ohm 0.5W speaker](https://www.adafruit.com/product/1890),
an [electret microphone](https://www.adafruit.com/product/1064),
two [tactile buttons](https://www.adafruit.com/product/367),
a [SPDT Slide Switch](https://www.adafruit.com/product/805),
[stranded-core 22AWG wires](https://www.adafruit.com/product/3111),
[0.1" male](https://www.adafruit.com/product/392) and [female headers](https://www.adafruit.com/product/598).

## Building steps

### Case 3D printing
The case as been designed with [OpenSCAD](http://www.openscad.org/), a tool that allows developers to create
3D models with a scripting language. It is free and available on most operating systems. The advantages of using a
scripting language instead of a graphical mouse-controlled CAD software like [Thingiverse](https://www.thingiverse.com/)
is that it is very easy to parametrize (for example sizes and positions can be saved in variables and used by
several parts of the code, which is useful for connectors or components that need to fit together),
it is [VCS-friendly](https://en.wikipedia.org/wiki/Version_control) (for example
difference between several versions of a script can be easily visualized on GitHub) and reusable
[part libraries](https://github.com/openscad/openscad/wiki/Libraries) can be shared on internet. The main disadvantage
is that is it harder to use, especially for beginners.

In order to visualize the 3D model, setup OpenSCAD on your computer and open the `3dmodel/Box.scad` file. You should
then see something like this:

![3D model - assembled box](3dmodel/assembled_box.png?raw=true "3D model - assembled box")

Before printing the case, you need to generate [STL files](https://en.wikipedia.org/wiki/STL_(file_format)). To do that,
scroll the text editor to the bottom of the file `3dmodel/Box.scad`:

    //rotate([0, 180, 0])
    Box_assembled(includeBackCover = true, includeFrontCover = true, includeComponents = false);

These two lines generate the assembled box, which cannot be used directly. Modify these two lines like this:

    //rotate([0, 180, 0])
    Box_assembled(includeBackCover = true, includeFrontCover = false, includeComponents = false);

You should then see the following 3D model, which is the back cover:

![3D model - back cover](3dmodel/back_cover.png?raw=true "3D model - back cover")

Then render it and export it as `back_cover.stl`.

In order to generate the front cover, modify the last two lines like this:

    rotate([0, 180, 0])
    Box_assembled(includeBackCover = false, includeFrontCover = true, includeComponents = false);

You should then see the following 3D model:

![3D model - front cover](3dmodel/front_cover.png?raw=true "3D model - front cover")

Then render it and export it as `front_cover.stl`.

Finally, if you want, generate the brand label by opening the file `3dmodel/BrandName.scad`, and modify the last
lines like this:

    /*
    difference() {
        BrandName_frame(1, true);
        BrandName_frame(1, false);
    }*/
    
    rotate([0, 180, 0])
        difference() {
            BrandName_coloredBrandName(1, true);
            BrandName_coloredBrandName(1, false);
        }

You should then see something like this:

![3D model - brand name](3dmodel/brand_name.png?raw=true "3D model - brand name")

Then render it and export it as `brand_name.stl`.

You can then use your favorite tools to generate [G-code](https://en.wikipedia.org/wiki/G-code) for each STL file and
send them to your 3D printer, with tools like [Slic3r](http://slic3r.org/) +
[Printrun](http://www.pronterface.com/) or [Cura](https://ultimaker.com/en/products/cura-software).

If you are lucky, the result should look like this:

![3D printed cases](3dmodel/3d_printed_cases.jpg?raw=true "3D printed cases")

### Wiring
Now that the case is ready, it is time to add electronic components in it!

Because 22AWG wire are quite thick, it is important to place components in order to avoid too many wire crossings.
The following schema is useful for preparing wires before soldering them on the back cover:

![Wire positioning](wiring/wire_positioning.png?raw=true "Wire positioning")

To print this schema with the right size, install [LibreOffice](https://www.libreoffice.org/), open and print the file
`wiring/wire_positioning.odg`.
The following photo shows how to prepare the wires according to this schema (the same photo at bigger resolution
is [available here](wiring/photo_wire_positioning.jpg)):

![Wire positioning photo](wiring/photo_wire_positioning_small.jpg?raw=true "Wire positioning photo")

As you can see on the schema, 0.1" headers must be prepared at the following size:
* 6 pins for connecting the RedBearLab Nano with the Adafruit FONA.
* 4 pins for connecting the Adafruit FONA with the speaker and microphone (the schema shows 5 pins but you can put less).
* 6 pins for connecting the RedBearLab Nano with the tactile buttons and battery.
* 3 pins for connecting the RedBearLab Nano with the screen (the schema shows 5 pins but you can put less).
* 2 pins for connecting the screen with the battery.

As you can see, the RedBearLab Nano have more soldered wires connected to headers than necessary, but it is useful
for flashing the microcontroller by connecting it to the [MK20 USB Board](http://redbearlab.com/blenano/#mk20usbboard).

The following photo shows the wires soldered and connected to each others (a bigger resolution is
[available here](wiring/photo_wired_components.jpg)):

![Connected wires photo](wiring/photo_wired_components_small.jpg?raw=true "Connected wires photo")

Positioning wires on the front cover is much simpler, no need to prepare a shema for that, the following photo
is clear enough:

![Front wires photo](wiring/photo_wire_positioning_front.jpg?raw=true "Front wires photo")

The following photos shows both covers with connected components (a bigger resolution is
[available here](wiring/photo_components_placed_and_connected.jpg)):

![Connected components photo](wiring/photo_components_placed_and_connected_small.jpg?raw=true "Connected components photo")

### Electromagnetic shielding
There is a missing but important component is the previous photo: the GSM antenna. If you "naively" put it inside the
case you will experience unexpected resets because the GSM signal interferes with the microcontroller.

To solve this problem, you can create a layer of [aluminium foil](https://en.wikipedia.org/wiki/Aluminium_foil)
in sandwich between two layers of paper. And then put this in the middle of the case with the RedBearLab Nano on one
side (back cover side) and the GSM antenna on the other side (front cover side).

The following photos show how you can reproduce this protection:

* Electromagnetic shield width ([bigger resolution](electromagnetic_shield/photo_shield_width.jpg)):

![Shield width](electromagnetic_shield/photo_shield_width_small.jpg?raw=true "Shield width")

* Electromagnetic shield height ([bigger resolution](electromagnetic_shield/photo_shield_height.jpg)):

![Shield height](electromagnetic_shield/photo_shield_height_small.jpg?raw=true "Shield height")

* Electromagnetic shield in place ([bigger resolution](electromagnetic_shield/photo_shield_in_place.jpg)):

![in place](electromagnetic_shield/photo_shield_in_place_small.jpg?raw=true "Shield in place")

### Compile and flash the firmware
This is the step where you will discover all your wiring or soldering mistakes! :-)

#### Setup development environment
The following section describes briefly how to setup your environment in order to compile the firmware with
[GCC](https://gcc.gnu.org/).

Detailed information about how to setup [Eclipse IDE](https://www.eclipse.org) can be found in
[this tutorial](https://devzone.nordicsemi.com/tutorials/7/development-with-gcc-and-eclipse/) from Nordic Semiconductor.

1. Setup the [GNU toolchain for ARM Cortex-M](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads).
2. Add the toolchain into the PATH environment variable. To check if it is correctly set, open a new terminal
   and type the following command:
   ```bash
   arm-none-eabi-gcc --version
   ```
   The result should be something like:
   ```text
   arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors 6-2017-q1-update) 6.3.1 20170215 (release) [ARM/embedded-6-branch revision 245512]
   Copyright (C) 2016 Free Software Foundation, Inc.
   This is free software; see the source for copying conditions.  There is NO
   warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   ```
3. Setup GNU Make:
  * On Linux, setup the package provided by your distribution, like `build-essential` on Debian.
  * On Windows, setup the GNU ARM Eclipse Windows Build Tools package from the
    [GNU ARM Eclipse plug-in project](http://gnuarmeclipse.github.io/windows-build-tools/).
  * On MAC OSX, setup the 
    [Xcode tools](https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man1/gnumake.1.html).
  * Check the result by opening a new terminal and by typing the following command:
     ```bash
     make -v
     ```
     The result should be something like:
     ```text
     GNU Make 3.81
     Copyright (C) 2006  Free Software Foundation, Inc.
     This is free software; see the source for copying conditions.
     There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A
     PARTICULAR PURPOSE.
     
     This program built for i386-apple-darwin11.3.0
     ```
4. Setup the nRF5x-Command-Line-Tools
  * Download the nRF5x-Command-Line-Tools from the 
    [Nordic Semiconductor download page](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF51822).
  * On Linux and MAC OSX, unzip the tools archive and add `<nRF5x-Command-Line-Tools>/mergehex` into the PATH
    environment variable.
  * Check the result by opening a new terminal and by typing the following command:
     ```bash
     mergehex --version
     ```
     The result should be something like:
     ```text
     mergehex version: 9.5.0
     ```
5. Download and unzip the [Nordic nRF5x SDK 8.1.0](http://developer.nordicsemi.com/) (a greater version may not be
   compatible).
6. Edit the SDK Makefile with a text editor:
  * On Linux and MAC OSX, open `<SDK>/components/toolchain/gcc/Makefile.posix`.
  * On Windows, open `<SDK>/components/toolchain/gcc/Makefile.posix`.
  * Replace the `GNU_INSTALL_ROOT` property value by the path where you setup the GNU toolchain and `GNU_VERSION`
    by the toolchain version:
    ```text
    GNU_INSTALL_ROOT := /opt/gcc-arm-none-eabi-6-2017-q1-update
    GNU_VERSION := 6.3.1
    GNU_PREFIX := arm-none-eabi
    ```
7. With a text editor open the project file `device/firmware/Makefile` and replace the following property values:
  * `TARGET_BOARD` with `TARGET_BOARD_IS_BLE_NANO`
  * `SDK_PATH` with the path where you unzipped the Nordic nRF5x SDK (e.g. `/opt/nRF51_SDK_8.1.0_b6ed55f`).

#### Configure the firmware
The firmware needs the URL of the [webserver](../../webserver) where it will send the device location. It means that
you need to have a server with a public IP address that the device can reach from the GPRS connection.

If you want you can pause here and start installing the [webserver](../../webserver). Once you have the URL of your
server, you can resume this firmware configuration. A typical URL would be "http://myhostname.com/tourox/".

To configure the firmware, open the file `service/service_geolocation.c` and find the following constants:
```c
#define SEND_GEOLOCATION_URL               "http://www.tourox.io/sl/imeiimeiimeiime/-xx.dddddd/-xx.dddddd"
#define SEND_GEOLOCATION_URL_LENGTH        (sizeof(SEND_GEOLOCATION_URL))
#define SEND_GEOLOCATION_URL_IMEI_POS      (sizeof("http://nwww.tourox.io/sl"))
#define SEND_GEOLOCATION_URL_LNGLAT_POS    (sizeof("http://www.tourox.io/sl/imeiimeiimeiime"))
```
replace them by
```c
#define SEND_GEOLOCATION_URL               "http://myhostname.com/tourox/sl/imeiimeiimeiime/-xx.dddddd/-xx.dddddd"
#define SEND_GEOLOCATION_URL_LENGTH        (sizeof(SEND_GEOLOCATION_URL))
#define SEND_GEOLOCATION_URL_IMEI_POS      (sizeof("http://myhostname.com/tourox/sl"))
#define SEND_GEOLOCATION_URL_LNGLAT_POS    (sizeof("http://myhostname.com/tourox/sl/imeiimeiimeiime"))
```

> Note the pattern: replace "http://www.tourox.io/" with your URL "http://myhostname.com/tourox/".

In fact it is a bad practice to hardcode an URL inside a firmware. Instead, a better solution would be to set
it each time the Android application configures the device. But it is good enough for a prototype. :-)

#### Compile the firmware
A detailed tutorial about how to compile and flash the board is [available here](http://redbearlab.com/nrf51822-sdk).

To compile with a command line, type the following commands:
```bash
make
make nrf51422_xxac_s110 package
```

#### Prepare the MK20 USB board
The [MK20 USB board](http://redbearlab.com/blenano/#mk20usbboard) is necessary for flashing the Nano board:

![MK20 board with Nano](readme_resources/mk20_board_with_nano.jpg?raw=true "MK20 board with Nano")

Wires and headers must be soldered on the MK20 USB board. The wire color code must be the same as the Nano board:

![MK20 wiring 1](readme_resources/mk20_wiring_1.jpg?raw=true "MK20 wiring 1")
![MK20 wiring 2](readme_resources/mk20_wiring_2.jpg?raw=true "MK20 wiring 2")

#### Flash the RedBearLab Nano board
Plug the RedBearLab Nano board to the MK20 USB board and the later to the computer:

![Flash Nano board](readme_resources/flashing_nano_board.jpg?raw=true "Flash Nano board")

A new USB drive will appear with the "MBED" name.
Copy the file `_build/nrf51422_xxac_s110_packed.hex` into this drive.

After few seconds the MBED drive will refresh itself without the .hex file you just copied. At this stage, unplug the
board and put back the Nano into the Tourox case and close it.

If everything works as expected, you should see it working!

![Working device 1](readme_resources/working_device_1.jpg?raw=true "Working device 1")
![Working device 2](readme_resources/working_device_2.jpg?raw=true "Working device 2")

To actually use your device, you also need to build the Android and server applications. Please follow the
next instructions in the [parent document](../README.md).