//This example shows how a custom VGA resolution can be created for one of the base modes
//You need to connect a VGA screen cable to the pins specified below.
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

///draws a bouncing balls
void balls() {
	//some basic gravity physics
	static VGA3BitI::Color c[4] = {vga.RGB(0, 255, 0), vga.RGB(255, 255, 0), vga.RGB(255, 0, 255), vga.RGB(0, 255, 255)};
	static float y[4] = {40, 40, 40, 40};
	static float x[4] = {40, 40, 40, 40};
	static float vx[4] = {.01, -0.07, .05, -.03};
	static float vy[4] = {0, 1, 2, 3};
	static unsigned long lastT = 0;
	unsigned long t = millis();
	float dt = (t - lastT) * 0.001f;
	lastT = t;
	const int r = 12;
	for (int i = 0; i < 4; i++)
	{
		int rx = r;
		int ry = r;
		vy[i] += -9.81f * dt * 100;
		x[i] += vx[i];
		y[i] += vy[i] * dt;
		//check for boundaries and bounce back
		if (y[i] < r && vy[i] < 0)
		{
			vy[i] = 200 + i * 10;
			ry = y[i];
		}
		if (x[i] < r && vx[i] < 0)
		{
			vx[i] = -vx[i];
			rx = x[i];
		}
		if (x[i] >= vga.xres - r && vx[i] > 0)
		{
			vx[i] = -vx[i];
			rx = vga.xres - x[i];
		}
		//draw a filled ellipse
		vga.fillEllipse(x[i], vga.yres - y[i] - 1, rx, ry, c[i]);
		vga.ellipse(x[i], vga.yres - y[i] - 1, rx, ry, 0);
	}
}
//initial setup
void setup()
{
	vga.setFrameBufferCount(2);
	vga.init(vga.MODE640x480.custom(320, 240), redPin, greenPin, bluePin, hsyncPin, vsyncPin);
	//using adafruit gfx
  gfx.setFont(&atawi8x6);
}

//the loop is done every frame
void loop()
{
	//draw a background
	for (int y = 0; y * 10 < vga.yres; y++)
		for (int x = 0; x * 10 < vga.xres; x++)
			vga.fillRect(x * 10, y * 10, 10, 10, (x + y) & 1 ? vga.RGB(255, 0, 0) : vga.RGB(255, 255, 255));
	//text position
	gfx.setCursor(40, 2);
  gfx.setTextSize(2);
	gfx.setTextColor(0x001F);
	//show the remaining memory
	gfx.print(vga.xres);
	gfx.print("x");
	gfx.println(vga.yres);
	gfx.print("free memory: ");
	gfx.print((int)heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
	//draw bouncing balls
	balls();
	//show the backbuffer (only needed when using backbuffering)
	vga.show();
  
}