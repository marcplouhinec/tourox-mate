# Tourox Mate - Device firmware

## Setup development environment
The development environment is based on [GCC](https://gcc.gnu.org/) and [Eclipse IDE](https://www.eclipse.org).
This section describes succinctly how to setup your environment on Windows. For more details, please follow
[this tutorial](https://devzone.nordicsemi.com/tutorials/7/development-with-gcc-and-eclipse/) from Nordic Semiconductor.

Please download the following files (if links are broken, 
please visit [Nordic Semiconductor download page](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF51822)):
* [nRF51 SDK v8.x.x](http://developer.nordicsemi.com/nRF51_SDK/nRF51_SDK_v8.x.x/nRF51_SDK_8.1.0_b6ed55f.zip)
* [GCC for ARM](https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q3-update/+download/gcc-arm-none-eabi-4_9-2015q3-20150921-win32.exe)
* [Make](http://gnuwin32.sourceforge.net/downlinks/make.php)
* [Core Utils](http://gnuwin32.sourceforge.net/downlinks/coreutils.php)
* [nRF5x Command Line Tools](https://www.nordicsemi.com/eng/nordic/download_resource/33444/42/78528581/53210)
* [JLink V502e](https://github.com/autonohm/robotworkshop/raw/9fcbfb46047406267b50ee339dbad9efc8c0205f/studierbot/drive/firmware/Setup_JLink_V502e.zip)
* [J-Link Debugger](https://download.segger.com/J-Link/J-LinkDebugger/J-LinkDebugger_Setup_Windows_V178.exe)
* [Eclipse Kepler](http://www.eclipse.org/downloads/download.php?file=/technology/epp/downloads/release/kepler/SR2/eclipse-cpp-kepler-SR2-win32-x86_64.zip)

Create a folder `C:\nrf51workspace` and setup the following tools:
* Unzip `nRF51_SDK_8.1.0_b6ed55f.zip` into `C:\nrf51workspace\nRF51_SDK_8.1.0`.
* Setup `C:\nrf51workspace\nRF51_SDK_8.1.0\nRF_mdk_7_2_1.msi`.
* Setup `gcc-arm-none-eabi-4_9-2015q3-20150921-win32.exe`
  At the end of the installation, select "Launch gccvar.bat", "Add path to environment variable" and
  "Add registry information".
  Close the command line.
* Setup `make-3.81.exe`
* Setup `coreutils-5.3.0.exe`
* Setup `nRF5x-Command-Line-Tools_9_5_0_Installer.exe`
* Setup `Setup_JLink_V502e.zip/Setup_JLink_V502e.exe`
* Setup `J-LinkDebugger_Setup_Windows_V178.exe`
* Unzip Eclipse in `C:\nrf51workspace` (so the Eclipse binary path is `C:\nrf51workspace\eclipse\eclipse.exe`).

Configure Eclipse:
* Start Eclipse and choose the workspace `C:\nrf51workspace\eclipseworkspace`.
* Install the plugin `embsysregview` from `http://embsysregview.sourceforge.net/update`.
* Copy `C:\nrf51workspace\nRF51_SDK_8.1.0\SVD\nrf51.xml` into `C:\nrf51workspace\eclipse\plugins\org.eclipse.cdt.embsysregview.data_0.2.5.r180\data\cortex-m0\Nordic` (create the Nordic folder).
* Install the plugins "GNU ARM C/C++ Cross Compiler" and "GNU ARM C/C++ J-Link Debugging" from `http://gnuarmeclipse.sourceforge.net/updates`.
* In window > preferences
  * Go to C/C++ > Build > Worskpace Tools Paths and set "Build tools folder" to `C:\Program Files (x86)\GnuWin32\bin`
    and "Toolchain folder" to `C:\Program Files (x86)\GNU Tools ARM Embedded\4.9 2015q3\bin`.
  * Go to C/C++ > Build > Environment, click on the "Add..." button, set "Name" to "PATH" and "Value" to `C:\Program Files (x86)\GNU Tools ARM Embedded\4.9 2015q3\bin`.
  * Go to C/C++ > Debug > EmbSys Register View, in "Architecture" select "cortex-m0", in "Vendor" select "Nordic", in "Chip" select "nrf51".
* In window > Show view > Others..., select Debug > EmbSys Registers.

Customize the nRF51 SDK:
* Open `C:\nrf51workspace\nRF51_SDK_8.1.0\components\toolchain\gcc\Makefile.windows` in [Notepad++](https://notepad-plus-plus.org/).
* Replace:

      GNU_INSTALL_ROOT := $(PROGFILES)/GNU Tools ARM Embedded/4.9 2015q1
    
* By:

      GNU_INSTALL_ROOT := C:\Program Files (x86)\GNU Tools ARM Embedded\4.9 2015q3
  
Open and build the project in Eclipse:
* In Eclipse, go to file > import > Existing Projects into Workspace, in "select root directory" put
  `local/path/to/device/firmware` and click on "finish".
* Hit CTRL+B and the project will normally be compiled with success.

To solve some missing header file errors:
* Right-click on the project > properties, select "C/C++ General > Preprocessor Include Paths, Macros etc.".
* In "Languages" select "GNU C", in "Setting Entries" select "CDT User Setting Entries" and click on "Add...".
* The following file system paths must be added:
    * `C:\nrf51workspace\nRF51_SDK_8.1.0\components\drivers_nrf\hal`
    * `C:\nrf51workspace\nRF51_SDK_8.1.0\components\libraries\util`
    * `C:\nrf51workspace\nRF51_SDK_8.1.0\components\softdevice\s110\headers`
* Hit CTRL+B to rebuild then right click the project and click Index > Rebuild to solve all symbol errors.

## Compile the firmware

### Compile and flash for the RedBearLab Nano board
A detailed tutorial about how to compile and flash the board is [available here](http://redbearlab.com/nrf51822-sdk).

To prepare the build, edit the `Makefile`, and set the `TARGET_BOARD` to `TARGET_BOARD_IS_BLE_NANO`:

    #
    # Indicate the target device (nRF51-DK or BLE Nano).
    #
    TARGET_BOARD := TARGET_BOARD_IS_BLE_NANO
    #TARGET_BOARD := TARGET_BOARD_IS_NRF51_DK


To compile with a command line, type the following commands:

    make
    make nrf51422_xxac_s110 package

Then plug the RedBearLab Nano board to the [MK20 USB board](http://redbearlab.com/blenano/#mk20usbboard) 
(the first time you will need to solder wires between the MK20 USB board and two 6 pins 0.1" male headers).

Finally copy the file `_build/nrf51422_xxac_s110_packed.hex` into the MBED drive (this USB drive appears when you
plug the MK20 USB board to the computer).

### Compile, flash and debug with the nRF51 DK board
The [nRF51 DK](https://www.nordicsemi.com/eng/Products/nRF51-DK) is very convenient for prototyping with a breadboard.

Prepare the board:
* Plug the nRF51-DK board, start it on the bootloader mode (press reset when switching it on), 
  and copy "nRF5x-OB-JLink-IF" from the [board download page](https://www.nordicsemi.com/eng/Products/nRF51-DK).
* Setup "nRFgo Studio-Win64" from the [board download page](https://www.nordicsemi.com/eng/Products/nRF51-DK).
* Download "S110-SD-v8" from the [board download page](https://www.nordicsemi.com/eng/Products/nRF51-DK) and
  program the Softdevice `s110_nrf51_8.0.0_softdevice.hex` with nRFgo Studio.

To prepare the build, edit the `Makefile`, and set the `TARGET_BOARD` to `TARGET_BOARD_IS_NRF51_DK`:

    #
    # Indicate the target device (nRF51-DK or BLE Nano).
    #
    #TARGET_BOARD := TARGET_BOARD_IS_BLE_NANO
    TARGET_BOARD := TARGET_BOARD_IS_NRF51_DK


To compile with a command line, type the following commands:

    make
    make flash_softdevice
    make nrf51422_xxac_s110 flash
      
To compile with Eclipse:
* Right-click on the project, choose "Make Targets > Build...", select "flash_softdevice" and click on "Build".
* Right-click on the project, choose "Make Targets > Build...", select "flash" and click on "Edit".
  In "Target name:" replace "flash" by "nrf51422_xxac_s110 flash".
* Click on "Build".

To debug with Eclipse:
* Right-click on the project an select "Debug As > Debug Configurations...", right click on 
  "GDB SEGGER J-Link Debugging" and select "New".
* A new line "ble_app_hrs Default" must be added. Select it.
* Under "C/C++ Application" write `_build\nrf51422_xxac_s110.out`.
* Select the debugger tab. Next to "Executable" write `C:\Program Files (x86)\SEGGER\JLink_V502\JLinkGDBServerCL.exe`.
  Next to "Device name", put `nRF51822_xxAC`.
* Select the Startup tab. Un-tick the Enable SWO box.
* Click on "Debug".
