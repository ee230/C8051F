//-----------------------------------------------------------------------------
// F04x_Ports_SwitchLED.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure port pins as digital inputs
// and outputs.  The C8051F040 target board has one push-button switch 
// connected to a port pin and one LED.  The program constantly checks the 
// status of the switch and if it is pushed, it turns on the LED.
//
// The program also monitors P4.0.  If P4.0 is high, it sets P4.1 low.  If
// P4.0 is low, P4.1 is set high.  The purpose of this part of the program
// is to show how to access the higher ports.  Ports 4-7 are not available
// on all SFR pages, so the SFRPAGE register must be set correctly before 
// reading or writing these ports.
//
//
// How To Test:
//
// 1) Download code to a 'F040 target board
// 2) Ensure that the J1 and J3 headers are shorted
// 3) Push the button (P3.7) and see that the LED turns on 
// 4) Set P4.0 high and low using an external connection and check that
//    the output on P4.1 is the inverse of P4.0.
//
//
// FID:            04X000002
// Target:         C8051F04x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (GP)
//    -15 NOV 2005
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f040.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

sbit LED1    = P1^6;                   // LED1 ='1' means ON
sbit SW1     = P3^7;                   // SW1 ='0' means switch pressed
sbit INPUT1  = P4^0;                   // port pin 4.0
sbit OUTPUT1 = P4^1;                   // port pin 4.1

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
   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

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

      SFRPAGE = CONFIG_PAGE;           // set SFR page before reading or writing
                                       // to P4 registers
	  
      if (INPUT1 == 0)                 // If input is low
      { 
         OUTPUT1 = 1;                  // Make OUTPUT1 inverse of INPUT1
      }
	  else   
      {  
         OUTPUT1 = 0;                  // Make OUTPUT1 inverse of INPUT1
      }                   	      
   }                                   // end of while(1)
}                                      // end of main()

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
// at its maximum frequency.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // set SFR page

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency (24.5 Mhz)

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
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
// Some ports pins do not have the option to be configured as analog or digital,
// so it not necessary to explicitly configure them as digital.
//
// An output pin can also be configured to be an open-drain output if system
// requires it.  For example, if the pin is an output on a multi-device bus,
// it will probably be configured as an open-drain output instead of a 
// push-pull output.  For the purposes of this example, the pin is configured
// as push-pull output because the pin in only connected to an LED.
//
// P1.6   digital   push-pull     LED1
// P3.7   digital   open-drain    Switch 1
// P4.0   digital   open-drain    Input 1
// P4.1   digital   push-pull     Output 1
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // set SFR page before writing to
                                       // registers on this page

   P1MDIN |= 0x40;                     // P1.6 is digital

   P1MDOUT = 0x40;                     // P1.6 is push-pull
   P3MDOUT = 0x00;                     // P3.7 is open-drain

   P3     |= 0x80;                     // Set P3.7 latch to '1'

   P4MDOUT = 0x02;                     // P4.0 is open-drain; P4.1 is push-pull
   P4      = 0x01;                     // Set P4.1 latch to '1'


   XBR2    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------