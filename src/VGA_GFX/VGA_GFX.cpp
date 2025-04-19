
#include "VGA_GFX.h"


VGA_GFX::VGA_GFX(int16_t w, int16_t h) : WIDTH(w), HEIGHT(h) {
  _width = WIDTH;
  _height = HEIGHT;
  rotation = 0;
  cursor_y = cursor_x = 0;
  textsize_x = textsize_y = 1;
  textcolor = textbgcolor = 0xFFFF;
  wrap = true;
  _cp437 = false;
  gfxFont = NULL;
}

  void VGA_GFX::writePixel(int x, int y, uint16_t color) {
    if (x < 0 || x >= _width || y < 0 || y >= _height)
      return;
    int16_t t;
    switch (rotation) {
      case 1:
        t = x;
        x = WIDTH - 1 - y;
        y = t;
        break;
      case 2:
        x = WIDTH - 1 - x;
        y = HEIGHT - 1 - y;
        break;
      case 3:
        t = x;
        x = y;
        y = HEIGHT - 1 - t;
        break;
    }
    drawPixel(x, y, color);
  }

  void VGA_GFX::xLine(int x0, int x1, int y, uint16_t color) {
    if (y < 0 || y >= _height)
      return;
    if (x0 > x1) {
      int xb = x0;
      x0 = x1;
      x1 = xb;
    }
    if (x0 < 0)
      x0 = 0;
    if (x1 > _width)
      x1 = _width;
    for (int x = x0; x < x1; x++)
      writePixel(x, y, color);
  }

  void VGA_GFX::drawLine(int x1, int y1, int x2, int y2, uint16_t color) {
    int x, y, xe, ye;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int dx1 = labs(dx);
    int dy1 = labs(dy);
    int px = 2 * dy1 - dx1;
    int py = 2 * dx1 - dy1;
    if (dy1 <= dx1) {
      if (dx >= 0) {
        x = x1;
        y = y1;
        xe = x2;
      } else {
        x = x2;
        y = y2;
        xe = x1;
      }
      writePixel(x, y, color);
      for (int i = 0; x < xe; i++) {
        x = x + 1;
        if (px < 0) {
          px = px + 2 * dy1;
        } else {
          if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
            y = y + 1;
          } else {
            y = y - 1;
          }
          px = px + 2 * (dy1 - dx1);
        }
        writePixel(x, y, color);
      }
    } else {
      if (dy >= 0) {
        x = x1;
        y = y1;
        ye = y2;
      } else {
        x = x2;
        y = y2;
        ye = y1;
      }
      writePixel(x, y, color);
      for (int i = 0; y < ye; i++) {
        y = y + 1;
        if (py <= 0) {
          py = py + 2 * dx1;
        } else {
          if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
            x = x + 1;
          } else {
            x = x - 1;
          }
          py = py + 2 * (dx1 - dy1);
        }
        writePixel(x, y, color);
      }
    }
  }

  void VGA_GFX::fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
  }

  void VGA_GFX::fillRect(int x, int y, int w, int h, uint16_t color) {
    if (x < 0) {
      w += x;
      x = 0;
    }
    if (y < 0) {
      h += y;
      y = 0;
    }
    if (x + w > _width)
      w = _width - x;
    if (y + h > _height)
      h = _height - y;
    for (int j = y; j < y + h; j++)
      for (int i = x; i < x + w; i++)
        writePixel(i, j, color);
  }

  void VGA_GFX::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
  }
  void VGA_GFX::fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
      _swap_int16_t(y0, y1);
      _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
      _swap_int16_t(y2, y1);
      _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
      _swap_int16_t(y0, y1);
      _swap_int16_t(x0, x1);
    }

    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
      a = b = x0;
      if (x1 < a)
        a = x1;
      else if (x1 > b)
        b = x1;
      if (x2 < a)
        a = x2;
      else if (x2 > b)
        b = x2;
      fillRect(a, y0, b - a + 1, 1, color);
      return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
    if (y1 == y2)
      last = y1; // Include y1 scanline
    else
      last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++) {
      a = x0 + sa / dy01;
      b = x0 + sb / dy02;
      sa += dx01;
      sb += dx02;
      /* longhand:
      a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
      b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
      */
      if (a > b)
        _swap_int16_t(a, b);
      fillRect(a, y, b - a + 1, 1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for (; y <= y2; y++) {
      a = x1 + sa / dy12;
      b = x0 + sb / dy02;
      sa += dx12;
      sb += dx02;
      /* longhand:
      a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
      b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
      */
      if (a > b)
        _swap_int16_t(a, b);
      fillRect(a, y, b - a + 1, 1, color);
    }
  }

  void VGA_GFX::circle(int x, int y, int r, uint16_t color) {
    int oxr = r;
    for(int i = 0; i < r + 1; i++) {
      int xr = (int)sqrt(r * r - i * i);
      xLine(x - oxr, x - xr + 1, y + i, color);
      xLine(x + xr, x + oxr + 1, y + i, color);
      if(i) {
        xLine(x - oxr, x - xr + 1, y - i, color);
        xLine(x + xr, x + oxr + 1, y - i, color);
      }
      oxr = xr;
    }
  }

  void VGA_GFX::fillCircle(int x, int y, int r, uint16_t color) {
    for(int i = 0; i < r + 1; i++) {
      int xr = (int)sqrt(r * r - i * i);
      xLine(x - xr, x + xr + 1, y + i, color);
      if(i) 
        xLine(x - xr, x + xr + 1, y - i, color);
    }
  }

  void VGA_GFX::ellipse(int x, int y, int rx, int ry, uint16_t color)  {
    if(ry == 0)
      return;
    int oxr = rx;
    float f = float(rx) / ry;
    f *= f;
    for(int i = 0; i < ry + 1; i++) {
      float s = rx * rx - i * i * f;
      int xr = (int)sqrt(s <= 0 ? 0 : s);
      xLine(x - oxr, x - xr + 1, y + i, color);
      xLine(x + xr, x + oxr + 1, y + i, color);
      if(i) {
        xLine(x - oxr, x - xr + 1, y - i, color);
        xLine(x + xr, x + oxr + 1, y - i, color);
      }
      oxr = xr;
    }
  }
  void VGA_GFX::fillEllipse(int x, int y, int rx, int ry, uint16_t color) {
    if(ry == 0)
      return;
    float f = float(rx) / ry;
    f *= f;		
    for(int i = 0; i < ry + 1; i++) {
      float s = rx * rx - i * i * f;
      int xr = (int)sqrt(s <= 0 ? 0 : s);
      xLine(x - xr, x + xr + 1, y + i, color);
      if(i) 
        xLine(x - xr, x + xr + 1, y - i, color);
    }
  }

  void VGA_GFX::Demiellipse(int x, int y, int rx, int ry, uint16_t color, bool sen) {
    if(ry == 0)
      return;
    int oxr = rx;
    float f = float(rx) / ry;
    f *= f;
    for(int i = 0; i < ry + 1; i++)
    {
      float s = rx * rx - i * i * f;
      int xr = (int)sqrt(s <= 0 ? 0 : s);
      if(sen) {
        xLine(x - oxr, x - xr + 1, y - i, color);
        xLine(x + xr, x + oxr + 1, y - i, color);
      } else {
        xLine(x - oxr, x - xr + 1, y + i, color);
        xLine(x + xr, x + oxr + 1, y + i, color);
      }
      oxr = xr;
    }
}
  void VGA_GFX::DemifillEllipse(int x, int y, int rx, int ry, uint16_t color, bool sen) {
    if(ry == 0)
      return;
    float f = float(rx) / ry;
    f *= f;		
    for(int i = 0; i < ry + 1; i++) {
      float s = rx * rx - i * i * f;
      int xr = (int)sqrt(s <= 0 ? 0 : s);
      if(sen)
        xLine(x - xr, x + xr + 1, y - i, color);
      else 
        xLine(x - xr, x + xr + 1, y + i, color);
    }
  }

  void VGA_GFX::DemifillCircle(int x, int y, int r, uint16_t color, bool sen) {
    for(int i = 0; i < r + 1; i++) {
      int xr = (int)sqrt(r * r - i * i);
      if(sen)
        xLine(x - xr, x + xr + 1, y - i, color);
      else 
        xLine(x - xr, x + xr + 1, y + i, color);
    }
  }
  void VGA_GFX::Demicircle(int x, int y, int r, uint16_t color, bool sen) {
    int oxr = r;
    for(int i = 0; i < r + 1; i++) {
      int xr = (int)sqrt(r * r - i * i);
      if(sen) {
        xLine(x - oxr, x - xr + 1, y - i, color);
        xLine(x + xr, x + oxr + 1, y - i, color);
      } else {
        xLine(x - oxr, x - xr + 1, y + i, color);
        xLine(x + xr, x + oxr + 1, y + i, color);
      }
      oxr = xr;
    }
  }

  void VGA_GFX::fillCare(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color) {
    fillTriangle(x0, y0, x1, y1, x2, y2, color);
    fillTriangle(x2, y2, x3, y3, x0, y0, color);
  }

  void VGA_GFX::DrawCare(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x3, y3, color);
    drawLine(x3, y3, x0, y0, color);
  }

  void VGA_GFX::fillParallelogram(int x, int y, int xr, int yr, uint16_t color) {
    int x0 = x - xr, x1 = x + xr;
    int y0 = y - yr, y1 = y + yr;
    fillTriangle(x0, y, x, y0, x, y1, color);
    fillTriangle(x, y0, x, y1, x1, y, color);
  }

  void VGA_GFX::parallelogram(int x, int y, int xr, int yr, uint16_t color) {
    int x0 = x - xr, x1 = x + xr;
    int y0 = y - yr, y1 = y + yr;
    drawLine(x0, y, x, y0, color);
    drawLine(x, y0, x1, y, color);
    drawLine(x1, y, x, y1, color);
    drawLine(x, y1, x0, y, color);
  }

  void VGA_GFX::DemifillParall(int x, int y, int xr, int yr, uint16_t color, bool sen) {
    int x0 = x - xr, x1 = x + xr;
    int y0 = y - yr, y1 = y + yr;
    if(sen)
      fillTriangle(x0, y, x, y0, x1, y, color);
    else
      fillTriangle(x0, y, x, y1, x1, y, color);
  }

  void VGA_GFX::Demiparall(int x, int y, int xr, int yr, uint16_t color, bool sen) {
    int x0 = x - xr, x1 = x + xr;
    int y0 = y - yr, y1 = y + yr;
    if(sen) {
    drawLine(x0, y, x, y0, color);
    drawLine(x0, y, x1, y, color);
    drawLine(x, y0, x1, y, color);
    }
    else {
    drawLine(x0, y, x, y1, color);
    drawLine(x0, y, x1, y, color);
    drawLine(x, y1, x1, y, color);
    }
  }
  void VGA_GFX::drawCircleHelper(int x0, int y0, int r, uint8_t cornername, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x;
      if (cornername & 0x4) {
        writePixel(x0 + x, y0 + y, color);
        writePixel(x0 + y, y0 + x, color);
      }
      if (cornername & 0x2) {
        writePixel(x0 + x, y0 - y, color);
        writePixel(x0 + y, y0 - x, color);
      }
      if (cornername & 0x8) {
        writePixel(x0 - y, y0 + x, color);
        writePixel(x0 - x, y0 + y, color);
      }
      if (cornername & 0x1) {
        writePixel(x0 - y, y0 - x, color);
        writePixel(x0 - x, y0 - y, color);
      }
    }
  }
  void VGA_GFX::fillCircleHelper(int x0, int y0, int r, uint8_t corners, int16_t delta, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    int16_t px = x;
    int16_t py = y;

    delta++; // Avoid some +1's in the loop

    while (x < y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x;
      if (x < (y + 1)) {
        if (corners & 1)
          drawLine(x0 + x, y0 - y, x0 + x, y0 - y + (2 * y + delta) - 1, color);
        if (corners & 2)
          drawLine(x0 - x, y0 - y, x0 - x, y0 - y + (2 * y + delta) - 1, color);
      }
      if (y != py) {
        if (corners & 1)
          drawLine(x0 + py, y0 - px, x0 + py, y0 - px + (2 * px + delta) - 1, color);
        if (corners & 2)
          drawLine(x0 - py, y0 - px, x0 - py, y0 - px + (2 * px + delta) - 1, color);
        py = y;
      }
      px = x;
    }
  }
  void VGA_GFX::drawRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius) r = max_radius;
    // smarter version
    fillRect(x + r, y, w - 2 * r, 1, color);           // Top
    fillRect(x + r, y + h - 1, w - 2 * r, 1, color);   // Bottom
    fillRect(x, y + r, 1, h - 2 * r, color);           // Left
    fillRect(x + w - 1, y + r, 1, h - 2 * r, color);   // Right

    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
    drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
  }
  void VGA_GFX::fillRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius)
    r = max_radius;
    fillRect(x + r, y, w - 2 * r, h, color);
    // draw four corners
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
  }


  void VGA_GFX::drawChar(int16_t x, int16_t y, unsigned char c,
                            uint16_t color, uint16_t bg, uint8_t size_x,
                            uint8_t size_y) {
    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
    uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size_x > 1 || size_y > 1) {
      xo16 = xo;
      yo16 = yo;
    }

    for (yy = 0; yy < h; yy++) {
      for (xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        if (bits & 0x80) {
          if (size_x == 1 && size_y == 1) {
            drawPixel(x + xo + xx, y + yo + yy, color);
          } else {
            fillRect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y,
                          size_x, size_y, color);
          }
        }
        bits <<= 1;
      }
    }
  }
  size_t VGA_GFX::write(uint8_t c) {
    if (c == '\n') {
      cursor_x = 0;
      cursor_y +=
          (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        uint8_t w = pgm_read_byte(&glyph->width),
                h = pgm_read_byte(&glyph->height);
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
          if (wrap && ((cursor_x + textsize_x * (xo + w)) > _width)) {
            cursor_x = 0;
            cursor_y += (int16_t)textsize_y *
                        (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
          }
          drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
                   textsize_y);
        }
        cursor_x +=
            (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize_x;
      }
    }
    if (cursor_x >= _width) return 1;
    return 0;
  }

  void VGA_GFX::getwidth(uint8_t c) {
    if (c == '\n') {
      cursor_x = 0;
      cursor_y +=
          (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        cursor_x += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize_x;
      }
    }
  }

  void VGA_GFX::write(const char *buffer, size_t size) {
    while (size--)
      if (write(*buffer++)) break;
  }
  void VGA_GFX::print(const int vai) {
    String s = String(vai);
    write(s.c_str(), s.length());
  }
  void VGA_GFX::print(const float vai) {
    String s = String(vai);
    write(s.c_str(), s.length());
  }
  void VGA_GFX::print(const String &s) {
    write(s.c_str(), s.length());
  }
  void VGA_GFX::print(const char str) {
    write(str);
  }
  void VGA_GFX::print(const char *str) {
    while (*str)
      if (write(*str++)) break;
  }
  void VGA_GFX::println(const char *str) {
    print(str);
    write('\n');
  }
  void VGA_GFX::println(const char str) {
    write(str);
    write('\n');
  }
  void VGA_GFX::println(const int vai) {
    print(vai); 
    write('\n');
  }
  void VGA_GFX::println(const float vai) {
    print(vai); 
    write('\n');
  }
  void VGA_GFX::println(const String &s) {
    print(s); 
    write('\n');
  }


void VGA_GFX::setTextSize(uint8_t s) { setTextSize(s, s); }

void VGA_GFX::setTextSize(uint8_t s_x, uint8_t s_y) {
  textsize_x = (s_x > 0) ? s_x : 1;
  textsize_y = (s_y > 0) ? s_y : 1;
}

void VGA_GFX::setRotation(uint8_t x) {
  rotation = (x & 3);
  switch (rotation) {
  case 0:
  case 2:
    _width = WIDTH;
    _height = HEIGHT;
    break;
  case 1:
  case 3:
    _width = HEIGHT;
    _height = WIDTH;
    break;
  }
}

void VGA_GFX::setFont(const GFXfont *f) {
  if (f) {
    if (!gfxFont) { 
      cursor_y += 6;
    }
  } else if (gfxFont) { 
    cursor_y -= 6;
  }
  gfxFont = (GFXfont *)f;
}