//-----------------------------------------------------------------------------
// T60x_Timer2_16bitReloadTimer.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program presents an example of use of the Timer2 of the C8051T60x's in
// 16-bit reload counter/timer mode. 
//
// The timer reload registers are initialized with a certain value so that
// the timer counts from that value up to FFFFh and when it overflows an
// interrupt is generated and the timer is automatically reloaded.
// In this interrupt the ISR toggles the LED.
//
// Pinout:
//
//    P0.2 -> LED (T606)
//    P0.6 -> LED (T600/1/2/3/4/5)
//
//    all other port pins unused
//
// How To Test:
//
// 1) Define the chip number being used under CHIPNUM
// 2) If a change in the interrupt period is required, change the value of
//    LED_TOGGLE_RATE.
// 3) Compile and download the code.
// 4) Verify the LED pins of J3 are populated on the 'T60x TB.
// 5) Run the code.
// 6) Check that the LED is blinking
//
//
// Target:         C8051T60x
// Tool chain:     KEIL C51 8.0 / KEIL EVAL C51
// Command Line:   None
//
// Release 1.0 / 31 JUL 2008 (ADT)
//    - Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <C8051T600_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define CHIPNUM   600                  // Define the chip number being used
                                       // Options: 600, 601, 602, 603,
                                       //          604, 605, 606

#define SYSCLK             24500000/8  // SYSCLK in Hz (24.5 MHz internal
                                       // oscillator / 8)
                                       // the internal oscillator has a
                                       // tolerance of +/- 2%

#define TIMER_PRESCALER            12  // Based on Timer2 CKCON and TMR2CN
                                       // settings

#define LED_TOGGLE_RATE           100  // LED toggle rate in milliseconds
                                       // if LED_TOGGLE_RATE = 1, the LED will
                                       // be on for 1 millisecond and off for
                                       // 1 millisecond

// There are SYSCLK/TIMER_PRESCALER timer ticks per second, so
// SYSCLK/TIMER_PRESCALER/1000 timer ticks per millisecond.
#define TIMER_TICKS_PER_MS  SYSCLK/TIMER_PRESCALER/1000

// Note: LED_TOGGLE_RATE*TIMER_TICKS_PER_MS should not exceed 65535 (0xFFFF)
// for the 16-bit timer

#define AUX1     -TIMER_TICKS_PER_MS


#define TIMER2_RELOAD            AUX1  // Reload value for Timer2

#if (CHIPNUM == 606)
SBIT (LED, SFR_P0, 2);                 // LED='1' means ON
#else
SBIT (LED, SFR_P0, 6);
#endif

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Port_Init (void);                 // Port initialization routine
void Timer2_Init (void);               // Timer2 initialization routine

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   PCA0MD &= ~0x40;                    // Clear watchdog timer enable
   REG0CN |= 0x01;                     // OTP Power Controller Enable

   Timer2_Init ();                     // Initialize the Timer2
   Port_Init ();                       // Init Ports
   EA = 1;                             // Enable global interrupts

   while (1);                          // Loop forever
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
// Pinout:
//
//    P0.2 -> LED (T606)
//    P0.6 -> LED (T600/1/2/3/4/5)
//
//    all other port pins unused
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   XBR2 = 0x40;                        // Enable crossbar
#if (CHIPNUM == 606)
   P0MDOUT = 0x04;                     // Set LED to push-pull
#else
   P0MDOUT = 0x40;
#endif
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures Timer2 as a 16-bit reload timer, interrupt enabled.
// Using the SYSCLK at 16MHz/8 with a 1:12 prescaler.
//
// Note: The Timer2 uses a 1:12 prescaler.  If this setting changes, the
// TIMER_PRESCALER constant must also be changed.
//-----------------------------------------------------------------------------
void Timer2_Init(void)
{
   CKCON &= ~0x60;                     // Timer2 uses SYSCLK/12
   TMR2CN &= ~0x01;

   TMR2RL = TIMER2_RELOAD;             // Reload value to be used in Timer2
   TMR2 = TMR2RL;                      // Init the Timer2 register

   TMR2CN = 0x04;                      // Enable Timer2 in auto-reload mode
   ET2 = 1;                            // Timer2 interrupt enabled
}


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// Here we process the Timer2 interrupt and toggle the LED
//
//-----------------------------------------------------------------------------
INTERRUPT (Timer2_ISR, INTERRUPT_TIMER2)
{
   static U16 i = 0;
   i++;
   
   if (i == LED_TOGGLE_RATE)
   {
      i = 0;
      LED = ~LED;                      // Toggle the LED
   }

   TF2H = 0;                           // Reset Interrupt
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------