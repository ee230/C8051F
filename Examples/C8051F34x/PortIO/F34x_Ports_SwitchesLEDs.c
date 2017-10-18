//-----------------------------------------------------------------------------
// F34x_Ports_SwitchesLEDs.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the port pins as digital inputs
// and outputs.  The C8051F340 target board has two push-button switches
// connected to port pins and and two LEDs.  The program constantly checks the 
// status of the switches and if they are pushed, it turns on the 
// respective LED.
//
//
// How To Test:
//
// 1) Download code to a 'F340 target board
// 2) Ensure that the Switch and LED pins on the J12 header are 
//    properly shorted
// 3) Push the buttons (P2.0 and P2.1) and see that the LEDs turn on 
//
//
// FID:            34X000001
// Target:         C8051F34x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (GP)
//    -15 NOV 2005
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f340.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

sbit SW1  =  P2^0;                     // SW1 ='0' means switch pressed
sbit SW2  =  P2^1;                     // SW2 ='0' means switch pressed
sbit LED1 =  P2^2;                     // LED1 ='1' means ON
sbit LED2 =  P2^3;                     // LED2 ='1' means ON

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);           
void PORT_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   PORT_Init();                        // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator
  
   while (1)
   {
      if (SW1 == 0)                    // If switch depressed
      { 
         LED1 = 1;                     // Turn on LED
      }
	  else   
      {  
         LED1 = 0;                     // Else, turn it off
      }                   
	  
      if (SW2 == 0)                    // If switch depressed
      { 
         LED2 = 1;                     // Turn on LED
      }
	  else   
      {  
         LED2 = 0;                     // Else, turn it off
      }                   
   }                                   // end of while(1)
}                                      // end of main()

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at its maximum frequency.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency (24.5 Mhz)
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
// 
// To configure a pin as a digital input, the pin is configured as digital
// and open-drain and the port latch should be set to a '1'.  The weak-pullups
// are used to pull the pins high.  Pressing the switch pulls the pins low.
//
// To configure a pin as a digital output, the pin is configured as digital
// and push-pull.  
//
// An output pin can also be configured to be an open-drain output if system
// requires it.  For example, if the pin is an output on a multi-device bus,
// it will probably be configured as an open-drain output instead of a 
// push-pull output.  For the purposes of this example, the pin is configured
// as push-pull output because the pin in only connected to an LED.
//
// P2.0   digital   open-drain    Switch 1
// P2.1   digital   open-drain    Switch 2
// P2.2   digital   push-pull     LED1
// P2.3   digital   push-pull     LED2
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P2MDIN |= 0x0F;                     // Lower four pins on P2 are digital

   P2MDOUT = 0x0C;                     // enable LEDs as push-pull outputs
                                       // enable Switches as open-drain

   P2     |= 0x03;                     // Set port latches for P2.0 
                                       // and P2.1 to '1'

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------