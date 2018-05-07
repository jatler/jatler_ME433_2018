/* 
 * File:   main.c
 * Author: jatler
 *
 * Created on May 1, 2018, 11:45 AM
 */

#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro
#include "ST7735.h"       // LCD functions
#include "draw_LCD.h"     // custom LCD functions
#include <stdio.h>        // sprintf

// DEVCFG0
#pragma config DEBUG = OFF // no debugging, debugger disabled
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // use slowest wdt, 1:1048576
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz, divide by 2
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV, multiply 24x for 96MHz output
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz, divide by 2 for 48 MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0xAAAA  // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF   // allow multiple reconfigurations
#pragma config IOL1WAY = OFF    // allow multiple reconfigurations
#pragma config FUSBIDIO = ON    // USB pins controlled by USB module
#pragma config FVBUSONIO = ON    // USB BUSON controlled by USB module

int main(void) {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISBbits.TRISB4 = 1;  // set pushbutton pin (RB4) as input pin
    TRISAbits.TRISA4 = 0;  // set LED pin as output pin
    LATAbits.LATA4 = 1; // set LED output to high
    
    __builtin_enable_interrupts();
    
    LCD_init();
    LCD_clearScreen(WHITE);
    char message[30];
    sprintf(message, "Hello world!");
    drawString(28,32,message,BLUE,WHITE);
    
    int progress = 0;
    char prog_msg[10];
    
    while(1) {
	// use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
	// remember the core timer runs at half the sysclk
        if (PORTBbits.RB4 < 1.0) {  //if button is pressed do nothing
        } else {   
            _CP0_SET_COUNT(0);
            if (progress > 99){
                progress = 0;   // progress from 0 to 100
            }
            while (_CP0_GET_COUNT() < 2400000){  // increment progress bar at 10Hz
                drawProgressBar(28,50,10,progress,100,YELLOW,BLUE);
                sprintf(prog_msg,"%d  ",progress);
                drawString(28,60,prog_msg,BLUE,WHITE);
                progress++;
            }
        }  
    }
}