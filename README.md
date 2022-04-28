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

# More info
See https://jaxeadv.com/gear-gadgets/navigation-setup/handlebar-buttons/
