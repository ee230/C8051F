//-----------------------------------------------------------------------------
// CapTouchSense_WakeOnRTC_Calibrate.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This file contains functions related to the calibration/configuration of 
// the Capacitive TouchSense switches with Wake on RTC.
//
//
// Target:         C8051F931, C8051F921
// Tool chain:     Generic
// Command Line:   None
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

void Calibrate (void)
{
   U8 i, CLKSEL_save;
   UU16 timer_count_diff;
   U16 timer_count_a, timer_count_b;

   // Erase Flash page that will hold the calibration values
   FLASH_PageErase (CAL_CONSTANTS_PAGE_ADDR, 0);

   // Indicate calibration required
   RED_LED = 0;

   while (SW2 == SWITCH_IDLE); // Wait till user presses S2 switch

   RED_LED = 1;

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

      // Add standard threshold and subtract SWITCH_SENSITIVITY
      timer_count_diff.U16 = timer_count_diff.U16 + SWITCH_STANDARD_THRESHOLD - SWITCH_SENSITIVITY;

      // Store calibration threshold in non-volatile FLASH memory
      FLASH_ByteWrite (((U16)(CS_Switch_Cal_Threshold) + MSB + i*2), timer_count_diff.U8[MSB], 0);
      FLASH_ByteWrite (((U16)(CS_Switch_Cal_Threshold) + LSB + i*2), timer_count_diff.U8[LSB], 0);

      P1 &= ~LED_P1_Mask[i];
   }

   // Wait some time to allow enough time for user to notice the lit LEDs
   CLKSEL_save = CLKSEL;
   CLKSEL = 0x74;                     // Set system clock divisor to 128
   for (timer_count_diff.U16 = 0; timer_count_diff.U16 < 0x1FFF; timer_count_diff.U16++);
   CLKSEL = CLKSEL_save;               // Restore original clock setting
}

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of the byte to write to
//                    valid range is from 0x0000 to 0xFBFF for 64K devices
//                    valid range is from 0x0000 to 0x7FFF for 32K devices
//   2) char byte - byte to write to Flash.
//
// This routine writes <byte> to the linear FLASH address <addr>.
//-----------------------------------------------------------------------------
void FLASH_ByteWrite (U16 addr, U8 byte, U8 SFLE)
{
   U8 EA_Save = IE;                    // Preserve EA
   SEGMENT_VARIABLE_SEGMENT_POINTER (pwrite, U8, xdata, data); 
   //unsigned char xdata * data pwrite;// FLASH write pointer

   EA = 0;                             // Disable interrupts

   VDM0CN = 0x80;                      // Enable VDD monitor


   RSTSRC = 0x06;                      // Enable VDD monitor as a reset source
                                       // Leave missing clock detector enabled

   pwrite = (char xdata *) addr;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x01;                      // PSWE = 1
   
   if(SFLE)
   {
      PSCTL |= 0x04;                   // Access Scratchpad
   }

   VDM0CN = 0x80;                      // Enable VDD monitor


   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   *pwrite = byte;                     // Write the byte

   PSCTL &= ~0x05;                     // SFLE = 0; PSWE = 0

   if ((EA_Save & 0x80) != 0)          // Restore EA
   {
      EA = 1;
   }

}



//-----------------------------------------------------------------------------
// FLASH_PageErase
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of any byte in the page to erase
//                    valid range is from 0x0000 to 0xFBFF for 64K devices
//                    valid range is from 0x0000 to 0x7FFF for 32K devices
//
// This routine erases the FLASH page containing the linear FLASH address
// <addr>.  Note that the page of Flash containing the Lock Byte cannot be
// erased if the Lock Byte is set.
//
//-----------------------------------------------------------------------------
void FLASH_PageErase (U16 addr, U8 SFLE)
{
   U8 EA_Save = IE;                    // Preserve EA
   
   SEGMENT_VARIABLE_SEGMENT_POINTER (pwrite, U8, xdata, data); 
   //unsigned char xdata * data pwrite;// FLASH write pointer

   EA = 0;                             // Disable interrupts

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x06;                      // Enable VDD monitor as a reset source
                                       // Leave missing clock detector enabled

   pwrite = (char xdata *) addr;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1
   
   if(SFLE)
   {
      PSCTL |= 0x04;                   // Access Scratchpad
   }

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
   *pwrite = 0;                        // Initiate page erase

   PSCTL &= ~0x07;                     // SFLE = 0; PSWE = 0; PSEE = 0

   if ((EA_Save & 0x80) != 0)          // Restore EA
   {
      EA = 1;
   }
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------