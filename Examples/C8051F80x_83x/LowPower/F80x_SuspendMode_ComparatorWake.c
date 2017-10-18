//-----------------------------------------------------------------------------
// F80x_SuspendMode_ComparatorWake.c
//-----------------------------------------------------------------------------
// Copyright (C) 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program blinks an LED using Timer 2 whenever the configured
// comparator output is positive.  When the comparator toggles to a
// negative value, the comparator ISR sets the device to suspend mode.
// Whenever the comparator's output toggles back to a positive value,
// the comparator wakes the device and blinking resumes.
//
// How To Test:
//
// 1) Download code to the 'F80x target board
// 2) Ensure that J3 connects P1.0_LED to P1.0, and P1.4_SW to P1.4
// 3) Connect port 1.1 to an external voltage source set to a mid-rail
//    voltage or the on-board potentiometer.  Connect port 1.2 to the
//    on-board Switch.
// 4) Run the program.
//
//
// Target:         C8051F80x
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

SBIT (LED,    SFR_P1, 0);              // '0' means ON, '1' means OFF

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK             24500000L


//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void);
void Comparator0_Init(void);
void PORT_Init (void);
void Timer2_Init (void);
INTERRUPT_PROTO (TIMER2_ISR, INTERRUPT_TIMER2);
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (disable watchdog timer)

   PORT_Init ();                       // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator
   Comparator0_Init();                 // Initialize Comparator0
   Timer2_Init();                      // Initialize Timer 2

   EA = 1;                             // Enable global interrupts

   //----------------------------------
   // Main Application Loop
   //----------------------------------
   while (1)
   {
      OSCICN |= 0x20;                  // Place MCU in suspend mode
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
// use an external CMOS oscillator, which will be used to clock Timer 3.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCXCN    = 0x20;
   CLKSEL = 0x00;                      // Select internal osc.
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
// P1.0   digital   push-pull     LED
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P1MDIN &= ~0x06;                    // P1.1, P1.2 are analog

   P1MDOUT |= 0x01;                    // P1.0 is push-pull

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}


//-----------------------------------------------------------------------------
// Comparator0_Init
//
// Return Value : None
// Parameters   : None
//
//  This function configures the comparator to pins P1.1 and P1.2
//-----------------------------------------------------------------------------

void Comparator0_Init (void)
{
   U8 i;
   CPT0CN = 0x80;                      // Comparator enabled

   for (i = 0; i < 35; i++);           // Wait 10us for initialization

   CPT0CN    &= ~0x30;
   CPT0MX = 0x45;                      // P1.1 = Negative Input
                                       // P1.2 = Positive Input

   for (i = 0; i < 35; i++);           // Wait 10us for initialization
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures Timer2 as a 16-bit reload timer, interrupt enabled.
// Using the SYSCLK at 24.5MHz/8 with a 1:12 prescaler.
//
// Note: The Timer2 uses a 1:12 prescaler.  If this setting changes, the
// TIMER_PRESCALER constant must also be changed.
//-----------------------------------------------------------------------------
void Timer2_Init(void)
{
   CKCON &= ~0x60;                     // Timer2 uses SYSCLK/12
   TMR2CN &= ~0x01;

   TMR2RL = -(SYSCLK/12/1000);         // Reload value to be used in Timer2
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
INTERRUPT (TIMER2_ISR, INTERRUPT_TIMER2)
{
   LED = !LED;                         // Toggle the LED
   TF2H = 0;                           // Reset Interrupt
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
