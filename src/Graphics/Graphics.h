/*
	Author: bitluni 2019
	License: 
	Creative Commons Attribution ShareAlike 4.0
	https://creativecommons.org/licenses/by-sa/4.0/
	
	For further details check out: 
		https://youtube.com/bitlunislab
		https://github.com/bitluni
		http://bitluni.net
*/
#pragma once
#include <stdlib.h>

template<typename Color>
class Graphics
{
  public:
	int cursorX, cursorY, cursorBaseX;
	long frontColor, backColor;
	int frameBufferCount;
	int currentFrameBuffer;
	Color **frameBuffers[3];
	Color **frontBuffer;
	Color **backBuffer;
	bool autoScroll;

	int xres;
	int yres;

	virtual void dotFast(int x, int y, Color color) = 0;
	virtual void dot(int x, int y, Color color) = 0;
	virtual void dotAdd(int x, int y, Color color) = 0;
	virtual void dotMix(int x, int y, Color color) = 0;
	virtual void clear(Color color) = 0;
	virtual Color get(int x, int y) = 0;
	virtual Color** allocateFrameBuffer() = 0;
	virtual Color** allocateFrameBuffer(int xres, int yres, Color value)
	{
		Color** frame = (Color **)malloc(yres * sizeof(Color *));
		if(!frame)
			ERROR("Not enough memory for frame buffer");				
		for (int y = 0; y < yres; y++)
		{
			frame[y] = (Color *)malloc(xres * sizeof(Color));
			if(!frame[y])
				ERROR("Not enough memory for frame buffer");
			for (int x = 0; x < xres; x++)
				frame[y][x] = value;
		}
		return frame;
	}
	virtual Color RGBA(int r, int g, int b, int a = 255) const = 0;
	virtual int R(Color c) const = 0;
	virtual int G(Color c) const = 0;
	virtual int B(Color c) const = 0;
	virtual int A(Color c) const = 0;
	Color RGB(unsigned long rgb) const 
	{
		return RGBA(rgb & 255, (rgb >> 8) & 255, (rgb >> 16) & 255);
	}
	Color RGBA(unsigned long rgba) const 
	{
		return RGBA(rgba & 255, (rgba >> 8) & 255, (rgba >> 16) & 255, rgba >> 24);
	}
	Color RGB(int r, int g, int b) const 
	{
		return RGBA(r, g, b);
	}

	void setFrameBufferCount(unsigned char i)
	{
		frameBufferCount = i > 3 ? 3 : i;
	}

	virtual void show(bool vSync = false)
	{
		if(!frameBufferCount)
			return;
		currentFrameBuffer = (currentFrameBuffer + 1) % frameBufferCount;
		frontBuffer = frameBuffers[currentFrameBuffer];
		backBuffer = frameBuffers[(currentFrameBuffer + frameBufferCount - 1) % frameBufferCount];
	}

	Graphics(int xres = 0, int yres = 0)
	{
		this->xres = xres;
		this->yres = yres;
		cursorX = cursorY = cursorBaseX = 0;
		frontColor = -1;
		backColor = 0;
		frameBufferCount = 1;
		for(int i = 0; i < 3; i++)
			frameBuffers[i] = 0;
		frontBuffer = 0;
		backBuffer = 0;
		autoScroll = true;
	}

	virtual bool allocateFrameBuffers()
	{
		if(yres <= 0 || xres <= 0)
			return false;
		for(int i = 0; i < frameBufferCount; i++)
			frameBuffers[i] = allocateFrameBuffer();
		currentFrameBuffer = 0;
		show();
		return true;
	}

	virtual void setResolution(int xres, int yres)
	{
		this->xres = xres;
		this->yres = yres;
		allocateFrameBuffers();
	}

	virtual float pixelAspect() const
	{
		return 1;
	}


	virtual void xLine(int x0, int x1, int y, Color color)
	{
		if (y < 0 || y >= yres)
			return;
		if (x0 > x1)
		{
			int xb = x0;
			x0 = x1;
			x1 = xb;
		}
		if (x0 < 0)
			x0 = 0;
		if (x1 > xres)
			x1 = xres;
		for (int x = x0; x < x1; x++)
			dotFast(x, y, color);
	}

	void triangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color)
	{
          line(x1, y1, x2, y2, color);
          line(x2, y2, x3, y3, color);
          line(x3, y3, x1, y1, color);
	}

	void line(int x1, int y1, int x2, int y2, Color color)
	{
		int x, y, xe, ye;
		int dx = x2 - x1;
		int dy = y2 - y1;
		int dx1 = labs(dx);
		int dy1 = labs(dy);
		int px = 2 * dy1 - dx1;
		int py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}
			dot(x, y, color);
			for (int i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
				{
					px = px + 2 * dy1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						y = y + 1;
					}
					else
					{
						y = y - 1;
					}
					px = px + 2 * (dy1 - dx1);
				}
				dot(x, y, color);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}
			dot(x, y, color);
			for (int i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
				{
					py = py + 2 * dx1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						x = x + 1;
					}
					else
					{
						x = x - 1;
					}
					py = py + 2 * (dx1 - dy1);
				}
				dot(x, y, color);
			}
		}
	}

	void fillRect(int x, int y, int w, int h, Color color)
	{
		if (x < 0)
		{
			w += x;
			x = 0;
		}
		if (y < 0)
		{
			h += y;
			y = 0;
		}
		if (x + w > xres)
			w = xres - x;
		if (y + h > yres)
			h = yres - y;
		for (int j = y; j < y + h; j++)
			for (int i = x; i < x + w; i++)
				dotFast(i, j, color);
	}

	void rect(int x, int y, int w, int h, Color color)
	{
		fillRect(x, y, w, 1, color);
		fillRect(x, y, 1, h, color);
		fillRect(x, y + h - 1, w, 1, color);
		fillRect(x + w - 1, y, 1, h, color);
	}

	void circle(int x, int y, int r, Color color)
	{
		int oxr = r;
		for(int i = 0; i < r + 1; i++)
		{
			int xr = (int)sqrt(r * r - i * i);
			xLine(x - oxr, x - xr + 1, y + i, color);
			xLine(x + xr, x + oxr + 1, y + i, color);
			if(i) 
			{
				xLine(x - oxr, x - xr + 1, y - i, color);
				xLine(x + xr, x + oxr + 1, y - i, color);
			}
			oxr = xr;
		}
	}

	void fillCircle(int x, int y, int r, Color color)
	{
		for(int i = 0; i < r + 1; i++)
		{
			int xr = (int)sqrt(r * r - i * i);
			xLine(x - xr, x + xr + 1, y + i, color);
			if(i) 
				xLine(x - xr, x + xr + 1, y - i, color);
		}
	}

	void ellipse(int x, int y, int rx, int ry, Color color)
	{
		if(ry == 0)
			return;
		int oxr = rx;
		float f = float(rx) / ry;
		f *= f;
		for(int i = 0; i < ry + 1; i++)
		{
			float s = rx * rx - i * i * f;
			int xr = (int)sqrt(s <= 0 ? 0 : s);
			xLine(x - oxr, x - xr + 1, y + i, color);
			xLine(x + xr, x + oxr + 1, y + i, color);
			if(i) 
			{
				xLine(x - oxr, x - xr + 1, y - i, color);
				xLine(x + xr, x + oxr + 1, y - i, color);
			}
			oxr = xr;
		}
	}

	void fillEllipse(int x, int y, int rx, int ry, Color color)
	{
		if(ry == 0)
			return;
		float f = float(rx) / ry;
		f *= f;		
		for(int i = 0; i < ry + 1; i++)
		{
			float s = rx * rx - i * i * f;
			int xr = (int)sqrt(s <= 0 ? 0 : s);
			xLine(x - xr, x + xr + 1, y + i, color);
			if(i) 
				xLine(x - xr, x + xr + 1, y - i, color);
		}
	}

	virtual void scroll(int dy, Color color)
	{
		if(dy > 0)
		{
			for(int d = 0; d < dy; d++)
			{
				Color *l = backBuffer[0];
				for(int i = 0; i < yres - 1; i++)
				{
					backBuffer[i] = backBuffer[i + 1];
				}
				backBuffer[yres - 1] = l;
				xLine(0, xres, yres - 1, color);
			}
		}
		else
		{
			for(int d = 0; d < -dy; d++)
			{
				Color *l = backBuffer[yres - 1];
				for(int i = 1; i < yres; i++)
				{
					backBuffer[i] = backBuffer[i - 1];
				}
				backBuffer[0] = l;
				xLine(0, xres, 0, color);
			}
		}
		cursorY -= dy;
	}

	virtual Color R5G5B4A2ToColor(unsigned short c)
	{
		int r = (((c << 1) & 0x3e) * 255 + 1) / 0x3e;
		int g = (((c >> 4) & 0x3e) * 255 + 1) / 0x3e;
		int b = (((c >> 9) & 0x1e) * 255 + 1) / 0x1e;
		int a = (((c >> 13) & 6) * 255 + 1) / 6;
		return RGBA(r, g, b, a);
	}

	virtual Color R2G2B2A2ToColor(unsigned char c)
	{
		int r = ((int(c) & 3) * 255 + 1) / 3;
		int g = (((int(c) >> 2) & 3) * 255 + 1) / 3;
		int b = (((int(c) >> 4) & 3) * 255 + 1) / 3;
		int a = (((int(c) >> 6) & 3) * 255 + 1) / 3;
		return RGBA(r, g, b, a);
	}

	virtual Color R4G4B4A4ToColor(unsigned short c)
	{
		int r = (((c << 1) & 0x1e) * 255 + 1) / 0x1e;
		int g = (((c >> 3) & 0x1e) * 255 + 1) / 0x1e;
		int b = (((c >> 7) & 0x1e) * 255 + 1) / 0x1e;
		int a = (((c >> 11) & 0x1e) * 255 + 1) / 0x1e;
		return RGBA(r, g, b, a);
	}

	virtual Color R8G8B8A8ToColor(unsigned long c)
	{
		return RGBA(c & 255, (c >> 8) & 255, (c >> 16) & 255, (c >> 24) & 255);
	}

};
