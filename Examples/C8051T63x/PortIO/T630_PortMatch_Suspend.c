//-----------------------------------------------------------------------------
// T630_PortMatch_Suspend.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program presents an example of use of the port match feature of the 
// C8051T630. It uses the 'T630MB+DB as the HW platform.
//
// If the P0.7 pin is grounded, it will cause a Port Match event, and will
// wake up the MCU from suspend. On wake up, the MCU will toggle the LED
// and go back into suspend.
//
// Pinout:
//
//    P1.3 -> LED
//    P0.7 -> Switch SW1
//
//    all other port pins unused
//
// How To Test:
//
// 1) Open the T630_PortMatch_Suspend.c file in the Silicon Labs IDE.
// 2) Compile and download the code.
// 3) Verify the LED pins of J9 are populated on the 'T630 MB.
// 4) Run the code.
// 5) Press the switch labeled P0.7. This should toggle the LED state.
// 6) Release the switch to let the MCU go back into suspend.
//
//
// Target:         C8051T630
// Tool chain:     KEIL C51 7.20 / KEIL EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (PKC)
//    -19 NOV 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051T630_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

SBIT (LED, SFR_P1, 3);                 // LED==1 means ON
SBIT (SW1, SFR_P0, 7);                 // SW1==0 means pressed

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Port_Init (void);                 // Port initialization routine

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   unsigned int i;

   PCA0MD &= ~0x40;                    // Clear watchdog timer enable

   Port_Init ();                       // Init Ports

   while (1)                           // Loop forever
   {
      OSCICN |= 0x20;                  // Suspend Internal H-F Oscillator.
                                       // Port Match event will wake it up.
                                       
      // 1) If the MCU did not stay in suspend till the Port Match event, the
      //    LED will blink without having to press the switch.
      
      // 2) If the MCU stayed in suspend indefinitely, the LED will stay solid
      //    even if the button was pressed.
      
      // 3) If the MCU left suspend mode correctly on the Port Match event, then
      //    the LED will toggle state every time the switch is pressed and released.
      
      // Press switch P0.7 to toggle LED.
      LED = ~LED;                      // Toggle the LED

      for (i = 1; i != 0; i++);        // Wait for some period of time
      
      while (SW1 == 0);                // Wait till switch is released
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P0.7  digital  open-drain Switch (SW1)
// P1.3  digital  push-pull  LED
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDOUT &= ~0x80;                   // Set SW1 to open-drain
   P1MDOUT = 0x08;                     // Set LED to push-pull
  
   P0MASK = 0x80;                      // Port Match event when 
                                       // P0.7 NOT equal to '1'

   XBR1 = 0x40;                        // Enable crossbar
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------