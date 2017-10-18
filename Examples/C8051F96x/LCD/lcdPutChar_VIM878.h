#ifndef  LCD_PUTCHAR_VIM878_H
#define  LCD_PUTCHAR_VIM878_H
//=============================================================================
// lcdPutChar_VIM878.h
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//
// Target:
//    C8051F96x.
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
//    C8051F96x LCD Example
//
// Release 0.1
//    - TBD
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
//=============================================================================
// Font map defines
//=============================================================================
//-----------------------------------------------------------------------------
// Segment constants for the VIM-878 LCD
//-----------------------------------------------------------------------------
//
// This section changes if the mapping between LCD segment/driver pins changes.
//
// LCD Segment Constants (Bit Mapping)
// I J K L    H G N M    CA F E D    A B C DP
//-----------------------------------------------------------------------------
#define  SEG_A    0x0008
#define  SEG_B    0x0004
#define  SEG_C    0x0002
#define  SEG_D    0x0010
#define  SEG_E    0x0020
#define  SEG_F    0x0040
#define  SEG_G    0x0400
#define  SEG_H    0x0800
#define  SEG_I    0x8000
#define  SEG_J    0x4000
#define  SEG_K    0x2000
#define  SEG_L    0x1000
#define  SEG_M    0x0100
#define  SEG_N    0x0200
#define  SEG_DP   0x0001
#define  SEG_CA   0x0080
//-----------------------------------------------------------------------------
// Simple Font Table  for 14-segment alpha-numeric displays
//-----------------------------------------------------------------------------
// Numeric
//-----------------------------------------------------------------------------
#define  CHAR_0   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_J|SEG_N)
#define  CHAR_1   (SEG_B|SEG_C)
#define  CHAR_2   (SEG_A|SEG_B|SEG_D|SEG_E|SEG_G|SEG_K)
#define  CHAR_3   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_K)
#define  CHAR_4   (SEG_B|SEG_C|SEG_F|SEG_G|SEG_K)
#define  CHAR_5   (SEG_A|SEG_C|SEG_D|SEG_F|SEG_G|SEG_K)
#define  CHAR_6   (SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_K)
#define  CHAR_7   (SEG_A|SEG_B|SEG_C)
#define  CHAR_8   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_K)
#define  CHAR_9   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G|SEG_K)
//-----------------------------------------------------------------------------
// Aplha (capital letters)
//-----------------------------------------------------------------------------
#define  CHAR_A   (SEG_A|SEG_B|SEG_C|SEG_E|SEG_F|SEG_G|SEG_K)
#define  CHAR_B   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_G|SEG_K|SEG_I|SEG_M)
#define  CHAR_C   (SEG_A|SEG_F|SEG_E|SEG_D)
#define  CHAR_D   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_I|SEG_M)
#define  CHAR_E   (SEG_A|SEG_D|SEG_E|SEG_F|SEG_G|SEG_K)
#define  CHAR_F   (SEG_A|SEG_E|SEG_F|SEG_G)
#define  CHAR_G   (SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_K)
#define  CHAR_H   (SEG_B|SEG_C|SEG_E|SEG_F|SEG_G|SEG_K)
#define  CHAR_I   (SEG_A|SEG_D|SEG_I|SEG_M)
#define  CHAR_J   (SEG_B|SEG_C|SEG_D|SEG_E)
#define  CHAR_K   (SEG_E|SEG_F|SEG_G|SEG_J|SEG_L)
#define  CHAR_L   (SEG_D|SEG_E|SEG_F)
#define  CHAR_M   (SEG_B|SEG_C|SEG_E|SEG_F|SEG_H|SEG_J)
#define  CHAR_N   (SEG_B|SEG_C|SEG_E|SEG_F|SEG_H|SEG_L)
#define  CHAR_O   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F)
#define  CHAR_P   (SEG_A|SEG_B|SEG_E|SEG_F|SEG_G|SEG_K)
#define  CHAR_Q   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_L)
#define  CHAR_R   (SEG_A|SEG_B|SEG_E|SEG_F|SEG_G|SEG_K|SEG_L)
#define  CHAR_S   (SEG_A|SEG_C|SEG_D|SEG_F|SEG_G|SEG_K)
#define  CHAR_T   (SEG_A|SEG_I|SEG_M)
#define  CHAR_U   (SEG_B|SEG_C|SEG_D|SEG_E|SEG_F)
#define  CHAR_V   (SEG_B|SEG_C|SEG_H|SEG_L)
#define  CHAR_W   (SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_M)
#define  CHAR_X   (SEG_H|SEG_J|SEG_L|SEG_N)
#define  CHAR_Y   (SEG_H|SEG_J|SEG_M)
#define  CHAR_Z   (SEG_A|SEG_D|SEG_J|SEG_N)
//-----------------------------------------------------------------------------
// Special characters the VIM-878 LCD
//-----------------------------------------------------------------------------
#define CHAR_PERIOD     (SEG_DP)
#define CHAR_APOSTROPHE (SEG_CA)
#define CHAR_PLUS       (SEG_G|SEG_K|SEG_I|SEG_M)
#define CHAR_MINUS      (SEG_G|SEG_K)
#define CHAR_ASTERIX    (SEG_G|SEG_H|SEG_I|SEG_J|SEG_K|SEG_L|SEG_M|SEG_N)
//-----------------------------------------------------------------------------
// Special Symbol for LCD test
//-----------------------------------------------------------------------------
#define CHAR_AT_SIGN     (SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_H|SEG_I|SEG_J|SEG_K|SEG_L|SEG_M|SEG_N)
//-----------------------------------------------------------------------------
#define NUM_ALPHA       26
#define NUM_NUMERIC     10
//=============================================================================
// Public Function Prototypes (API)
//=============================================================================
void lcdPutChar (char c);
//=============================================================================
// end LCD_PUTCHAR_VIM878_H
//=============================================================================
#endif //LCD_PUTCHAR_VIM878_H