/* 
 * File:   main.c
 * Author: jatler
 *
 * Created on April 9, 2018, 8:24 PM
 */

#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

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

// SPI Chip Select
#define CS LATAbits.LATA0

// Function Prototypes
void initSPI1();
unsigned char SPI1_IO(unsigned char write);
void setVoltage(char channel, int voltage);

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
    initSPI1();

    __builtin_enable_interrupts();
    
    setVoltage(0,512); //test Ch1
    setVoltage(1,256); //test Ch2

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

unsigned char SPI1_IO(unsigned char write){
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
    }
    return SPI1BUF;
}


void initSPI1() {
    //setup CS pin (A0) as an output
    TRISAbits.TRISA0 = 0;
    CS = 1; // initialized high
    
    //setup SDO1 pin (A1)
    RPA1Rbits.RPA1R = 0b0011; // assign SDO1 to A1
    
    //setup SDI1 pin (B8)
    SDI1Rbits.SDI1R = 0b0110; // assign SDI1 to B8
    
    //setup SPI1
    SPI1CON = 0;              // turn off and reset SPI module
    SPI1BUF;                  // read and clear rx buffer
    SPI1BRG = 0x1;            // max. baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired)) - 1]
    SPI1STATbits.SPIROV = 0;  // clear overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock from high to low
    SPI1CONbits.MSTEN = 1;        // master operation
    SPI1CONbits.ON = 1;       // turn on SPI 1
    
}

void setVoltage(char a, int v) {

	unsigned short t = 0;
	t= a << 15; //a is at the very end of the data transfer
	t = t | 0b01110000000000000;
	t = t | ((v&0b1111111111) <<2); //rejecting excessive bits (above 10)
	CS = 0;
	SPI1_IO(t>>8);
    SPI1_IO(t);
    CS = 1;
}