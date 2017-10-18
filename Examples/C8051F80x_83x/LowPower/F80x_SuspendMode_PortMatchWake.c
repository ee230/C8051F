//-----------------------------------------------------------------------------
// F80x_SuspendMode_PortMatchWake.c
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
// 1) Download code to the 'F80x target board
// 2) Ensure that J3 connects P1.0_LED to P1.0, and P1.4_SW to P1.4
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
#include <C8051F800_defs.h>            // SFR declarations


//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (GREEN_LED,    SFR_P1, 0);        // '0' means ON, '1' means OFF
SBIT (SW,           SFR_P1, 4);        // SW == 0 means switch pressed

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK          24500000       // SYSCLK frequency in Hz


#define LED_ON           0
#define LED_OFF          1

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
volatile U16 delay;

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
   PCA0MD &= ~0x40;                    // WDTE = 0 (disable watchdog timer)

   PORT_Init ();                       // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator

   // Setup the Port Match Wake-up Source to wake up on the falling
   // edge of P1.1 (when the switch is pressed)
   P1MASK = 0x10;                      // Mask out all pins except for P1.1
   P1MAT = 0x10;                       // P1.1 should be HIGH while the device
                                       // is in the low power mode
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

         OSCICN |= 0x20;               // Place MCU in suspend mode

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
   OSCICN |= 0x83;                     // Enable the precision internal osc.

   CLKSEL = 0x00;                      // Select internal internal osc.
                                       // divided by 1 as the system clock
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
// P1.4   digital   open-drain    Switch
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P1MDIN |= 0x11;                     // P1.0, P1.1 are digital

   P1MDOUT &= ~0x10;                   // P1.1 is open-drain
   P1MDOUT |= 0x01;                    // P1.0 is push-pull

   P1     |= 010;                     // Set P1.1 latch to '1'

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
