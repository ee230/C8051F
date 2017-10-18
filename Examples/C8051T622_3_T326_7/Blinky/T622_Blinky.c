//-----------------------------------------------------------------------------
// T622_Blinky.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program flashes the P1.2 green LED on the C8051T62x target board
// five times a second using the interrupt handler for Timer2.
//
//
// How To Test:
//
// 1) Download code to a 'T622 daughter card connected to a 'T62x target board.
// 2) Place a jumper on J10 to connect P1.2 and LED2
// 3) Run the code; the P1.2 LED will blink.
//
// Note: P1 is tied to both J3 and J4 (Port 2) on the 'T62x Mother Board.
//
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
// Command Line:   None

//
// Release 1.1
//    -Modified testing instructions (ES)
//    -Updated tool chain information
//    -13 SEP 2009
//
// Release 1.0
//    -Initial Revision (TP)
//    -02 SEP 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T622_defs.h"            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       12000000 / 8      // SYSCLK frequency in Hz

#define BLINK_RATE   10                // Timer2 Interrupts per second

SBIT (LED1, SFR_P1, 2);                // LED='1' means ON
                                       // LED='1' means ON

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void Timer2_Init (int counts);

INTERRUPT_PROTO(Timer2_ISR, INTERRUPT_TIMER2);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init ();                 // Initialize system clock
   Port_Init ();                       // Initialize crossbar and GPIO

   Timer2_Init (SYSCLK / 12 / BLINK_RATE); // Init Timer2 to generate
                                           // interrupts at a 10Hz rate.

   EA = 1;                             // Enable global interrupts
   while (1) {}                        // Spin forever
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
// This function initializes the system clock to (12 Mhz / 8)
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x80;                      // Configure internal oscillator for
                                       // its lowest frequency
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
// P1.2   digital   push-pull     LED1
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   XBR0 = 0x00;                        // No digital peripherals selected
   XBR1 = 0x40;                        // Enable crossbar and weak pull-ups
   P1MDOUT |= 0x04;                    // Enable LED as a push-pull output
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1)  int counts - calculated Timer overflow rate
//                    range is postive range of integer: 0 to 32767
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
//-----------------------------------------------------------------------------
void Timer2_Init (int counts)
{
   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // Use SYSCLK/12 as timebase
   CKCON &= ~0x60;                     // Timer2 clocked based on T2XCLK;

   TMR2RL = -counts;                   // Init reload values
   TMR2 = 0xffff;                      // Set to reload immediately
   ET2 = 1;                            // Enable Timer2 interrupts
   TR2 = 1;                            // Start Timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer2 overflows.
//
//-----------------------------------------------------------------------------
INTERRUPT(Timer2_ISR, INTERRUPT_TIMER2)
{
   TF2H = 0;                           // Clear Timer2 interrupt flag
   LED1 = !LED1;                       // Change state of LED
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------