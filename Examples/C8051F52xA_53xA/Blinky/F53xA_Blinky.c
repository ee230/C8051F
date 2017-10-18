//-----------------------------------------------------------------------------
// F53xA_Blinky.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// File Description:
//
// This program blinks an LED every second.
//
// Pinout:
//
// P1.3 - LED
//
// How To Test:
//
// 1) Download code to either Side A or Side B of a C8051F530A target board.
// 2) Ensure that the LED jumper is populated; The jumper is located on
//    HDR4 for the Side A LED and HDR3 for the Side B LED.
// 3) Run the code.
// 4) The code is running when the LED blinks.
//
// Target:         C8051F52xA/F53xA
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.2 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.1
//    -29 JAN 2008 (TP)
//    -Updated for C8051F52xA/F53xA
//
// Release 1.0
//    -Initial Revision (CG)
//    -07 SEP 2006
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include <C8051f520A_defs.h>           // SFR declarations

//-----------------------------------------------------------------------------
// SBIT definitions
//-----------------------------------------------------------------------------

SBIT (LED, SFR_P1, 3);                 // LED definition = P1.3

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

long Counter;                          // Counter used to measure time between
                                       // toggles of the LED

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define SYSCLK 24500000                // System Clock in MHz

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void Timer2_Init (void);

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------
void main ( void )
{
   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   Counter = 0;                        // Initialize timer internal counter

   OSCILLATOR_Init ();                 // Initialize the oscillator
   PORT_Init ();                       // Initialize the crossbar and ports
   Timer2_Init ();                     // Initialize Timer2

   EA = 1;                             // Enable global interrupts
   while (1)                           // Loop forever
   {
      if ( Counter == 0 )              // If the counter is zero reload it
      {                                // and toggle the LED
         Counter = 100;                // Initialize the counter to be
                                       // decremented by timer2; time
                                       // in miliseconds
         LED = ~LED;                   // Toggle LED
      }
   }
}

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
// This function initializes the system clock to (24.5 Mhz)
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN = 0x87;                      // Configure internal oscillator for
                                       // its highest frequency
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P1.3   digital   push-pull     LED1
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P1MDOUT = 0x08;                     // Force LED pin to be push-pull
   XBR1 = 0x40;                        // Enable crossbar, enable weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes Timer2 to interrupt every 1 ms.
//
// Note: This function uses the value of SYSCLK to determine the 1 ms wait
// time.
//-----------------------------------------------------------------------------
void Timer2_Init (void)
{
   CKCON |= 0x10;                      // Timer2 uses the SYSCLK
   TMR2RL = -((SYSCLK+500L)/1000);     // Time for 1msec @SYSCLK in both reload
   TMR2 = TMR2RL;                      // and timer register
   TMR2CN = 0x04;                      // Timer2 in 16-bit reload timer mode
   ET2 = 1;                            // Enable interrupt in timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// Timer2 Interrupt. Asserted every 1msec and decrements the the counter used
// to trigger an LED toggle.
//-----------------------------------------------------------------------------
void Timer2_ISR (void) interrupt 5
{
   TF2H = 0;                           // Reset timer2 interrupt flag
   if ( Counter )                      // Decrement counter if necessary
   {
      Counter--;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------