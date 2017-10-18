//-----------------------------------------------------------------------------
// T620_Oscillator_Suspend.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The program demonstrates the power saving SUSPEND mode operation on the
// T620. The MCU will sleep in low power suspend mode and wait until a switch
// is pressed. When the MCU wakes up it will switch to a 12 MHz internal
// oscillator & turn on an LED. After 1 second, it will go back into SUSPEND
// until the switch is pressed again.
//
// How To Test:
//
// 1) Download code to a 'T620 device on a 'T62x mother board.
// 2) Make sure J10 has a jumper connecting the LED to P2.2.
// 3) Make sure J9 has a jumper connecting SW1 to P2.0.
// 4) Connect P2.0 on J4 to P1.0 on J3.
// 5) Run the code.
// 6) Verify the LED turns on when the switch is pressed.
//
// Target:         C8051T620/1 or 'T320/1/2/3
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -27 JUN 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK    12000000             // System Clock in Hz

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

SBIT (LED, SFR_P2, 2);                 // Port pin used for LED

//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD  &= ~0x40;                   // Disable watchdog timer

   Oscillator_Init ();                 // Initialize 12 MHz oscillator
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
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Oscillator_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the internal oscillator to 12 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init ()
{
   OSCICN = 0x83;                      // Enable 12 MHz int osc
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
// P1.0   open-drain   input    port-match input
//
// P2.0   open-drain   input    SW1
//
// P2.2   push-pull    output   LED
//
//-----------------------------------------------------------------------------
void Port_Init ()
{
   P1MDOUT &= ~0x01;                   // Eanble P1.0 as open-drain
   P2MDOUT &= ~0x01;                   // Enable P2.0 as open-drain
   P2MDOUT |= 0x04;                    // Enable P2.2 as push-pull

   XBR1 |= 0x40;                       // Enable crossbar

   P1MAT = 0x01;                       // Set up the port-match value on P1
   P1MASK = 0x01;                      // An event will occur if
                                       // (P1 & P1MASK) != (P1MAT & P1MASK)
                                       // The SW1 pin = 1 if the switch isn't
                                       // pressed
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

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

   // Disable any additional peripherals here

   VDM0CN &= ~0x80;                    // Disable voltage monitor
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
   VDM0CN |= 0x80;                     // Enable voltage monitor
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
// This function will implement a delay using T0. A value of 1000 for <ms> is
// equal to one second.
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
