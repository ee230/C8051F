//-----------------------------------------------------------------------------
// F580_PCA1_16Bit_PWM_Output.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program sends a PWM waveform out of an I/O pin, using the PCA's
// 16-bit PWM Output Mode.  The duty cycle of the waveform is modified
// periodically within the main loop.
//
// In this example, PCA1 Module 6 is used to generate the waveform, and the
// crossbar is configured to send the CEX6 pin out on P0.0.
//
// How To Test:
//
// 1) Download code to a 'F580 device which has an oscilloscope monitoring P0.0
// 2) Run the program - the waveform should be visible on the oscilloscope.
// 3) Verify that the duty cycle of the waveform varies smoothly between
//    minimum and maximum values.
//
//
// Target:         C8051F580 (Side A of a C8051F580-TB)
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
#include <C8051F580_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       24000000          // Internal oscillator frequency in Hz

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

U16 CEX6_Compare_Value;                // Holds current PCA compare value

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   U8 delay_count;                     // Used to implement a delay
   bit duty_direction = 0;             // 0 = Decrease; 1 = Increase

   SFRPAGE = ACTIVE_PAGE;              // Set SFR Page for PCA0 registers

   PCA0MD = 0x00;                      // Disable watchdog timer

   PORT_Init ();                       // Initialize crossbar and GPIO
   OSCILLATOR_Init ();                 // Initialize oscillator
   PCA1_Init ();                       // Initialize PCA1

   EA = 1;                             // Globally enable interrupts

   SFRPAGE = ACTIVE2_PAGE;             // Set SFR Page for PCA1 registers

   while (1)
   {
      // Wait a little while
      for (delay_count = 120; delay_count > 0; delay_count--);

      if (duty_direction == 1)         // Direction = Increase
      {
         // First, check the ECOM1 bit
         if ((PCA1CPM6 & 0x40) == 0x00)
         {
            PCA1CPM6 |= 0x40;          // Set ECOM1 if it is '0'
         }
         else                          // Increase duty cycle otherwise
         {
            CEX6_Compare_Value--;      // Increase duty cycle

            if (CEX6_Compare_Value == 0x0000)
            {
               duty_direction = 0;     // Change direction for next time
            }
         }
      }
      else                             // Direction = Decrease
      {
         if (CEX6_Compare_Value == 0xFFFF)
         {
            PCA1CPM6 &= ~0x40;         // Clear ECOM1
            duty_direction = 1;        // Change direction for next time
         }
         else
         {
            CEX6_Compare_Value++;      // Decrease duty cycle
         }
      }
   }
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
// This function configures the PCA time base, and sets up 16-bit PWM output
// mode for Module 6 (CEX6 pin).
//
// The frequency of the PWM signal generated at the CEX6 pin is equal to the
// PCA main timebase frequency divided by 65536.
//
// The PCA time base in this example is configured to use SYSCLK, and SYSCLK
// is set up to use the internal oscillator running at 24 MHz.  Therefore,
// the frequency of the PWM signal will be 24 MHz / 65536 = 366.2 Hz.
// Using different PCA clock sources or a different processor clock will
// result in a different frequency for the PWM signal.
//
//    -------------------------------------------------------------------------
//    How "16-Bit PWM Mode" Works:
//
//       The PCA's 16-bit PWM Mode works by setting an output pin low every
//    time the main 16-bit PCA counter (PCA1H) overflows, and then setting
//    the pin high whenever a specific match condition is met.
//
//    Upon a PCA1 overflow (PCA1 incrementing from 0xFFFF to 0x0000), the
//    CEXn pin will be set low.
//
//    When the PCA1 register increments and matches the PCA1CPn register for
//    the selected module, the CEXn pin will be set high, except when the
//    ECOMn bit in PCA1CPMn is cleared to '0'.  By varying the value of the
//    PCA1CPn register, the duty cycle of the waveform can also be varied.
//
//    When ECOMn = '1', the duty cycle of the PWM waveform is:
//
//       16-bit PWM Duty Cycle = (65536 - PCA1CPn) / 65536
//
//    To set the duty cycle to 100%, a value of 0x0000 should be loaded into
//    the PCA1CPn register for the module being used (with ECOMn set to '1').
//    When the value of PCA1CPn is equal to 0x0000, the pin will never be
//    set low.
//
//    To set the duty cycle to 0%, the ECOMn bit in the PCA1CPMn register
//    should be cleared to 0.  This prevents the PCA1CPn match from occuring,
//    which results in the pin never being set high.
//
// When adjusting the PWM duty cycle, the low byte of the PCA1CPn register
// (PCA1CPLn) should be written first, followed by the high byte (PCA1CPHn).
// Writing the low byte clears the ECOMn bit, and writing the high byte will
// restore it.  This ensures that a match does not occur until the full
// 16-bit value has been written to the compare register.  Writing the high
// byte first will result in the ECOMn bit being set to '0' after the 16-bit
// write, and the duty cycle will also go to 0%.
//
// It is also advisable to wait until just after a match occurs before
// updating the PWM duty cycle.  This will help ensure that the ECOMn
// bit is not cleared (by writing PCA1CPLn) at the time when a match was
// supposed to happen.  This code implements the compare register update in
// the PCA ISR upon a match interrupt.
//    -------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void PCA1_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = ACTIVE2_PAGE;

   // Configure PCA time base; overflow interrupt disabled
   PCA1CN = 0x00;                      // Stop counter; clear all flags
   PCA1MD = 0x08;                      // Use SYSCLK as time base

   PCA1CPM6 = 0xCB;                    // Module 6 = 16-bit PWM mode and
                                       // enable Module 6 Match and Interrupt
                                       // Flags

   // Configure initial PWM duty cycle = 50%
   CEX6_Compare_Value = 65536 - (65536 * 0.5);

   PCA1CPL6 = (CEX6_Compare_Value & 0x00FF);
   PCA1CPH6 = (CEX6_Compare_Value & 0xFF00) >> 8;

   EIE2 |= 0x10;                       // Enable PCA1 interrupts

   // Start PCA1 counter
   CR1 = 1;

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// PCA1_ISR
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This is the ISR for the PCA.  It handles the case when a match occurs on
// CEX6, and updates the PCA1CPn compare register with the value held in
// the global variable "CEX6_Compare_Value".
//
//-----------------------------------------------------------------------------
INTERRUPT(PCA1_ISR, INTERRUPT_PCA1)
{
   CCF6 = 0;                           // Clear module 6 interrupt flag.

   PCA1CPL6 = (CEX6_Compare_Value & 0x00FF);
   PCA1CPH6 = (CEX6_Compare_Value & 0xFF00) >> 8;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------