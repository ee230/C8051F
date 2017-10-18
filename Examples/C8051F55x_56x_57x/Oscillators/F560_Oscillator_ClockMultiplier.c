//-----------------------------------------------------------------------------
// F560_Oscillator_ClockMultiplier.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the clock multiplier to multiply
// the internal system clock from 24 MHz to 48 MHz.  The system clock is output
// to P0.0.
//
//
// How To Test:
//
// 1) Remove the shorting block on J22 on the C8051F560-TB
// 2) Download and run the code
// 3) Measure the frequency output on P0.0.
//
//
// Target:         C8051F568 (Side A of a C8051F560-TB)
// Tool chain:     Raisonance / Keil / SDCC
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0 / 15 JAN 2009 (GP)
//    -Initial Revision
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F560_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// External Oscillator RC Frequency Selection
//-----------------------------------------------------------------------------
//
// Please initialize the following constant to the target frequency
//
#define INTOSC     24000000            // Maximum internal oscillator frequency
#define SYSCLK     48000000            // Multiplied Frequency

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   SFRPAGE = ACTIVE_PAGE;              // Set SFR Page for PCA0MD

   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   PORT_Init();                        // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator

   while (1) {};                       // Loop forever
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
// This function initializes the system clock to use the clock multiplier
// output as the system clock
//
// If the system clock speed is greater than 25 MHz, the Flash timing should
// be adjusted using FLSCL SFR.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   U16 i;                              // Delay counter

   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   OSCICN = 0xC7;                      // Set internal osc. divider to 1

   CLKMUL = 0x00;                      // Reset the Clock Multiplier
                                       // Set input to Internal Oscillator

   CLKMUL |= 0x80;                     // Enable the Clock Multiplier

   for (i = 0; i < 120; i++);          // Need to delay (5 us) 120 clock cycles

   CLKMUL |= 0xC0;                     // Initialize the Clock Multiplier

   while (!(CLKMUL & 0x20));

   FLSCL = 0x10;                       // Set new Flash read timing for higher
                                       // clock speed

   CLKSEL = 0x02;                      // Switch system clock to multiplier

   SFRPAGE = SFRPAGE_save;
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
// P0.0   digital    push-pull     /SYSCLK
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   P0MDOUT |= 0x01;                    // P0.0 is push-pull

   XBR1    = 0x02;                     // Route /SYSCLK to P0.0
   XBR2    = 0x40;                     // Enable Crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------