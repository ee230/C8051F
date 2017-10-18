//-----------------------------------------------------------------------------
// T60x_Ports_SwitchLED.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure port pins as digital inputs
// and outputs.  The C8051T600 target board has one push-button switch
// connected to a port pin and one LED.  The program constantly checks the 
// status of the switch and if it is pushed, it turns on the LED.
//
//
// How To Test:
//
// 1) Define chip number being used under CHIPNUM
// 2) Download code to a 'T600 target board
// 3) Ensure that the Switch and LED pins on the J3 header are 
//    properly shorted
// 4) Push the button (P0.3) and see that the LED turns on 
//
//
// FID:            
// Target:         C8051T60x
// Tool chain:     Keil C51 8.0 / Keil EVAL C51
// Command Line:   None
//
// Release 1.1 / 31 JUL 2008 (ADT)
//    - Updated to use compiler_defs.h
//    - Includes support for T606
//
// Release 1.0
//    - Initial Revision (BD)
//    - 17 JAN 2007
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <C8051T600_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

#define CHIPNUM   600                  // Define the chip number being used
                                       // Options: 600, 601, 602, 603,
                                       //          604, 605, 606

SBIT (SW1, SFR_P0, 3);                 // SW1 ='0' means switch pressed

#if (CHIPNUM == 606)                   // LED1 ='1' means ON
 SBIT (LED1, SFR_P0, 2);
#else
 SBIT (LED1, SFR_P0, 6);                     
#endif

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

   REG0CN |= 0x01;                     // OTP Power Controller Enable

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
   }                                   // End of while(1)
}                                      // End of main()

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
// This function configures the crossbar and port pins.
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
// P0.2   digital   push-pull     LED (C8051T606)
// P0.3   digital   open-drain    Switch 1
// P0.6   digital   push-pull     LED (C8051T600/1/2/3/4/5)
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
#if (CHIPNUM == 606)
   P0MDOUT = 0x04;                     // P0.3 is open-drain; P0.2 is push-pull
#else
   P0MDOUT = 0x40;                     // P0.3 is open-drain; P0.6 is push-pull
#endif

   P0     |= 0x08;                     // Set P0.3 latch to '1'

   XBR2    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------