//-----------------------------------------------------------------------------
// F326_Oscillator_CMOS.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This example demonstrates how to initialize for and switch to an external
// CMOS Oscillator.
// - Pinout:
//    P0.0 -> /SYSCLK
//
//    P0.3 -> XTAL2 - External CMOS Oscillator input
//
//    P2.2 -> LED
//
//    all other port pins unused
//
// How To Test:
//
// 1) Define the input CMOS clock frequency using <CMOS_clock>
// 2) Compile and download code to a 'F326/7 device.
// 3) Connect the CMOS input clock to XTAL2 / P0.3 (or J10 on the TB)
// 4) Run the code:
//      - the test will blink an LED at a frequency based on the CMOS
//         Oscillator
//      - the 'F326/7 will also output the SYSCLK to a port pin (P0.0) for
//         observation
//
//
// FID:            326000029
// Target:         C8051F326/7
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -1 JUNE 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F326.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define CMOS_clock 3062500             // CMOS oscillator frequency

// Timer2 using SYSCLK/12 as its time base
// Timer2 counts 65536 SYSCLKs per Timer2 interrupt
// LED target flash rate = 10 Hz (may be slower, depending on CMOS_clock)
//
// If CMOS_clock is too slow to divide into a number of counts,
// <count> will always remain 0. In this case, the LED Flash rate will be
// slower than 10 Hz.
#define LED_interrupt_count CMOS_clock/12/65536/10

sbit LED = P2^2;                       // LED='1' means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void ExtCMOSOsc_Init (void);
void Port_Init (void);
void Timer1_Init (void);

void Timer1_ISR (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, switches to the external CMOS
// oscillator, and loops forever, blinking the LED.
//
void main (void) {

   SYSCLK_Init ();                     // Initialize system clock to 12MHz

   ExtCMOSOsc_Init ();                 // Initialize for and switch to the
                                       // external CMOS oscillator

   Port_Init ();                       // Initialize crossbar and GPIO

   Timer1_Init ();                     // Init Timer1 to generate
                                       // interrupts for the LED.

   EA = 1;                             // Enable global interrupts

   while (1) {                         // Spin forever
   }
}

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SYSCLK_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal 12 MHz
// oscillator as its clock source.  Also enables missing clock detector reset.
//
void SYSCLK_Init (void)
{
   OSCICN = 0x83;                      // Configure internal oscillator for
                                       // its highest frequency (12 MHz)

   RSTSRC = 0x04;                      // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// ExtCMOSOsc_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine switches to the External CMOS Oscillator.
//
// Note: In CMOS clock configuration, the clock source should be wired to the
// XTAL2 pin as shown in the Oscillators section of  the datasheet.
//
void ExtCMOSOsc_Init (void)
{
   CLKSEL = 0x01;                      // Switch to the external CMOS clock
}

//-----------------------------------------------------------------------------
// Port_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P0.0   digital   push-pull     /SYSCLK
// P0.1                           unused
// P0.2                           unused
// P0.3   digital   open-drain    XTAL2 - External CMOS Oscillator input
// P0.4                           unused
// P0.5                           unused
// P0.6                           unused
// P0.7                           unused
//
// P2.0                           unused
// P2.1                           unused
// P2.2   digital   push-pull     LED
// P2.3                           unused
// P2.4                           unused
// P2.5                           unused
//
// P3.0                           unused
//
void PORT_Init (void)
{
   P0MDOUT |= 0x01;                    // Enable /SYSCLK as a push-pull output

   P2MDOUT |= 0x04;                    // Enable LED as a push-pull output

   GPIOCN |= 0x01;                     // Route /SYSCLK to P0.0
   GPIOCN |= 0x80;                     // Enable weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer1_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer1 to 16-bit mode and generate an interrupt after the
// maximum possible time.
//
void Timer1_Init (void)
{
   TCON = 0x00;                        // Stop Timer1; Clear TF1

   TMOD = 0x10;                        // Timer1 in 16-bit mode

   CKCON = 0x00;                       // Timer1 uses SYSCLK/12

   TH1 = 0x00;                         // Interrupt after the maximum time
   TL1 = 0x00;

   ET1 = 1;                            // Enable Timer1 interrupts

   TR1 = 1;                            // Start Timer1
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer1_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer1 overflows.
//
void Timer1_ISR (void) interrupt 3
{
   static unsigned int count = 0;

   TF1 = 0;                            // Clear Timer1 interrupt flag

   TH1 = 0x00;                         // Reload Timer1 to interrupt after the
   TL1 = 0x00;                         // maximum time

   if (count == LED_interrupt_count)
   {
      LED = ~LED;                      // Change state of LED

      count = 0;
   }
   else
   {
      count++;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------