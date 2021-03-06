/* 
 * File:   main.c
 * Author: jatler
 *
 * Created on April 9, 2018, 8:24 PM
 */

#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

// DEVCFG0
#pragma config DEBUG = 1 // no debugging, debugger disabled
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 11 // use PGED1 and PGEC1
#pragma config PWP = 111111111 // no write protect
#pragma config BWP = 1 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // disable secondary osc
#pragma config FPBDIV = 00 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = 1 // do not enable clock switch
#pragma config WDTPS = 10100 // use slowest wdt, 1:1048576
#pragma config WINDIS = 1 // wdt no window mode
#pragma config FWDTEN = 0 // wdt disabled
#pragma config FWDTWINSZ = 11 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = 001 // divide input clock to be in range 4-5MHz, divide by 2
#pragma config FPLLMUL = 111 // multiply clock after FPLLIDIV, multiply 24x for 96MHz output
#pragma config FPLLODIV = 001 // divide clock after FPLLMUL to get 48MHz, divide by 2 for 48 MHz
#pragma config UPLLIDIV = 001 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0xAAAA // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module


int main() {

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

    while(1) {
	// use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
	// remember the core timer runs at half the sysclk
        if (PORTBbits.RB4 < 1.0) {  //if button is pressed do nothing
        } else {
            LATAbits.LATA4 = 1;  // turn on LED
            _CP0_SET_COUNT(0);   // reset Core Timer 
            while (_CP0_GET_COUNT() < 12000){
                // delay 0.5ms. 12000 = 5e-3*24e6
            }
            LATAbits.LATA4 = 0;  // turn off LED
            _CP0_SET_COUNT(0);   // reset Core Timer
            while (_CP0_GET_COUNT() < 12000){
                // delay 0.5 ms
            }
        }  
    }
}