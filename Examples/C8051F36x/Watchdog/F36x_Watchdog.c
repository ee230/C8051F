//-----------------------------------------------------------------------------
// F36x_Watchdog.c
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
//
// How to Test:
// 1) Compile and download code to a 'F360 device.
// 2) Run the code:
//        - The test will blink the LED at a rate of 10Hz until the switch SW1
//          (P3.0) is pressed.
//        - Once the the switch is pressed it will cause the WDT to trip and
//          cause a reset.
//        - Upon reset the code checks for a WDT reset and blinks the LED five
//          times faster than before.
//
// FID:            36X000041
// Target:         C8051F36x
// Tool chain:     Keil C51 8.0 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (SM / TP)
//    -13 OCT 2006

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F360.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F36x
//-----------------------------------------------------------------------------

sfr16 TMR2RL   = 0xCA;                 // Timer2 reload value
sfr16 TMR2     = 0xCC;                 // Timer2 counter

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000 / 8      // SYSCLK frequency in Hz

sbit LED = P3^2;                       // LED='1' means ON
sbit SW1 = P3^0;                       // SW1='0' means switch pressed

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void Watchdog_Init (void);
void Timer0_Init (void);
void Timer2_Init (unsigned int counts);

void Timer0_ISR (void);
void Timer2_ISR (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
//
// The MAIN routine performs all the intialization, and then loops until the
// switch is pressed. When SW1 (P3.0) is pressed the code checks the RSTSRC
// register to make sure if the last reset is because of WDT.
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init();                  // Initialize system clock to 24.5/8 MHz
   Watchdog_Init();                    // Intialize the Watchdog Timer
   Port_Init();                        // Initialize crossbar and GPIO
   Timer0_Init();                      // Initialize Timer0 to "kick" the WDT

   EA = 1;                             // Enable global interrupts

   if ((RSTSRC & 0x02) == 0x00)        // First check the PORSF bit. If PORSF
   {                                   // is set, all other RSTSRC flags are
                                       // invalid

      if (RSTSRC == 0x08)              // Check if the last reset was due to a
      {                                // WDT.
         PCA0MD &= ~0x40;              // Disable the WDT until the next reset

         // Make LED blink at 50Hz
         Timer2_Init (SYSCLK / 12 / 50);

         while (SW1 != 0);             // Wait until SW1 is pressed to reset
                                       // the WDT

         while (SW1 == 0);             // Wait until the switch is released
                                       // before continuing

         RSTSRC = 0x10;                // Force a SW reset to reset the WDT
      }
      else
      {
         // Init Timer2 to generate interrupts at a 10Hz rate.
         Timer2_Init (SYSCLK / 12 / 10);
      }
   }

   SFRPAGE = LEGACY_PAGE;

   while (1)
   {
      if (SW1 == 0)                    // If the switch (SW) is pressed, stop
      {                                // resetting the WDT, causing a reset
         TR0 = 0;

         while(1);
      }
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
// This routine initializes the system clock to use the internal 24.5MHz / 8
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN = 0x80;                      // Configure internal oscillator for
                                       // its lowest frequency
   RSTSRC = 0x04;                      // Enable missing clock detector

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Watchdog_Init
//-----------------------------------------------------------------------------
// Return Value : None
// Parameters   : None
//
// This routine initializes the Watchdog timer (WDT) to use the SYSCLK / 12
// as its clock source.  It also sets the offset value by writing to PCA0CPL5.
//-----------------------------------------------------------------------------
void Watchdog_Init()
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   // Step 1: Disable the WDT by writing a ‘0’ to the WDTE bit.
   // (already disabled)

   // Step 2: Select the desired PCA clock source (with the CPS2-CPS0 bits).
   PCA0MD &= ~0x0E;                   // Timebase selected - System clock / 12

   // Step 3: Load PCA0CPL5 with the desired WDT update offset value.
   PCA0CPL5 = 0xFF;                   // A value of 255 (0xFF) is 32.1 ms
                                       // with a timebase of SYSCLK/12

   // Step 4: Configure the PCA Idle mode (set CIDL if the WDT should be
   // suspended while the CPU is in Idle mode).
   PCA0MD &= ~0x80;                   // PCA runs normally in IDLE mode

   // Step 5: Enable the WDT by setting the WDTE bit to ‘1’.
   PCA0MD |= 0x40;

   // Step 6: Write a value to PCA0CPH5 to reload the WDT.
   PCA0CPH5 = 0x00;

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
// Return Value : None
// Parameters   : None
//
// This function configures the Crossbar and GPIO ports.
//
// Pinout:
//
// P3.0   digital   open-drain    SW1
//
// P3.2   digital   push-pull     LED
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   P3MDOUT  = 0x04;                    // Enable SW1 as open-drain
                                       // Enable LED as push-pull

   XBR0     = 0x00;                    // No digital peripherals selected
   XBR1     = 0x40;                    // Enable crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Timer0_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer0 to reset the WDT approximately every 28 ms (WDT will trip
// every 32.1 ms).
// - 8-bit auto-reload mode
// - use SYSCLK/48 as timebase
// - high-priority interrupt
//
// NOTE: this routine changes the CKCON.1-0 (SCA bits) settings, so Timer1
// will also be affected.
//
void Timer0_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

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

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Timer2_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//    1) unsigned int counts - number of counts until the Timer should overflow
//                      range: all unsigned int values = 0 to 65535
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt after
// <counts> number of timer ticks.
//
void Timer2_Init (unsigned int counts)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // auto-reload mode

   CKCON &= ~0x60;                     // Use SYSCLK/12 as timebase

   TMR2RL = -counts;                   // Init reload value
   TMR2 = 0xFFFF;                      // Set to reload immediately

   ET2 = 1;                            // Enable Timer2 interrupts

   TR2 = 1;                            // Start Timer2

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
   // 3.0625 MHz / 48 = 63.8 kHz
   // 256 counts per interrupt, so 63.8 kHz / 256 = 249 Hz interrupts
   // 28 ms = 36 Hz
   // 249 Hz / 36 Hz = ~6 interrupts for 28 ms
   if (count == 6)
   {
      PCA0CPH5 = 0x00;                 // Reset the WDT

      count = 0;
   }
}

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer2 overflows.
//-----------------------------------------------------------------------------
void Timer2_ISR (void) interrupt 5
{
   TF2H = 0;                           // clear Timer2 interrupt flag
   LED = ~LED;                         // change state of LED
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
