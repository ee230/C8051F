//-----------------------------------------------------------------------------
// F53xA_Oscillator_Suspend.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The program demonstrates the power saving SUSPEND mode operation on the
// F52xA/F53xA. The MCU will sleep in low power suspend mode and wait until a
// switch is pressed. When the MCU wakes up it will switch to a 24.5 MHz
// internal oscillator & turn on an LED. After 1 second, it will go back into
// SUSPEND until the switch is pressed again.
//
// How To Test:
//
// 1) Download code to a 'F530A target board (either device A or device B).
// 2) Ensure the switch and LED jumpers are populated (HDR4 for device A and
//    HDR3 for device B).
// 3) Run the code.
// 4) Verify the LED turns on when the switch is pressed.
//
// Target:         C8051F53x
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.2 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.1
//    -Updated for 'F53xA TB (TP)
//    -30 JAN 2008
//
// Release 1.0
//    -Initial Revision (SM)
//    -15 JUN 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051F520A_defs.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK    24500000             // SYSCLK frequency in Hz

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);

void configSleep (void);
void configWake (void);
void delay (unsigned int ms);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

SBIT (LED, SFR_P1, 3);                 // Port pin used for LED

//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD  &= ~0x40;                   // Disable watchdog timer

   Oscillator_Init ();                 // Initialize 24.5 MHz oscillator
   Port_Init ();                       // Initialize IO ports

   EA = 1;                             // Set global interrupt enable

   while (1)
   {
      configSleep ();                  // Configure MCU for low power sleep

      OSCICN |= 0x20;                  // Set suspend mode bit

      // Wait for a switch press

      configWake ();                   // Configure MCU for wake mode

      LED = 1;                         // Turn on LED

      delay (1000);                    // Wait 1000 ms (1 second)

      LED = 0;                         // Turn off LED
   }
}

//-----------------------------------------------------------------------------
// Port_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P1.3   push-pull    output   LED
//
// P1.4   open-drain   input    P1.4_SW
//
//-----------------------------------------------------------------------------
void Port_Init ()
{
   P1MDOUT = 0x08;                     // Enable P1.3 as push-pull

   XBR1 |= 0x40;                       // Enable crossbar

   P1MAT = 0x10;                       // Set up the port-match value on P1
   P1MASK = 0x10;                      // An event will occur if
                                       // (P1 & P1MASK) != (P1MAT & P1MASK)
                                       // The SW1 pin = 1 if the switch isn't
                                       // pressed
}

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 24.5 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x87;                      // Set the internal oscillator to
                                       // 24.5 MHz
}

//-----------------------------------------------------------------------------
// configSleep ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the MCU for low power sleep by disabling the
// crossbar, enabling weak pull-ups, and turning off MCU peripherals. The
// lowest possible sleep current can be obtained by turning off the Voltage
// monitor.
//
//-----------------------------------------------------------------------------
void configSleep (void)
{
   XBR1 &= ~0xC0;                      // Disable crossbar, enabled weak pull
                                       // ups

   // Disable all peripherals for low power sleep

   SCON0 &= ~0x10;                     // Disable UART0
   TCON &= ~0x10;                      // Disable T0
   TCON &= ~0x40;                      // Disable T1
   TMR2CN &= ~0x04;                    // Disable T2
}

//-----------------------------------------------------------------------------
// configWake ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will configure the MCU for wake mode operation.
// Enable additional peripherals as needed.
//
//-----------------------------------------------------------------------------
void configWake (void)
{
   TCON |= 0x10;                       // Enable T0

   // Enable any additional peripherals here

   XBR1 |= 0x40;                       // Enable crossbar
}

//-----------------------------------------------------------------------------
// delay ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1)  unsigned int ms - number of milliseconds to delay
//                    range is entire range of unsigned integer: 0 to 65535
//
// This function will implement a delay using T0.
// A value of 1000 for <ms> is equal to one second.
//
//-----------------------------------------------------------------------------
void delay (unsigned int ms)
{
   TCON &= ~0x30;                      // Stop T0 & clear flag
   TMOD &= ~0x0F;                      // Clear T0 nibble of TMOD
   TMOD |= 0x01;                       // T0 16 bit counter mode
   CKCON |= 0x04;                      // T0 uses SYSCLK

   // set T0 using time
   TL0 = (unsigned char) (-(SYSCLK / 1000) & 0x00FF);
   TH0 = (unsigned char) ((-(SYSCLK / 1000) & 0xFF00) >> 8);

   TCON |= 0x10;                       // Enable T0

   while (ms > 0)
   {
      while (!TF0);                    // Wait for TOF

      TF0 = 0;                         // Clear the timer flag

      // Reload the Timer0 registers
      TL0 = (unsigned char) (-(SYSCLK / 1000) & 0x00FF);
      TH0 = (unsigned char) ((-(SYSCLK / 1000) & 0xFF00) >> 8);

      ms--;
   }
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
