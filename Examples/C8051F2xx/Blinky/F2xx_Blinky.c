//------------------------------------------------------------------------------------
// F2xx_Blinky.c
//------------------------------------------------------------------------------------
// Copyright (C) 2007 Silicon Laboratories, Inc.
//
// AUTH: BW
// DATE: 10 OCT 01
//
// This program flashes the green LED on the C8051F2xx target board about five times
// a second using the interrupt handler for Timer2.
//
// Target: C8051F2xx
//
// Tool chain: KEIL Eval 'c'
//

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <c8051f200.h>                    // SFR declarations

//------------------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F2xx
//------------------------------------------------------------------------------------

sfr16 DP       = 0x82;                    // data pointer
sfr16 ADC0     = 0xbe;                    // ADC0 data
sfr16 ADC0GT   = 0xc4;                    // ADC0 greater than window
sfr16 ADC0LT   = 0xc6;                    // ADC0 less than window
sfr16 RCAP2    = 0xca;                    // Timer2 capture/reload
sfr16 T2       = 0xcc;                    // Timer2

//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------

#define SYSCLK 2000000                    // approximate SYSCLK frequency in Hz

sbit  LED = P2^4;                         // green LED: '1' = ON; '0' = OFF

//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------
void PORT_Init (void);
void Timer2_Init (int counts);
void Timer2_ISR (void);

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main (void) {

   // disable watchdog timer
   WDTCN = 0xde;
   WDTCN = 0xad;

   PORT_Init ();
   Timer2_Init (SYSCLK / 12 / 10);        // Init Timer2 to generate interrupts
                                          // at a 10Hz rate.

   EA = 1;											// enable global interrupts

   while (1) {                            // spin forever
   }
}

//------------------------------------------------------------------------------------
// PORT_Init
//------------------------------------------------------------------------------------
//
// Configure port mux and GPIO ports
//
void PORT_Init (void)
{
   PRT2CF |= 0x10;                        // enable P2.4 (LED) as push-pull output
}

//------------------------------------------------------------------------------------
// Timer2_Init
//------------------------------------------------------------------------------------
//
// Configure Timer2 to auto-reload and generate an interrupt at interval
// specified by <counts> using SYSCLK/12 as its time base.
//
void Timer2_Init (int counts)
{
   T2CON  = 0x00;                         // Stop Timer2; configure for auto-reload
   CKCON &= ~0x20;                        // T2M=0 (use SYSCLK/12 as timebase)
   RCAP2  = -counts;                      // Init reload value
   T2     = 0xffff;                       // set to reload immediately
   ET2    = 1;                            // enable Timer2 interrupts
   TR2    = 1;                            // start Timer2
}

//------------------------------------------------------------------------------------
// Interrupt Service Routines
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Timer2_ISR
//------------------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer2 overflows.
//
void Timer2_ISR (void) interrupt 5
{
   TF2 = 0;                               // clear Timer2 overflow interrupt flag
   LED = ~LED;                            // change state of LED
}