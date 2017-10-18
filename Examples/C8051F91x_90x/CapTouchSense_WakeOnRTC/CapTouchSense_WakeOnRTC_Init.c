//-----------------------------------------------------------------------------
// CapTouchSense_WakeOnRTC_Init.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This file contains the peripheral initialization functions related to the 
// Capacitive TouchSense Switched with Wake-on-RTC implementation.
//
//
// Target:         C8051F931, C8051F921, C8051F912
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.2 
//    - Compiled and tested for Raisonance Toolchain (FB)
//    - 17 MAY 10
//
// Release 1.1 
//    - Compiled and tested for C8051F930-TB, C8051F912-TB,
//    - and TOOLSTICK TOUCHSENSE DC (JH)
//    - Port to C8051F912-TB from C8051F930-TB (JH)
//    - 06 JULY 2009
//
// Release 1.0
//    - Initial Revision (PKC)
//    - 20 MAY 2008
//    - Based on CapTouchSenseDC_Memory_Game.c (FB)
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

#if PCB == CAPTOUCHSENSEDC

   U8 CS_Switch_Mux_Setting[] =
      {
         0xC0, // Blue switch    - P0.0
         0x0C, // Green switch   - P0.1
         0xC5, // Red switch     - P1.2
         0x1C  // Yellow switch  - P0.3
      };

   U8 LED_P1_Mask[] =
      {
         0x01, // Blue LED    - P1.0
         0x02, // Green LED   - P1.1
         0x20, // Red LED     - P1.5
         0x40  // Yellow LED  - P1.6
      };

#elif PCB == C8051F930TB

   U8 CS_Switch_Mux_Setting[] =
      {
         0xC8, // Red switch     - P2.0
         0x8C  // Yellow switch  - P2.1
      };

   U8 LED_P1_Mask[] =
      {
         0x20, // Red LED     - P1.5
         0x40  // Yellow LED  - P1.6
      };
#elif PCB == C8051F912TB

   U8 CS_Switch_Mux_Setting[] =
      {
         0xC4, // Red switch     - P1.0
         0x4C  // Yellow switch  - P1.1
      };

   U8 LED_P1_Mask[] =
      {
         0x20, // Red LED     - P1.5
         0x40  // Yellow LED  - P1.6
      };

#endif

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal low-power
// oscillator with divide-by-4 and enables the missing clock detector.
//
//-----------------------------------------------------------------------------

void SYSCLK_Init (void)
{
   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor enabled.

   CLKSEL = 0x24;                      // Select internal low-power osc.
                                       // divided by 4 as the system clock

}

//-----------------------------------------------------------------------------
// Port_IO_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will initialize the Port I/O pins.
//
//-----------------------------------------------------------------------------
void PORT_IO_Init (void)
{

#if PCB == CAPTOUCHSENSEDC

   // P0.0  -  Skipped,     Open-Drain, Analog  (Cap Switch A - Blue)
   // P0.1  -  Skipped,     Open-Drain, Analog  (Cap Switch B - Green)
   // P0.2  -  Skipped,     Open-Drain, Digital (Push Button Switch, S2)
   // P0.3  -  Skipped,     Open-Drain, Analog  (Cap Switch D - Yellow)
   // P0.4  -  Unassigned,  Open-Drain, Digital
   // P0.5  -  Unassigned,  Open-Drain, Digital
   // P0.6  -  Unassigned,  Open-Drain, Digital
   // P0.7  -  Unassigned,  Open-Drain, Digital

   // P1.0  -  Skipped,     Open-Drain, Digital (Blue LED)
   // P1.1  -  Skipped,     Open-Drain, Digital (Green LED)
   // P1.2  -  Skipped,     Open-Drain, Analog  (Cap Switch C - Red)
   // P1.3  -  Unassigned,  Open-Drain, Digital
   // P1.4  -  Unassigned,  Open-Drain, Digital
   // P1.5  -  Skipped,     Open-Drain, Digital (Red LED)
   // P1.6  -  Skipped,     Open-Drain, Digital (Yellow LED)
   // P1.7  -  Unassigned,  Open-Drain, Digital

   // P2.0  -  Unassigned,  Open-Drain, Digital
   // P2.1  -  Unassigned,  Open-Drain, Digital
   // P2.2  -  Unassigned,  Open-Drain, Digital
   // P2.3  -  Unassigned,  Open-Drain, Digital
   // P2.4  -  Unassigned,  Open-Drain, Digital
   // P2.5  -  Unassigned,  Open-Drain, Digital
   // P2.6  -  Unassigned,  Open-Drain, Digital
   // P2.7  -  Skipped,     Open-Drain, Digital

   DC0CN = 0x06;                       // Set Supply Voltage to 3.0 Volts

   P0MDIN    = 0xF4;
   P1MDIN    = 0xFB;
   P0SKIP    = 0x0F;
   P1SKIP    = 0x67;
   XBR2      = 0x40;

#elif PCB == C8051F930TB

   // P0.0  -  Unassigned,  Open-Drain, Digital
   // P0.1  -  Unassigned,  Open-Drain, Digital
   // P0.2  -  Skipped,     Open-Drain, Digital (Push Button Switch, SW2)
   // P0.3  -  Skipped,     Open-Drain, Digital (Push Button Switch, SW3)
   // P0.4  -  Unassigned,  Open-Drain, Digital
   // P0.5  -  Unassigned,  Open-Drain, Digital
   // P0.6  -  Unassigned,  Open-Drain, Digital
   // P0.7  -  Unassigned,  Open-Drain, Digital

   // P1.0  -  Unassigned,  Open-Drain, Digital
   // P1.1  -  Unassigned,  Open-Drain, Digital
   // P1.2  -  Unassigned,  Open-Drain, Digital
   // P1.3  -  Unassigned,  Open-Drain, Digital
   // P1.4  -  Unassigned,  Open-Drain, Digital
   // P1.5  -  Skipped,     Open-Drain, Digital (Red LED)
   // P1.6  -  Skipped,     Open-Drain, Digital (Yellow LED)
   // P1.7  -  Unassigned,  Open-Drain, Digital

   // P2.0  -  Skipped,     Open-Drain, Analog  (Cap Switch 0)
   // P2.1  -  Skipped,     Open-Drain, Analog  (Cap Switch 1)
   // P2.2  -  Unassigned,  Open-Drain, Digital
   // P2.3  -  Unassigned,  Open-Drain, Digital
   // P2.4  -  Unassigned,  Open-Drain, Digital
   // P2.5  -  Unassigned,  Open-Drain, Digital
   // P2.6  -  Unassigned,  Open-Drain, Digital
   // P2.7  -  Skipped,     Open-Drain, Digital

   DC0CN = 0x06;                       // Set Supply Voltage to 3.0 Volts

   P2MDIN    = 0xFC;
   P0SKIP    = 0x0C;
   P1SKIP    = 0x60;
   P2SKIP    = 0x83;
   XBR2      = 0x40;

#elif PCB == C8051F912TB

   // P0.0  -  Unassigned,  Open-Drain, Digital
   // P0.1  -  Unassigned,  Open-Drain, Digital
   // P0.2  -  Skipped,     Open-Drain, Digital (Push Button Switch, SW2)
   // P0.3  -  Skipped,     Open-Drain, Digital (Push Button Switch, SW3)
   // P0.4  -  Unassigned,  Open-Drain, Digital
   // P0.5  -  Unassigned,  Open-Drain, Digital
   // P0.6  -  Unassigned,  Open-Drain, Digital
   // P0.7  -  Unassigned,  Open-Drain, Digital

   // P1.0  -  Skipped,     Open-Drain, Analog  (Cap Switch 0)
   // P1.1  -  Skipped,     Open-Drain, Analog  (Cap Switch 1)
   // P1.2  -  Unassigned,  Open-Drain, Digital
   // P1.3  -  Unassigned,  Open-Drain, Digital
   // P1.4  -  Unassigned,  Open-Drain, Digital
   // P1.5  -  Skipped,     Open-Drain, Digital (Red LED)
   // P1.6  -  Skipped,     Open-Drain, Digital (Yellow LED)
   // P1.7  -  Unassigned,  Open-Drain, Digital

   // P2.0  -  Unassigned,  Open-Drain, Digital
   // P2.1  -  Unassigned,  Open-Drain, Digital
   // P2.2  -  Unassigned,  Open-Drain, Digital
   // P2.3  -  Unassigned,  Open-Drain, Digital
   // P2.4  -  Unassigned,  Open-Drain, Digital
   // P2.5  -  Unassigned,  Open-Drain, Digital
   // P2.6  -  Unassigned,  Open-Drain, Digital
   // P2.7  -  Skipped,     Open-Drain, Digital

   DC0CN = 0x06;                       // Set Supply Voltage to 3.0 Volts

   P1MDIN    = 0xFC;
   P0SKIP    = 0x0C;
   P1SKIP    = 0x63;
   XBR2      = 0x40;

#endif

}

//-----------------------------------------------------------------------------
// TouchSense_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Comparator 0 and Timer 2 for use with Touchsense.
//-----------------------------------------------------------------------------
void TouchSense_Init (void)
{
   // Initialize Comparator0
   CPT0MX = CS_Switch_Mux_Setting[0];  // Positive Mux: TouchSense Compare
                                       // Negative Mux: First TouchSense Switch


   CPT0CN = 0x8F;                      // Enable Comparator0; clear flags
                                       // select maximum hysterisis
   CPT0MD = 0x0F;                      // Comparator interrupts disabled,
                                       // lowest power mode

   // Wait for comparator to settle
   Delay_Microseconds (COMPARATOR_POWER_UP_TIME_CNT);
   CPT0CN &= ~0x30;                    // Clear the comparator edge flags

   // Initialize Timer2
   CKCON |= 0x10;                      // Timer2 counts system clocks
   TMR2CN = 0x12;                      // Capture mode enabled, capture
                                       // trigger is Comparator0.
                                       
   TR2 = 1;                            // Start Timer2
}

//-----------------------------------------------------------------------------
// Delay_Microseconds
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : U16 count - Timer3 count value.
//
// Delays the specified number of microseconds.
//-----------------------------------------------------------------------------
void Delay_Microseconds (U16 count)
{  
   CKCON |= 0x40;    // Timer3 clocked by SYSCLK
   TMR3CN = 0x00;    // Timer3 clocked by SYSCLK
                     // Clear TF3H
   TMR3 = -count;
   TMR3CN |= 0x04;   // Start Timer3
   while ((TMR3CN & 0x80) == 0); // Wait till TF3H overflow is set
}

// NOTE:
// The RTC_Init function is defined in the file "CapTouchSense_WakeOnRTC_RTC.c"

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------