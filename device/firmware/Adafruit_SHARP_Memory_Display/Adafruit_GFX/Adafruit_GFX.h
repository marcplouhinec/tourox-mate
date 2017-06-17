 /*
  *  Modified by Marc PLOUHINEC 04/10/2015 for use in nRF51 SDK
  */

#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "glcdfont.h"

#define BLACK 0
#define WHITE 1

/**
 * @brief Low-level function that must be implemented and passed as a parameter to the Adafruit_GFX_init function.
 */
typedef void (*Adafruit_GFX_draw_pixel_t) (int16_t x, int16_t y, uint16_t color);

/**
 * @brief Initialize the Adafruit_GFX library.
 *
 * Note that this function was originally a class constructor. It has been modified in order to be compatible with C
 * by using the function pointer Adafruit_GFX_draw_pixel_t.
 */
void Adafruit_GFX_init(int16_t w, int16_t h, Adafruit_GFX_draw_pixel_t p_draw_pixel);

/** @fn void Adafruit_GFX::invertDisplay(bool i)
 *   @bref Do nothing, must be subclassed if supported
 *   @param i invert
 */
void Adafruit_GFX_invertDisplay(bool i);

void Adafruit_GFX_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

void Adafruit_GFX_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

void Adafruit_GFX_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

void Adafruit_GFX_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void Adafruit_GFX_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void Adafruit_GFX_fillScreen(uint16_t color);

/** @fn void Adafruit_GFX::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
 *   @bref draw a circle outline from the coordinates of the center.
 *   @param x0 x position
 *   @param y0 y position
 *   @param r Radius of the circle
 *   @param color 16bit color
 */
void Adafruit_GFX_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

void Adafruit_GFX_drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);

/** @fn void Adafruit_GFX::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
 *   @bref draw a circle from the coordinates of the center.
 *   @param x0 x position
 *   @param y0 y position
 *   @param r Radius of the circle
 *   @param color 16bit color
 */
void Adafruit_GFX_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

void Adafruit_GFX_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);

/** @fn void Adafruit_GFX::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
 *   @bref draw a triangle outline from the vertex with color.
 *   @param x0 first vertex x
 *   @param y0 first vertex y
 *   @param x1 second vertex x
 *   @param y1 second vertex y
 *   @param x2 third vertex x
 *   @param y2 third vertex y
 *   @param color 16bit color
 */
void Adafruit_GFX_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

/** @fn void Adafruit_GFX::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
 *   @bref draw a triangle from the vertex with color.
 *   @param x0 first vertex x
 *   @param y0 first vertex y
 *   @param x1 second vertex x
 *   @param y1 second vertex y
 *   @param x2 third vertex x
 *   @param y2 third vertex y
 *   @param color 16bit color
 */
void Adafruit_GFX_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

/** @fn void Adafruit_GFX::drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
 *   @bref draw a round rectangle outline with color.
 *   @param x0 first vertex x
 *   @param y0 first vertex y
 *   @param w width
 *   @param h height
 *   @param radius radius
 *   @param color 16bit color
 */
void Adafruit_GFX_drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);

/** @fn void Adafruit_GFX::drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
 *   @bref draw a round rectangle with color.
 *   @param x0 first vertex x
 *   @param y0 first vertex y
 *   @param w width
 *   @param h height
 *   @param radius radius
 *   @param color 16bit color
 */
void Adafruit_GFX_fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);

void Adafruit_GFX_drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

void Adafruit_GFX_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

void Adafruit_GFX_setCursor(int16_t x, int16_t y);

void Adafruit_GFX_setTextSize(uint8_t s);

void Adafruit_GFX_setTextColor(uint16_t c);

void Adafruit_GFX_setTextColorAndBackgroundColor(uint16_t c, uint16_t b);

void Adafruit_GFX_setTextWrap(bool w);

size_t Adafruit_GFX_writeChar(uint8_t);

void Adafruit_GFX_writeText(char* text);

int16_t Adafruit_GFX_getHeight(void);

int16_t Adafruit_GFX_getWidth(void);

void Adafruit_GFX_setRotation(uint8_t r);

uint8_t Adafruit_GFX_getRotation(void);

#endif // _ADAFRUIT_GFX_H
