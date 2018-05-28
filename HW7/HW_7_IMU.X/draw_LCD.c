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

void drawAccelerations(float xl_x, float xl_y, short bar, unsigned short color1, unsigned short color2) {
    
    //Setup
    short x_center = (128-bar)/2;
    short y_center = (160-bar)/2;
    short length = 50;
    short x_start = x_center - length + bar/2;
    short x_finish = x_center + length + bar/2;
    short y_start = y_center - length + bar/2;
    short y_finish = y_center + length + bar/2;
    short row;
    short col;
    unsigned short color;
    
    //Convert xl_x, xl_y into lengths
    short plot_xl_x_start;
    short plot_xl_x_finish;
    short plot_xl_y_start;
    short plot_xl_y_finish;
    
    if (xl_x < 0){
        plot_xl_x_start = x_center;
        plot_xl_x_finish = x_center + bar + abs((int)(xl_x/2*(length-bar)));
    } else {
        plot_xl_x_start = x_center - abs((int)(xl_x/2*length));
        plot_xl_x_finish = x_center + bar;
    }
    
    if (xl_y < 0){
        plot_xl_y_start = y_center;
        plot_xl_y_finish = y_center + bar + abs((int)(xl_y/2*length-bar));
    } else{
        plot_xl_y_start = y_center  - abs((int)(xl_y/2*length));
        plot_xl_y_finish = y_center + bar;
    }
    
    /*
    char msg[20];
    sprintf(msg, "Plot XL Y: %d, %d  ",plot_xl_y_start,plot_xl_y_finish);           // print accelerometer data
    drawString(28,140,msg,BLUE,WHITE);
    */
    
    for (col = x_start; col < x_finish; col++){
        if (col > plot_xl_x_start & col < plot_xl_x_finish){
             color = color2;
        } else {
            color = color1;
        }
        for (row = y_center; row < (y_center+bar); row++){
            LCD_drawPixel(col,row,color);
        }
    }
    
    for (row = y_start; row < y_finish; row++){
        if (row > plot_xl_y_start & row < plot_xl_y_finish){
            color = color2;
        } else {
            color = color1;
        }
        for (col = x_center; col < (x_center+bar); col++){
            LCD_drawPixel(col,row,color);
        }
    } 
}