//-----------------------------------------------------------------------------
// F53xA_Ports_SwitchLED.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure port pins as digital inputs
// and outputs. Each device on the C8051F530A target board has one push-button
// switch connected to a port pin and one LED.  The program constantly checks
// the status of the switch and if it is pushed, it turns on the LED.
//
//
// How To Test:
//
// 1) Download the code to a 'F53xA (either device A or device B on the target
//    board).
// 2) Ensure that the Switch and LED jumpers are populated (HDR4 for device A
//    and HDR3 for device B).
// 3) Run the code.
// 4) Push the button (P1.4) and see that the LED turns on.  The LED will
//    remain lit as long as the button is pressed.
//
//
// Target:         C8051F52xA/F53xA (C8051F530A TB)
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.2 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.1
//    -Updated for 'F530A TB (TP)
//    -31 JAN 2008
//
// Release 1.0
//    -Initial Revision (SM)
//    -13 JUN 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051F520A_defs.h"

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (SW1, SFR_P1, 4);                 // SW1 ='0' means switch pressed
SBIT (LED1, SFR_P1, 3);                // LED1 ='1' means ON

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
      if (SW1 == 0)                    // If switch pressed
      {
         LED1 = 1;                     // Turn on LED
      }
      else
      {
         LED1 = 0;                     // Else, turn it off
      }
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 24.5 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x87;                      // Set the internal oscillator to
                                       // 24.5 MHz
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
// P1.3   digital   push-pull     LED1
// P1.4   digital   open-drain    Switch 1
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P1MDIN |= 0x08;                     // P1.3 is digital
   P1MDIN |= 0x10;                     // P1.4 is digital

   P1MDOUT = 0x08;                     // P1.3 is push-pull

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------