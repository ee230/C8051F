//-----------------------------------------------------------------------------
// F96x_Ports_SwitchLED.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure port pins as digital inputs
// and outputs.  The C8051F96x target board has two push-button switches
// and two LEDs connected to port pins.  The program constantly checks the
// status of the switches and if one or both are closed, it turns off
// one or both LEDs, respectively.
//
//
// How To Test:
//
// 1) Set Board as 0 or 1 in Pin Declarations 
// 2) Download code to a 'F960 target board
// 3) Ensure shorting block are installed on the first 8 pins of the
//    J8 header.
// 4) Push the button (P0.2 or P0.3) and see that the corresponding
//    LED (Red or Yellow) turns off.
//
//
// Target:         C8051F96x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    - Initial Revision (FB)
//    - 19 MAY 2010
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F960_defs.h>            // SFR declarations

#define UDP_F960_MCU_MUX_LCD
//-----------------------------------------------------------------------------
// Hardware Dependent definitions
//-----------------------------------------------------------------------------
#ifdef UDP_F960_MCU_MUX_LCD
SBIT (SW1, SFR_P0, 0);
SBIT (SW2, SFR_P0, 1);
SBIT (LED3,  SFR_P0, 2);
SBIT (LED4,  SFR_P0, 3);
#endif
//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             20000000    // Clock speed in Hz

#define LED_ON  0
#define LED_OFF 1

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
LOCATED_VARIABLE_NO_INIT (reserved, U8, SEG_XDATA, 0x0000);

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)


   PORT_Init();                        // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator

   while (1)
   {


      // Set the state of the LED3
      if (SW1 == 0)                    // If switch pressed
      {
         LED3 = LED_ON;                // Turn on LED3
      }
      else
      {
         LED3 = LED_OFF;               // Else, turn it off
      }

      // Set the state of the LED4
      if (SW2 == 0)                    // If switch pressed
      {
         LED4 = LED_ON;                // Turn on LED4
      }
      else
      {
         LED4 = LED_OFF;               // Else, turn it off
      }
   }                                  // end of while(1)
}                                     // end of main()

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
// This function initializes the system clock to use the internal low power
// oscillator at its maximum frequency.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   SFRPAGE = LEGACY_PAGE;
   CLKSEL = 0x04;                     // Select low power oscillator with a
                                      // clock divider value of 1 (20MHz)
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
// To configure a pin as a digital input, the pin is configured as digital
// and open-drain and the port latch should be set to a '1'.  The weak-pullups
// are used to pull the pins high.  Pressing the switch pulls the pins low.
//
// To configure a pin as a digital output, the pin is configured as digital
// and push-pull.
//
// An output pin can also be configured to be an open-drain output if system
// requires it.  For example, if the pin is an output on a multi-device bus,
// it will probably be configured as an open-drain output instead of a
// push-pull output.  For the purposes of this example, the output pins are
// configured as push-pull outputs because the pins are only connected to LEDs.
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   SFRPAGE = LEGACY_PAGE;

   XBR2    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
