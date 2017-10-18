//-----------------------------------------------------------------------------
// F36x_Ports_SwitchesLEDs.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the port pins as digital inputs
// and outputs.  The C8051F360 target board has two push-button switches
// connected to port pins and and two LEDs.  The program constantly checks the 
// status of the switches and if they are pushed, it turns on the 
// respective LED.
//
//
// How To Test:
//
// 1) Download code to a 'F360 target board
// 2) Ensure that the Switch and LED pins on the J12 header are 
//    properly shorted
// 3) Push the buttons (P3.0 and P3.1) and see that the LEDs turn on 
//
// Target:         C8051F36x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (GP)
//    -29 AUG 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F360.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK    24500000             // System clock in Hz

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

sbit SW1 = P3^0;                       // Push-button switch on board
sbit SW2 = P3^1;                       // Push-button switch on board
sbit LED1 = P3^2;                      // Green LED
sbit LED2 = P3^3;                      // Green LED

//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD  &= ~0x40;                   // Disable watchdog timer

   PORT_Init();                        // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator
  
   while (1)
   {
      if (SW1 == 0)                    // If switch depressed
      { 
         LED1 = 1;                     // Turn on LED
      }
	  else   
      {  
         LED1 = 0;                     // Else, turn it off
      }                   
	  
      if (SW2 == 0)                    // If switch depressed
      { 
         LED2 = 1;                     // Turn on LED
      }
	  else   
      {  
         LED2 = 0;                     // Else, turn it off
      }                   
   }                                   // end of while(1)
}                                      // end of main()

//-----------------------------------------------------------------------------
// PORT_Init ()
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
// push-pull output.  For the purposes of this example, the pin is configured
// as push-pull output because the pin in only connected to an LED.
//
// P3.0 - digital	open-drain 	SW1 (Switch 1)
// P3.1 - digital	open-drain 	SW2 (Switch 2)
// P3.2 - digital	push-pull 	LED1
// P3.3 - digital	push-pull	LED2
//
//-----------------------------------------------------------------------------
void PORT_Init ()
{  
   unsigned char SFRPAGE_save = SFRPAGE;      // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   P3MDOUT |= 0x0C;                    // Configure P3.2,P3.3 to push-pull

   XBR1 |= 0x40;                       // Enable crossbar

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 24.5 MHz.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE;    // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN |= 0x03;                     // Initialize internal oscillator to
                                       // highest frequency

   RSTSRC  = 0x04;                     // Enable missing clock detector
   
   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
