/* 
 * File:   main.c
 * Author: jatler
 *
 * Created on April 17, 2018, 8:03 PM
 */

#include <stdio.h>
#include <stdlib.h>

// constants, funcs for startup and UART
// Demonstrates spi by accessing external ram
// PIC is the master, ram is the slave
// Uses microchip 23K256 ram chip (see the data sheet for protocol details)
// SDO1 -> SDI (pin A1 (3) -> pin 5)
// SDI1 -> SO (pin F4 (2) -> pin 2) - not in this SDI? 
// SCK1 -> SCK (pin B14 (25) -> pin 4)
// SS1 -> CS (pin A0 (2) -> pin 3)
// Additional SRAM connections
// Vss  (Pin 12) -> ground
// Vdd  (Pin 1)  -> 3.3 V
// LDAC (Pin 8)  -> ground (update at rising CS)
// SHDN (Pin 9)  -> 3.3 V (high to prevent shutdown)
// VoutB(Pin 10) -> nScope Ch2
// VrefB(Pin 11) -> 3.3V
// VrefA(Pin 13) -> 3.3V
// VoutA(Pin 14) -> nScope Ch1

// Only uses the SRAM's sequential mode
//
#define CS LATAbits.LATA0      // chip select pin

// send a byte via spi and return the response
unsigned char SPI1_IO(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void initSPI1() {
    
    //Output pins (SDO1(A1) ,SS1(A0))
    RPA1bits.RPA1R = 0b0011; // assign SDO1 to pin A1
    RPA0bits.RPA0R = 0b0011; // assign SS1 to pin A0
    
    //Input pins (SDI1(B8) (pin 17)
    SDI1Rbits.SDI1R = 0b0100; //assign SDI1 to pin B8
    
}
// initialize spi1 and the ram module
void ram_init() {
  // set up the chip select pin as an output
  // the chip select pin is used by the sram to indicate
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  TRISAbits.TRISA0 = 0;
  CS = 1;

  // Master - SPI1, pins are: SDI1(XX), SDO1(A1), SCK1(B14).  
  // we manually control SS1 as a digital output (A0)
  // since the pic is just starting, we know that spi is off. We rely on defaults here
 
  // setup SPI1
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x3;            // baud rate to 10 MHz [SPI1BRG = (80000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on SPI 1

                            // send a ram set status command.
  CS = 0;                   // enable the ram
  spi_io(0x01);             // ram write status
  spi_io(0x41);             // sequential mode (mode = 0b01), hold disabled (hold = 0)
  CS = 1;                   // finish the command
}

// write len bytes to the ram, starting at the address addr
void ram_write(unsigned short addr, const char data[], int len) {
  int i = 0;
  CS = 0;                        // enable the ram by lowering the chip select line
  spi_io(0x2);                   // sequential write operation
  spi_io((addr & 0xFF00) >> 8 ); // most significant byte of address
  spi_io(addr & 0x00FF);         // the least significant address byte
  for(i = 0; i < len; ++i) {
    spi_io(data[i]);
  }
  CS = 1;                        // raise the chip select line, ending communication
}

// read len bytes from ram, starting at the address addr
void ram_read(unsigned short addr, char data[], int len) {
  int i = 0;
  CS = 0;
  spi_io(0x3);                   // ram read operation
  spi_io((addr & 0xFF00) >> 8);  // most significant address byte
  spi_io(addr & 0x00FF);         // least significant address byte
  for(i = 0; i < len; ++i) {
    data[i] = spi_io(0);         // read in the data
  }
  CS = 1;
}

int main(void) {
  unsigned short addr1 = 0x1234;                  // the address for writing the ram
  char data[] = "Help, I'm stuck in the RAM!";    // the test message
  char read[] = "***************************";    // buffer for reading from ram
  char buf[100];                                  // buffer for comm. with the user
  unsigned char status;                           // used to verify we set the status 
  ram_init(); 

  // check the ram status
  CS = 0;
  spi_io(0x5);                                      // ram read status command
  status = spi_io(0);                               // the actual status
  CS = 1;

  sprintf(buf, "Status 0x%x\r\n",status);
  NU32_WriteUART3(buf);

  sprintf(buf,"Writing \"%s\" to ram at address 0x%x\r\n", data, addr1);
  NU32_WriteUART3(buf);
                                                    // write the data to the ram
  ram_write(addr1, data, strlen(data) + 1);         // +1, to send the '\0' character
  ram_read(addr1, read, strlen(data) + 1);          // read the data back
  sprintf(buf,"Read \"%s\" from ram at address 0x%x\r\n", read, addr1);
  NU32_WriteUART3(buf);

  while(1) {
    ;
  }
  return 0;
}
