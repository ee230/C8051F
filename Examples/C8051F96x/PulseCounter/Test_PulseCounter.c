//=============================================================================
// test_PulseCounter.c
//=============================================================================
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//
// Target:
//    C8051F960
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
//    C8051F960 Code Examples
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
#include <C8051F960_defs.h>
#include "PulseCounter.h"
#include "SmaRTClockF960.h"
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Main Prototypes
//-----------------------------------------------------------------------------
void main (void)
{
   U8 value;
   U8 status;
   U32 time;
   U32 current;

   SFRPAGE = LEGACY_PAGE;
   PCA0MD  &= ~0x40;                   // disable watchdog timer

   RTC_Init();

   PC0_Init (PC0_DUAL_MODE, PC0_SAMPLING_RATE_2MS, 0);

   value = PC0_ReadPC0STAT();
   value = PC0_ReadPC0HIST();
   value = PC0_ReadPC0INT0();
   value = PC0_ReadPC0INT1();

   time = PC0_ReadCounter0 ();
   time = PC0_ReadCounter1 ();
   PC0_WriteComparator0 (0x123456);
   PC0_WriteComparator1 (0x789ABC);
   time = PC0_ReadComparator0 ();
   time = PC0_ReadComparator1 ();

   value = PC0_SetDebounceTimeHigh (2000);
   value = PC0_SetDebounceTimeLow (2000);
   value = PC0_SetDebounceTimeHigh (4000);
   value = PC0_SetDebounceTimeLow (4000);
   value = PC0_SetDebounceTimeHigh (5000);
   value = PC0_SetDebounceTimeLow (5000);


   current = PC0_SetPullUpCurrent (0);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (31);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (47);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (63);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (94);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (125);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (188);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (250);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (375);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (500);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (750);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (1000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (1500);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (2000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (3000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (4000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (6000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (8000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (12000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (16000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (24000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (32000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (48000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (64000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (96000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (128000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (192000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (250000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (375000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (500000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (750000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (1000000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (1500000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (2000000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (3000000);
   current = PC0_ReadPullUpCurrent ();
   current = PC0_SetPullUpCurrent (0);
   current = PC0_ReadPullUpCurrent ();

   status = PC0_AutoCalibrate (PC0_CAL_PC0);

   while(status);
   current = PC0_ReadPullUpCurrent ();

   while(1);
}
//=============================================================================
