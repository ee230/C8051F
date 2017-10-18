//=============================================================================
// lcdConfig.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
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
//    Si102x/3x LCD example
//
// Release 1.0
//    - Initial Revision (MRF)
//    - 29 SEPTEMBER 2011
//
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//
// This file is written using 8051 compiler independent code.
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1020_defs.h>
#include "lcdConfig.h"
//-----------------------------------------------------------------------------
// LCD0_Config ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will configure the LCD for the different modes.
//
//-----------------------------------------------------------------------------
void LCD0_Config(U8 muxMode, U8 segments, U8 contrastMode, U16 contrast, U8 refresh, U8 toggle)
{
   LCD0_ConfigClear();

   LCD0_ConfigMuxMode(muxMode);

   LCD0_ConfigContrastMode(contrastMode);

   LCD0_SetContrast(contrast);

   LCD0_ConfigSegments(segments);

   LCD0_ConfigRefresh(refresh, muxMode);

   LCD0_ConfigToggle(toggle, refresh, muxMode);

   LCD0_ConfigEnable();
}
//-----------------------------------------------------------------------------
// LCD0_ConfigClear ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will configure the LCD for the different modes.
//
//-----------------------------------------------------------------------------
void LCD0_ConfigClear(void)
{
   U8 restore;
   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;

   SFRPAGE = LCD0_PAGE;

   LCD0MSCN = 0x00;                    // Clear LCD0MSCN
   LCD0MSCN = 0x02;                    // Reset LCD Data registers
   LCD0MSCN = 0x00;                    // Clear reset bit
   LCD0CN   = 0x00;                    // Clear LCD0CN

   SFRPAGE = restore;
}
//-----------------------------------------------------------------------------
// LCD0_ConfigMuxMode ()
//-----------------------------------------------------------------------------
//
// Return Value : U8 muxMode
// Parameters   : None
//
// This function will initialize mux mode for the LCD. This involves setting
// the mux mode in the LCD0CD SFR, enabling the bias, and configuring the COMx
// pins for analog input mode.
//
// This function clears LCD0CN and LCD0MSCN.
//
//-----------------------------------------------------------------------------
void LCD0_ConfigMuxMode(U8 muxMode)
{
   U8 restore;
   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;

   switch (muxMode)
   {
      case 1:
         LCD0CN   &= ~0x07;            // clear MUXMD & BIAS
         LCD0CN    =  0x00;            // Set 1 mux mode
         LCD0MSCN &= ~0x40 ;           // Disable bias
         LCD0CF    =  0x20;            // Disable comparator, (bias must be disabled)
         SFRPAGE   =  LEGACY_PAGE;
         P2MDIN   &= ~0x10;            // COM0 analog mode
         break;

      case 2:
         SFRPAGE   =  LCD0_PAGE;
         LCD0CN   &= ~0x07;            // clear MUXMD & BIAS
         LCD0CN   |=  0x03;            // Set 2 mux mode, 1/2 bias
         LCD0MSCN |=  0x40 ;           // Enable bias
         LCD0CF    =  0x00;            // Enable comparator, (bias must be enabled)
         SFRPAGE   =  LEGACY_PAGE;
         P2MDIN   &= ~0x30;            // COM0/1 analog mode
         break;;

      case 3:
         SFRPAGE   =  LCD0_PAGE;
         LCD0CN   &= ~0x07;            // clear MUXMD & BIAS
         LCD0CN   |=  0x04;            // Set 3 mux mode
         LCD0MSCN |=  0x40 ;           // Enable bias
         LCD0CF    =  0x00;            // Enable comparator, (bias must be enabled)
         SFRPAGE   =  LEGACY_PAGE;
         P2MDIN   &= ~0x70;            // COM0/1/2 analog mode
         break;

      case 4:
         SFRPAGE   =  LCD0_PAGE;
         LCD0CN   &= ~0x07;            // clear MUXMD & BIAS
         LCD0CN   |=  0x06;            // Set 4 mux mode
         LCD0MSCN |=  0x40 ;           // Enable bias
         LCD0CF    =  0x00;            // Enable comparator, (bias must be enabled)
         SFRPAGE   =  LEGACY_PAGE;
         P2MDIN   &= ~0xF0;            // COM0/1/2/3 analog mode
         break;

      default:
         break;
   }

   SFRPAGE = restore;
}
//-----------------------------------------------------------------------------
// LCD0_ConfigSegments ()
//-----------------------------------------------------------------------------
//
// Return Value : U8 segments
// Parameters   : None
//
// This function will configure the MCU according to the number of LCD segments
// used. The 32-bit mode with be enabled if the number of segments is greater
// than 16. The analog pins are configured according to the number of
// segments. This assumes that the segments start at P3.0 and are allocated
// contiguously.
//
//-----------------------------------------------------------------------------
void LCD0_ConfigSegments(U8 segments)
{
   UU32 segmentBits;
   U8 restore;
   restore = SFRPAGE;

   if(segments>16)
   {
      SFRPAGE = LCD0_PAGE;
      LCD0CN |= 0x08;                  // Set 32 segment mode
   }

   // intialize bit mask to 32 ones.
   segmentBits.U32 = 0xFFFFFFFF;

   // shift right by 32 minus number of segments
   segmentBits.U32 >>= (32 - segments);

   // Resulting number of ones is equal to number of segments

   // Complement to get PxMDIN setting.
   segmentBits.U32 = ~ segmentBits.U32;

   SFRPAGE = CONFIG_PAGE;
   P3MDIN = segmentBits.U8[b0];
   P4MDIN = segmentBits.U8[b1];
   P5MDIN = segmentBits.U8[b2];
   P6MDIN = segmentBits.U8[b3];

   SFRPAGE = restore;
}
//-----------------------------------------------------------------------------
// LCD0_ConfigContrastMode ()
//-----------------------------------------------------------------------------
//
// Return Value : U8 contrastMode, U16 contrast, U16 minContrast
// Parameters   : None
//
// This function will configure LCD SFR bits according to the selected
// contrast mode. The contrast and VBAT monitor thresholds are set as needed.
//-----------------------------------------------------------------------------
void LCD0_ConfigContrastMode(U8 contrastMode)
{
   U8 restore;
   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;

   switch (contrastMode)
   {
      case LCD_BYPASS_MODE:
         LCD0MSCN &= ~0x04;            // Disable low-power charge pump
         LCD0MSCF |= 0x01;             // Enable charge pump bypass
         LCD0PWR &= ~0x08;             // Clear MODE bit for mode 1 & 4
         LCD0VBMCN = 0x00;             // VBM not used
         break;

      case LCD_MIN_CONTRAST_MODE:
         LCD0MSCN |=  0x40 ;           // Enable bias
         LCD0CF    =  0x00;            // Enable comparator, (bias must be enabled)
         LCD0MSCN &= ~0x04;            // Disable low-power charge pump
         LCD0MSCF |= 0x01;             // Enable charge pump bypass
         LCD0PWR |= 0x08;              // Set mode bit for mode 2 & 3
         LCD0VBMCN = 0xC1;             // enable, offset mode, contrast +60 mV
        break;

      case LCD_CONSTANT_CONTRAST_MODE:
         LCD0MSCN |=  0x40 ;           // Enable bias
         LCD0CF    =  0x00;            // Enable comparator, (bias must be enabled)
         LCD0MSCN |= 0x04;             // Enable low-power change pump
         LCD0MSCF &= ~0x01;            // Disable charge pump bypass
         LCD0PWR |= 0x08;              // Set mode bit for mode 2 & 3
         LCD0VBMCN = 0xC1;             // enable, offset mode, contrast +60 mV
         break;

      case LCD_AUTO_BYPASS_MODE:
         LCD0MSCN |=  0x40 ;           // Enable bias
         LCD0CF    =  0x00;            // Enable comparator, (bias must be enabled)
         LCD0MSCN |= 0x04;             // Enable low-power change pump
         LCD0MSCF &= ~0x01;            // Disable charge pump bypass
         LCD0PWR &= ~0x08;             // Clear MODE bit for mode 1 & 4
         LCD0VBMCN = 0xC1;             // Enable, offset mode, contrast +60 mV
         break;

      default:
         break;
   }

   SFRPAGE = restore;
}
//-----------------------------------------------------------------------------
// LCD0_SetContrast ()
//-----------------------------------------------------------------------------
//
// Return Value : U16 contrast
// Parameters   : None
//
// This function will calculate the resister setting for the desired contrast
// voltage.
//
//-----------------------------------------------------------------------------
void LCD0_SetContrast (U16 contrast)
{
   U32 value;

   U8 restore;
   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;

   if(contrast<1900)
      LCD0CNTRST = 0;

   value = contrast - 1900;

   value *= 31;
   value += 1820/2;                    // add half of divisor for rounding
   value /= 1820;

   if (value <32)
      LCD0CNTRST = (U8) (value);
   else
      LCD0CNTRST = 31;

   SFRPAGE = restore;
}
//-----------------------------------------------------------------------------
// LCD0_ConfigRefresh ()
//-----------------------------------------------------------------------------
//
// Return Value : U8 refresh, U8 muxMode
// Parameters   : None
//
// This function will set the LCD clock divider according to the desired
// refresh rate. The lcdClockFreq is set according to the RTC mode as defined
// in the lcdInit.h header file.
//
//-----------------------------------------------------------------------------
void LCD0_ConfigRefresh(U8 refresh, U8 muxMode)
{
   UU16 divider;
   U16  lcdClockFreq;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;

#ifdef RTC_LFO_MODE
   lcdClockFreq = RTC_LFO_FREQ;
#endif

#ifdef RTC_XTAL_MODE
   lcdClockFreq = RTC_XTAL_FREQ/2;
   LCD0CN |= 0x20;                  // lcdClock = rtcClock/2
#endif

   divider.U16 = lcdClockFreq;
   divider.U16 /= refresh;
   divider.U16 /= muxMode;
   divider.U16 >>=2;
   divider.U16--;

   if(divider.U16 > 0x3FF)
      divider.U16 = 0x3FF;

   LCD0CLKDIVH = divider.U8[MSB];
   LCD0CLKDIVL = divider.U8[LSB];

   SFRPAGE = restore;
}
//-----------------------------------------------------------------------------
// LCD0_ConfigToggle ()
//-----------------------------------------------------------------------------
//
// Return Value : U8 toggle, U8 refresh,  U8 muxMode
// Parameters   : None
//
// This function will set the LCD toggle divider exponent according to the
// target toggle rate. The chosen divider rate will be the next larger
// power of two divider.
//
//-----------------------------------------------------------------------------
void LCD0_ConfigToggle(U8 toggle, U8 refresh,  U8 muxMode)
{
   U16 targetDivider;
   U16 toggleDivider;
   U8 toggleExponent;
   U8 restore;


   targetDivider = refresh;
   targetDivider <<= 1;
   targetDivider *= muxMode;
   targetDivider /= toggle;

   toggleExponent = 0;
   toggleDivider = 1;

   while (toggleDivider < targetDivider)
   {
      toggleDivider <<= 1;
      toggleExponent++;
   }

   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;
   LCD0TOGR = toggleExponent;
   SFRPAGE = restore;

}
//-----------------------------------------------------------------------------
// LCD0_ConfigEnable ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will configure the LCD for the different modes.
//
//-----------------------------------------------------------------------------
void LCD0_ConfigEnable(void)
{
   U8 restore;
   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;

   SFRPAGE = LCD0_PAGE;
   LCD0MSCN |= 0x11;                   // Enable LCD, enable Clocks

   SFRPAGE = restore;
}
//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
