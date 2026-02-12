#ifndef ILI9341_H
#define ILI9341_H

#include "main.h"

// ILI9341 Befehle
#define ILI9341_SOFTRESET             0x01
#define ILI9341_SLEEPIN               0x10
#define ILI9341_SLEEPOUT              0x11
#define ILI9341_DISPLAYOFF            0x28
#define ILI9341_DISPLAYON             0x29
#define ILI9341_COLUMNADDR            0x2A
#define ILI9341_PAGEADDR              0x2B
#define ILI9341_MEMORYWRITE           0x2C
#define ILI9341_PIXELFORMAT           0x3A
#define ILI9341_MADCTL                0x36

// Funktionen
void ILI9341_Init(void);
void ILI9341_WriteCommand(uint8_t cmd);
void ILI9341_WriteData(uint8_t data);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

#endif
