//-----------------------------------------------------------------------------
// F36x_Oscillator_Suspend.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The program demonstrates the power saving SUSPEND mode operation on the
// F360. The MCU will sleep in low power suspend mode and wait until a switch
// is pressed. When the MCU wakes up it will switch to a 24.5 MHz internal
// oscillator & turn on an LED. After 1 second, it will go back into SUSPEND
// until the switch is pressed again.
//
// How To Test:
//
// 1) Download code to a 'F360 target board.
// 2) Make sure J12 has a jumper connecting the LED to P3.2.
// 3) Make sure J12 has a jumper connecting SW1 to P3.0.
// 4) Connect P3.0 to P2.0.
// 5) Run the code.
// 6) Verify the LED turns on when the switch is pressed.
//
// FID:            36X000020
// Target:         C8051F36x
// Tool chain:     Keil C51 7.05 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (KAB / TP)
//    -13 OCT 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F360.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK    24500000             // Sleep 1 second

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

sbit LED = P3^2;                       // Port pin used for LED

//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD  &= ~0x40;                   // Disable watchdog timer

   Oscillator_Init ();                 // Initialize 24.5 MHz oscillator
   Port_Init ();                       // Initialize IO ports

   EA = 1;                             // Set global interrupt enable

   SFRPAGE = LEGACY_PAGE;

   while (1)
   {
      configSleep ();                  // Configure MCU for low power sleep

      SFRPAGE = CONFIG_PAGE;

      OSCICN |= 0x20;                  // Set suspend mode bit

      // Wait for a switch press

      SFRPAGE = LEGACY_PAGE;

      configWake ();                   // Configure MCU for wake mode

      SFRPAGE = LEGACY_PAGE;

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
// P2.0   open-drain   input    port-match input
//
// P3.0   open-drain   input    SW1
//
// P3.2   push-pull    output   LED
//
//-----------------------------------------------------------------------------
void Port_Init ()
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   P2MDOUT &= ~0x01;                   // Enable P2.0 as open-drain

   P3MDOUT = 0x04;                     // Enable P3.0 as open-drain
                                       // Enable P3.2 as push-pull

   XBR1 |= 0x40;                       // Enable crossbar

   SFRPAGE = LEGACY_PAGE;

   P2MAT = 0x01;                       // Set up the port-match value on P2
   P2MASK = 0x01;                      // An event will occur if
                                       // (P2 & P2MASK) != (P2MAT & P2MASK)
                                       // The SW1 pin = 1 if the switch isn't
                                       // pressed

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Oscillator_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the internal oscillator to 24.5 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init ()
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN = 0x83;                      // Enable 24.5 MHz int osc

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   XBR1 &= ~0xC0;                      // Disable crossbar, enabled weak pull
                                       // ups

   // Disable all peripherals for low power sleep

   SCON0 &= ~0x10;                     // Disable UART0
   TCON &= ~0x10;                      // Disable T0
   TCON &= ~0x40;                      // Disable T1
   TMR2CN &= ~0x04;                    // Disable T2

   // Disable any additional peripherals here

   VDM0CN &= ~0x80;                    // Disable voltage monitor

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   VDM0CN |= 0x80;                     // Enable voltage monitor
   TCON |= 0x10;                       // Enable T0

   // enable any additional peripherals here

   XBR1 |= 0x40;                       // Enable crossbar

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
