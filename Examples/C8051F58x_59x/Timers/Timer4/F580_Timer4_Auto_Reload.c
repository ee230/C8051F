//-----------------------------------------------------------------------------
// F580_Timer4_Auto_Reload.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program uses Timer 4 Auto-Reload mode to toggle an LED on Timer 4
// overflow events.
//
// In this example, Timer 4 will either count up or down, depending on the
// state of the T4EX (P1.4). If T4EX is logic high, Timer 4 will count up
// and trigger an overflow interrupt whenever the counter transitions from
// 0xFFFF to 0x0000. Timer 4 will then be reloaded with the value in TMR4CAP.
// If T4EX is logic low, Timer 4 will count down and trigger an
// underflow interrupt whenever the counter matches the value in TMR4CAP.
// Timer 4 will then be reloaded with 0xFFFF.
//
//
// How To Test:
//
// 1) Download code to a 'F580 device with a jumper on J19.
// 2) Run the program and take note of the LED frequency
// 3) Press and hold the switch on P1.4. The rate at which the LED blinks
//    should be twice as slow. You can also step through the program (while
//    holding down the switch) with the timer watch window open to see TMR4
//    decrementing.
//
// Target:         C8051F580 / C8051F580
// Tool chain:     Keil C51 8.00 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0 / 29 JUL 2008 (ADT)
//    -Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <c8051f580_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK           24000000      // Internal oscillator frequency in Hz

// Make the interrupt rate when counting down: 0xFFFF - 0xFFFF/3
#define T4_RELOAD_COUNT_DOWN   0xFFFF / 3

// Make the interrupt rate when counting up twice as fast
#define T4_RELOAD_COUNT_UP  T4_RELOAD_COUNT_DOWN * 2

// Whenever the counter variable reaches this value
// toggle the LED
// LED_TOGGLE_RATE = # of seconds between each toggle / T4 interrupt rate
//                 = .25 / (T4_RELOAD_COUNT_UP / (SYSCLK / 12)) = 23
#define LED_TOGGLE_RATE       23

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void Timer4_Init (void);

SBIT (SW, SFR_P1, 4);                  // 1 = not pressed, 0 = pressed
SBIT (LED, SFR_P1, 3);                 // 1 = on, 0 = off

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   SFRPAGE = ACTIVE_PAGE;              // Set SFR Page for PCA0MD

   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   PORT_Init ();                       // Initialize crossbar and GPIO
   OSCILLATOR_Init ();                 // Initialize oscillator
   Timer4_Init ();                     // Initialize Timer 4

   EA = 1;

   SFRPAGE = ACTIVE2_PAGE;             // Set for Timer 4 registers

   while (1)
   {
      if (SW)
      {
         // Load value to count from to 0xFFFF
         TMR4CAPH = (U8)((T4_RELOAD_COUNT_UP >> 8) & 0x00FF);
         TMR4CAPL = (U8)(T4_RELOAD_COUNT_UP & 0xFF);
      }
      else
      {
         // Load value to be a trigger when counting down from 0xFFFF
         TMR4CAPH = (U8)((T4_RELOAD_COUNT_DOWN >> 8) & 0x00FF);
         TMR4CAPL = (U8)(T4_RELOAD_COUNT_DOWN & 0xFF);
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
// This function initializes the system clock to use the internal oscillator
// at 24.5 MHz
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   char SFRPAGE_save = SFRPAGE;        // Save Current SFR page
   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x87;                      // Set internal oscillator to run
                                       // at its maximum frequency

   CLKSEL = 0x00;                      // Select the internal osc. as
                                       // the SYSCLK source

   SFRPAGE = SFRPAGE_save;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P1.3   digital   push-pull       LED
// P1.4   digital   open-drain      SW (T4EX)
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;          // Save Current SFR page
   SFRPAGE = CONFIG_PAGE;              // Set SFR Page

   P1MDOUT = 0x08;                     // P1.3 is push-pull
                                       // P1.4 is open-drain

   P0SKIP = 0xFF;                      // Skip to P1.4
   P1SKIP = 0x0F;
   XBR3    = 0x20;                     // T4EX routed to P1.4
   XBR2    = 0x40;                     // Enable crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;             // Restore SFR Page
}

//-----------------------------------------------------------------------------
// Timer4_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Initialize Timer4 for Auto-Reload mode. DCEN is enabled, so T4EX will
// determine which direction TMR4 will count.
//-----------------------------------------------------------------------------
void Timer4_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;          // Save Current SFR page
   SFRPAGE = ACTIVE2_PAGE;             // Set SFR Page

   TMR4CN = 0x08;                      // T4EX enabled
                                       // Timer4 is in Auto-Reload mode

   TMR4CF = 0x01;                      // Timer4 uses SYSCLK / 12
                                       // DCEN = 1, T4EX controls direction
                                       // T4EX = 1, increment
                                       // T4EX = 0, decrement

   // Set initial reload rate
   TMR4CAPH = (U8)((T4_RELOAD_COUNT_UP >> 8) & 0x00FF);
   TMR4CAPL = (U8)(T4_RELOAD_COUNT_UP & 0xFF);

   TMR4H = TMR4CAPH;                   // Initialize Timer
   TMR4L = TMR4CAPL;

   EIE2 |= 0x40;                       // Enable Timer 4 interrupts

   TMR4CN |= 0x04;                     // Timer 4 run enabled

   SFRPAGE = SFRPAGE_save;             // Restore SFR Page
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TIMER4_ISR
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This is the ISR for Timer4.  It toggles an LED at the specified
// LED_TOGGLE_RATE
//
//-----------------------------------------------------------------------------
INTERRUPT (TIMER4_ISR, INTERRUPT_TIMER4)
{
   static U16 i = 0;

   if ((TMR4CN & 0x80) != 0)           // Overflow caused interrupt
   {
      i++;                             // Increment counter

      if (i == LED_TOGGLE_RATE)
      {
         i = 0;                        // Reset counter
         LED = !LED;                   // Toggle LED
      }
   }

   TMR4CN &= ~0xC0;                    // Clear Timer 4 overflow/underflow flag
                                       // Clear T4EX flag
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------