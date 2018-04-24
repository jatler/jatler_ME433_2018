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

void setGP0(char level){
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR_WRITE);  // send slave address
    i2c_master_send(0x09);              // send GPIO register
    char gp0 = (0b00000000 | level);
    i2c_master_send(gp0);               // set GP0
    i2c_master_stop();
    
}
void setExpander(char pin, char level){
    
    unsigned char pin_set = (0b00000000 | (level << pin));
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR_WRITE);
    i2c_master_send(0x09);
    i2c_master_send(pin_set);
    i2c_master_stop();    
}


unsigned char getExpander(void){
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR_WRITE);
    i2c_master_send(0x09);                 //send GPIO to register
    i2c_master_restart();
    i2c_master_send(SLAVE_ADDR_READ);
    unsigned char message = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    
    return message;
    //OLAT register reads output latches
    //Writing modifies output latches that modify pins as outputs
    //OLAT (ADDR 0x0A)
}