//-----------------------------------------------------------------------------
// F41x_Ports_SwitchesLEDs.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the port pins as digital inputs
// and outputs.  The C8051F410 target board has two push-button switches
// connected to port pins and and two LEDs.  The program constantly checks the
// status of the switches and if they are pushed, it turns on the
// respective LED.
//
//
// How To Test:
//
// 1) Download code to a 'F410 target board
// 2) Ensure that the Switch and LED pins on the J3 header are
//    properly shorted
// 3) Push the buttons (P1.4 and P1.5) and see that the LEDs turn on
//
//
// FID:            41X000001
// Target:         C8051F41x
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0
//    -Initial Revision (GP)
//    -09 MAR 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F410_defs.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

sbit SW2  =  P1^4;                     // SW1 ='0' means switch pressed
sbit SW3  =  P1^5;                     // SW2 ='0' means switch pressed
sbit LED1 =  P2^1;                     // LED1 ='1' means ON
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
      if (SW2 == 0)                    // If switch depressed
      {
         LED1 = 1;                     // Turn on LED
      }
	  else
      {
         LED1 = 0;                     // Else, turn it off
      }

      if (SW3 == 0)                    // If switch depressed
      {
         LED2 = 1;                     // Turn on LED
      }
	  else
      {
         LED2 = 0;                     // Else, turn it off
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
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN |= 0x02;                     // Configure internal oscillator for
                                       // 24.5/8 Mhz (~3 Mhz)
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
// P1.4   digital   open-drain    Switch 2
// P1.5   digital   open-drain    Switch 3
// P2.1   digital   push-pull     LED1
// P2.3   digital   push-pull     LED2
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P1MDIN |= 0x30;                     // Make switch pins digital
   P2MDIN |= 0x0A;                     // Make LED pins digital

   P1MDOUT &= ~0x30;                   // Make switch pins open-drain
   P2MDOUT |= 0x0A;                    // Make LED pins push-pull

   P1     |= 0x30;                     // Set port latches for P1.4
                                       // and P1.5 to '1'

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------