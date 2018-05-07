#include <stdio.h>
#include "ST7735.h"

void drawChar(short x,short y, char mess, short color1, short color2 ){
    char row = mess - 0x20; //ASCII starts at 0x20
    char col; 
    char pixels;
    int i;
    for (col = 0; col < 5; col++){
        pixels = ASCII[row][col];
        for (i = 0; i < 8; i++){
            if  ((y+i) < 160 & (x+col < 128)){ // check if on screen
                if (pixels>>i & 1) {
                    LCD_drawPixel(x+col, y+i, color1);
                } else {
                    LCD_drawPixel(x+col, y+i, color2);
                }
            }
        }
    }
}

void drawString(short x, short y, char* message, short color1, short color2){
    int i = 0;
    while(message[i]){ // while not null character
        drawChar(x,y,message[i],color1,color2);
        x = x+5;
        i++;
    }
}

void drawProgressBar(short x, short y, short height, short len1, short len2, unsigned short color1, unsigned short color2) {
    short row;
    short col;
    unsigned short color;
    for (col = x; col < len2; col++ ){
        if (col < len1){
            color = color1;
        } else {
            color = color2;
        }
        for (row = y; row < (y+height); row++){
            
                LCD_drawPixel(col,row,color);
        }
    }
}