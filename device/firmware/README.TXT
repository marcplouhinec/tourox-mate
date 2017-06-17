Group Guided Tour Assistant
===========================

Compile the firmware
--------------------

After installing your environment as described in SETUPENV.TXT, edit "main2.c" in order to define the target (uncomment the right target):
    #define TARGET_BOARD_IS_NRF51_DK
    //#define TARGET_BOARD_IS_BLE_NANO

Then open a terminal, go the the folder of this file, and run the following command:
    make


Setup the firmware on the NRF51-DK board
----------------------------------------

Run the commands:
    make flash_softdevice
    make nrf51422_xxac_s110 flash


Setup the firmware on the BLE Nano
----------------------------------

Run the commands:
    nrf51422_xxac_s110 package

Then copy the file "_build/nrf51422_xxac_s110_packed.hex" into the MBED drive.
