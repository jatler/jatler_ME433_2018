#ifndef DRAW_LCD_H__  /* Guard against multiple inclusion */
#define DRAW_LCD_H__

void drawProgressBar(short x, short y, short height, short len1, short len2, unsigned short color1, unsigned short color2);

void drawChar(short x,short y, char mess, short color1, short color2 );

void drawString(short x, short y, char* message, short color1, short color2);

void drawAccelerations(float xl_x, float xl_y, short bar, unsigned short color1, unsigned short color2);


#endif