//-----------------------------------------------------------------------------
// T620_Timer0_13bitExtTimer.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program presents an example of use of the Timer0 of the C8051T620's in
// 13-bit counter/timer in counter mode from an external pin. It uses two I/O
// pins; one to create the input pulses (SIGNAL) to be counted and another one
// to enable the counting process (GTE).
//
// This code uses the 'T620DK as HW platform.
//
// Pinout:
//
//    P0.0 -> T0 (Timer0 External Input)
//    P0.1 -> /INT0
//
//    P1.5 -> GTE (digital, push-pull)
//
//    P1.7 -> SIGNAL (digital, push-pull)
//
//    P2.0 -> BUTTON1 (switch)
//
//    P2.2 -> LED1 (digital, push-pull)
//
//    all other port pins unused
//
// How To Test:
//
// 1) Open the T620_Timer0_13bitExtTimer.c file in the Silicon Labs IDE.
// 2) To change the number of input pulses/interrupt, modify
//    PULSES_PER_TOGGLE.
// 3) To change the speed of the SIGNAL waveform, modify
//    SOFTWARE_DELAY
// 4) Compile the project
// 5) Download code to a 'T620 device on a 'T62x mother board.
// 6) Verify the LED and switch pins of J9 and J10 are populated on the
//    'T62x MB.
// 7) Connnect the following pins:
//
//                   P0.0 <--> P1.7 (T0 with SIGNAL)
//                   P0.1 <--> P1.5 (/INT0 with GTE)
//
// 8) Run the code
// 9) To enable the counting, press and hold BUTTON1 (switch), which will be
//    polled to enable the timer.
// 10) The LED will blink and SIGNAL can be observed on an oscilloscope.
//
//
// Target:         C8051T620/1 or 'T320/1/2/3
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -20 JUN 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             12000000/8  // SYSCLK in Hz (12 MHz internal
                                       // oscillator / 8)
                                       // the internal oscillator has a
                                       // tolerance of +/- 2%

#define PULSES_PER_TOGGLE       1000   // Arbitrary number of pulses in the
                                       // input pin necessary to create an
                                       // interrupt.
                                       // Limited to 0x1FFF or 8191d for a
                                       // 13-bit timer

#define SOFTWARE_DELAY  SYSCLK/100000  // Software timer to generate the
                                       // SIGNAL output
                                       // Generate a signal in the kHz range

#define AUX0 0x1FFF-PULSES_PER_TOGGLE+1
#define AUX1 AUX0&0x001F               // 5 LSBs of timer value in TL0[4:0]
#define AUX2 ((AUX0&0x1FFF)>>5)        // High 8 bits of timer in TH0

#define TIMER0_RELOAD_HIGH      AUX2   // Reload value for Timer0 high byte
#define TIMER0_RELOAD_LOW       AUX1   // Reload value for Timer0 5 LSBs

SBIT (GTE, SFR_P1, 5);                 // Gate control signal for Timer0
SBIT (LED, SFR_P2, 2);                 // LED='1' means ON
SBIT (SIGNAL, SFR_P1, 7);              // SIGNAL is used to input pulses into
                                       // T0 pin
SBIT (BUTTON1, SFR_P2, 0);             // Button that enables counting

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Port_Init (void);                 // Port initialization routine
void Timer0_Init (void);               // Timer0 initialization routine

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   unsigned int counter;

   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Timer0_Init ();                     // Initialize the Timer0
   Port_Init ();                       // Init Ports

   LED = 0;
   EA = 1;                             // Enable global interrupts

   while (1)
   {
      if (BUTTON1 == 0)                // If button pressed, enable counting
      {
         GTE = 1;
      }
      else
      {
         GTE = 0;
      }

      // Wait a certain time before toggling signal
      for (counter=0; counter < SOFTWARE_DELAY; counter++);

      SIGNAL = ~SIGNAL;                // Toggle the SIGNAL pin
   }
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
//    P0.0 -> T0 (Timer0 External Input)
//    P0.1 -> /INT0
//
//    P1.5 -> GTE (digital, push-pull)
//
//    P1.7 -> SIGNAL (digital, push-pull)
//
//    P2.0 -> BUTTON1 (switch)
//
//    P2.2 -> LED1 (digital, push-pull)
//
//    all other port pins unused
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   XBR1 = 0x50;                        // INT0 and T0 available at the I/O pins
                                       // Enable crossbar
   P1MDOUT = 0xA0;                     // Set I/Os to be used as push-pull
   P2MDOUT = 0x04;
}

//-----------------------------------------------------------------------------
// Timer0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the Timer0 as a 13-bit timer, interrupt enabled.
// Using an external signal as clock source 1:8 and reloading the
// TH0 and TL0 registers it will interrupt and then toggle the LED upon roll-
// over of the timer.
//
// Note: In this example the GATE0 gate control is used.
//
//-----------------------------------------------------------------------------
void Timer0_Init(void)
{
   TMOD = 0x0C;                        // Timer0 in 13-bit mode ext. counter
                                       // gated counting T0 input
   ET0 = 1;                            // Timer0 interrupt enabled
   TCON = 0x11;                        // Timer0 ON with INT0 edge active
   TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
   TL0 = TIMER0_RELOAD_LOW;            // Reinit Timer0 Low register
}


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer0_ISR
//-----------------------------------------------------------------------------
//
// Toggle the LED.
//
//-----------------------------------------------------------------------------
INTERRUPT(Timer0_ISR, INTERRUPT_TIMER0)
{
   LED = !LED;                         // Toggle the LED
   TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
   TL0 = TIMER0_RELOAD_LOW;            // Reinit Timer0 Low register
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------