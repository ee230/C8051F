//-----------------------------------------------------------------------------
// CP240x_lcd.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// This file contains generic high level LCD routines
//
// Target:         Multiple
// Tool chain:     Generic
//                 Silicon Laboratories IDE version 3.40 
// Project Name:   CP240x_Firmware
//
// 
//
// Release 1.0
//    -26 OCT 2009
//
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F930_defs.h>
#include "lcd_lib.h"


//-----------------------------------------------------------------------------
// LCD_Init
//-----------------------------------------------------------------------------
//
// Parameter: none
// Return Value: ASCII Character
//
// Initializes the LCD
//
void LCD_Init(void)
{
   U8 xdata lcd_reg_array[7];
   U8 LCD0CF_value;
   
   #if(ALLOW_STOPWATCH_CALLBACKS)
   U16 LCD0DIV;
   #endif

   
   #if(MUX_MODE == STATIC)
      
      #if(ALLOW_STOPWATCH_CALLBACKS)
         LCD0DIV = (CP240x_Measured_RTCCLK/4/1/REFRESH_RATE_HZ)-1; 
      #else
         #define LCD0DIV  (CP240X_RTCCLK/4/1/REFRESH_RATE_HZ)-1  
      #endif

   #elif (MUX_MODE == TWO_MUX)
      
      #if(ALLOW_STOPWATCH_CALLBACKS)
         LCD0DIV = (CP240x_Measured_RTCCLK/4/2/REFRESH_RATE_HZ)-1; 
      #else
         #define LCD0DIV  (CP240X_RTCCLK/4/2/REFRESH_RATE_HZ)-1
      #endif

   #elif (MUX_MODE == THREE_MUX)

      #if(ALLOW_STOPWATCH_CALLBACKS)
         LCD0DIV = (CP240x_Measured_RTCCLK/4/3/REFRESH_RATE_HZ)-1; 
      #else
         #define LCD0DIV  (CP240X_RTCCLK/4/3/REFRESH_RATE_HZ)-1   
      #endif

   #elif (MUX_MODE == FOUR_MUX)

      #if(ALLOW_STOPWATCH_CALLBACKS)
         LCD0DIV = (CP240x_Measured_RTCCLK/4/4/REFRESH_RATE_HZ)-1; 
      #else
         #define LCD0DIV  (CP240X_RTCCLK/4/4/REFRESH_RATE_HZ)-1   
      #endif

   #endif

   // Calculate value for LCD0CF
   #if(ALLOW_STOPWATCH_CALLBACKS)
         LCD0CF_value = (CP240x_Measured_RTCCLK/500); 
   #else
         LCD0CF_value = CP240X_RTCCLK/500;   
   #endif
   
   if(LCD0CF_value > 63) LCD0CF_value = 63;

   // Initialize Port I/O For LCD functionality
   CP240x_PortIO_Registers.P0MDI_Local = 0x00;
   CP240x_PortIO_Registers.P1MDI_Local = 0x00;
   CP240x_PortIO_Registers.P2MDI_Local = 0x00;
   CP240x_PortIO_Registers.P3MDI_Local = 0x00;
   CP240x_PortIO_Registers.P4MDI_Local = 0x00;

   CP240x_PortIO_Configure();

   // Initialize LCD control registers
   lcd_reg_array[0] = (0x08 | MUX_MODE | BIAS);// LCD0CN
   lcd_reg_array[1] = 0x01;                    // CONTRAST     
   lcd_reg_array[2] = (0x80 | LCD0CF_value);   // LCD0CF
   lcd_reg_array[3] = (LCD0DIV & 0xFF);        // LCD0DIVL  
   lcd_reg_array[4] = ((LCD0DIV>>8) & 0xFF);   // LCD0DIVH
   lcd_reg_array[5] = 0;                       // LCD0TOGR   
   lcd_reg_array[6] = 0x10;                    // LCD0PWR
   
   // Write the LCD registers
   CP240x_RegBlockWrite(LCD0CN, lcd_reg_array, 7);
   
   // Enable LCD Functionality
   CP240x_MSCN_SetBits(0x01);
   
}

//-----------------------------------------------------------------------------
// LCD_OutString
//-----------------------------------------------------------------------------
//
// Parameter: 8 character array containing ASCII values
// Return Value: none
//
// Prints the 8 character array to the LCD
//
void LCD_OutString(U8 dat[8])
{
	U8 i;

	for (i = 0; i < 8; i++)
	{
		LCD_OutChar(i,dat[i]);
	}
   
}

//-----------------------------------------------------------------------------
// LCD_Clear
//-----------------------------------------------------------------------------
//
// Parameter: none
// Return Value: none
//
// Clears the 8 digits of the LCD display
//
void LCD_Clear()
{
	U8 i;
	for(i = 0; i < 8; i++)
	{
		LCD_OutChar(i,0);
	}
}
