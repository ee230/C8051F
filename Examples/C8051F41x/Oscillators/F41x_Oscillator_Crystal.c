//-----------------------------------------------------------------------------
// F41x_Oscillator_Crystal.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the oscillator for use with an
// external quartz crystal.
//
//
// How To Test:
//
// 1) Ensure that a quartz crystal, 10Meg resistor, and 33pF loading capacitors
//    are installed.
// 2) Specify the crystal frequency in the constant <CRYSTAL_FREQUENCY>.
// 3) Download code to an 'F41x target board.
// 4) Measure the frequency output on P0.0.
//
//
// FID:            41X000029
// Target:         C8051F41x
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0
//    -Initial Revision (FB)
//    -17 JUL 2006

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F410_defs.h>                 // SFR declarations


//-----------------------------------------------------------------------------
// External Oscillator Crystal Frequency Selection
//-----------------------------------------------------------------------------
//
// Please initialize the following constant to the crystal frequency
//
#define CRYSTAL_FREQUENCY    32768     //  Crystal Frequency in Hz

// XFCN Setting Macro
#if  (CRYSTAL_FREQUENCY <= 20000)
   #define XFCN 0
#elif(CRYSTAL_FREQUENCY <= 58000)
   #define XFCN 1
#elif(CRYSTAL_FREQUENCY <= 155000)
   #define XFCN 2
#elif(CRYSTAL_FREQUENCY <= 415000)
   #define XFCN 3
#elif(CRYSTAL_FREQUENCY <= 1100000)
   #define XFCN 4
#elif(CRYSTAL_FREQUENCY <= 3100000)
   #define XFCN 5
#elif(CRYSTAL_FREQUENCY <= 8200000)
   #define XFCN 6
#elif(CRYSTAL_FREQUENCY <= 25000000)
   #define XFCN 7
#else
   #error "Crystal Frequency must be less than 25MHz"
#endif

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
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   PORT_Init();                        // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator

   while (1) {};                       // loop forever

}                                      // end of main()

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
// This function initializes the system clock to use the external oscillator
// in crystal mode.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{

   int i;                              // delay counter

   OSCXCN = (0x60 | XFCN);             // start external oscillator with
                                       // the appropriate XFCN setting
                                       // based on crystal frequency

   for (i=0; i < 256; i++);            // Wait for crystal osc. to start

   while (!(OSCXCN & 0x80));           // Wait for crystal osc. to settle

   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // VDD Monitor reset

   CLKSEL = 0x01;                      // Select external oscillator as system
                                       // clock source. /SYSCLK on the port
                                       // pin is equal to the system clock.

   OSCICN = 0x00;                      // Disable the internal oscillator.

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
// The oscillator pins should be configured as analog inputs when using the
// external oscillator in crystal mode.
//
// P0.0   digital    push-pull      /SYSCLK
// P1.0   analog     don't care     XTAL1
// P1.1   analog     don't care     XTAL2
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
    // Oscillator Pins
   P1MDIN &= ~0x03;                    // P1.0, P1.1 are analog
   P1SKIP |= 0x03;                     // P1.0, P1.1 skipped in the Crossbar
   P1     |= 0x03;                     // P1.0, P1.1 port latches set to '1'

   // Buffered System Clock Output
   P0MDOUT |= 0x01;                    // P0.0 is push-pull

   // Crossbar Initialization
   XBR0    = 0x08;                     // Route /SYSCLK to first available pin
   XBR1    = 0x40;                     // Enable Crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------