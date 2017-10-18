//-----------------------------------------------------------------------------
// F41x_PortMatch.c
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
// Port Match:
// P0.4
// P1.4
//
// P1.4 - SW1 (Switch 1)
// P1.5 - SW2 (Switch 2) ----> P0.4
// P2.1 - LED1
// P2.3 - LED2
//
// How To Test:
//
// 1) Compile and download code to a 'F41x target board.
// 2) Ensure that the four shorting blocks are installed on the J5 header
// 3) On the target board, connect P1.5 to P0.4
// 4) Press the the two switches.  When they are pressed, the P2.1 or P2.3
//    LED should toggle.
//
// Target:         C8051F41x
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0
//    -Initial Revision (SM)
//    -13 JULY 2007
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F410_defs.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             24500000    // Clock speed in Hz

sbit SW1 = P1^4;                       // Push-button switch on board
sbit SW2 = P1^5;                       // Push-button switch on board
sbit LED1 = P2^1;                      // Green LED
sbit LED2 = P2^3;                      // Green LED

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
   Interrupt_Init();                   // Initialize Port Match Interrupts

   EA = 1;

   P1MAT  |= 0x10;                     // Set up the port-match value on P1
   P1MASK |= 0x10;                     // An event will occur if
                                       // (P1 & P1MASK) != (P1MAT & P1MASK)
                                       // The SW pin = 1 if the switch isn't
                                       // pressed

   P0MAT  |= 0x10;                     // Set up the port-match value on P0
   P0MASK |= 0x10;

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
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x87;                      // Set internal oscillator to run
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
// P1.4 - digital	open-drain 	SW1 (Switch 1)
// P1.5 - digital	open-drain 	SW2 (Switch 2)
// P2.2 - digital	push-pull 	LED1
// P2.3 - digital	push-pull	LED2
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   XBR1     = 0x40;                    // Enable crossbar and weak pullups

   P2MDOUT  = 0x0A;                    // LED1 and LED2 are push-pull outputs

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
   EIE2 |= 0x02;
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORTMATCH_ISR
//-----------------------------------------------------------------------------
//
// Whenever an edge appears on P0.4, LED1 is toggled.  When an edge appears on
// P1.4, LED2 is toggled.
//
//-----------------------------------------------------------------------------
void PORTMATCH_ISR (void) interrupt 16
{
   // If the Port Match event occured on Port 1 than LED1 is toggled
   if((P1 & P1MASK) != (P1MAT & P1MASK))  {
      LED1 = !LED1; }

   // Waiting for switch 1 to be released
   while((P1 & P1MASK) != (P1MAT & P1MASK));

   // If the Port Match event occured on Port 0 than LED2 is toggled
   if((P0 & P0MASK) != (P0MAT & P0MASK))  {
      LED2 = !LED2;  }

   // Waiting for switch 2 to be released
   while((P0 & P0MASK) != (P0MAT & P0MASK));
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
