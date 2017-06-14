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
2. Build and setup the web-server (coming soon...)
2. Build and setup the tour guide Android application (coming soon...)

## Test
Coming soon...