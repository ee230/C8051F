//-----------------------------------------------------------------------------
// F36x_Oscillator_PLL.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the internal PLL in the 'F36x
// to produce a 98 MHz clock derived from the internal oscillator.
//
// Pinout:
//
//    P0.0 - SYSCLK
//
//    all other port pins unused
//
// How To Test:
//
// 1) Load the F36x_Oscillator_PLL.c file in the Silicon Labs IDE.
// 2) Compile and download the code to an 'F36x TB.
// 3) Measure the frequency output on P0.0.  The frequency should be 98 MHz.
//
//
// FID:            36X000018
// Target:         C8051F36x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -26 OCT 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F360.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK  98000000               // SYSCLK frequency in Hz

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void PLL_Init (void);
void Port_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init ();                 // Initialize Oscillator
   PLL_Init ();
   Port_Init ();                       // Initialize Port I/O

   SFRPAGE = LEGACY_PAGE;

   while (1) {};                       // Loop forever

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
// This function initializes the internal oscillator to its maximum frequency
// of 24.5 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN |= 0x03;

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// PLL_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the external oscillator
// in RC mode.
//
//-----------------------------------------------------------------------------
void PLL_Init (void)
{
   char i;
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   // Step 2. Set the PLLSRC bit (PLL0CN.2) to select the desired
   // clock source for the PLL.
   PLL0CN &= ~0x04;                    // Internal oscillator

   // Step 3. Program the Flash read timing bits, FLRT (FLSCL.5-4) to the
   // appropriate value for the new clock rate (see Section 15. Flash Memory
   // on page 199).
   SFRPAGE = LEGACY_PAGE;
   FLSCL |= 0x30;                      // >= 100 MHz
   SFRPAGE = CONFIG_PAGE;

   // Step 4. Enable power to the PLL by setting PLLPWR (PLL0CN.0) to ‘1’.
   PLL0CN |= 0x01;

   // Step 5. Program the PLL0DIV register to produce the divided reference
   // frequency to the PLL.
   PLL0DIV = 0x01;

   // Step 6. Program the PLLLP3-0 bits (PLL0FLT.3-0) to the appropriate
   // range for the divided reference frequency.
   PLL0FLT |= 0x01;

   // Step 7. Program the PLLICO1–0 bits (PLL0FLT.5–4) to the appropriate
   // range for the PLL output frequency.
   PLL0FLT &= ~0x30;

   // Step 8. Program the PLL0MUL register to the desired clock multiplication
   // factor.
   PLL0MUL = 0x04;

   // Step 9. Wait at least 5 µs, to provide a fast frequency lock.
   for (i = 100; i > 0; i--);

   // Step 10. Enable the PLL by setting PLLEN (PLL0CN.1) to ‘1’.
   PLL0CN |= 0x02;

   // Step 11. Poll PLLLCK (PLL0CN.4) until it changes from ‘0’ to ‘1’.
   while ((PLL0CN & 0x10) != 0x10);

   // Step 12. Switch the System Clock source to the PLL using the CLKSEL
   // register.
   CLKSEL = 0x04;

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
// Pinout:
//
// P0.0   digital    push-pull     /SYSCLK
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   // Buffered System Clock Output
   P0MDOUT |= 0x01;                    // P0.0 is push-pull

   // Crossbar Initialization
   XBR0    = 0x08;                     // Route /SYSCLK to first available pin
   XBR1    = 0x40;                     // Enable Crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------