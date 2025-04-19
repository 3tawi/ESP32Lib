#ifndef _VGA_GFX_H
#define _VGA_GFX_H

#include "Arduino.h"
#include "gfxfont.h"

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
  return gfxFont->glyph + c;
}
inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
  return gfxFont->bitmap;
}

class VGA_GFX {
private:

public:
  VGA_GFX(int16_t w, int16_t h); // Constructor

  virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

  virtual size_t write(uint8_t c);
  virtual void writePixel(int x, int y, uint16_t color);
  virtual void fillRect(int x, int y, int w, int h, uint16_t color);
  virtual void xLine(int x0, int x1, int y, uint16_t color); 
  virtual void drawLine(int x1, int y1, int x2, int y2, uint16_t color);
  virtual void fillScreen(uint16_t color);
  virtual void setRotation(uint8_t r);
  virtual void getwidth(uint8_t c);

  void fillCircle(int x, int y, int r, uint16_t color);
  void circle(int x, int y, int r, uint16_t color);
  void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);
  void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);
  void fillCare(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
  void DrawCare(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
  void ellipse(int x, int y, int rx, int ry, uint16_t color);
  void fillEllipse(int x, int y, int rx, int ry, uint16_t color);
  void Demiellipse(int x, int y, int rx, int ry, uint16_t color, bool sen = true);
  void DemifillEllipse(int x, int y, int rx, int ry, uint16_t color, bool sen = true);
  void DemifillCircle(int x, int y, int r, uint16_t color, bool sen = true);
  void Demicircle(int x, int y, int r, uint16_t color, bool sen = true);
  void fillParallelogram(int x, int y, int xr, int yr, uint16_t color);
  void parallelogram(int x, int y, int xr, int yr, uint16_t color);
  void DemifillParall(int x, int y, int xr, int yr, uint16_t color, bool sen = true);
  void Demiparall(int x, int y, int xr, int yr, uint16_t color, bool sen = true);
  void drawCircleHelper(int x0, int y0, int r, uint8_t cornername, uint16_t color);
  void fillCircleHelper(int x0, int y0, int r, uint8_t corners, int16_t delta, uint16_t color);
  void drawRoundRect(int x, int y, int w, int h, int r, uint16_t color);
  void fillRoundRect(int x, int y, int w, int h, int r, uint16_t color);

  void write(const char *buffer, size_t size);
  void print(const int vai);
  void print(const float vai);
  void print(const String &s);
  void print(const char str);
  void print(const char *str);
  void println(const char str);
  void println(const char *str);
  void println(const int vai);
  void println(const float vai);
  void println(const String &s);
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,  uint16_t bg, uint8_t size_x, uint8_t size_y);
  void setTextSize(uint8_t s);
  void setTextSize(uint8_t sx, uint8_t sy);
  void setFont(const GFXfont *f = NULL);

  void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }

  void setTextColor(uint16_t c) { textcolor = textbgcolor = c; }

  void setTextColor(uint16_t c, uint16_t bg) {
    textcolor = c;
    textbgcolor = bg;
  }

  void setTextWrap(bool w) { wrap = w; }

  void cp437(bool x = true) { _cp437 = x; }

  int16_t width(void) const { return _width; };
  int16_t height(void) const { return _height; }
  uint8_t getRotation(void) const { return rotation; }
  int16_t getCursorX(void) const { return cursor_x; }
  int16_t getCursorY(void) const { return cursor_y; };

protected:
  int16_t WIDTH;        ///< This is the 'raw' display width - never changes
  int16_t HEIGHT;       ///< This is the 'raw' display height - never changes
  int16_t _width;       ///< Display width as modified by current rotation
  int16_t _height;      ///< Display height as modified by current rotation
  int16_t cursor_x;     ///< x location to start print()ing text
  int16_t cursor_y;     ///< y location to start print()ing text
  uint16_t textcolor;   ///< 16-bit background color for print()
  uint16_t textbgcolor; ///< 16-bit text color for print()
  uint8_t textsize_x;   ///< Desired magnification in X-axis of text to print()
  uint8_t textsize_y;   ///< Desired magnification in Y-axis of text to print()
  uint8_t rotation;     ///< Display rotation (0 thru 3)
  bool wrap = false;            ///< If set, 'wrap' text at right edge of display
  bool _cp437 = false;          ///< If set, use correct CP437 charset (default is off)
  GFXfont *gfxFont;     ///< Pointer to special font
};

#endif // _VGA_GFX_H
