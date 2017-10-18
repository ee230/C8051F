//-----------------------------------------------------------------------------
// F580_Timer4_Toggle_Output.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program uses Timer 4 Toggle Output mode to toggle a port pin every
// Timer 4 overflow.
//
// In this example, Timer 4 will create a square wave of frequency
// <OUTPUT_FREQ> with a duty cycle of 50%. Since Auto-Reload is enabled,
// the value in TMR4CAP will be loaded into the register every timer overflow.
// The output pin associated with T4 (P0.0) will also change state.
//
// How To Test:
//
// 1) Download code to a F580 device which has an oscilloscope monitoring P0.0
// 2) Run the program - the waveform should be visible on the oscilloscope.
//
// Target:         C8051F580 / C8051F580
// Tool chain:     Keil C51 8.00 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0 / 29 JUL 2008 (ADT)
//    -Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <c8051f580_defs.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK           24000000      // Internal oscillator frequency in Hz

#define OUTPUT_FREQ      10000          // Frequency output in Hz

// SYSCLK cycles per overflow to create a 50% duty cycle
#define TIMER4_RELOAD    (U16)(0xFFFF - (SYSCLK/OUTPUT_FREQ/2))


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void Timer4_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   SFRPAGE = ACTIVE_PAGE;              // Set SFR Page for PCA0MD

   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init ();                       // Initialize crossbar and GPIO
   Timer4_Init ();                     // Initialize Timer 2

   while (1);
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
// This function initializes the system clock to use the internal oscillator
// at 24 MHz
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   char SFRPAGE_save = SFRPAGE;        // Save Current SFR page
   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x87;                      // Set internal oscillator to run
                                       // at its maximum frequency

   CLKSEL = 0x00;                      // Select the internal osc. as
                                       // the SYSCLK source

   SFRPAGE = SFRPAGE_save;             // Restore SFR page
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
// P0.0   digital   push-pull      T4
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;          // Save Current SFR page
   SFRPAGE = CONFIG_PAGE;              // Set SFR Page

   P0MDOUT = 0x01;                     // P1.6 is push-pull

   XBR3    = 0x10;                     // T4 routed to P0.0
   XBR2    = 0x40;                     // Enable crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;             // Restore SFR Page
}

//-----------------------------------------------------------------------------
// Timer4_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Initialize Timer4 for Toggle Output mode. Every overflow will toggle the
// port pin associated with T24 (P0.0). You can also monitor the state of
// the output by looking at TOG4(TMR4CF.2).
//-----------------------------------------------------------------------------
void Timer4_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;          // Save Current SFR page
   SFRPAGE = ACTIVE2_PAGE;             // Set SFR Page

   TMR4CN = 0x00;                      // Timer4 is in Auto-Reload mode

   TMR4CF = 0x0A;                      // Timer4 uses SYSCLK
                                       // Toggle output bit

   // Load reload value
   TMR4CAPH = (U8)((TIMER4_RELOAD >> 8) & 0x00FF);
   TMR4CAPL = (U8)(TIMER4_RELOAD & 0xFF);

   TMR4H = TMR4CAPH;                   // Initialize Timer 4
   TMR4L = TMR4CAPL;

   TMR4CN |= 0x04;                     // Timer 4 run enabled

   SFRPAGE = SFRPAGE_save;             // Restore SFR Page
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------