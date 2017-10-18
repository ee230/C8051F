#ifndef  LCD_PUTCHAR_VI401_H
#define  LCD_PUTCHAR_VI401_H
//=============================================================================
// lcdPutChar_VI401.h
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
// Segment constants for the VI-401 LCD
//-----------------------------------------------------------------------------
//
// This section changes if the mapping between LCD segment/driver pins changes.
//
// LCD Segment Constants (Bit Mapping)
// x x x x x x x x   x x x x x x x x   x x x x x x x x   x x x x x x x x
//       G       F         E       D         C       B         A       DP
//-----------------------------------------------------------------------------
#define SEG_DP 0x00000001
#define SEG_A  0x00000010
#define SEG_B  0x00000100
#define SEG_C  0x00001000
#define SEG_D  0x00010000
#define SEG_E  0x00100000
#define SEG_F  0x01000000
#define SEG_G  0x10000000
//-----------------------------------------------------------------------------
// Colon mapped to digit zero bit zero for blink
//-----------------------------------------------------------------------------
#define SEG_COLON  0x00000001
//-----------------------------------------------------------------------------
// Simple Font Table  for 7-segment numeric displays
//-----------------------------------------------------------------------------
// Numeric
//-----------------------------------------------------------------------------
#define  CHAR_0   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F)
#define  CHAR_1   (SEG_B|SEG_C)
#define  CHAR_2   (SEG_A|SEG_B|SEG_D|SEG_E|SEG_G)
#define  CHAR_3   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_G)
#define  CHAR_4   (SEG_B|SEG_C|SEG_F|SEG_G)
#define  CHAR_5   (SEG_A|SEG_C|SEG_D|SEG_F|SEG_G)
#define  CHAR_6   (SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G)
#define  CHAR_7   (SEG_A|SEG_B|SEG_C)
#define  CHAR_8   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G)
#define  CHAR_9   (SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G)
//-----------------------------------------------------------------------------
// Limited Alpha characters for Hex display
//-----------------------------------------------------------------------------
#define  CHAR_A   (SEG_A|SEG_B|SEG_C|SEG_E|SEG_F|SEG_G)        // 'A'
#define  CHAR_B   (SEG_C|SEG_D|SEG_E|SEG_F|SEG_G)              // 'b'
#define  CHAR_C   (SEG_A|SEG_F|SEG_E|SEG_D)                    // 'C'
#define  CHAR_D   (SEG_B|SEG_C|SEG_D|SEG_E|SEG_G)              // 'd'
#define  CHAR_E   (SEG_A|SEG_D|SEG_E|SEG_F|SEG_G)              // 'E'
#define  CHAR_F   (SEG_A|SEG_E|SEG_F|SEG_G)                    // 'F'
//-----------------------------------------------------------------------------
// Special characters the VIM-878 LCD
//-----------------------------------------------------------------------------
#define CHAR_MINUS      (SEG_G)
#define CHAR_PERIOD     (SEG_DP)
#define CHAR_COLON      (SEG_COLON)
//-----------------------------------------------------------------------------
// lenght
//-----------------------------------------------------------------------------
#define NUM_ALPHA       6
#define NUM_NUMERIC     10
//=============================================================================
// Public Function Prototypes (API)
//=============================================================================
void lcdPutChar (char c);
//=============================================================================
// end LCD_PUTCHAR_VI401_H
//=============================================================================
#endif //LCD_PUTCHAR_VI401_H