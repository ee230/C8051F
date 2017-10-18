//-----------------------------------------------------------------------------
// F80x_SuspendMode_Timer2Wake.c
//-----------------------------------------------------------------------------
// Copyright (C) 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program blinks the C8051F800 target board's LED using Timer 2 and
// suspend mode.  The device is placed in suspend mode with timer 2 running
// from an external source.  When timer 2 overflows, the device wakes up,
// toggles the LED on or off, and then puts itself back to suspend mode.
//
// How To Test:
//
// 1) Download code to the 'F80x target board
// 2) Ensure that pins J3 connects P1.0_LED to P1.0
// 3) Connect an external clock source to P1.3.  The code is
//    currently configured for a CMOS clock running at 50kHz.
// 4) Ensure J9 is connected.
// 4) Run the program.
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

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK             24500000L
#define EXTCLK             50000L/8L

#define LED_TOGGLE_RATE           200L // LED toggle rate in milliseconds
                                       // if LED_TOGGLE_RATE = 1, the LED will
                                       // be on for 1 millisecond and off for
                                       // 1 millisecond

// There are SYSCLK/TIMER_PRESCALER timer ticks per second, so
// SYSCLK/TIMER_PRESCALER/1000 timer ticks per millisecond/ 8(Ext.Osc Circuit).
#define TIMER_TICKS_PER_MS  EXTCLK/1000L

// Note: LED_TOGGLE_RATE*TIMER_TICKS_PER_MS should not exceed 65535 (0xFFFF)
// for the 16-bit timer

#define AUX1     TIMER_TICKS_PER_MS*LED_TOGGLE_RATE
#define AUX2     -AUX1

#define TIMER2_RELOAD            AUX2  // Reload value for Timer2

#define LED_ON           0
#define LED_OFF          1

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void);
void PORT_Init (void);
void Timer2_Init (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (disable watchdog timer)

   PORT_Init ();                       // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator
   Timer2_Init();                      // Initialize Timer 2 to operate
                                       // as a suspend mode wake-up source
   EA = 1;                             // Enable global interrupts

   //----------------------------------
   // Main Application Loop
   //----------------------------------
   while (1)
   {
      GREEN_LED = !GREEN_LED;          // Toggle LED on and off

      OSCICN |= 0x20;                  // Place MCU in suspend mode
      // MCU remains in suspend mode until timer 2 overflows
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
// oscillator.  The function also configures the external clock FR to
// use an external CMOS oscillator, which will be used to clock Timer 2.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN |= 0x80;                     // Enable the precision internal osc.

   OSCXCN = 0x20;

   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor reset enabled

   CLKSEL = 0x01;                      // Select external osc.
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
// P1.0   digital   push-pull          LED
// P0.3   digital   open-drain/High-Z  XTAL2
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   // Oscillator Pins
   P0MDIN  |=  0x08;                   // P0.3 is digital
   P0SKIP  |=  0x08;                   // P0.3 skipped in the Crossbar
   P0MDOUT &= ~0x08;                   // P0.3 output mode is open-drain
   P0      |=  0x08;                   // P0.3 output drivers are OFF

   P1MDIN |= 0x01;                     // P1.0 is digital

   P1MDOUT |= 0x01;                    // P1.0 is push-pull

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures Timer2 as a 16-bit reload timer, interrupt enabled.
// Using the SYSCLK at 20MHz/8 with a 1:12 prescaler.
//
// Note: The Timer2 uses a 1:12 prescaler.  If this setting changes, the
// TIMER_PRESCALER constant must also be changed.
//-----------------------------------------------------------------------------
void Timer2_Init(void)
{
   CKCON &= ~0x30;                     // Timer2 uses external clock
   TMR2CN = 0x01;

   TMR2 = TIMER2_RELOAD;               // Init the Timer2 register
   TMR2RL = TIMER2_RELOAD;             // Reload value to be used in Timer2
   TMR2CN    = 0x05;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
