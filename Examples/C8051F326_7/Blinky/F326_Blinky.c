//-----------------------------------------------------------------------------
// F326_Blinky.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program flashes the P2.2 green LED on the C8051F326 target board
// five times a second using the interrupt handler for Timer1.
//
//
// How To Test:
//
// 1) Download code to a 'F326 target board
// 2) Run the code and if the P2.2 LED blinks, the code works
//
//
// FID:            326000001
// Target:         C8051F326/7
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (GP)
//    -01 DEC 2005
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "c8051f326.h"                 // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK         1200000 / 8     // SYSCLK frequency in Hz
#define REFRESH_RATE   SYSCLK / 4 / 10 // Generate interrupts at 10 Hz

sbit LED = P2^2;                       // LED='1' means ON

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F326
//-----------------------------------------------------------------------------

sfr16 TMR1     = 0x8A;                 // Timer1 counter

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void Timer1_Init (void);
void Timer1_ISR (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   OSCILLATOR_Init ();                 // Initialize system clock
   PORT_Init ();                       // Initialize crossbar and GPIO
   Timer1_Init ();                     // Init Timer1 to generate
                                       // interrupts at a 10Hz rate.

   EA = 1;                             // Enable global interrupts

   while (1) {}                        // Spin forever
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
// This routine initializes the system clock to use the internal 12MHz / 8
// oscillator as its clock source.
//
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void)
{
   OSCICN = 0x80;                      // Configure internal oscillator
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the GPIO ports.
//
// P2.2   digital   push-pull     LED1
//-----------------------------------------------------------------------------

void PORT_Init (void)
{
   P2MDOUT |= 0x04;                    // Enable LED as a push-pull output
}

//-----------------------------------------------------------------------------
// Timer1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer1 to 16-bit mode and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
//-----------------------------------------------------------------------------
void Timer1_Init (void)
{
   TCON    = 0x00;                     // Stop Timer1; Clear TF1;
   TMR1    = -REFRESH_RATE;            // Clear Timer1

   TMOD    = 0x10;                     // Timer1 is a Mode 1: 16-bit timer;

   CKCON   = 0x01;                     // Timer 0/1 clocked by SYSCLK/4

   ET1     = 1;                        // Enable Timer1 interrupts
   TR1     = 1;                        // Start Timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer1_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer1 overflows.
//
//-----------------------------------------------------------------------------

void Timer1_ISR (void) interrupt 3
{
   TF1 = 0;                            // Clear Timer1 interrupt flag
   LED = ~LED;                         // Change state of LED
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------