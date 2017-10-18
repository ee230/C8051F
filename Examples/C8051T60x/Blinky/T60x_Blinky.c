//-----------------------------------------------------------------------------
// T60x_Blinky.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program flashes the green LED on the C8051T60x target board about 
// five times a second using the interrupt handler for Timer2.
//
// How To Test:
//   1) Download code to a T60x device.
//   2) Define chip number being used under CHIPNUM
//   3) Run code and verify the LED is blinking
//
// FID:            
// Target:         C8051T60x
// Tool chain:     Keil C51 8.0 / Keil EVAL C51
// Command Line:   None
//
// Release 1.1 / 29 JUL 2008 (ADT)
//    - Updated to use compiler_defs.h
//    - Includes support for T606
//
// Release 1.0
//    -Initial Revision (BD)
//    -17 JAN 2007

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <C8051T600_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define CHIPNUM   600                  // Define the chip number being used
                                       // Options: 600, 601, 602, 603,
                                       //          604, 605, 606

#define SYSCLK       24500000 / 8      // SYSCLK frequency in Hz

SBIT (SW1, SFR_P0, 3);                 // SW1='0' means switch pressed

#if (CHIPNUM == 606)
 SBIT (LED, SFR_P0, 2);                // LED='1' means ON
#else 
 SBIT (LED, SFR_P0, 6);                // LED='1' means ON
#endif

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void SYSCLK_Init (void);
void PORT_Init (void);
void Timer2_Init (U16 counts);

INTERRUPT_PROTO (TIMER2_ISR, INTERRUPT_TIMER2);
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void) 
{
   // disable watchdog timer
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer 
                                       // enable)

   REG0CN |= 0x01;                     // OTP Power Controller Enable
	
   SYSCLK_Init ();                     // Initialize system clock to 
                                       // (24.5 / 8) Mhz
   PORT_Init ();                       // Initialize crossbar and GPIO
   Timer2_Init (SYSCLK / 12 / 10);     // Init Timer2 to generate 
                                       // interrupts at a 10Hz rate.

   EA = 1;							    	   // enable global interrupts

   while (1);                          // spin forever
}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// This routine initializes the system clock to use the internal 24.5MHz / 8 
// oscillator as its clock source.  Also enables missing clock detector reset.
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   OSCICN = 0x04;                      // configure internal oscillator for
                                       // its lowest frequency
   RSTSRC = 0x04;                      // enable missing clock detector
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Configure the Crossbar and GPIO ports.
// P0.0 - 
// P0.1 -
// P0.2 - LED (push-pull, T606)
// P0.3 - SW1
// P0.4 - 
// P0.5 - 
// P0.6 - LED (push-pull, T600/1/2/3/4/5)
// P0.7 - C2D
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   XBR0     = 0x40;                    // skip P0.6 (LED) in crossbar pin
                                       // assignments
   XBR1     = 0x00;                    // no digital peripherals selected
   XBR2     = 0x40;                    // Enable crossbar and weak pull-ups
#if (CHIPNUM == 606)
   P0MDOUT |= 0x04;
#else
   P0MDOUT |= 0x40;                    // enable LED as a push-pull output
#endif
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at 
// interval specified by <counts> using SYSCLK/48 as its time base.
//-----------------------------------------------------------------------------
void Timer2_Init (U16 counts)
{
   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;
                                       // use SYSCLK/12 as timebase
   CKCON  &= ~0x60;                    // Timer2 clocked based on T2XCLK;

   TMR2RL  = -counts;                  // Init reload values
   TMR2    = 0xffff;                   // set to reload immediately
   ET2     = 1;                        // enable Timer2 interrupts
   TR2     = 1;                        // start Timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TIMER2_ISR
//-----------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer2 overflows.
//-----------------------------------------------------------------------------
INTERRUPT (TIMER2_ISR, INTERRUPT_TIMER2)
{
   TF2H = 0;                           // clear Timer2 interrupt flag
   LED = ~LED;                         // change state of LED
}