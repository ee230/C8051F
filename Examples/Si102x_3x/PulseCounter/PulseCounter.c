//=============================================================================
// PulseCounter.c
//=============================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This file contains the Pulse Counter example code for the Si102x/3x.
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
//    - 27 OCTOBER 2011
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1020_defs.h>
#include "PulseCounter.h"
//-----------------------------------------------------------------------------
// SBIT definitions for PCO & PC1
//-----------------------------------------------------------------------------
SBIT (PC0, SFR_P1, 0);
SBIT (PC1, SFR_P1, 1);
//-----------------------------------------------------------------------------
// Code constants = Used for the pull-up look-up table
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (PullUpResistorCurrent[8], U32, SEG_CODE) =
{
   0,
   1000,
   4000,
   16000,
   64000,
   256000,
   1000000,
   4000000
};

const SEGMENT_VARIABLE (PullUpDutyEigths[4], U8, SEG_CODE) =
{
   2,
   3,
   4,
   6
};


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
U8 calcDebounce (U32 time);
//-----------------------------------------------------------------------------
// PulseCounterInit ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will initialize the PulseCounter.
// All values are calculated using preprocessor macros defined in the header
// file.
//
// Run-time functions for setting the debounce and pull-up strenght are
// also provided.
//
//-----------------------------------------------------------------------------
void PC0_Init (U8 mode, U8 sampleRate, U8 flutterOptions)
{
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   // Set mode according to parameters
   // writing to this sfr resets PC0
   PC0MD = (mode|sampleRate|flutterOptions);

   // Default thresholds are 40/60 %
   PC0TH = PC0_THRESHI_60|PC0_THRESLO_40;

   SFRPAGE = restore;
}
//=============================================================================
// Reads using the Read Valid Bit
//=============================================================================
//-----------------------------------------------------------------------------
// PC0_ReadPC0STAT()
//-----------------------------------------------------------------------------
U8    PC0_ReadPC0STAT(void)
{
   U8 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   do
   {
      value = PC0STAT;                 // Attempt to read
   } while((PC0TH&0x01)==0);           // repeat if invalid.

   SFRPAGE = restore;
   return value;
}
//-----------------------------------------------------------------------------
// PC0_ReadPC0HIST()
//-----------------------------------------------------------------------------
U8    PC0_ReadPC0HIST(void)
{
   U8 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   do
   {
      value = PC0HIST;                 // Attempt to read
   } while((PC0TH&0x01)==0);           // repeat if invalid.

   SFRPAGE = restore;
   return value;
}
//-----------------------------------------------------------------------------
// PC0_ReadPC0INT0()
//-----------------------------------------------------------------------------
U8    PC0_ReadPC0INT0(void)
{
   U8 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   do
   {
      // Attempt to read
      value = PC0INT0;
   } while((PC0TH&0x01)==0);           // repeat if invalid.

   SFRPAGE = restore;
   return value;
}
//-----------------------------------------------------------------------------
// PC0_ReadPC0INT1()
//-----------------------------------------------------------------------------
U8    PC0_ReadPC0INT1(void)
{
   U8 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   do
   {
      // Attempt to read
      value = PC0INT1;
   } while((PC0TH&0x01)==0);           // repeat if invalid.

   SFRPAGE = restore;
   return value;
}
//-----------------------------------------------------------------------------
// PC0_ReadCounter0()
//-----------------------------------------------------------------------------
U32  PC0_ReadCounter0 (void)
{
   UU32 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   do
   {
      // Attempt to read
      value.U8[b0] = PC0CTR0L;
   } while((PC0TH&0x01)==0);           // repeat if invalid.

   // OK to read M & H bytes
   value.U8[b1] = PC0CTR0M;
   value.U8[b2] = PC0CTR0H;
   value.U8[b3] = 0;

   SFRPAGE = restore;
   return value.U32;
}
//-----------------------------------------------------------------------------
// PC0_ReadCounter1()
//-----------------------------------------------------------------------------
U32  PC0_ReadCounter1 (void)
{
   UU32 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   do
   {
      // Attempt to read
      value.U8[b0] = PC0CTR1L;
   } while((PC0TH&0x01)==0);           // repeat if invalid.

   // OK to read M & H bytes
   value.U8[b1] = PC0CTR1M;
   value.U8[b2] = PC0CTR1H;
   value.U8[b3] = 0;

   SFRPAGE = restore;
   return value.U32;
}
//=============================================================================
// Write Comparator functions
//=============================================================================
//-----------------------------------------------------------------------------
// PC0_WriteComparator1()
//-----------------------------------------------------------------------------
void  PC0_WriteComparator0 (U32 count)
{
   UU32 value;
   U8 restore;

   restore = SFRPAGE;
   value.U32 = count;

   SFRPAGE = DPPE_PAGE;

   PC0CMP0H = value.U8[b2];
   PC0CMP0M = value.U8[b1];

   // Low byte must be written last
   PC0CMP0L = value.U8[b0];

   SFRPAGE = restore;
}
//-----------------------------------------------------------------------------
// PC0_WriteComparator1()
//-----------------------------------------------------------------------------
void  PC0_WriteComparator1 (U32 count)
{
   UU32 value;
   U8 restore;

   restore = SFRPAGE;
   value.U32 = count;

   SFRPAGE = DPPE_PAGE;

   PC0CMP1H = value.U8[b2];
   PC0CMP1M = value.U8[b1];

   // Low byte must be written last
   PC0CMP1L = value.U8[b0];

   SFRPAGE = restore;
}
//=============================================================================
// Read Comparator functions
//=============================================================================
//-----------------------------------------------------------------------------
// PC0_ReadComparator0()
//-----------------------------------------------------------------------------
U32  PC0_ReadComparator0 (void)
{
   UU32 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   // The order of this multi-byte read does not matter.
   value.U8[b0] = PC0CMP0L;
   value.U8[b1] = PC0CMP0M;
   value.U8[b2] = PC0CMP0H;
   value.U8[b3] = 0;

   SFRPAGE = restore;
   return value.U32;
}
//-----------------------------------------------------------------------------
// PC0_ReadComparator1()
//-----------------------------------------------------------------------------
U32  PC0_ReadComparator1 (void)
{
   UU32 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   // The order of this multi-byte read does not matter.
   value.U8[b0] = PC0CMP1L;
   value.U8[b1] = PC0CMP1M;
   value.U8[b2] = PC0CMP1H;
   value.U8[b3] = 0;

   SFRPAGE = restore;
   return value.U32;
}
//=============================================================================
// SetDebounce functions
//=============================================================================
//-----------------------------------------------------------------------------
// SetDebounceTimeHigh()
//
// This function will set the debounce time based on a parameter in
// microseconds. It will set the sfr and return the sfr value.
//
// The PC0_Init() function should be called first to set the sample rate.
//
//-----------------------------------------------------------------------------
U8 PC0_SetDebounceTimeHigh (U32 time)
{
   U32 value;
   U8 restore;

   restore = SFRPAGE;
   if(time==0)                         // Check for zero
   {
      PC0DCL = 0;                      // Disable debounce
      return 0;
   }

   value = calcDebounce(time);

   SFRPAGE = DPPE_PAGE;
   PC0DCH = value;

   SFRPAGE = restore;
   return value;
}
//-----------------------------------------------------------------------------
// SetDebounceTimeLow()
//
// This function will set the debounce time based on a parameter in
// microseconds. It will set the sfr and return the sfr value.
//
// The PC0_Init() function should be called first to set the sample rate.
//
//-----------------------------------------------------------------------------
U8 PC0_SetDebounceTimeLow (U32 time)
{
   U32 value;
   U8 restore;

   restore = SFRPAGE;
   if(time==0)                         // Check for zero
   {
      PC0DCL = 0;                      // Disable debounce
      return 0;
   }

   value = calcDebounce(time);

   SFRPAGE = DPPE_PAGE;
   PC0DCL = value;

   SFRPAGE = restore;
   return value;
}

//-----------------------------------------------------------------------------
// calcDebounce()
//
// This function will calcualte the debounce setting based on a parameter in
// microseconds. It is called by both PC0_SetDebounceTimeLow and
// PC0_SetDebounceTimeLow.
//
// The PC0_Init() function should be called first to set the sample rate.
//
//-----------------------------------------------------------------------------

U8 calcDebounce (U32 time)
{
   U8 rate;
   U32 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   rate = ((PC0MD>>4)&0x03);           // Get rate from PC0MD sfr.

   value = time;
   value += 125;                       // add 1/2 of divisor for rounding
   value /= 250;                       // Divide by 250 us.
   value >>= rate;                     // Divide by 2^rate
   value--;                            // Minus one

   if(value==0)                        // Check for zero resultant.
      value = 1;                       // Min active debounce is 1 sample

   if(value>255)                       // Check for 8-bit overflow
      value = 255;                     // Saturate to 255

   SFRPAGE = restore;
   return value;
}
//=============================================================================
// Set PullUp Current functions
//=============================================================================
//-----------------------------------------------------------------------------
// PC0_SetPullUpCurrent()
//
// This function will find the appropriate pull sfr setting for a given
// pull-up current in nA. This function uses a reverse table search.
// So it is not fast. It will find the first value equal to or greater
// than the specified value.
//
// This function will return all F's if the current is higher than possible
// at the current sampling rate.
//
// The PC0_Init() function should be called first to set the sample rate.
//
//-----------------------------------------------------------------------------
U32 PC0_SetPullUpCurrent (U32 current)
{
   U8 rate, resistor, duty, pullupConfig;
   U32 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   if(current==0)
   {
      PC0PCF = 0;
      SFRPAGE = restore;
      return 0;
   }

   rate = ((PC0MD>>4)&0x03);

   for(resistor=1;resistor<8;resistor++)
   {
      for(duty=0;duty<4;duty++)
      {
         value = PullUpResistorCurrent[resistor];
         value *= PullUpDutyEigths[duty];
         value >>= 3;                  // divide by 8
         if(rate!=0)
         {
            // divide by 2^rate with rounding
            // add 1/2 of divisor
            // or 2^(rate-1)
            value += (1<<(rate-1));
            // then divide by 2^rate
            value >>= rate;
         }

         if(value >= current)
         {
            pullupConfig = ((resistor<<2) + duty);
            PC0PCF = pullupConfig;
            SFRPAGE = restore;
            return value;
         }
      }
   }
   SFRPAGE = restore;
   return 0xFFFFFFFF;
}
//-----------------------------------------------------------------------------
// PC0_ReadPullUpCurrent()
//
// This function will return the approximate pull-up current in nA based on
// the current sfr settings. This function can be used to read the current
// after a calibration.
//
// The PC0_Init() function should be called first to set the sample rate.
//
//-----------------------------------------------------------------------------
U32 PC0_ReadPullUpCurrent (void)
{
   U8 pullUpConfig, sampleRate, resistor, duty;
   U32 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;
   pullUpConfig = PC0PCF;

   resistor = ((pullUpConfig>>2)&0x07);

   if(resistor==0)
   {
      SFRPAGE = restore;
      return 0;
   }

   duty = (pullUpConfig & 0x03);

   sampleRate = ((PC0MD>>4)&0x03);

   value = PullUpResistorCurrent[resistor];

   value *= PullUpDutyEigths[duty];

   value >>= 3;                        // divide by 8

   if(sampleRate!=0)
   {
      // divide by 2^sampleRate with rounding
      // add 1/2 of divisor
      // or 2^(sampleRate-1)
      value += (1<<(sampleRate-1));
      // then divide by 2^sampleRate
      value >>= sampleRate;
   }

   SFRPAGE = restore;
   return value;
}
//=============================================================================
// Set PullUp Current functions
//=============================================================================
U8 PC0_AutoCalibrate (U8 channel)
{
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;
   // calibrate PC0
   PC0PCF = PC0_CAL_START|channel;

   //wait for calibration
   while(PC0PCF & PC0_CAL_BUSY);

   if(PC0PCF & PC0_CAL_PASS)
   {
      PC0PCF +=1;                      // Add one to be conservative
      SFRPAGE = restore;
      return 0;                        // return success.
   }
   else
   {
      SFRPAGE = restore;
      return 1;
   }
}

//-----------------------------------------------------------------------------

//=============================================================================
