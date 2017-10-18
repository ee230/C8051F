//=============================================================================
// test_VIM878.c
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
//    C8051F960 LCD Code Examples
//
// Description:
//    This is the main module for the VIM878 LCD driver test workspace.
// This project uses the lcdPrintf and lcdPutString modules.
//
// The LCD_Init_VIM878 code in this module is specific to the VIM878.
// This is a 4-mux 128-segment display.
//
// The corresponding lcdPutChar_VIM878 module must be included in the project.
// The lcdPutchar function is specific to the particular display.
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include "C8051F960_defs.h"
#include "lcdPutString.h"
#include "lcdPrintf.h"
#include "lcdConfig.h"
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void RTC_Init (void);
void LCD_Init_VIM878 (void);
unsigned char RTC_Read (U8 reg);
void RTC_Write (U8 reg, U8 value);
//-----------------------------------------------------------------------------
// Main Prototypes
//-----------------------------------------------------------------------------
void main (void)
{
   SFRPAGE = LEGACY_PAGE;
   PCA0MD  &= ~0x40;                   // disable watchdog timer

   RTC_Init();
   LCD_Init_VIM878();

   SFRPAGE = LEGACY_PAGE;
   CLKSEL = 0x03;

	 //lcdPutString("'@.'@.'@.'@.'@.'@.'@.'@.");
    //lcdPrintf("%u", 12345);
    lcdPutString("'F960 LCD");

  while(1);
}
//-----------------------------------------------------------------------------
// LCD_Init_VIM878
//
// Description:
// This function will configure the LCD for a 128-segment 4-mux display.
//
// The refresh rate assumes a 16 kHz RTC clock using the LFO.
//
// This function uses the lcdConfig module to configure the LCD regsiters.
//
//-----------------------------------------------------------------------------
void LCD_Init_VIM878 (void)
{
   //          +- 4-mux
   //          |  +- 32 segments
   //          |  |   +- Mode 3 - Constant Contrast mode
   //          |  |   |  +- Contrast 2.9 volts
   //          |  |   |  |     +- Refresh 60 Hz
   //          |  |   |  |     |   +- Toggle 1 Hz
   LCD0_Config(4, 32, 3, 2900, 60, 1);
}
//=============================================================================
