![wiring](https://raw.githubusercontent.com/joostbijl/navibox/main/circuit%20and%20buttons.png)

## Buy it
You need these items to assemble a navibox unit:
|Component|Amount|Notes|
|---|---|---|
|Buttons|8|https://www.aliexpress.com/item/32672509409.html|
|Led|1|https://www.aliexpress.com/item/1000007251357.html|
|Wemos C3 mini|1|https://www.wemos.cc/en/latest/c3/c3_mini.html|
|USB cable||A USB cable that you can use to provide power to the arduino|
|UTP cable||short section of wire (10-15cm) to solder buttons to the arduino|
|Screws for mounting the arduino|2|6mm length, 2,0mm width, philips head|
|Screws for case assembly|4|12,6mm length, 2,8mm width, philips head|
|M4 nut|2|Designed for the regular variety, nylock will extrude a bit|
|M4x25 hex screw|2|To mount navibox to your handlebar. If you use 1 or more spacers, add 5mm per spacer|

## Print it
* Use the 3mf files from the 3dfiles-v2 directory
* print with a wall thickness of 4 layers minimum so that the unit is sturdy
* Use a small print width of ~0.12mm, this helps in preventing delamination
* Orientation on the print bed:
  * Front: print with the front facing upward. This way the front looks best. You do need to remove the support from the inside though.
  * Back: print with the back facing downward. This way you only have to remove the support for the overhang that makes room for the mirror
  * Bracket, spacer, adapter: prin sideways to prevent lamination.
* Use a 4mm drill to clean up the holes
* Remove any support 

## Flash it
Program the Arduino. To use a Wemos C3 Mini board, follow these steps:
* Install https://github.com/T-vK/ESP32-BLE-Keyboard 0.3.1-beta or higher. Don't use NimBLE support since this did not work for me
* Install keypad library
* Install ESP32 board library, at least version 2.0.3rc1 to support C3. Add one of these URLs as additional board locations in the Arduino IDE preferences
  * Stable version: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
  * Development preview `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json`
* Update partition scheme (in order to facilitate OTA updates)
  * Exit the Arduino IDE
  * Update partition scheme in `Userdirectory:AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.3-RC1\boards.txt` to add an option for minimal spiffs to accomodate larger images by adding these lines:
    * `lolin_c3_mini.menu.PartitionScheme.min_spiffs=Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)`
    * `lolin_c3_mini.menu.PartitionScheme.min_spiffs.build.partitions=min_spiffs`
    * `lolin_c3_mini.menu.PartitionScheme.min_spiffs.upload.maximum_size=1966080`
  * Remove the directory `Userdirectory:/AppData/Roaming/arduino-ide/` (due to a [bug](https://github.com/arduino/arduino-ide/issues/1030) in Arduino IDE)
  * Start Arduino IDE
  * Choose LOLIN C3 mini board in IDE
  * Set partition scheme to 'Minimal SPIFFS' (after restart of IDE) (If the partition scheme is not available, restart Arduino IDE one more time)
* Before uploading from Arduino IDE, press and hold button 9, press and release RST, release button 9 when you hear the 'new device' sound.

## Solder it
* The buttons are connected in a keymap configuration to the arduino:
  * Use the diagram above as a guide for connecting the buttons
  * Note that the terminals of buttons are also connected. 
  * For example: the two terminals of button number 4 have both 2 wires connected
    * One terminal has the dashed green wires: one goes to button 1, the other to button 7.=
    * One terminal has the blue wires: one goes to the arduino on pin 1, the other goes to button nr 5
* The led is connected to gnd and pin 6
* You can use utp cable wiring because it's color coded
* For power supply you can connect a USB cable. Cut off the micro/mini/usb-c connector of a regular charging cable.
    * Connect the black wire to the gnd pin on the Arduino board
    * Connect the red wire to the vbus pin on the Arduino board
    * Make sure to route the cable through the housing before soldering

## Assemble it
* Mount the Wemos C3 mini Arduino with 2mm screws of 6mm lenght (2x) to the inside of the case
* Put 2 m4 nuts in the recesses. Press pull them, or heat them slightly with a soldering iron
* Put the front (with the buttons) on the back, and be careful not to pinch the wires to the buttons

## Mount it
* Measure the space needed between the handlebar and the controls. If it's just a mirror it's usually ~10mm. If it's a bit control it can be more
* Use spacer to get enough room for the navibox
* Mount with an M4 screw. If you have no spacers, use a 25mmscrew, otherwise use longer screws. Be careful with tightening it, it's only M4.

## Use it
See https://jaxeadv.com/gear-gadgets/navigation-setup/handlebar-buttons/
