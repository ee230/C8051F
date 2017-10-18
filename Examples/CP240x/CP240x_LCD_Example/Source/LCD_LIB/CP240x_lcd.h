//-----------------------------------------------------------------------------
// CP240x_lcd.h
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// Functions for outputting to an LCD display connected to the CP240x
// 
//
// Target:         Multiple
// Tool chain:     Generic
//                 Silicon Laboratories IDE version 3.40 
// Project Name:   CP240x_Firmware
//
// Release 1.0
//    -26 OCT 2009

//-----------------------------------------------------------------------------
// LCD Functions
//-----------------------------------------------------------------------------

extern void LCD_Init();
extern void LCD_Clear();
extern void LCD_OutString(U8 dat[8]);
extern void LCD_OutChar(U8 position, U8 dat);
