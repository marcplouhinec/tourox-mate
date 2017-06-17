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

[Please follow this guide in order to build an flash the firmware.](firmware/README.md)

If everything works as expected, you should see it working!

![Working device 1](working_device_1.jpg?raw=true "Working device 1")
![Working device 2](working_device_2.jpg?raw=true "Working device 2")

To actually use your device, you also need to build the Android and server applications. Please follow the
next instructions in the [parent document](../README.md).