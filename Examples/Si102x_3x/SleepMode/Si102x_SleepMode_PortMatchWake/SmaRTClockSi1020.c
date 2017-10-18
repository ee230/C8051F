//=============================================================================
// SmaRTClockSi1020.c
//=============================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This file contains RTC code for the Si102x/3x.
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
//    Si102x/3x Example Code
//
// Release 1.0
//    - Initial Revision (MRF)
//    - 28 SEPTEMBER 2011
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
// RTC_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will initialize the RTC.
//
//-----------------------------------------------------------------------------
void RTC_Init (void)
{
   SFRPAGE = LEGACY_PAGE;

   // Unlocking the interface is not necessary for the 'F960.

   // configure using low-power LFO (lowest power option)

   RTC_Write (RTC0XCN, 0x08);          // LFO mode

   RTC_Write (RTC0XCF, 0x06);          // load capacitance to 12 pF

   RTC_Write (RTC0CN, 0x80);           // Enable smaRTClock

}
//-----------------------------------------------------------------------------
// RTC_Write ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : reg, value
//
// This function will Write one byte from the specified RTC register.
// Using a register number greater that 0x0F is not permited,
//
// This function uses the new F960 fast write mode.
//
//-----------------------------------------------------------------------------
void RTC_Write (unsigned char reg, unsigned char value)
{
   reg &= 0x0F;                        // mask low nibble
   SFRPAGE = LEGACY_PAGE;
   RTC0ADR  = reg;                     // pick register
   RTC0DAT = value;                    // write data
}
//-----------------------------------------------------------------------------
// RTC_Read ()
//-----------------------------------------------------------------------------
//
// Return Value : RTC0DAT
// Parameters   : reg
//
// This function will read one byte from the specified RTC register.
// Using a register number greater that 0x0F is not permited,
//
//
// This function uses the new F960 fast read mode.
//
//-----------------------------------------------------------------------------
unsigned char RTC_Read (U8 reg)
{
   reg &= 0x0F;                        // mask low nibble
   SFRPAGE = LEGACY_PAGE;
   RTC0ADR  = (reg |0x80);             // write address setting READ bit
   return RTC0DAT;                     // return value
}
//=============================================================================
// Timer Set and Capture Functions using new F960 fast mode.
//-----------------------------------------------------------------------------
// RTC_SetTimer  ()
//-----------------------------------------------------------------------------
//
// Parameters   : U32 Timer set time
// Return Value : U8  returns 0xFF for error.
//
// This function will write to the Capture registers and set the timer.
//
//-----------------------------------------------------------------------------
U8 RTC_SetTimer(U32 time)
{
   UU32 value;
   U8 control;

   value.U32 = time;

   SFRPAGE = LEGACY_PAGE;
   control = RTC_Read(RTC0CN);

   if((control&0x80)==0)
      return 0xFF;                     // error RTC must be enabled

   // write using auto-increment
   RTC0ADR = 0x00;
   RTC0DAT = value.U8[b0];
   RTC0DAT = value.U8[b1];
   RTC0DAT = value.U8[b2];
   RTC0DAT = value.U8[b3];

   RTC_Write(RTC0CN, (control|0x06));  // set using F960 fast mode

   // wait for set bit to go to zero
   while(((RTC_Read(RTC0CN))&0x02)==0x02);

   return 0;
}
//-----------------------------------------------------------------------------
// RTC_CaptureTimer  ()
//-----------------------------------------------------------------------------
//
// Return Value : U32 capture time
// Parameters   : none
//
// This function will read the 32-bit capture value.
//
//-----------------------------------------------------------------------------
U32 RTC_CaptureTimer (void)
{
   UU32 timer;
   U8 control;

   SFRPAGE = LEGACY_PAGE;

   control = RTC_Read(RTC0CN);

   if((control&0x80)==0)
      return 0xFFFF;                   // error RTC must be enabled

   RTC_Write(RTC0CN, (control|0x05));  // capture using F960 fast mode

   // wait for capture bit to go to zero
   while(((RTC_Read(RTC0CN))&0x01)==0x01);

   // read using auto-increment
   RTC0ADR = (0x80 | CAPTURE0);        // read CAPTURE0
   timer.U8[b0]= RTC0DAT;
   timer.U8[b1]= RTC0DAT;
   timer.U8[b2]= RTC0DAT;
   timer.U8[b3]= RTC0DAT;

   return timer.U32;
}
//-----------------------------------------------------------------------------
// RTC_WriteAlarm  ()
//-----------------------------------------------------------------------------
//
// Parameters   : U32 Alarm time
// Parameters   : U8 alarm  0, 1, or 2
//
// This function will write to the alarm, but does not enable it.
//
//-----------------------------------------------------------------------------
void RTC_WriteAlarm(U8 alarm, U32 time)
{
   UU32 value;

   value.U32 = time;

   SFRPAGE = LEGACY_PAGE;

   // wrte using auto-increment
   RTC0ADR = ALARM0B0 + (alarm<<2);
   RTC0DAT = value.U8[b0];
   RTC0DAT = value.U8[b1];
   RTC0DAT = value.U8[b2];
   RTC0DAT = value.U8[b3];
}
//-----------------------------------------------------------------------------
// RTC_ReadAlarm  ()
//-----------------------------------------------------------------------------
//
// Return Value : U32 Alarm time
// Parameters   : U8 alarm  0, 1, or 2
//
// This function is provided if you want to read the current alarm value and
// modify the results.
//
//-----------------------------------------------------------------------------
U32 RTC_ReadAlarm (U8 alarm)
{
   UU32 time;
   U8 addr;

   addr = ALARM0B0 + (alarm<<2);

   SFRPAGE = LEGACY_PAGE;

   // read using auto-increment
   RTC0ADR = (0x80 | addr);          // read byte 0 of ALARMn
   time.U8[b0]= RTC0DAT;
   time.U8[b1]= RTC0DAT;
   time.U8[b2]= RTC0DAT;
   time.U8[b3]= RTC0DAT;

   return time.U32;
}


//=============================================================================
