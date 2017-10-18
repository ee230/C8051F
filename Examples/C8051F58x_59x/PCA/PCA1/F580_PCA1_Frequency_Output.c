//-----------------------------------------------------------------------------
// F580_PCA1_Frequency_Output.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program sends a square wave out of an I/O pin, using the PCA's
// Frequency Output Mode.
//
// In this example, PCA1 Module 6 is used to generate the waveform, and the
// crossbar is configured to send the CEX6 pin out on P0.0.
//
// How To Test:
//
// 1) Download code to a 'F580 device which has an oscilloscope monitoring P0.0
// 2) Make sure the jumper is disconnected on J22.
// 3) Run the program - the waveform should be visible on the oscilloscope.
//    The frequency measured should be the same as <CEX6_FREQUENCY>.
//
//
// Target:         C8051F580 (C8051F580-TB)
// Tool chain:     Keil C51 8.0 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0 / 21 JUL 2008 (ADT)
//    - Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F580_defs.h>            // SFR declaration

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       24000000          // Internal oscillator frequency in Hz

#define CEX6_FREQUENCY  50000          // Frequency to output on CEX6 (Hz)

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void PCA1_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   SFRPAGE = ACTIVE_PAGE;              // Set SFR Page for PCA0MD

   PCA0MD &= ~0x40;                    // Disable watchdog timer

   PORT_Init ();                       // Initialize crossbar and GPIO
   OSCILLATOR_Init ();                 // Initialize oscillator
   PCA1_Init ();                       // Initialize PCA1

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
// at 24 MHz.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   OSCICN = 0x87;                      // Set internal oscillator to run
                                       // at its maximum frequency

   CLKSEL = 0x00;                      // Select the internal osc. as
                                       // the SYSCLK source

   SFRPAGE = SFRPAGE_save;
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
// P0.0   digital   push-pull     PCA1 CEX6
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   P0MDOUT |= 0x01;                    // Set CEX6 (P0.0) to push-pull

   XBR3    = 0x01;                     // Enable CEX6 on crossbar
   XBR2    = 0x40;                     // Enable crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// PCA1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the PCA1 time base, and sets up frequency output
// mode for Module 6 (CEX6 pin).
//
// The frequency generated at the CEX6 pin is equal to CEX6_FREQUENCY Hz,
// which is defined in the "Global Constants" section at the beginning of the
// file.
//
// The PCA time base in this example is configured to use SYSCLK / 12.
// The frequency range that can be generated using this example is ~4 kHz to
// ~1 MHz when the processor clock is 24 MHz.  Using different PCA clock
// sources or a different processor clock will generate different frequency
// ranges.
//
//    -------------------------------------------------------------------------
//    How "Frequency Output Mode" Works:
//
//       The PCA's Frequency Output Mode works by toggling an output pin every
//    "N" PCA clock cycles.  The value of "N" should be loaded into the PCA1CPH
//    register for the module being used (in this case, module 0).  Whenever
//    the register PCA1L (PCA counter low byte) and the module's PCA1CPL value
//    are equivalent, two things happen:
//
//    1) The port pin associated with the PCA module toggles logic state
//    2) The value stored in PCA1CPH is added to PCA1CPL.
//
//    Using this mode, a square wave is produced at the output port pin of the
//    PCA module. The speed of the waveform can be changed by changing the
//    value of the PCA1CPH register.
//    -------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void PCA1_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = ACTIVE2_PAGE;

   // Configure PCA time base; overflow interrupt disabled
   PCA1CN = 0x00;                      // Stop counter; clear all flags
   PCA1MD = 0x00;                      // Use SYSCLK/12 as time base

   PCA1CPM6 = 0x46;                    // Module 6 = Frequency Output mode

   // Configure frequency for CEX0
   // PCA1CPH6 = (SYSCLK/12)/(2*CEX6_FREQUENCY), where:
   // SYSCLK/12 = PCA time base
   // CEX6_FREQUENCY = desired frequency
   PCA1CPH6 = (SYSCLK/12)/((U32) 2* (U32) CEX6_FREQUENCY);

   // Start PCA1 counter
   CR1 = 1;

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------