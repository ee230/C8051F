//-----------------------------------------------------------------------------
// F53xA_Timer0_16bitTimer.c
//-----------------------------------------------------------------------------
//
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program presents an example of use of the Timer0 of the C8051F53xA's
// in 16-bit counter/timer mode. It uses the 'F530A TB as the HW platform and
// blinks a LED. To do so it used an interrupt driven by the Timer0 which
// toggles the LED every 100msec.
//
// Pinout:
//
//    P1.3 -> LED
//
//    all other port pins unused
//
// How To Test:
//
// This code enables the Timer0 as a 16-bit timer that uses the system clock
// (SYSCLK, in this case the internal clock @24.5MHz/8 or about 3MHz) with a
// 1:48 prescaler.
// The interrupt routine then toggles the LED. To execute the test please
// follow these steps:
//
// 1) Open the F53xA_Timer0_16bitTimer.c file in the Silicon Labs IDE.
// 2) If a different LED blink rate is required change the following constant:
//       -> LED_TOGGLE_RATE
// 3) Download code to a 'F53xA device (either device A or B on the TB).
// 4) Verify the LED and switch jumpers are populated (HDR4 for device A and
//    HDR3 for device B).
// 5) Run the code.
// 6) Check that the LED is blinking.
//
//
// Target:         C8051F52xA/F53xA (C8051F530A TB)
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.2 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.1
//    -Updated for 'F530A TB (TP)
//    -31 JAN 2008
//
// Release 1.0
//    -Initial Revision (SM)
//    -JUN 11 2007
//


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051F520A_defs.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK            24500000/8   // Internal oscillator frequency in Hz

#define TIMER_PRESCALER           48   // Based on Timer CKCON settings
#define LED_TOGGLE_RATE           50   // LED toggle rate in milliseconds
                                       // if LED_TOGGLE_RATE = 1, the LED will
                                       // be on for 1 millisecond and off for
                                       // 1 millisecond

// There are SYSCLK/TIMER_PRESCALER timer ticks per second, so
// SYSCLK/TIMER_PRESCALER/1000 timer ticks per millisecond.
#define TIMER_TICKS_PER_MS  SYSCLK/TIMER_PRESCALER/1000

// Note: LED_TOGGLE_RATE*TIMER_TICKS_PER_MS should not exceed 65535 (0xFFFF)
// for the 16-bit timer

#define AUX1     TIMER_TICKS_PER_MS*LED_TOGGLE_RATE
#define AUX2     -AUX1
#define AUX3     AUX2&0x00FF
#define AUX4     ((AUX2&0xFF00)>>8)

#define TIMER0_RELOAD_HIGH       AUX4  // Reload value for Timer0 high byte
#define TIMER0_RELOAD_LOW        AUX3  // Reload value for Timer0 low byte

SBIT (LED, SFR_P1, 3);                 // LED

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void Timer0_Init(void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init ();
   Timer0_Init ();                     // Initialize the Timer0
   Port_Init ();                       // Init Ports
   EA = 1;                             // Enable global interrupts

   while (1);                          // Loop forever

}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 24.5/8 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x84;                      // Set the internal oscillator to
                                       // 24.5/8 MHz
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// Pinout:
//
//    P1.3 -> LED
//
//    all other port pins unused
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P1MDOUT = 0x08;                     // Set LED to push-pull

   XBR1 = 0x40;                        // Enable crossbar
}

//-----------------------------------------------------------------------------
// Timer0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the Timer0 as a 16-bit timer, interrupt enabled.
// Using the internal osc. at 12MHz with a prescaler of 1:8 and reloading the
// T0 registers with TIMER0_RELOAD_HIGH/LOW it will interrupt and then toggle
// the LED.
//
// Note: The Timer0 uses a 1:48 prescaler.  If this setting changes, the
// TIMER_PRESCALER constant must also be changed.
//-----------------------------------------------------------------------------
void Timer0_Init(void)
{
   TH0 = TIMER0_RELOAD_HIGH;           // Init Timer0 High register
   TL0 = TIMER0_RELOAD_LOW ;           // Init Timer0 Low register
   TMOD = 0x01;                        // Timer0 in 16-bit mode
   CKCON = 0x02;                       // Timer0 uses a 1:48 prescaler
   ET0 = 1;                            // Timer0 interrupt enabled
   TCON = 0x10;                        // Timer0 ON
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer0_ISR
//-----------------------------------------------------------------------------
//
// Here we process the Timer0 interrupt and toggle the LED
//
//-----------------------------------------------------------------------------
void Timer0_ISR (void) interrupt 1
{
   LED = ~LED;                         // Toggle the LED
   TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
   TL0 = TIMER0_RELOAD_LOW;            // Reinit Timer0 Low register
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------