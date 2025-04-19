### ESP32VGA3Bit
based on the ESP32Lib & Adafruit_GFX

 original library which can be found at :
 
##  https://github.com/bitluni/ESP32Lib
   
##   https://github.com/adafruit/Adafruit-GFX-Library


## Installation

This library only supports the ESP32.
I be able to install the ESP32 features in the board manager you need to add an additional Boards Manager URL in the Preferences (File -> Preferences)
```
https://dl.espressif.com/dl/package_esp32_index.json
```
The ESP32Lib can be found in the Library Manager (Sketch -> Include Library -> Manage Libaries)
To be able to use the library featues the main header needs to included in the sketch
```cpp
#include <    ESP32VGA3Bit.h>
```

### Pin configuration

An VGA cable can be used to connect to the ESP32
The connector pins can be found here: https://en.wikipedia.org/wiki/VGA_connector
The 3Bit modes are very easy to set up. You can connect 
the Ground, Red, Green, Blue, hSync and vSync to output pins of the ESP32.

![3Bit color setup](/Documentation/schematic3bit.png)

The R, G and B pins are passed as single integers for the 3Bit version. Please try the examples
The following modes are predefined:
- MODE720x400
- MODE640x480
- MODE640x400
