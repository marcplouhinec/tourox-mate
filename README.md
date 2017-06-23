# Tourox Mate

## Introduction
![Product overview](readme_resources/tourox-overview.png?raw=true "Product overview")

Tourox Mate is a device that helps tour guides during group travels. It is worn by each participant with a lanyard.
Each device contains a chip that connects it to internet via the cellular network. The tour guide is thus in permanent
contact with each member of his group via his smartphone.

Tourox Mate provides the following features:
* During travels with a great number of people, the tour guide is immediately warned if a member of the group doesn't
  follow anymore or leave unexpectedly.
  
  ![Use case: person not following](readme_resources/use-case-person-not-following.png?raw=true "Use case: person not following")
* During outdoor visits, it happens that the tour guide allows his group members to explore a place by themselves
  (market, city-center, ...) during a given period of time before gathering again in order to resume the visit.
  Thanks to its internet connection, Tourox Mate can:
  * Allow the tour guide to visualize in real-time the geographical position of each group member on a map.
  
    ![Use case: get tourist location](readme_resources/use-case-get-tourist-locations-during-free-time.png?raw=true "Use case: get tourist location")
  * Let a group member to call the guide in case of a problem.
  
    ![Use case: tourist calls guide](readme_resources/use-case-tourist-call-guide.png?raw=true "Use case: tourist calls guide")
  * Let the guide to call his group members in case they are late or too far.
  
    ![Use case: guide calls tourist](readme_resources/use-case-guide-call-tourist.png?raw=true "Use case: guide calls tourist")
* Finally, Tourox Mate can also replace voice transmission devices.

  ![Use case: voice transmitter](readme_resources/use-case-tour-guide-voice-transmitter.png?raw=true "Use case: voice transmitter")

Technically, Tourox Mate is made of the following parts:
 * A physical device, made with electronics, a 3D printed case and a firmware.
 * A web-server collecting information from devices over internet.
 * An Android application for the Tour Guide that allows him to configure and interact with the physical devices.
 
## Build
Please follow the following instructions if you want to build Tourox Mate yourself:
1. [Build the physical device.](device/README.md)
2. [Build and setup the web-server](webserver/README.md)
2. [Build and setup the tour guide Android application](android/README.md)

## Usage
The current version is a prototype so only the basic features are available.

To start using Tourox Mate, turn on the device with the slide switch and open the "Tourox Device Tester" app on the
Android device. The Android application scans for bluetooth devices and quickly find the Tourox Mate:

![Device next to Android phone](readme_resources/photo_device_next_to_android_phone.jpg?raw=true "Device next to Android phone")

When you select this device, the following screen opens:

![Device activity](readme_resources/device_activity_small.png?raw=true "Device activity")

It allows you to execute the following actions:
* Set the device time.
* Set the proximity level that reflects the distance between the tour guide and a tourist wearing the device.
* Configure a tour with the following properties:
  * The SIM card PIN number.
  * The [APN](https://en.wikipedia.org/wiki/Access_Point_Name) with the corresponding username and password.
    These values can be found on the telecom provider contract or support website.
  * The tour guide phone number. It is the number the device will call when the tourist presses the "HELP" button.
* Clear the tour configuration.
* Cancel a help request on the device.
* Schedule a gathering with the following properties:
  * The gathering time.
  * A gathering text description.
* Un-schedule a gathering.

To start using the device, set the time and proximity level, and configure a tour:

![Tour configured](readme_resources/photo_tour_configured.jpg?raw=true "Tour configured")

From the device, a tourist can call the tour guide by pressing on the help button:

![Tourist calls guide](readme_resources/photo_tourist_calls_guide.jpg?raw=true "Tourist calls guide")

In a similar way, the tour guide can also call the device by dialing its number:

![Guide calls tourist](readme_resources/photo_guide_calls_tourist.jpg?raw=true "Guide calls tourist")

The tour guide can see the device location by browsing to the webserver URL:

![Map with devices](readme_resources/map_with_devices.png?raw=true "Map with devices")

When the tour guide schedules a gathering, the device shows the gathering description:

![Gathering](readme_resources/photo_gathering.jpg?raw=true "Gathering")

Canceling a gathering reverts the device to its normal state.

Finally, to disconnect the device from internet, the tour guide can clear the tour configuration.

___

Thank you for reading! don't hesitate to contact me if you have any remark or question!
