//-----------------------------------------------------------------------------
// F36x_Timer3_16bitReloadTimer.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program presents an example of use of the Timer3 of the C8051F36x's in
// 16-bit reload counter/timer mode. It uses the 'F36xDK as HW platform.
//
// The timer reload registers are initialized with a certain value so that
// the timer counts from that value up to FFFFh and when it overflows an
// interrupt is generated and the timer is automatically reloaded.
// In this interrupt the ISR toggles the LED.
//
// Pinout:
//
//    P3.2 -> LED
//
//    all other port pins unused
//
// How To Test:
//
// 1) Open the F36x_Timer3_16bitReloadTimer.c file in the Silicon Labs IDE.
// 2) If a change in the interrupt period is required, change the value of
//    LED_TOGGLE_RATE.
// 3) Compile and download the code.
// 4) Verify the LED pins of J12 are populated on the 'F36x TB.
// 5) Run the code.
// 6) Check that the LED is blinking
//
//
// FID:            36X000039
// Target:         C8051F36x
// Tool chain:     KEIL C51 7.20 / KEIL EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (CG)
//    -23 OCT 2005
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F360.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             24500000/8  // SYSCLK in Hz (24.5 MHz internal
                                       // oscillator / 8)
                                       // the internal oscillator has a
                                       // tolerance of +/- 2%

#define TIMER_PRESCALER            12  // Based on Timer3 CKCON and TMR3CN
                                       // settings

#define LED_TOGGLE_RATE            50  // LED toggle rate in milliseconds
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

#define TIMER3_RELOAD            AUX2  // Reload value for Timer3

sbit LED = P3^2;                       // LED='1' means ON


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Port_Init (void);                 // Port initialization routine
void Timer3_Init (void);               // Timer3 initialization routine

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

sfr16 TMR3RL = 0x92;                   // Timer3 Reload Register
sfr16 TMR3 = 0x94;                     // Timer3 Register

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   PCA0MD &= ~0x40;                    // Clear watchdog timer enable

   Timer3_Init ();                     // Initialize the Timer3
   Port_Init ();                       // Init Ports
   EA = 1;                             // Enable global interrupts

   SFRPAGE = LEGACY_PAGE;

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
//    P3.2 -> LED
//
//    all other port pins unused
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   XBR1 = 0x40;                        // Enable crossbar
   P3MDOUT = 0x04;                     // Set LED to push-pull

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures Timer3 as a 16-bit reload timer, interrupt enabled.
// Using the SYSCLK at 16MHz/8 with a 1:12 prescaler.
//
// Note: The Timer3 uses a 1:12 prescaler.  If this setting changes, the
// TIMER_PRESCALER constant must also be changed.
//-----------------------------------------------------------------------------
void Timer3_Init(void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   CKCON &= ~0x60;                     // Timer3 uses SYSCLK/12
   TMR3CN &= ~0x01;

   TMR3RL = TIMER3_RELOAD;             // Reload value to be used in Timer3
   TMR3 = TMR3RL;                      // Init the Timer3 register

   TMR3CN = 0x04;                      // Enable Timer3 in auto-reload mode
   EIE1 |= 0x80;                       // Timer3 interrupt enabled

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer3_ISR
//-----------------------------------------------------------------------------
//
// Here we process the Timer3 interrupt and toggle the LED
//
//-----------------------------------------------------------------------------
void Timer3_ISR (void) interrupt 14
{
   LED = ~LED;                         // Toggle the LED
   TMR3CN &= ~0x80;                    // Reset Interrupt
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------