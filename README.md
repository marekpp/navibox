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
* Update partition scheme in order to facilitate OTA updates
  * Exit the Arduino IDE
  * Update partition scheme in `Userdirectory:AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.3-RC1\boards.txt` to add an option for minimal spiffs to accomodate larger images by adding these lines:
    * `lolin_c3_mini.menu.PartitionScheme.min_spiffs=Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)`
    * `lolin_c3_mini.menu.PartitionScheme.min_spiffs.build.partitions=min_spiffs`
    * `lolin_c3_mini.menu.PartitionScheme.min_spiffs.upload.maximum_size=1966080`
  * Remove the directory `Userdirectory:/AppData/Roaming/arduino-ide/` (due to a [bug](https://github.com/arduino/arduino-ide/issues/1030))
  * Start Arduino IDE
  * Choose LOLIN C3 mini board in IDE
  * Set partition scheme to 'Minimal SPIFFS' (after restart of IDE) (If the partition scheme is not available, restart Arduino IDE one more time)
* Before uploading from Arduino IDE, press and hold button 9, press and release RST, release button 9 when you hear the 'new device' sound.

## Component list
You need these items to assemble a navibox unit:
|Component|Amount|Notes|
|---|---|---|
|Buttons|8|https://www.aliexpress.com/item/32672509409.html?spm=a2g0o.9042311.0.0.27424c4dFWvWvS|
|Led|1|https://www.aliexpress.com/item/1000007251357.html?spm=a2g0o.9042311.0.0.27424c4dFWvWvS|
|Wemos C3 mini|1|https://www.wemos.cc/en/latest/c3/c3_mini.html|
|Buck converter|1|https://www.aliexpress.com/item/32817933017.html?spm=a2g0o.9042311.0.0.27424c4dFWvWvS|
|Fuse holder|1|https://www.aliexpress.com/item/32984778910.html?spm=a2g0o.9042311.0.0.27424c4dwZ6pHM|
|Battery terminal connectors|2|To securely connect to the on-board power of your bike|
|Power cable||I  have used red/black speaker cable, length dependent on your setup|
|UTP cable||Length dependent on your setup|
|Screws for case assembly|4|12,6mm length, 2,8mm width, philips head|
|M4 nut|2|Designed for the regular variety, nylock will extrude a bit|
|M4x25 hex screw|2|To mount navibox to your handlebar. If you use 1 or more spacers, add 5mm per spacer|


# More info
See https://jaxeadv.com/gear-gadgets/navigation-setup/handlebar-buttons/
