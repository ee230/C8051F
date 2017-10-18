//-----------------------------------------------------------------------------
// F80x_Watchdog.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The Watchdog Timer (WDT) is a hardware timer enabled on every reset that
// will timeout after a programmable interval.  When the WDT times out, it
// will force a device reset. To prevent the reset, the WDT must be restarted
// by application software before it overflows. If the system experiences a
// software or hardware malfunction preventing the software from restarting
// the WDT, the WDT will overflow and cause a reset, returning the MCU to a
// known state.
//
// This example demonstrates how to initialize and reset the Watchdog Timer
// (using a timer) and check for a Watchdog Timer reset.
// - Pinout:
//    P1.0 -> LED
//
//    P1.1 -> SW
//
//    all other port pins unused
//
// How To Test:
//
// 1) Compile and download code to a 'F80x device.
// 2) Run the code:
//      - The test will reset the WDT and blink an LED until the switch (SW)
//          is pressed, simulating an external stimulus.
//      - When the WDT trips, it will cause a reset.  The code checks for a
//         WDT reset and will disable the WDT and blink the LED twice as fast
//         to indicate the current state.
//      - Press the switch (SW) again to reset the test.
//
//
// Target:         C8051F80x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (PD)
//    -09 JUL 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>             // compiler declarations
#include <C8051F800_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK 24500000                // from Internal Oscillator

SBIT (LED,    SFR_P1, 0);              // '0' means ON, '1' means OFF
SBIT (SW,     SFR_P1, 4);              // '0' means ON, '1' means OFF
//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void Watchdog_Init (void);
void Port_Init (void);
void Timer0_Init (void);
void Timer2_Init (void);

INTERRUPT_PROTO (TIMER0_ISR, INTERRUPT_TIMER0);
INTERRUPT_PROTO (TIMER2_ISR, INTERRUPT_TIMER2);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all initialization, and then loops, while Timer0
// resets the WDT.  When SW is pressed, Timer0 stops and a WDT reset occurs.
// The code checks the RSTSRC register to check if the last reset is because
// of a WDT.
//
void main (void) {

   PCA0MD &= ~0x40;                    // WDTE = 0 (disable watchdog timer)

   Port_Init ();                       // Initialize crossbar and GPIO

   while (SW == 0)                     // Wait for the switch (SW) to be
   {                                   // released before continuing
   }

   SYSCLK_Init ();                     // Initialize system clock to 24.5MHz

   Watchdog_Init ();                   // Initialize the Watchdog timeout
                                       // interval

   Timer0_Init ();                     // Init Timer2 to periodically reset
                                       // the WDT

   Timer2_Init ();                     // Init Timer2 to generate
                                       // interrupts for the LED.

   EA = 1;                             // Enable global interrupts


// Start test code ------------------------------------------------------------

   if ((RSTSRC & 0x02) == 0x00)        // First check the PORSF bit... if PORSF
   {                                   // is set, all other RSTSRC flags are
                                       // invalid

      if (RSTSRC == 0x08)              // Check if the last reset was due to a
      {                                // WDT.
         
         PCA0MD &= ~0x40;              // WDTE = 0 (disable watchdog timer)

         TMR2RL = 0x8000;              // Blink the LED twice as fast

         while (SW != 0);              // Wait until the SW is pressed to reset
                                       // the WDT

         while (SW == 0);              // Wait until the switch is released
                                       // before continuing

         RSTSRC = 0x10;                // Force a SW reset to reset the WDT
      }
   }



   while (1) {                         // Spin forever

      if (SW == 0)                     // If the switch (SW) is pressed, stop
      {                                // resetting the WDT, causing a reset
         TR0 = 0;

         while(1);
      }
   }

// End test code --------------------------------------------------------------

}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal precision
// oscillator at 24.5 MHz  and enables the missing clock
// detector.
//
//-----------------------------------------------------------------------------

void SYSCLK_Init (void)
{
   OSCICN |= 0x83;                     // Enable the precision internal osc.

   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor enabled.

   CLKSEL = 0x00;                      // Select precision internal osc.
                                       // divided by 1 as the system clock
}

//-----------------------------------------------------------------------------
// Watchdog_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the Watchdog Timer for a period of 32.1 ms
//
void Watchdog_Init (void)
{
   PCA0MD    &= ~0x0E;                // Set PCA clock source to SYSCLK/12
   PCA0CPL2 = 255;                    
   PCA0MD |= 0x40;                    // WDTE = 1 (enable watchdog timer)
   PCA0CPH2 = 255;                    // Reset the WDT
}

//-----------------------------------------------------------------------------
// Port_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P1.0   digital   push-pull     LED
//
// P1.4   digital   open-drain    SW (switch)
//
void Port_Init (void)
{
   P1MDOUT |= 0x01;                    // Enable LED as a push-pull output

   XBR1 = 0x40;                        // Enable crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer0_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer0 to reset the WDT approximately every 18 ms (WDT will trip
// every 21.4 ms).
// - 8-bit auto-reload mode
// - use SYSCLK/48 as timebase
// - high-priority interrupt
//
// NOTE: this routine changes the CKCON.1-0 (SCA bits) settings, so Timer1
// will also be affected.
//
void Timer0_Init (void)
{
   TCON &= ~0x30;                      // Stop Timer 0 and clear TF0

   TMOD |= 0x02;                       // Mode 2: 8-bit auto-reload mode

   CKCON |= 0x02;                      // Use SYSCLK/48 for the timebase

   TH0 = 0x00;                         // Use maximum count time
   TL0 = 0x00;

   ET0 = 1;                            // Enable T0 interrupts

   PT0 = 1;                            // Enable T0 interrupts as high priority
                                       // so the WDT reset will always be given
                                       // priority over other interrupts (in
                                       // this case, just the LED)

   TR0 = 1;                            // Start Timer0
}

//-----------------------------------------------------------------------------
// Timer2_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt after the
// maximum possible time (TMR2RL = 0x0000).
//
void Timer2_Init (void)
{
   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // auto-reload mode

   TMR2RL = 0x0000;                    // Init reload value
   TMR2 = 0xFFFF;                      // Set to reload immediately

   ET2 = 1;                            // Enable Timer2 interrupts

   TR2 = 1;                            // Start Timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer0_ISR (high-priority)
//-----------------------------------------------------------------------------
//
// This routine resets the WDT approximately every 18 ms.  Because this ISR
// is given high priority, it will interrupt the Timer2_ISR, so the WDT will
// always be reset in time.
//
INTERRUPT(TIMER0_ISR, INTERRUPT_TIMER0)
{
   static U8 count = 0;

   count++;

   // Timer0 is in 8-bit mode (256 counts) using SYSCLK/48 as the time base
   //
   // 12.25 MHz / 48 = 255 kHz
   // 256 counts per interrupt, so 255 kHz / 256 = 996 Hz interrupts
   // 18 ms = 55 Hz
   // 996 Hz / 55 Hz = ~18 interrupts for 18 ms
   if (count == 36)
   {
      PCA0CPH2 = 255;                    // Reset the WDT

      count = 0;
   }
}

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer2 overflows.
//
INTERRUPT(TIMER2_ISR, INTERRUPT_TIMER2)
{
   TF2H = 0;                           // Reset Interrupt

   LED = !LED;                         // Change state of LED
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------