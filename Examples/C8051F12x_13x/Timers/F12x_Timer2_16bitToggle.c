//-----------------------------------------------------------------------------
// F12x_Timer2_16bitToggle.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program presents an example of use of the Timer2 of the C8051F12x's in
// 16-bit output toggle mode.
//
// It uses the 'F120DK as HW platform. This example code counts in timer2 until
// it overflows. At this moment the T2 output is toggled.
//
//
// Pinout:
//
//    P0.0 -> T2 toggle output
//
// How To Test:
//
// 1) Load the F12x_Timer2_16bitToggle.c
// 2) To change the toggling rate modify TOGGLE_RATE value (0 to 255 -> msec)
// 3) Compile and download the code
// 4) Run
// 5) Check the P0.0 pin (T2)
//
// FID:            12X000001
// Target:         C8051F12x
// Tool chain:     KEIL C51 7.20 / KEIL EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (CG)
//    -09 NOV 2005
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f120.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define TOGGLE_RATE              200L  // TOGGLE Rate of P0.0 in msec (255 max)


// The timer is operating from a 24.5MHz (SYSCLK) with a prescaler of 1:12,
// therefore the frequency is of 255KHz
#define AUX0                    - (TOGGLE_RATE)*256
#define AUX1                    AUX0&0x00FF
#define AUX2                    ((AUX0&0xFF00)>>8)

#define TIMER2_RELOAD_HIGH       AUX2 // Reload value for timer2 high byte
#define TIMER2_RELOAD_LOW        AUX1 // Reload value for timer2 low byte

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void PORT_Init (void);                 // Port initialization routine
void TIMER2_Init (void);               // Timer0 initialization routine

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;
   TIMER2_Init ();                     // Initialize the timer2
   PORT_Init ();                       // Init Ports
   EA = 1;                             // Enable global interrupts
   while (1);                          // Loop
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // set SFR page

   XBR1 = 0x20;                        // T2 available in the pins

   XBR2 = 0x40;                        // Enable crossbar
   P0MDOUT = 0x01;                     // Enable P0.0 as output
   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// TIMER2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the timer2 as 16-bit auto-reload.
// It Uses the internal osc. at 24.5MHz/8 with a 1:12 prescaler.
// At each overflow the P0.0 is toggled.
//-----------------------------------------------------------------------------
void TIMER2_Init(void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TMR2_PAGE;                // Set SFR page
   TMR2CF = 0x02;                      // output enabled, SYSCLK/12
   RCAP2L = TIMER2_RELOAD_LOW;         // Reload value to be used in Timer2
   RCAP2H = TIMER2_RELOAD_HIGH;
   TMR2CN = 0x04;                      // Enable timer2 in reload mode
   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------