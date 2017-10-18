//-----------------------------------------------------------------------------
// T622_PortMatch.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This software shows the necessary configuration to use Port Match as an
// interrupt source.  The code executes the initialization routines and then
// spins in an infinite while() loop.  If the button on P2.0 (on the 'T62x
// mother board) is pressed, then the Port Match Interrupt will cause the LED
// to toggle.
//
// Pinout:
//
// P0.1 - SW1 (Switch 1), Port Match
//
// P0.6 - LED1
//
// P1.0 - SW2 (Switch 2), Port Match
//
// P1.2 - LED2
//
// How To Test:
//
// 1) Compile and download code to a 'T622 device on a 'T62x mother board.
// 2) On the target board, verify the P0.1 and P1.0 switch jumpers are
//    populated (J9).
// 3) Verify P0.6 and P1.2 LED jumpers are populated (J10).
// 4) Run the code.
// 5) Press the switches.  Every time a switch is pressed, the P0.6 or P1.2
//    LEDs should toggle.
//
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (TP)
//    -02 SEP 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T622_defs.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK    12000000             // System Clock in Hz

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void Interrupt_Init (void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

SBIT (SW1, SFR_P0, 1);                 // Push-button switch on board
SBIT (SW2, SFR_P1, 0);                 // Push-button switch on board
SBIT (LED1, SFR_P0, 6);                // Green LED
SBIT (LED2, SFR_P1, 2);                // Green LED
SBIT (PM1, SFR_P0, 1);                 // Port Match Input 1
SBIT (PM2, SFR_P1, 0);                 // Port Match Input 2


//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // Disable watchdog timer

   Oscillator_Init ();                 // Initialize 12 MHz oscillator
   Port_Init ();                       // Initialize IO ports
   Interrupt_Init ();                  // Initialize Port Match Interrupts

   EA = 1;                             // Set Global interrupt enable

   P0MAT = 0x02;                       // Set up the port-match value on P0.1
   P0MASK = 0x02;                      // An event will occur if
                                       // (P0 & P0MASK) != (P0MAT & P0MASK)
                                       // The SW1 pin = 1 if the switch isn't
                                       // pressed
   P1MAT = 0x01;                       // Set up the port-match value on P1.0
   P1MASK = 0x01;                      // An event will occur if
                                       // (P1 & P1MASK) != (P1MAT & P1MASK)
                                       // The SW2 pin = 1 if the switch isn't
                                       // pressed

   while (1);                          // Wait for interrupt
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Port_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P0.1 - digital   open-drain  SW1 (Switch 1), Port Match Input 1
//
// P0.6 - digital   push-pull   LED1
//
// P1.0 - digital   open-drain  SW2 (Switch 2), Port Match Input 2
//
// P1.2 - digital   push-pull   LED2
//
//-----------------------------------------------------------------------------
void Port_Init ()
{
   P0MDOUT &= ~0x02;                   // P0.1 is open-drain
   P1MDOUT &= ~0x01;                   // P1.0 is open-drain
   P0MDOUT |= 0x40;                    // Configure P0.6 to push-pull
   P1MDOUT |= 0x04;                    // Configure P1.2 to push-pull

   P0SKIP |= 0x02;                     // Skip the pins used for Port Match
   P1SKIP |= 0x01;                     // and switches
   P0SKIP |= 0x40;                     // Skip the LED pins
   P1SKIP |= 0x04;

   XBR1 |= 0x40;                       // Enable crossbar
}

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 12 MHz and enable the missing clock detector.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN |= 0x03;                     // Initialize internal oscillator to
                                       // 12 MHz.
   RSTSRC  = 0x04;                     // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// Interrupt_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function enables Port Match Interrupt.
//
//-----------------------------------------------------------------------------
void Interrupt_Init (void)
{
   EIE2 |= 0x08;                       // Enable the Port Match interrupt
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PortMatch_ISR
//-----------------------------------------------------------------------------
//
// Whenever an edge appears on P1.0, LED1 is toggled.  Whenever an edge appears
// on P1.1, LED2 is toggled.
//
//-----------------------------------------------------------------------------
INTERRUPT(PortMatch_ISR, INTERRUPT_PORT_MATCH)
{
   // If the Port Match event occured on P1.0 (PM1), LED1 is toggled
   // Switch pressed = '0'
   if (PM1 == 0)
   {
      LED1 = !LED1;
   }
   // If the Port Match event occured on P1.1 (PM2), LED2 is toggled
   // Switch pressed = '0'
   else if (PM2 == 0)
   {
      LED2 = !LED2;
   }

   // Wait for the Switch to be released
   while((PM1 == 0) || (PM2 == 0));
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
