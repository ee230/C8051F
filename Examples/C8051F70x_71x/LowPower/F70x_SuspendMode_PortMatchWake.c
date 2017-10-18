//-----------------------------------------------------------------------------
// F70x_SuspendMode_PortMatchWake.c
//-----------------------------------------------------------------------------
// Copyright (C) 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program turns on or off the LED before going into suspend mode.  The
// devie wakes from suspend when a port match event occurs on the switch.
//
// How To Test:
//
// 1) Download code to the 'F70x target board
// 2) Ensure that pins 1-2, 3-4, 5-6, 7-8 are shorted together on the
//    J8 header
// 3) Run the program. Press the switch to place the device in suspend mode.
//    Press the switch again to wake it back up. 
//
//
// Target:         C8051F70x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (PD)
//    -10 JUL 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F700_defs.h>            // SFR declarations


//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (GREEN_LED,    SFR_P1, 0);        // '0' means ON, '1' means OFF
SBIT (SW,           SFR_P1, 1);        // SW == 0 means switch pressed

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK          24500000       // SYSCLK frequency in Hz


#define LED_ON           0
#define LED_OFF          1

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void);
void PORT_Init (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   U8 delay;

   WDTCN = 0xDE;                       // Disable watchdog timer
   WDTCN = 0xAD;

   PORT_Init ();                       // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator

   // Setup the Port Match Wake-up Source to wake up on the falling
   // edge of P1.1 (when the switch is pressed)
   SFRPAGE = LEGACY_PAGE;
   P1MASK = 0x02;                      // Mask out all pins except for P1.1
   P1MAT = 0x02;                       // P1.1 should be HIGH while the device
                                       // is in the low power mode
   SFRPAGE = LEGACY_PAGE;

   EA = 1;                             // Enable global interrupts

   //----------------------------------
   // Main Application Loop
   //----------------------------------
   while (1)
   {
      if(!SW)                          // If the P1.1 switch is pressed
      {
         // Simple de-bounce delay
         for (delay = 0; delay < 250; delay++);
         while (!SW);                  // Wait for switch to be released
         for (delay = 0; delay < 250; delay++);

         // Configure the Port I/O for Low Power Mode
         GREEN_LED = LED_OFF;          // Turn off the LED or other
                                       // high-current devices

         SFRPAGE = CONFIG_PAGE;
         OSCICN |= 0x20;               // Place MCU in suspend mode
         SFRPAGE = LEGACY_PAGE;

         // At this point the device is in suspend mode
         // A port match event on P1.1

         // Simple de-bounce delay
         for (delay = 0; delay < 250; delay++);
         while (!SW);                  // Wait for switch to be released
         for (delay = 0; delay < 250; delay++);

         GREEN_LED = LED_ON;

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
// This function initializes the system clock to use the internal high power
// oscillator.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   // Save the current SFRPAGE
   U8 SFRPAGE_save = SFRPAGE;

   SFRPAGE = CONFIG_PAGE;

   OSCICN |= 0x80;                     // Enable the precision internal osc.

   CLKSEL = 0x00;                      // Select internal internal osc.
                                       // divided by 1 as the system clock

   SFRPAGE = SFRPAGE_save;

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
// P1.0   digital   push-pull     LED
// P1.1   digital   open-drain    Switch
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   // Save the current SFRPAGE
   U8 SFRPAGE_save = SFRPAGE;

   SFRPAGE = CONFIG_PAGE;

   P1MDIN |= 0x03;                     // P1.0, P1.1 are digital

   P1MDOUT &= ~0x02;                   // P1.1 is open-drain
   P1MDOUT |= 0x01;                    // P1.0 is push-pull

   P1     |= 0x02;                     // Set P1.1 latch to '1'

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
