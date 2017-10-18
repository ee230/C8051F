//-----------------------------------------------------------------------------
// T622_Ports_SwitchesLEDs.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the port pins as digital inputs
// and outputs.  The C8051T62x mother board has two push-button switches
// connected to port pins and and two LEDs.  The program constantly checks the
// status of the switches and if they are pushed, it turns on the
// respective LED.
//
//
// How To Test:
//
// 1) Download code to a 'T622 device on a 'T62x mother board.
// 2) Ensure that the P1.x and P0.x Switch and LED pins on the J9 and J10
//    headers are properly shorted.
// 3) Push the buttons (P0.1 and P1.0) and see that the LEDs turn on.
//
//
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (GP / TP)
//    -02 SEP 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T622_defs.h"            // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (SW1, SFR_P0, 1);                 // SW1 ='0' means switch pressed
SBIT (SW2, SFR_P1, 0);                 // SW2 ='0' means switch pressed
SBIT (LED1, SFR_P0, 6);                // LED1 ='1' means ON
SBIT (LED2, SFR_P1, 2);                // LED2 ='1' means ON

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Port_Init();                        // Initialize Port I/O
   Oscillator_Init ();                 // Initialize Oscillator

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
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 12 Mhz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // 12 MHz.
}

//-----------------------------------------------------------------------------
// Port_Init
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
// P0.1   digital   open-drain    Switch 1
//
// P0.6   digital   push-pull     LED1
//
// P1.0   digital   open-drain    Switch 2
//
// P1.2   digital   push-pull     LED2
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDIN |= 0x42;                     // Switch and LED pins are digital
   P1MDIN |= 0x05;

   P0MDOUT |= 0x40;                    // Enable LEDs as push-pull outputs
   P1MDOUT |= 0x04;                    // Enable Switches as open-drain

   P0 |= 0x02;                         // Set port latches for the switches to
   P1 |= 0x01;                         // '1'

   XBR1 = 0x40;                        // Enable crossbar and enable
                                       // weak pull-ups
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------