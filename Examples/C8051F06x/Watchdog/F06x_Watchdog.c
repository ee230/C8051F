//-----------------------------------------------------------------------------
// F06x_Watchdog.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
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
//    P1.6 -> LED
//
//    P3.7 -> SW
//
//    all other port pins unused
//
// How To Test:
//
// 1) Compile and download code to a 'F06x device.
// 2) Verify J1 and J3 are connected (LED and SW jumpers).
// 3) Run the code:
//      - The test will reset the WDT and blink an LED until the switch (SW)
//          is pressed, simulating an external stimulus.
//      - When the WDT trips, it will cause a reset.  The code checks for a
//         WDT reset and will disable the WDT and blink the LED twice as fast
//         to indicate the current state.
//      - Press the switch (SW) again to reset the test.
//
//
// FID:            06X000012
// Target:         C8051F06x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -5 JUNE 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F060.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK 12250000                // from Internal Oscillator

sfr16 RCAP2 = 0xCA;                    // Timer2 reload value
sfr16 TMR2 = 0xCC;                     // Timer2 counter

sbit LED = P1^6;                       // LED='1' means ON

sbit SW = P3^7;                        // SW='0' means pressed

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void Watchdog_Init (void);
void Port_Init (void);
void Timer0_Init (void);
void Timer2_Init (void);

void Timer0_ISR (void);
void Timer2_ISR (void);

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

   WDTCN = 0xA5;                       // Reset the WDT

   SFRPAGE = CONFIG_PAGE;

   Port_Init ();                       // Initialize crossbar and GPIO

   while (SW == 0)                     // Wait for the switch (SW) to be
   {                                   // released before continuing
      WDTCN = 0xA5;                    // Reset the WDT
   }

   SYSCLK_Init ();                     // Initialize system clock to 12.25MHz

   Watchdog_Init ();                   // Initialize the Watchdog timeout
                                       // interval

   SFRPAGE = TIMER01_PAGE;

   Timer0_Init ();                     // Init Timer0 to periodically reset
                                       // the WDT


   SFRPAGE = TMR2_PAGE;

   Timer2_Init ();                     // Init Timer2 to generate
                                       // interrupts for the LED.

   EA = 1;                             // Enable global interrupts


// Start test code ------------------------------------------------------------

   SFRPAGE = LEGACY_PAGE;              // Sit in this SFRPAGE

   if ((RSTSRC & 0x02) == 0x00)        // First check the PORSF bit... if PORSF
   {                                   // is set, all other RSTSRC flags are
                                       // invalid

      if (RSTSRC == 0x08)              // Check if the last reset was due to a
      {                                // WDT.
         WDTCN = 0xDE;                 // Disable the WDT until the next reset
         WDTCN = 0xAD;

         SFRPAGE = TMR2_PAGE;

         RCAP2 = 0x8000;               // Blink the LED twice as fast

         SFRPAGE = LEGACY_PAGE;

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
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SYSCLK_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal 24.5 MHz / 2
// oscillator as its clock source.  Also enables missing clock detector reset.
//
void SYSCLK_Init (void)
{
   OSCICN = 0x82;                      // Configure internal oscillator for
                                       // 24.5 MHz / 2 = 12.25 MHz

   RSTSRC = 0x04;                      // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// Watchdog_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the Watchdog Timer for a maximum period of 21.4 ms
// with WDTCN[2-0] = 110b and SYSCLK = 12.25 MHz:
//
// 4 ^ (3 + WDTCN[2-0]) x SYSCLK
// 4 ^ (3 + 6) x 1/(12.25 MHz)
// = 21.4 ms
//
void Watchdog_Init (void)
{
   WDTCN &= ~0x80;                     // WDTCN.7 must be logic 0 when setting
                                       // the interval.

   WDTCN |= 0x06;                      // Set the WDTCN[2-0] to 110b
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
// P1.6   digital   push-pull     LED
//
// P3.7   digital   open-drain    SW (switch)
//
void PORT_Init (void)
{
   P1MDOUT |= 0x40;                    // Enable LED as a push-pull output

   XBR1 = 0x00;                        // no peripherals enabled
   XBR2 = 0x40;                        // Enable crossbar and weak pull-ups
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

   TMR2CF = 0x00;                      // use SYSCLK/12 as timebase; count up

   RCAP2 = 0x0000;                     // Init reload value
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
void Timer0_ISR (void) interrupt 1
{
   static unsigned char count = 0;

   count++;

   // Timer0 is in 8-bit mode (256 counts) using SYSCLK/48 as the time base
   //
   // 12.25 MHz / 48 = 255 kHz
   // 256 counts per interrupt, so 255 kHz / 256 = 996 Hz interrupts
   // 18 ms = 55 Hz
   // 996 Hz / 55 Hz = ~18 interrupts for 18 ms
   if (count == 18)
   {
      WDTCN = 0xA5;                    // Reset the WDT

      count = 0;
   }
}

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer2 overflows.
//
void Timer2_ISR (void) interrupt 5
{
   TF2 = 0;                            // Clear Timer2 interrupt flag

   LED = ~LED;                         // Change state of LED
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------