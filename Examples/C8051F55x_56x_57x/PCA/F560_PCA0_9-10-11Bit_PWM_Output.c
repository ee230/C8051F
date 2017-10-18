//-----------------------------------------------------------------------------
// F560_PCA0_9-10-11Bit_PWM_Output.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program sends a PWM waveform out of an I/O pin, using the PCA's
// 9/10/11-bit PWM Output Mode.  The duty cycle of the waveform is modified
// periodically within the main loop.
//
// In this example, PCA Module 0 is used to generate the waveform, and the
// crossbar is configured to send the CEX0 pin out on P0.0.
//
// How To Test:
//
// 1) Change the BIT_MODE global constant to choose the correct mode
// 2) Download code to a F54x device which has an oscilloscope monitoring P0.0
//    If running on a 'F560 target board, remove the shorting block from the
//    J22 header (Side A) or J5 header (Side B).
// 3) Run the program - the waveform should be visible on the oscilloscope.
// 4) Verify that the duty cycle of the waveform varies smoothly between
//    minimum and maximum values.
//
//
// Target:         C8051F560
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
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       24000000          // Internal oscillator frequency in Hz
//Defines the number of bits used in Pulse Width Modulation.
//9 = 9 bit, 10 = 10 bit, 11 = 11 bit
#define BIT_MODE 9

#if(BIT_MODE == 9)

   #define  PWM_MODE 0x81              //Enable 9-bit PWM
   #define  BIT_VALUE 512
   #define  DELAY 6000                 //Arbitrary delay so it's smooth

 #elif(BIT_MODE == 10)

   #define  PWM_MODE 0x82              //Enable 10-bit PWM
   #define  BIT_VALUE 1024
   #define  DELAY 4000

 #elif(BIT_MODE == 11)

   #define  PWM_MODE 0x83              //Enable 11-bit PWM
   #define  BIT_VALUE 2048
   #define  DELAY 2000

#else

   #error "Invalid value for BIT_MODE."

#endif

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void PCA0_Init (void);
INTERRUPT_PROTO (PCA0_ISR, INTERRUPT_PCA0);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

U16 CEX0_Compare_Value;                // Holds current PCA compare value

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   U16 delay_count;                    // Used to implement a delay
   bit duty_direction = 0;             // 0 = Decrease; 1 = Increase

   SFRPAGE = ACTIVE_PAGE;

   PCA0MD = 0x00;                      // Disable watchdog timer

   PORT_Init ();                       // Initialize crossbar and GPIO
   OSCILLATOR_Init ();                 // Initialize oscillator
   PCA0_Init ();                       // Initialize PCA0

   EA = 1;                             // Globally enable interrupts

   while (1)
   {
      // Wait a little while
      for (delay_count = DELAY; delay_count > 0; delay_count--);

      if (duty_direction == 1)         // Direction = Increase
      {
         // First, check the ECOM0 bit
         if ((PCA0CPM0 & 0x40) == 0x00)
         {
            PCA0CPM0 |= 0x40;          // Set ECOM0 if it is '0'
         }
         else                          // Increase duty cycle otherwise
         {
            CEX0_Compare_Value--;      // Increase duty cycle

            if (CEX0_Compare_Value == 0x0000)
            {
               duty_direction = 0;     // Change direction for next time
            }
         }
      }
      else                             // Direction = Decrease
      {
         if (CEX0_Compare_Value == (BIT_VALUE - 1)) //Check for overflow
         {
            PCA0CPM0 &= ~0x40;         // Clear ECOM0
            duty_direction = 1;        // Change direction for next time
         }
         else
         {
            CEX0_Compare_Value++;      // Decrease duty cycle
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

   OSCICN = 0xC7;                      // Set internal oscillator to run
                                       // at its maximum frequency
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
// P0.0   digital   push-pull     PCA0 CEX0
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   P0MDOUT |= 0x01;                    // Set CEX0 (P0.0) to push-pull

   XBR1    = 0x04;                     // Enable CEX0 on crossbar
   XBR2    = 0x40;                     // Enable crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the PCA time base, and sets up 9/10/11-bit PWM
// output mode for Module 0 (CEX0 pin).
//
// The frequency of the PWM signal generated at the CEX0 pin is equal to the
// PCA main timebase frequency divided by 512/1024/2048 respectively.
//
// The PCA time base in this example is configured to use SYSCLK, and SYSCLK
// is set up to use the internal oscillator running at 24.5 MHz.  Therefore,
// the frequency of the PWM signal will be:

// 9  bits => 24.5 MHz / 512  = 47.851 KHz
// 10 bits => 24.5 MHz / 1024 = 23.926 KHz
// 11 bits => 24.5 MHz / 2048 = 11.963 KHz

// Using different PCA clock sources or a different processor clock will
// result in a different frequency for the PWM signal.
//
//    -------------------------------------------------------------------------
//    How "9/10/11-Bit PWM Mode" Works:
//
//       The PCA's 9/10/11-bit PWM Mode works by setting an output pin low
//    every time the main n-bits in the PCA counter (PCA0H) overflows, and
//    then setting the pin high whenever a specific match condition is met.
//
//    Upon a PCA0 nth-bit overflow (PCA0 incrementing from 0x01FF to 0x0000),
//    the CEXn pin will be set low.
//
//    When the PCA0 register increments and matches the PCA0CPn register for
//    the selected module, the CEXn pin will be set high, except when the
//    ECOMn bit in PCA0CPMn is cleared to '0'.  By varying the value of the
//    PCA0CPn register, the duty cycle of the waveform can also be varied.
//
//    When ECOMn = '1', the duty cycle of the PWM waveform is:
//
//       9-bit PWM Duty Cycle = ( 512 - PCA0CPn) / 512
//      10-bit PWM Duty Cycle = (1024 - PCA0CPn) / 1024
//      11-bit PWM Duty Cycle = (2048 - PCA0CPn) / 2048
//
//    To set the duty cycle to 100%, a value of 0x0000 should be loaded into
//    the PCA0CPn register for the module being used (with ECOMn set to '1').
//    When the value of PCA0CPn is equal to 0x0000, the pin will never be
//    set low.
//
//    To set the duty cycle to 0%, the ECOMn bit in the PCA0CPMn register
//    should be cleared to 0.  This prevents the PCA0CPn match from occuring,
//    which results in the pin never being set high.
//
// The duty cycle of the PWM output signal in 9/10/11-bit PWM mode should be
// varied by writing to an "Auto-Reload" Register, which is dual-mapped into
// the PCA0CPHn and PCA0CPLn register locations. The data written to define
// the duty cycle should be right-justified in the registers. The auto-reload
// registers are accessed (read or written) when the bit ARSEL in PCA0PWM is
// set to ‘1’. The capture/compare registers are accessed when ARSEL is set
// to ‘0’.

// When adjusting the PWM duty cycle, the low byte of the PCA0CPn register
// (PCA0CPLn) should be written first, followed by the high byte (PCA0CPHn).
// Writing the low byte clears the ECOMn bit, and writing the high byte will
// restore it.  This ensures that a match does not occur until the full
// n-bit value has been written to the compare register.  Writing the high
// byte first will result in the ECOMn bit being set to '0' after the 16-bit
// write, and the duty cycle will also go to 0%.
//
// It is also advisable to wait until just after a match occurs before
// updating the PWM duty cycle.  This will help ensure that the ECOMn
// bit is not cleared (by writing PCA0CPLn) at the time when a match was
// supposed to happen.  This code implements the compare register update in
// the PCA ISR upon a match interrupt.
//    -------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void PCA0_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = ACTIVE_PAGE;

   // Configure PCA time base; overflow interrupt disabled
   PCA0CN = 0x00;                      // Stop counter; clear all flags
   PCA0MD = 0x08;                      // Use SYSCLK as time base

   PCA0CPM0 = 0x4B;                    // Module 0 = 9/10/11-bit PWM mode and
                                       // enable Module 0 Match and Interrupt
                                       // Flags

   SFRPAGE = CONFIG_PAGE;

   PCA0PWM = PWM_MODE;                 //Sets PWM mode according to BIT_MODE
                                       //Auto-Reload Registers are also enabled
   SFRPAGE = ACTIVE_PAGE;

   // Configure initial PWM duty cycle = 50%
   CEX0_Compare_Value = BIT_VALUE - (BIT_VALUE * 0.5);

   PCA0CPL0 = (CEX0_Compare_Value & 0x00FF);
   PCA0CPH0 = (CEX0_Compare_Value & 0xFF00)>>8;

   SFRPAGE = CONFIG_PAGE;
   PCA0PWM &= ~0x80;                   // Disable Auto-Reload Registers

   EIE1 |= 0x08;                       // Enable PCA interrupts

   SFRPAGE = ACTIVE_PAGE;
   // Start PCA counter
   CR = 1;

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// PCA0_ISR
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This is the ISR for the PCA.  It handles the case when a match occurs on
// channel 0, and updates the PCA0CPn compare register with the value held in
// the global variable "CEX0_Compare_Value".
//
//-----------------------------------------------------------------------------
INTERRUPT(PCA0_ISR, INTERRUPT_PCA0)
{

   CCF0 = 0;                           // Clear module 0 interrupt flag.

   SFRPAGE = CONFIG_PAGE;
   PCA0PWM |= 0x80;                    // Enable Auto-Reload Registers
   SFRPAGE = ACTIVE_PAGE;

   PCA0CPL0 = (CEX0_Compare_Value & 0x00FF);
   PCA0CPH0 = (CEX0_Compare_Value & 0xFF00)>>8;

   SFRPAGE = CONFIG_PAGE;
   PCA0PWM &= ~0x80;                   //Disable Auto-Reload Registers

}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------