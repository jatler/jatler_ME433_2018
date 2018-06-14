/*******************************************************************************
 System Interrupts File

  File Name:
    system_interrupt.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/common/sys_common.h"
#include "app.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************

 
void __ISR(_USB_1_VECTOR, ipl4AUTO) _IntHandlerUSBInstance0(void)
{
    DRV_USBFS_Tasks_ISR(sysObj.drvUSBObject);
}

void __ISR(_TIMER_4_VECTOR, IPL4SOFT) Timer4ISR(void) {
  // code for PI control goes here
    
    
    float right;
    float left;
    int error;
    int MAX_DUTY = (int) 2399*(19.0/100);
    float e_i;
    float kp = 15;
    float ki = 2;
    extern int rxVal;
    
    //Integral control
    int e_int;
    int i_max = 100;
    
    int right_encoder = TMR3;
    int left_encoder = TMR5;
    
    
    //rxVal between 50 and 600
    
    // error max 300
    error = rxVal - 270; // 360 means the dot is in the middle of the screen
    
    //Integral error
    e_int += error;
    if (e_int > i_max){
        e_int = i_max;
    } else if (e_int < i_max){
        e_int = -i_max;
    }
    // go straight from encoder control
    
    //left =  (int)(MAX_DUTY*((float)TMR3)/TMR5);
    //right = (int)(MAX_DUTY*((float)TMR5)/TMR3);
    
    
    if (error < 0) { // slow down the left motor to steer to the left
        error  = -error;
        left = MAX_DUTY - kp*error - ki*e_int;
        right = MAX_DUTY;
        if (left < 0){
            left = 0;
        }
    }
    else { // slow down the right motor to steer to the right
        right = MAX_DUTY - kp*error - ki*e_int;
        left = MAX_DUTY;
        if (right<0) {
            right = 0;
        }
    }
    
    
    OC1RS = (int) left;
    OC4RS = (int) right;
    
    if (LATAbits.LATA4){  //blink LED
        LATAbits.LATA4 = 0;
    } else {
        LATAbits.LATA4 = 1;
    }
    
    TMR3 = 0;
    TMR5 = 0;
    IFS0bits.T4IF = 0; // clear interrupt flag, last line
}

/*******************************************************************************
 End of File
*/
