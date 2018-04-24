/* ************************************************************************** */
/** I2C Pin Expander Functions

  @Company
    Company Name

  @File Name
 i2c_hw5.c

  @Summary
 * I/O expander initialization function, setExpander, and getExpander functions
 * I2C2 used with 400 kHz baud

**/
#include "i2c_master_noint.h"
#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

#define SLAVE_ADDR_WRITE 0b01000000 //default write, A0, A1, A2 are 0
#define SLAVE_ADDR_READ  0b01000001

void initExpander(void){
    
    ANSELBbits.ANSB2 = 0;   // turn off analog input for B2 (SDA2)
    ANSELBbits.ANSB3 = 0;   // turn off analog input for B3 (SCL2)
    i2c_master_setup();     // initialize I2C2
    
    i2c_master_start();                  // start transmission on I2C bus
    i2c_master_send(SLAVE_ADDR_WRITE);   // send slave address
    i2c_master_send(0x00);               // send address of IODIR register to configure pins
    i2c_master_send(0b11110000);         // configure GP7-4 as inputs, GP3-0 as outputs
    i2c_master_stop();
}

void setGP0(void){
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR_WRITE);  // send slave address
    i2c_master_send(0x09);              // send GPIO register
    i2c_master_send(0b00000001);        // set GP7 to high
    i2c_master_stop();
    
}
void setExpander(char pin, char level){

}


char getExpander(void){
    //OLAT register reads output latches
    //Writing modifies output latches that modify pins as outputs
    //OLAT (ADDR 0x0A)
}