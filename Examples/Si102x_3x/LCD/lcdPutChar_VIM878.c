//=============================================================================
// lcdPutChar_VIM878.c
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
// This file is written using 8051 compiler independent code, but it is
// specific to the Si102x/3x and VIM878 display.
//
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1020_defs.h>
#include "lcdPutChar_VIM878.h"
//-----------------------------------------------------------------------------
// Local code constant arrays for alpha and Numeric fonts.
// Public extern declartions in header file.
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (FontMapNumeric[NUM_NUMERIC], U16, SEG_CODE) =
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

const SEGMENT_VARIABLE (FontMapAlpha[NUM_ALPHA], U16, SEG_CODE) =
{
   CHAR_A,
   CHAR_B,
   CHAR_C,
   CHAR_D,
   CHAR_E,
   CHAR_F,
   CHAR_G,
   CHAR_H,
   CHAR_I,
   CHAR_J,
   CHAR_K,
   CHAR_L,
   CHAR_M,
   CHAR_N,
   CHAR_O,
   CHAR_P,
   CHAR_Q,
   CHAR_R,
   CHAR_S,
   CHAR_T,
   CHAR_U,
   CHAR_V,
   CHAR_W,
   CHAR_X,
   CHAR_Y,
   CHAR_Z
};
//-----------------------------------------------------------------------------
// Internal function prototypes
//-----------------------------------------------------------------------------
void lcdClear(void);
void lcdSetChar(char c, U8 index);
void lcdSetBitMap(U16 bitmap, U8 index);
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
// For the VIM878, the decimal point is associated with the character to the
// left. So the decimal point will set the bit of the previous character,
// and does not increment the index. The decimal point is only valid for
// an index of 1 through 8.
//
// For the VIM878, the apostrophe is associated with the character to the
// right. So the decimal point will set the bit of the current character,
// and does not increment the index. The apostrophe is only valid for
// an index of 0 through 7.
//
// Note that repeated decimal points or appostrophes will not increment
// the index. So a space is needed between repeated decimal points
// or appostrophes.
//
//-----------------------------------------------------------------------------
void lcdPutChar (char c)
{
   static U8 index;

   if(c=='\r')
   {
      index=0;                         // carraige return resets index
   }
   else if(c=='\n')
   {
      lcdClear();                      // new line clears display
   }
   else if(c=='.')
   {
      if ((index>0)&&(index<9))
      {
         lcdSetChar(c, index-1);       // set period in previous character                                          // index not incremented
      }
   }
   else if(c=='\'')
   {
      if(index < 8)
      {
         lcdSetChar(c, index);         // set apostrophe
      }                                // index not incremented
   }
   else if(index<8)                    // display overflow ignored
   {
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
// This function will use the VIM878 font map character constants to look-up
// the corresponding bitmap for the current character. The VI401 uses a
// 16-bit bitmap.
//
// The VIM878 font map in the header file supports numbers 0-9 and upper
// case alpha characters A-Z. Lower case a-z are displayed as upper-case
// characters.
//
// Supported special characters are PERIOD, APOSTROPHE, PLUS, MINUS, and
// ASTERIX.
//
// The AT_SIGN character '@' will be displayed as a starburst pattern will all
// alphanumeric segments on, not including the appostrophes or decimal points.
// This can be used to test the display. To turn on all segment, including
// appostrophes and decimal points use this string:
// "'@.'@.'@.'@.'@.'@.'@.'@."
//
// Other characters are treated as spaces.
//
//-----------------------------------------------------------------------------
void lcdSetChar(char c, U8 index)
{
   U16 bitmap;

   if(c >= '0' && c <= '9')
   {
      bitmap = FontMapNumeric[c - '0'];
   }
   else if(c >= 'A' && c <= 'Z')
   {
      bitmap = FontMapAlpha[c - 'A'];
   }
   else if(c >= 'a' && c <= 'z')
   {
      bitmap = FontMapAlpha[c - 'a'];
   }
   // special character handling
   else if(c =='.')
   {
      bitmap = CHAR_PERIOD;
   }
   else if(c =='\'')
   {
      bitmap = CHAR_APOSTROPHE;
   }
   else if(c =='+')
   {
      bitmap = CHAR_PLUS;
   }
   else if(c =='-')
   {
      bitmap = CHAR_MINUS;
   }
   else if(c =='*')
   {
      bitmap = CHAR_ASTERIX;
   }
   else if(c =='@')
   {
      bitmap = CHAR_AT_SIGN;
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
// For a 4-mux display , a 16-bit word represents one character. All bits
// are used.
//
//-----------------------------------------------------------------------------
void lcdSetBitMap(U16 bitmap, U8 index)
{
   UU16 value;
   U8 restore;

   restore = SFRPAGE;
   SFRPAGE = LCD0_PAGE;

   value.U16 = bitmap;

   switch(index)
   {
      case 0:
         LCD0D0 |= value.U8[LSB];
         LCD0D1 |= value.U8[MSB];
         break;

      case 1:
         LCD0D2 |= value.U8[LSB];
         LCD0D3 |= value.U8[MSB];
         break;

      case 2:
         LCD0D4 |= value.U8[LSB];
         LCD0D5 |= value.U8[MSB];
         break;

      case 3:
         LCD0D6 |= value.U8[LSB];
         LCD0D7 |= value.U8[MSB];
         break;

      case 4:
         LCD0D8 |= value.U8[LSB];
         LCD0D9 |= value.U8[MSB];
         break;

      case 5:
         LCD0DA |= value.U8[LSB];
         LCD0DB |= value.U8[MSB];
         break;

      case 6:
         LCD0DC |= value.U8[LSB];
         LCD0DD |= value.U8[MSB];
         break;

      case 7:
         LCD0DE |= value.U8[LSB];
         LCD0DF |= value.U8[MSB];
         break;

      default:
          break;
   }
   
   SFRPAGE = restore;
}
//=============================================================================
// end of file
//=============================================================================




