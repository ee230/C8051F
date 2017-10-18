//=============================================================================
// PulerCounter.c
//=============================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This file contains the Pulse Counter example code for the F960.
//
// Target:
//    C8051F96x.
//
// IDE:
//    Silicon Laboratories IDE   version 3.3
//
// Tool Chains:
//    Keil
//       c51.exe     version 8.0.8
//       bl51.exe    version 6.0.5
//    SDCC
//       sdcc.exe    version 2.8.0
//       aslink.exe  version 1.75
//
// Project Name:
//    C8051F960 Example Code
//
// Beta Release
//    - TBD
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <c8051F960_defs.h>
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
//=============================================================================
// Function Prototypes
//=============================================================================
U8 calcDebounce (U32 time);
//=============================================================================
// API Functions Function
//=============================================================================
//-----------------------------------------------------------------------------
// PulseCounterInit ()
//
// Return Value : None
// Parameters   : U8 mode, U8 sampleRate, U8 flutterOptions
//
// This function will initialize the PulseCounter.
//
// Writing to the PC0MD SFR will clear the counters. This function will
// configure the PC0MD SFR according to the function parameters.
//
// The PC0TH SFR is set to 40/60% by default.
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
//
// Return Value : U8 value
// Parameters   : none
//
// This accessor function will read the PC0STAT SFR.
//
// This real-time SFR read must be qualified by reading the read valid bit.
//
//-----------------------------------------------------------------------------
U8    PC0_ReadPC0STAT(void)
{
   U8 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   do
   {
     // Attempt to read
     value = PC0STAT;
      // repeat if invalid.
   } while((PC0TH&PC0TH_READ_VALID_MASK)==0);

   SFRPAGE = restore;
   return value;
}
//-----------------------------------------------------------------------------
// PC0_ReadPC0HIST()
//
// Return Value : U8 value
// Parameters   : none
//
// This accessor function will read the PC0STAT SFR.
//
// This real-time SFR read must be qualified by reading the read valid bit.
//
//-----------------------------------------------------------------------------
U8    PC0_ReadPC0HIST(void)
{
   U8 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = DPPE_PAGE;

   do
   {
      // Attempt to read
      value = PC0HIST;
      // repeat if invalid.
   } while((PC0TH&PC0TH_READ_VALID_MASK)==0);

   SFRPAGE = restore;
   return value;
}
//-----------------------------------------------------------------------------
// PC0_ReadPC0INT0()
//
// Return Value : U8 value
// Parameters   : none
//
// This accessor function will read the PC0INT0 SFR.
//
// This real-time SFR read must be qualified by reading the read valid bit.
//
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
      // repeat if invalid.
   } while((PC0TH&PC0TH_READ_VALID_MASK)==0);
   SFRPAGE = restore;
   return value;
}
//-----------------------------------------------------------------------------
// PC0_ReadPC0INT1()
//
// Return Value : U8 value
// Parameters   : none
//
// This accessor function will read the PC0INT1 SFR.
//
// This real-time SFR read must be qualified by reading the read valid bit.
//
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
      // repeat if invalid.
   } while((PC0TH&PC0TH_READ_VALID_MASK)==0);

   SFRPAGE = restore;
   return value;
}
//-----------------------------------------------------------------------------
// PC0_ReadCounter0()
//
// Return Value : U32 value
// Parameters   : none
//
// This accessor function will read the PC0CTR0x SFRs.
//
// This real-time SFR read must be qualified by reading the read valid bit.
//
// The least significant byte must be read first and qualified with the read
// valid bit. This latches the middle and high bytes. A 32-bit union is used
// to assemble the 32-bit value.
//
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
      // repeat if invalid.
   } while((PC0TH&PC0TH_READ_VALID_MASK)==0);

   // OK to read M & H bytes
   value.U8[b1] = PC0CTR0M;
   value.U8[b2] = PC0CTR0H;
   value.U8[b3] = 0;

   SFRPAGE = restore;
   return value.U32;
}
//-----------------------------------------------------------------------------
// PC0_ReadCounter1()
//
// Return Value : U32 value
// Parameters   : none
//
// This accessor function will read the PC0CTR1x SFRs.
//
// This real-time SFR read must be qualified by reading the read valid bit.
//
// The least significant byte must be read first and qualified with the read
// valid bit. This latches the middle and high bytes. A 32-bit union is used
// to assemble the 32-bit value.
//
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
      // repeat if invalid.
   } while((PC0TH&PC0TH_READ_VALID_MASK)==0);

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
// PC0_WriteComparator0()
//
// Return Value : none
// Parameters   : U32 value
//
// This accessor function will write to the PC0CMP0x SFRs.
//
// A 32-bit union is used to decompose the 32-bit value.
//
// The low-byte must be written last.
//
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
//
// Return Value : none
// Parameters   : U32 value
//
// This accessor function will write to the PC0CMP1x SFRs.
//
// A 32-bit union is used to decompose the 32-bit value.
//
// The low-byte must be written last.
//
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
//
// Return Value : U32 value
// Parameters   : none
//
// This accessor function will read the PC0CMP0x SFRs.
//
// These SFRs do not require any specific access method. But this function is
// included for completeness.
//
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
//
// Return Value : U32 value
// Parameters   : none
//
// This accessor function will read the PC0CMP1x SFRs.
//
// These SFRs do not require any specific access method. But this function is
// included for completeness.
//
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
// Return Value : U32 time (microseconds)
// Parameters   : U8 PC0DCH value
//
// This function will set the debounce time based on a parameter in
// microseconds. It will set the sfr and return the sfr value.
//
// The PC0_Init() function should be called first to set the sample rate.
//
// This function assumes the RTC is using a 16 kHz crystal.
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
// Return Value : U32 time
// Parameters   : none
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
//-----------------------------------------------------------------------------
// PC0_AutoCalibrate()
//
// This function will perform an automatic calibration on the selected channel.
//
// The PC0_Init() function should be called first to set the sample rate.
//
// The auto-calibrate will fail if the switch is closed when this function
// is called.
//
// If auto-calibrations are performed on both channels, there should be a
// delay of at least 2 RTC clocks between calibrations.
//
// Note that the auto-calibrate will find the lowest value that will pull
// up to the threshold value. This value may be incremented using the
// PC0_ReadPullUpCurrent and PC0_SetPullUpCurrent functions.
//
// This will increment the current by 300 nA.
// PC0_SetPullUpCurrent(PC0_ReadPullUpCurrent()+300);
//
//-----------------------------------------------------------------------------
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
//=============================================================================
// Local (non-API) functions
//=============================================================================
//-----------------------------------------------------------------------------
// calcDebounce()
//
// This function will calculate the debounce setting based on a parameter in
// microseconds. It is called by both PC0_SetDebounceTimeLow and
// PC0_SetDebounceTimeLow. It is not an API function.
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
// End
//=============================================================================
