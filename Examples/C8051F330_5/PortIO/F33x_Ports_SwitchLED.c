//-----------------------------------------------------------------------------
// F33x_Ports_SwitchLED.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure port pins as digital inputs
// and outputs.  The C8051F330 target board has one push-button switch 
// connected to a port pin and one LED.  The program constantly checks the 
// status of the switch and if it is pushed, it turns on the LED.
//
//
// How To Test:
//
// 1) Download code to a 'F330 target board
// 2) Ensure that the Switch and LED pins on the J3 header are 
//    properly shorted
// 3) Push the button (P0.7) and see that the LED turns on 
//
//
// FID:            33X000004
// Target:         C8051F33x
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

#include <c8051f330.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

sbit SW1  =  P0^7;                     // SW1 ='0' means switch pressed
sbit LED1 =  P1^3;                     // LED1 ='1' means ON

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
// P0.7   digital   open-drain    Switch 1
// P1.3   digital   push-pull     LED1
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0MDIN |= 0x80;                     // P0.7 is digital
   P1MDIN |= 0x08;                     // P1.3 is digital

   P0MDOUT = 0x00;                     // P0.7 is open-drain
   P1MDOUT = 0x08;                     // P1.3 is push-pull

   P0     |= 0x80;                     // Set P0.7 latch to '1'

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------