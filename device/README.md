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
Comming soon...

### Compile and flash the firmware
Comming soon...