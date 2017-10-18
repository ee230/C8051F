//-----------------------------------------------------------------------------
// T630_Timer3_16bitReloadTimer_LFO_Suspend.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program presents an example of use of the Timer3 of the C8051T630 in
// 16-bit reload counter/timer mode. It uses the 'T630MB+DB as HW platform.
//
// The timer reload registers are initialized with a certain value so that
// the timer counts from that value up to FFFFh and is automatically reloaded.
// The LED is toggled in the main loop and then the MCU enters suspend mode.
// The Timer3 overflow event makes the MCU exit suspend mode.
//
// Pinout:
//
//    P1.3 -> LED
//
//    all other port pins unused
//
// How To Test:
//
// 1) Open the T630_Timer3_16bitReloadTimer_LFO_Suspend.c file in the
//    Silicon Labs IDE.
// 2) If a change in the blink rate is required, change the value of
//    LED_TOGGLE_RATE.
// 3) Compile and download the code.
// 4) Verify the LED pins of J9 are populated on the 'T630 MB.
// 5) Run the code.
// 6) Check that the LED is blinking.
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

#define SYSCLK             24500000/8  // SYSCLK in Hz (24.5 MHz internal
                                       // oscillator / 8)
                                       // the internal oscillator has a
                                       // tolerance of +/- 2%

#define LFO                     80000  // LFO frequency in Hz
                                       // (Internal Low-Freq Oscillator / 1)
                                       // Tolerance: +/- 10%

#define TIMER_PRESCALER            8   // Based on Timer3 CKCON and TMR3CN
                                       // settings

#define LED_TOGGLE_RATE           300  // LED toggle rate in milliseconds
                                       // if LED_TOGGLE_RATE = 1, the LED will
                                       // be on for 1 millisecond and off for
                                       // 1 millisecond

// There are LFO/TIMER_PRESCALER timer ticks per second, so
// LFO/TIMER_PRESCALER/1000 timer ticks per millisecond.
#define TIMER_TICKS_PER_MS  LFO/TIMER_PRESCALER/1000

// Note: LED_TOGGLE_RATE*TIMER_TICKS_PER_MS should not exceed 65535 (0xFFFF)
// for the 16-bit timer

#define AUX1     TIMER_TICKS_PER_MS*LED_TOGGLE_RATE
#define AUX2     -AUX1

#define TIMER3_RELOAD            AUX2  // Reload value for Timer3

SBIT (LED, SFR_P1, 3);                 // LED==1 means ON

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);           // Oscillator initialization routine
void Port_Init (void);                 // Port initialization routine
void Timer3_Init (void);               // Timer3 initialization routine

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   PCA0MD &= ~0x40;                    // Clear watchdog timer enable

   Oscillator_Init ();                 // Initialize the oscillators
   Timer3_Init ();                     // Initialize the Timer3
   Port_Init ();                       // Init Ports

   while (1)                           // Loop forever
   {
         LED = ~LED;                   // Toggle the LED
      
         OSCICN |= 0x20;               // Suspend Internal H-F Oscillator.
                                       // Timer3 overflow will wake it up.
         
      // 1) If the MCU did not stay in suspend till the Timer3 overflow event, 
      //    the LED will blink very fast (close to SYSCLK rate), making it
      //    look like it is always on, but slightly dimmer.
      
      // 2) If the MCU stayed in suspend indefinitely, the LED will stay dark.
      
      // 3) If the MCU left suspend mode correctly on Timer3 overflow, then the
      //    LED will blink at the rate specified by LED_TOGGLE_RATE.
   }
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
// This function configures the oscillators.
// SYSCLK is derived from high-freq internal osc; set to 24.5/8 MHz.
// Low-freq oscillator is initialized to 80/1 kHz (nominal).
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN &= ~0x03;   // Internal H-F osc set to divide-by-8.
   OSCLCN |= 0x83;   // Internal L-F osc set to divide-by-1.
   while ((OSCLCN & 0x40) == 0); // Wait till LFO starts.
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
// P1.3  digital  push-pull  LED
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   XBR1 = 0x40;                        // Enable crossbar
   P1MDOUT = 0x08;                     // Set LED to push-pull
}

//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures Timer3 as a 16-bit reload timer, interrupt disabled.
// The timer is clocked by the internal LFO/8 source.
//
// Note: The Timer3 uses a 1:8 prescaler of the LFO.  If this setting changes,
// the TIMER_PRESCALER constant must also be changed.
//-----------------------------------------------------------------------------
void Timer3_Init(void)
{
   TMR3CN &= ~0x07;                    // Stop Timer3 and clear T3XCLK bits
   CKCON &= ~0xC0;                     // Timer3 uses clock defined by T3XCLK

   TMR3RL = TIMER3_RELOAD;             // Reload value to be used in Timer3
   TMR3 = TIMER3_RELOAD;               // Init the Timer3 register

   TMR3CN = 0x07;                      // Enable Timer3 in auto-reload mode
                                       // T3XCLK=11b (clock = internal LFO/8)

   EIE1 &= ~0x80;                      // Timer3 interrupt disabled.
                                       // Interrupt is not required to exit 
                                       // suspend.
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------