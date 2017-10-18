//-----------------------------------------------------------------------------
// CapTouchSense_WakeOnRTC_Main.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program implements Low-Power Capacitive TouchSense with Wake on RTC.
//
// How To Test:
//
// Setup:
// 1) Customize the constants in "CapTouchSense_WakeOnRTC.h" and rebuild.
// 2) Download code to a C8051F930TB Target Board or a CapTouchSenseDC ToolStick
//    Daughter Card.
// 3) If uncalibrated, the RED LED will light up. In this case, ensure that 
//    no object or a finger is on any of the CapTouchSense switches and press 
//    the SW2 push-button switch to begin calibration.
// 4) All LEDs will flash once to indicate that calibration is complete.
// 5) Place a finger on each CapTouchSense switch to see the corresponding LED
//    light up.
//
//
// Target:         C8051F931, C8051F921
// Tool chain:     Generic
// Command Line:   None
//
//
// Release 1.1 
//    - Updated to be compatible with Raisonance Toolchain (FB)
//    - 14 APR 2010
//
// Release 1.0
//    -Initial Revision (PKC)
//    -20 MAY 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <C8051F930_defs.h>            // SFR declarations
#include "CapTouchSense_WakeOnRTC.h"

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

U8 System_State;

U8 CS_Switch_State[NUM_SWITCHES];

//LOCATED_VARIABLE(CS_Switch_Cal_Threshold[NUM_SWITCHES], UU16, code, 0x7800, 0);
#ifdef SDCC
	LOCATED_VARIABLE_NO_INIT(CS_Switch_Cal_Threshold[NUM_SWITCHES], UU16, SEG_CODE, CAL_CONSTANTS_PAGE_ADDR);
#else
	UU16 code CS_Switch_Cal_Threshold[NUM_SWITCHES] _at_ CAL_CONSTANTS_PAGE_ADDR;
#endif

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void)
{
   U8 i;

   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)
   SYSCLK_Init ();
   Port_IO_Init ();
   RTC_Init ();
   TouchSense_Init ();

   // Set RTC alarm wakeup interval
   RTC_SetAlarmInterval (WAKEUP_INTERVAL);
   EA = 1;                             // Enable global interrupts

   // Erased Flash locations are set to 0xFF. Check if calibration is required:
   if (CS_Switch_Cal_Threshold[0].U16 == 0xFFFF)
   {
      Calibrate ();
   }

   while (1)                           // Spin forever
   {
      // Task #1: Update the status of the switches

      CLKSEL = 0x24;                   // Select internal low-power osc.
                                       // divided by 4 as the system clock @ 5 MHz
      TouchSense_Update ();            // Update the status of the switches

      //-------------------

      // Task #2: Take action based on switch states
      // In this example, light up LEDs while switch is depressed

      // Also update the System_State variable:
      // In this example, when a switch is depressed, the device should not enter sleep mode
      // because the "action" is to have an LED lit. In other applications, this may not be necessary.

      CLKSEL = 0x04;                   // Select internal low-power osc.
                                       // divided by 1 as the system clock @ 20 MHz
      System_State = INACTIVE;

      for (i = 0; i < NUM_SWITCHES; i++)
      {
         if (CS_Switch_State[i] == SWITCH_PRESSED)
         {
            P1 &= ~LED_P1_Mask[i];
            System_State = ACTIVE;
         }
         else
         {
            P1 |= LED_P1_Mask[i];
         }
      }

      //-------------------

      // Task #3: If the system is inactive, enter sleep mode
      if (System_State == INACTIVE)
      {
         CPT0CN &= ~0x80;              // Disable the comparator

         PMU0CF = 0x20;                // Clear all wake-up flags
         PMU0CF = 0x8C;                // Enter SLEEP mode
                                       // Wake-up source: RTC
         // Power-up the comparator
         CPT0CN |= 0x80;               // Enable the comparator
         // Wait for comparator to settle
         Delay_Microseconds (COMPARATOR_POWER_UP_TIME_CNT);
         CPT0CN &= ~0x30;              // Clear the comparator edge flags

         // Task #4: On wake-up, check wake-up sources and take action
         if (PMU0CF & 0x08)
         {
            // RTC Oscillator Fail Event
            // Re-initialize oscillator
            RTC_Init ();
         }

         if (PMU0CF & 0x04)
         {
            // RTC Alarm Event
            // Proceed to CapTouchSense switch update
         }
      }
   }
}

//-----------------------------------------------------------------------------
// TouchSense_Update
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Updates the state of the Touch Sense switches.
//-----------------------------------------------------------------------------
void TouchSense_Update (void)
{
   UU16 timer_count_diff;
   U16 timer_count_a, timer_count_b;
   U8 i;

   for (i = 0; i < NUM_SWITCHES; i++)
   {
      CPT0MX = CS_Switch_Mux_Setting[i];

      // Prepare Timer2 for the first TouchSense reading
      TF2H = 0;                     // Clear Timer2 overflow flag
      ET2 = 1;                      // Enable Timer2 interrupts
      PCON |= IDLE;                 // Enter IDLE mode
      // Timer2 interrupt will occur when a comparator rising edge is captured
      // MCU will exit IDLE mode when the Timer2 interrupt occurs
      // Code in the Timer2 ISR will clear the TF2H overflow flag
      timer_count_a = TMR2RL;       // Capture count
      
      // Prepare Timer2 for the second TouchSense reading
      PCON |= IDLE;                 // Enter IDLE mode
      // Timer2 interrupt will occur when a comparator rising edge is captured
      // MCU will exit IDLE mode when the Timer2 interrupt occurs
      // Code in the Timer2 ISR will clear the TF2H overflow flag
      timer_count_b = TMR2RL;        // Capture count

      ET2 = 0;                      // Disable Timer2 interrupts

      // Calculate the oscillation period
      timer_count_diff.U16 = timer_count_b - timer_count_a;

      if (timer_count_diff.U16 > CS_Switch_Cal_Threshold[i].U16)
      {
         CS_Switch_State[i] = 0;
      }
      else
      {
         CS_Switch_State[i] = 1;
      }
   }
}

//-----------------------------------------------------------------------------
// TIMER2_ISR
//-----------------------------------------------------------------------------
//
// This ISR clears the Timer2 overflow flag.
//
//-----------------------------------------------------------------------------
INTERRUPT (TIMER2_ISR, INTERRUPT_TIMER2)
{
   TF2H = 0;                           // Clear overflow flag
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------