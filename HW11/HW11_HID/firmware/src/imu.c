#include "i2c_master_noint.h"
#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro
#include "imu.h"          // slave addresses
#include <stdio.h>        // sprintf
#include "ST7735.h"
#include "draw_LCD.h"

void initIMU(void){
    //Initialize I2C2
    ANSELBbits.ANSB2 = 0;   // turn off analog input for RB2 (SDA2)
    ANSELBbits.ANSB3 = 0;   // turn off analog input for RB3 (SCL2)
    
    i2c_master_setup();                  // initialize I2C2
    
    i2c_master_start();                  // start transmission on I2C bus
    i2c_master_send(SLAVE_ADDR_WRITE);   // send slave write address
    i2c_master_send(0x10);               // send address of CTRL1_XL register to configure accelerometer
    i2c_master_send(0b10000010);         // accelerometer sample rate 1.66kHz, 2g sensitivity, 100 Hz filter
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR_WRITE);
    i2c_master_send(0x11);              // send address of CTRL2_G register to configure gyro
    i2c_master_send(0b10001000);        // sample rate 1.66kHz. 1000 dps sensitivity.
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR_WRITE);
    i2c_master_send(0x12);              // send address of CTRL3_C register
    i2c_master_send(0b00000100);        // read multiple registers in a row w/o specifying location
    i2c_master_stop();
}

void IMU_read_multiple(unsigned char* data, int length, unsigned char register_start){
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR_WRITE);
    i2c_master_send(register_start);                 //start with OUT_TEMP_L register
    i2c_master_restart();
    i2c_master_send(SLAVE_ADDR_READ);
    int i;
    for (i = 0; i < length; i++){
        data[i] = i2c_master_recv();
        if (i < (length-1)){
            i2c_master_ack(0);
        } else {
            i2c_master_ack(1);
        }
    }
    i2c_master_stop();
}

void IMU_accelerations(unsigned char* data)
{
    //x,y offsets to be close to 0,0 when flat on table
    int x_offset = 150;  
    int y_offset = 2200;  
    
    // accelerometer data to shorts
    short xl_x = data[9];                   
    xl_x = (xl_x << 8) | data[8];
    short xl_y = data[11];
    xl_y = (xl_y << 8) | data[10];
    short xl_z= data[13];
    xl_z = (xl_z << 8) | data[12];
    
    
    char msg[20];
    sprintf(msg, "X: %d ",xl_x);           // print accelerometer data
    drawString(10,100,msg,BLUE,WHITE);

    sprintf(msg, "Y: %d ",xl_y);
    drawString(10,110,msg,BLUE,WHITE);

    sprintf(msg, "Z: %d ",xl_z);
    drawString(10,120,msg,BLUE,WHITE);
}

int IMU_mouse_x(unsigned char* data, int scale){
    
    int x_mouse;
    short xl_y = data[11];
    xl_y = (xl_y << 8) | data[10];
    
    int y_offset = 2000; 
    x_mouse = -(xl_y - y_offset)/scale; //close to zero when flat
    
    return x_mouse;
    
}

int IMU_mouse_y(unsigned char* data, int scale){
    
    short xl_x = data[9];                   
    xl_x = (xl_x << 8) | data[8];
    
    int x_offset = 400;
    int y_mouse = -(xl_x-x_offset)/scale; //close to zero when flat
    
    return y_mouse;
    
}

