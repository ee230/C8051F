//-----------------------------------------------------------------------------
// F580_PCA1_High_Speed_Output.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program sends a square wave out on an I/O pin, using the PCA1's
// High-Speed Output Mode.
//
// In this example, PCA1 Module 6 is used to generate the waveform, and the
// crossbar is configured to send the CEX6 pin out on P0.0.
//
// How To Test:
//
// 1) Download code to a 'F580 device which has an oscilloscope monitoring P0.0
// 2) Make sure there is no jumper on J22.
// 3) Run the program - the waveform should be visible on the oscilloscope.
//
// Target:         C8051F580 (C8051F580-TB)
// Tool chain:     Keil C51 8.0 / Keil EVAL C51
// Command Line:   None
//
// Release 1.1 / 14 APR 2009 (GP)
//    -Fixed clearing of PCA flags in PCA1 ISR
//
// Release 1.0 / 21 JUL 2008 (ADT)
//    - Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F580_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       24000000          // Internal oscillator frequency in Hz

#define CEX6_FREQUENCY  10000          // Frequency to output in Hz

#define DIVIDE_RATIO (SYSCLK/CEX6_FREQUENCY/2) // SYSCLK cycles per interrupt

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void PCA1_Init (void);

INTERRUPT_PROTO (PCA1_ISR, INTERRUPT_PCA1);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

U16 Next_Compare_Value;                // Next edge to be sent out in HSO mode

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   SFRPAGE = ACTIVE_PAGE;              // Set SFR Page for PCA0 registers

   PCA0MD &= ~0x40;                    // Disable watchdog timer

   PORT_Init ();                       // Initialize crossbar and GPIO
   OSCILLATOR_Init ();                 // Initialize oscillator
   PCA1_Init ();                       // Initialize PCA1

   EA = 1;                             // Globally enable interrupts

   while (1);                          // Spin here to wait for ISR
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
// This function configures the PCA1 time base, and sets up High-Speed output
// mode for Module 6 (CEX6 pin).
//
// The frequency of the square wave generated at the CEX6 pin is defined by
// the parameter CEX6_FREQUENCY.
//
// The maximum square wave frequency output for this example is about 230 kHz.
// The minimum square wave frequency output for this example is about 190 Hz
//
// The PCA time base in this example is configured to use SYSCLK, and SYSCLK
// is set up to use the internal oscillator running at 24 MHz.
// Using different PCA clock sources or a different processor clock will
// result in a different frequency for the square wave, and different
// maximum and minimum options.
//
//    -------------------------------------------------------------------------
//    How "High Speed Output Mode" Works:
//
//       The PCA's High Speed Output Mode works by toggling the output pin
//    associated with the module every time the PCA1 register increments and
//    the new 16-bit PCA1 counter value matches the module's capture/compare
//    register (PCA1CPn). When initially enabled in high-speed output mode, the
//    CEXn pin associated with the module will be at a logic high state.  The
//    first match will cause a falling edge on the pin.  The next match will
//    cause a rising edge on the pin, and so on.
//
//    By loading the PCA1CPn register with the next match value every time a
//    match happens, arbitrary waveforms can be generated on the pin with high
//    levels of precision.
//    -------------------------------------------------------------------------
//
// When setting the capture/compare register for the next edge value, the low
//  byte of the PCA1CPn register (PCA1CPLn) should be written first, followed
//  by the high byte (PCA1CPHn).  Writing the low byte clears the ECOMn bit,
//  and writing the high byte will restore it.  This ensures that a match does
//  not occur until the full 16-bit value has been written to the compare
//  register.  Writing the high byte first will result in the ECOMn bit being
//  set to '0' after the 16-bit write, and the next match will not occur at
// the correct time.
//
// It is best to update the capture/compare register as soon after a match
//  occurs as possible so that the PCA counter will not have incremented past
//  the next desired edge value. This code implements the compare register
//  update in the PCA ISR upon a match interrupt.
//
//-----------------------------------------------------------------------------
void PCA1_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = ACTIVE2_PAGE;

   // Configure PCA time base; overflow interrupt disabled
   PCA1CN = 0x00;                      // Stop counter; clear all flags
   PCA1MD = 0x08;                      // Use SYSCLK as time base

   PCA1CPM6 = 0x4D;                    // Module 6 = High Speed Output mode,
                                       // Enable Module 6 Interrupt flag,
                                       // Enable ECOM bit

   PCA1L = 0x00;                       // Reset PCA Counter Value to 0x0000
   PCA1H = 0x00;

   PCA1CPL6 = DIVIDE_RATIO & 0x00FF;   // Set up first edge
   PCA1CPH6 = (DIVIDE_RATIO & 0xFF00) >> 8;

   // Set up the variable for the following edge
   Next_Compare_Value = PCA1CP6 + DIVIDE_RATIO;

   EIE2 |= 0x10;                       // Enable PCA1 interrupts

   CR1 = 1;                            // Start PCA1

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PCA1_ISR
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This is the ISR for the PCA1.  It handles the case when a match occurs on
// CEX6, and updates the PCA1CPn compare register with the value held in
// the global variable "Next_Compare_Value".
//
//-----------------------------------------------------------------------------
INTERRUPT(PCA1_ISR, INTERRUPT_PCA1)
{
   if (CCF6)                           // If Module 6 caused the interrupt
   {
      CCF6 = 0;                        // Clear module 6 interrupt flag.

      PCA1CPL6 = (Next_Compare_Value & 0x00FF);
      PCA1CPH6 = (Next_Compare_Value & 0xFF00) >> 8;

      // Set up the variable for the following edge
      Next_Compare_Value = PCA1CP6 + DIVIDE_RATIO;
   }
   else                                // Interrupt was caused by other bits.
   {
      PCA1CN &= ~0xBE;                 // Clear other interrupt flags for PCA
   }
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------