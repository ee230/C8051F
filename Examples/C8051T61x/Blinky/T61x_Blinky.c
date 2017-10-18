//-----------------------------------------------------------------------------
// Blinky.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program flashes the green LED on the C8051T61x target board about
// five times a second using the interrupt handler for Timer2.
//
// FID:
// Target:         C8051T61x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.1
//    -added include for compiler_defs.h (HA)
//    -01 NOV 2007
// Release 1.0
//    -Initial Revision (PD)
//    -08 AUG 2007

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // SFR declarations
#include <C8051T610_defs.h>            // SFR declarations



//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000 / 8      // SYSCLK frequency in Hz

sbit LED = P1^0;                       // LED='1' means ON
sbit SW1 = P0^3;                       // SW1='0' means switch pressed

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void SYSCLK_Init (void);
void Port_Init (void);
void Timer2_Init (int counts);
void Timer2_ISR (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void) {


   PCA0MD &= ~0x40;                    // WDTE = 0 (disables watchdog timer

   SYSCLK_Init ();                     // Initialize system clock to
                                       // 24.5/8 MHz
   Port_Init ();                       // Initialize crossbar and GPIO
   Timer2_Init (SYSCLK / 12 / 10);     // Init Timer2 to generate
                                       // interrupts at a 10Hz rate.

   EA = 1;                             // Enable global interrupts

   while (1) {                         // Spin forever
   }
}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// This routine initializes the system clock to use the internal 24.5MHz / 8
// oscillator as its clock source.  Also enables missing clock detector reset.
//
void SYSCLK_Init (void)
{
   OSCICN = 0x80;                      // Configure internal oscillator for
                                       // its lowest frequency
   RSTSRC = 0x04;                      // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Configure the Crossbar and GPIO ports.
// P0.0 -
// P0.1 -
// P0.2 -
// P0.3 - 
// P0.4 -
// P0.5 -
// P0.6 - 
// P0.7 - SW1
// P1.0 - LED
//
void Port_Init (void)
{
   P1SKIP |= 0x01;                     // Skip P1.0 (LED) in crossbar pin
                                       // assignments
   P1MDOUT |= 0x01;                    // Configure P1.0 (LED) to push-pull
                                       // output
   XBR1     = 0x40;                    // Enable crossbar and weak pull-ups
   P0MDOUT |= 0x40;                    // Enable LED as a push-pull output
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
void Timer2_Init (int counts)
{
   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;
                                       // use SYSCLK/12 as timebase
   CKCON  &= ~0x60;                    // Timer2 clocked based on T2XCLK;

   TMR2RL  = -counts;                  // Init reload values
   TMR2    = 0xffff;                   // Set to reload immediately
   ET2     = 1;                        // Enable Timer2 interrupts
   TR2     = 1;                        // Start Timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer2 overflows.
//
void Timer2_ISR (void) interrupt 5
{
   TF2H = 0;                           // Clear Timer2 interrupt flag
   LED = ~LED;                         // Change state of LED
}