//=============================================================================
// Test_SmaRTClock.c
//=============================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//
// Target:
//    Si102x/3x
//
// IDE:
//    Silicon Laboratories IDE
//
// Tool Chains:
//    Keil
//    SDCC
//    Raisonance
//
// Project Name:
//    Si102x/3x Code Examples
//
// Release 1.0
//    - Initial Revision (MRF)
//    - 27 SEPTEMBER 2011
//
// Description:
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1020_defs.h>
#include "SmaRTClockSi1020.h"
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Main Prototypes
//-----------------------------------------------------------------------------
void main (void)
{
   U32 time;

   SFRPAGE = LEGACY_PAGE;
   PCA0MD  &= ~0x40;                   // disable watchdog timer

   RTC_Init();

   RTC_SetTimer(0x00001000);

   RTC_Write(RTC0CN, 0x90);           // run timer

   time = RTC_CaptureTimer();

   RTC_WriteAlarm(0,0x00002000);
   RTC_WriteAlarm(1,0x00003000);
   RTC_WriteAlarm(2,0x00004000);

   time = RTC_ReadAlarm(0);
   time = RTC_ReadAlarm(1);
   time = RTC_ReadAlarm(2);

   while(1);
}
//=============================================================================
