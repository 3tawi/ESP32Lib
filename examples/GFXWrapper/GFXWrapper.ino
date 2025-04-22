//This example shows how to use the GfxWrapper to be able to use the Adafruit GFX library with VGA
//cc by-sa 4.0 license
//bitluni
/*
 * GP_GFX library which can be found at  
    * https://github.com/3tawi/GP_GFX 
*/

#include <ESP32VGA3Bit.h>
#include <GP_GFX.h>
#include <VGAWrapper.h>
#include <GPfonts/atawi8x6.h>

//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 32;
const int vsyncPin = 33;

//VGA Device
VGA3BitI vga;
VGAWrapper<VGA3BitI> gfx(vga, 639, 399);

//initial setup
void setup()
{
	vga.init(vga.MODE720x400.custom(639, 399), redPin, greenPin, bluePin, hsyncPin, vsyncPin);
	//using adafruit gfx
  gfx.setFont(&atawi8x6);
  gfx.setCursor(100, 100);
  gfx.print("Hello");
}

//the loop is done every frame
void loop()
{
  
}
