//=============================================================================
// lcdPutChar_VI401.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// Target:
//    C8051F96x
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
//    F96x LCD
//
// Release 0.0
//    - TBD
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//
// This file is written using 8051 compiler independent code, but is specific
// to the C8051F96x and VI401 display.
//
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include  <C8051F960_defs.h>
#include  "lcdPutChar_VI401.h"
//-----------------------------------------------------------------------------
// Local code constant arrays for alpha and Numeric fonts.
// Public extern declartions in header file.
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (FontMapNumeric[NUM_NUMERIC], U32, SEG_CODE) =
{
   CHAR_0,
   CHAR_1,
   CHAR_2,
   CHAR_3,
   CHAR_4,
   CHAR_5,
   CHAR_6,
   CHAR_7,
   CHAR_8,
   CHAR_9
};

const SEGMENT_VARIABLE (FontMapAlpha[NUM_ALPHA], U32, SEG_CODE) =
{
   CHAR_A,
   CHAR_B,
   CHAR_C,
   CHAR_D,
   CHAR_E,
   CHAR_F
};
//-----------------------------------------------------------------------------
// Internal function prototypes
//-----------------------------------------------------------------------------
void lcdClear(void);
void lcdSetChar(char c, U8 index);
void lcdSetBitMap(U32 bitmap, U8 index);
//-----------------------------------------------------------------------------
// lcdPutChar()
//
// The lcdPutChar() function is specific to the particular display.
//
// A carraige return character resets the index to zero.
//
// A new line character will clear the display. It is necessary to clear the
// display before new string. Each putchar operation will use a logical or
// operation to set the active segment bits.
//
// Any decimal points or colons in the string are treated as special
// characters. The behavior of special character depends on the display.
//
// For the VI401, the decimal point is associated with the character to the
// right. So the decimal point will set the bit of the current character,
// but does not increment the index. The decimal point is only valid for
// an index of 1,2, or 3.
//
// Nore that repeated decimal points will not increment the index. So a
// space is needed between repeated decimal points " . . . ".
//
// A colon is only valid for an index of 2. Otherwise it is ignored. The
// colon always sets LCD0D0 bit 0. The colon character support blinking.
//
//-----------------------------------------------------------------------------
void lcdPutChar (char c)
{
   static U8 index;

   if(c=='\r')
   {
      index = 0;                       // carraige return resets index to left
   }
   else if(c=='\n')
   {
      lcdClear();                      // new line clears display
   }
   else if(c=='.')
   {
      if((index>0)&&(index<4))
      {
         lcdSetChar(c, (index));       // set period in previous character
      }                                // index not decremented
   }
   else if(c==':')
   {
      if(index==2)
      {
         lcdSetChar(c, 0);             // always set LCD0D0 bit 0
      }                                // index not decremented
   }
   else if(index<4)                    // display overflow ignored
   {
      // normal font map character processing
      lcdSetChar(c, index);
      index++;
   }
}
//-----------------------------------------------------------------------------
// lcdClear()
//
// This function will clear all LCD data bits, clearing the display. A bitwise
// logical OR is used to set bits. So clearing the display is necessary for
// each new string.
//-----------------------------------------------------------------------------
void lcdClear(void)
{
   U8 restore;
   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;

   LCD0D0 = 0x00;
   LCD0D1 = 0x00;
   LCD0D2 = 0x00;
   LCD0D3 = 0x00;
   LCD0D4 = 0x00;
   LCD0D5 = 0x00;
   LCD0D6 = 0x00;
   LCD0D7 = 0x00;
   LCD0D8 = 0x00;
   LCD0D9 = 0x00;
   LCD0DA = 0x00;
   LCD0DB = 0x00;
   LCD0DC = 0x00;
   LCD0DD = 0x00;
   LCD0DE = 0x00;
   LCD0DF = 0x00;

   SFRPAGE = restore;
}
//-----------------------------------------------------------------------------
// lcdSetChar()
//
// This function will use the VI401 font map character constants to look-up
// the corresponding bitmap for the current character. The VI401 uses a
// 32-bit bitmap.
//
// The VIM401 font map in the header file supports numbers 0-9 and upper
// case alpha characters A-F. Lower case a-f are displayed as upper-case
// characters.
//
// Supported special characters are PERIOD, COLON, and MINUS.
//
// Other characters are treated as spaces.
//

//-----------------------------------------------------------------------------
void lcdSetChar(char c, U8 index)
{
   U32 bitmap;

   if(c >= 'A' && c <= 'F')
   {
      bitmap = FontMapAlpha[c - 'A'];
   }
   else if(c >= 'a' && c <= 'f')
   {
      bitmap = FontMapAlpha[c - 'a'];
   }
   else if(c >= '0' && c <= '9')
   {
      bitmap = FontMapNumeric[c - '0'];
   }
   // special character handling
   else if(c =='.')
   {
      bitmap = CHAR_PERIOD;
   }
   else if(c ==':')
   {
      bitmap = CHAR_COLON;
   }
   else if(c =='-')
   {
      bitmap = CHAR_MINUS;
   }
   else
   {
      bitmap = 0x0000;
   }

   lcdSetBitMap(bitmap, index);
}
//-----------------------------------------------------------------------------
// lcdSetBitMap()
//
// This function uses the character bitmap to set bits in the LCD data sfrs.
//
// The data organization depends on the nature of the display.
//
// For a static display a 32-bit word represents one character. But only the
// mod 4 = 0 bits are actually used (0, 4, 8, 12, 16, 20, 24, 28).
//
//-----------------------------------------------------------------------------
void lcdSetBitMap(U32 bitmap, U8 index)
{
   UU32 value;
   U8 restore;
   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;

   value.U32 = bitmap;

   switch(index)
   {
     case 0:
        LCD0D0 |= value.U8[b0];
        LCD0D1 |= value.U8[b1];
        LCD0D2 |= value.U8[b2];
        LCD0D3 |= value.U8[b3];
        break;

     case 1:
        LCD0D4 |= value.U8[b0];
        LCD0D5 |= value.U8[b1];
        LCD0D6 |= value.U8[b2];
        LCD0D7 |= value.U8[b3];
        break;

     case 2:
        LCD0D8 |= value.U8[b0];
        LCD0D9 |= value.U8[b1];
        LCD0DA |= value.U8[b2];
        LCD0DB |= value.U8[b3];
        break;

     case 3:
        LCD0DC |= value.U8[b0];
        LCD0DD |= value.U8[b1];
        LCD0DE |= value.U8[b2];
        LCD0DF |= value.U8[b3];
        break;

     default:
         break;
   }

   SFRPAGE = restore;
}
//=============================================================================
// end of file
//=============================================================================




