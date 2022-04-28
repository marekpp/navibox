# navibox
Arduino code to control phone navigation via handlebar buttons

## Circuit
Connect the circuit from the buttons to the Arduino like this:
![wiring](https://raw.githubusercontent.com/joostbijl/navibox/main/navibox%20circuit.png)

The color coding assumes you are using a UTP cable.

## Status led
![status led](https://raw.githubusercontent.com/joostbijl/navibox/main/navibox%20status%20led.png)

## Keys
|key|Short press|Long press|
|---|---|---|
| 1 | Zoom in (vol up) | Repeat |
| 4 | Zoom out (vol down) | Repeat |
| 2 | Pan up (dpad up) | Repeat |
| 2 | Pan down (dpad down) | Repeat |
| 5 | Pan left (dpad left) | Repeat |
| 6 | Pan right (dpad right) | Repeat |
| 7 | Center map (c) | Change map view (d) |
| 3 | Start/stop Navigation (n) | Start OTA update |

## Case
In the 'navibox' directory are the 3D files to use with a 3D printer. 

Hints for best results:
* Print the back of the case with the back on the plate. This way you only have to remove the support for the overhang that makes room for the mirror
* Print the front of the case with the front facing up. This way the front looks best. You do need to remove the support from the inside though.

## Arduino code
The code can run on any ESP32 that has bluetooth capabilities.

To use a Wemos C3 Mini board, follow these steps:
* Install https://github.com/T-vK/ESP32-BLE-Keyboard 0.3.1-beta or higher. Don't use NimBLE support since this did not work for me
* Install keypad library
* Update ESP32 board library to at least 2.0.3rc1 to support C3 - add https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json to additional board url in preferences
* Choose LOLIN C3 mini board in IDE
* Update partition scheme in Userdirectory: AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.3-RC1\boards.txt to add an option for minimal spiffs to accomodate larger images by adding these lines:
  * lolin_c3_mini.menu.PartitionScheme.min_spiffs=Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS) 
  * lolin_c3_mini.menu.PartitionScheme.min_spiffs.build.partitions=min_spiffs
  * lolin_c3_mini.menu.PartitionScheme.min_spiffs.upload.maximum_size=1966080
* Set partition scheme to 'Minimal SPIFFS' (after restart of IDE)
* Before uploading from Arduino IDE, press and hold button 9, press and release RST, release button 9 when you hear the 'new device' sound.



# More info
See https://jaxeadv.com/gear-gadgets/navigation-setup/handlebar-buttons/
