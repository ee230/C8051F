//-----------------------------------------------------------------------------
// F53xA_PortMatch.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This software shows the necessary configuration to use Port Match as an
// interrupt source.  The code executes the initialization routines and then
// spins in an infinite while() loop.  If the button on P1.4 (on the target
// board) is pressed, then the Port Match Interrupt will cause the LED to
// toggle.
//
// Pinout:
//
// Port Match: P1.4
//
// P1.3 - LED
// P1.4 - SW1 (Switch 1)
//
// How To Test:
//
// 1) Compile and download code to a 'F53xA device (either device A or device
//    B on the target board).
// 2) Install the SW and LED shorting blocks (HRD4 for device A and HDR3 for
//    device B).
// 3) Run the code.
// 4) Press the switch P1.4.  Every time the switch is pressed, the P1.3
//    LED should toggle.
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
//    -13 JULY 2007
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051F520A_defs.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             245000000   // Clock speed in Hz

SBIT (SW1, SFR_P1, 4);                 // Push-button switch on board
SBIT (LED1, SFR_P1, 3);                // Green LED

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);           // Configure the system clock
void Port_Init (void);                 // Configure the Crossbar and GPIO
void Interrupt_Init (void);            // Configure Interrupts

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   Oscillator_Init();                  // Initialize the system clock
   Port_Init ();                       // Initialize crossbar and GPIO
   Interrupt_Init();                   // Initialize External Interrupts

   EA = 1;

   P1MAT = 0x10;                       // Set up the port-match value on P1
   P1MASK = 0x10;                      // An event will occur if
                                       // (P1 & P1MASK) != (P1MAT & P1MASK)
                                       // The SW1 pin = 1 if the switch isn't
                                       // pressed

   while(1);                           // Infinite while loop waiting for
                                       // an interrupt
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
// This routine initializes the system clock to use the precision internal
// oscillator as its clock source.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
    OSCICN = 0x87;                     // Set internal oscillator to run
                                       // at its maximum frequency
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// Pinout:
//
// P1.3 - digital   push-pull   LED
// P1.4 - digital   open-drain  Switch1 (SW1)
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P1MDOUT  = 0x08;                    // LED is a push-pull output

   XBR1     = 0x40;                    // Enable crossbar and weak pullups
}

//-----------------------------------------------------------------------------
// Interrupt_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function enables Port Match Interrupts.
//
//-----------------------------------------------------------------------------
void Interrupt_Init (void)
{
   EIE1 |= 0x80;
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PortMatch_ISR
//-----------------------------------------------------------------------------
//
// Whenever an edge appears on P1.4, the LED is toggled.
//
//-----------------------------------------------------------------------------
void PortMatch_ISR (void) interrupt 14
{
   // If the Port Match event occured on Port 1, then the LED is toggled
   if((P1 & P1MASK) != (P1MAT & P1MASK))  {
      LED1 = !LED1; }

   // Waiting for switch 1 to be released
   while((P1 & P1MASK) != (P1MAT & P1MASK));
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
