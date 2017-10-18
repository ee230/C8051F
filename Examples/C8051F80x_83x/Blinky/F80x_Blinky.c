//-----------------------------------------------------------------------------
// F80x_Blinky.c
//-----------------------------------------------------------------------------
// Copyright (C) 2009, Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program flashes the yellow LED on the C8051F800 target board about
// five times a second using the interrupt handler for Timer2.
//
//
// How To Test:
//
// 1) Ensure that a shorting block is present on the J2 header of the 'F800 
//    target board.
// 2) Download code to the 'F800 target board.
// 3) Ensure that pins 1 and 2 are shorted together on the J3 header.
// 4) Run the program.  If the Green LED flashes, the program is working.
//
//
// Target:         C8051F800
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0 - 15 JUL 2009 (PKC)
//    -Initial Revision.
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F800_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (GREEN_LED,    SFR_P1, 0);        // '1' means ON, '0' means OFF

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000/8        // SYSCLK frequency in Hz

#define LED_ON           1
#define LED_OFF          0

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void Oscillator_Init (void);
void PORT_Init (void);
void Timer2_Init (int counts);
INTERRUPT_PROTO (Timer2_ISR, INTERRUPT_TIMER2);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (disable watchdog timer)
   
   Oscillator_Init ();
   PORT_Init ();

   Timer2_Init (SYSCLK / 12 / 10);     // Init Timer2 to generate interrupts
                                       // at a 10 Hz rate.

   EA = 1;                             // Enable global interrupts

   while (1);                          // Spin forever
}

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 24.5 MHz / 8.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN |= 0x80;                      // Enable the precision internal osc.
   OSCICN &= ~0x03;                    // IFCN=00b ( = div by 8)
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
// P1.0   digital   push-pull     Green LED
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P1MDOUT |= 0x01;                    // P1.0 is push-pull

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
//-----------------------------------------------------------------------------
void Timer2_Init (int counts)
{
   TMR2CN  = 0x00;                        // Stop Timer2; Clear TF2;
                                          // use SYSCLK/12 as timebase
   CKCON  &= ~0x60;                       // Timer2 clocked based on T2XCLK;

   TMR2RL  = -counts;                     // Init reload values
   TMR2    = 0xffff;                      // set to reload immediately
   ET2     = 1;                           // enable Timer2 interrupts
   TR2     = 1;                           // start Timer2

}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer2 overflows.
//
//-----------------------------------------------------------------------------
INTERRUPT(Timer2_ISR, INTERRUPT_TIMER2)
{
   TF2H = 0;                              // clear Timer2 interrupt flag
   GREEN_LED = !GREEN_LED;                // change state of LEDs
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
