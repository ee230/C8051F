//-----------------------------------------------------------------------------
// lcd_lib_callback.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// Contains callbacks for lcd library defintions.
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

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F930_defs.h>            // SFR declarations

#include "LCD_LIB\lcd_lib.h" 


//-----------------------------------------------------------------------------
// Timing Routines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Wait_US
//-----------------------------------------------------------------------------
// Insert <us> microsecond delays using an on-chip timer.
// 
void Wait_US(U16 us)
{
   U8 every_other_cycle = 0;

   while(us)
   {
      // Clear the CCF interrupt flag
      CCF0 = 0;

      // Wait for the CCF interrupt flag
      while(CCF0);

      // Cancel the effect of truncation
      if(every_other_cycle == 0)
      { 
         every_other_cycle = 1;
         PCA0CPH0 = SYSCLK/1000000;
      } else
      {
        every_other_cycle = 0;
        PCA0CPH0 = (SYSCLK/1000000 + 1);
      }
      
       // Decrement the remaining microseconds
      us--;
   }


}

//-----------------------------------------------------------------------------
// Wait_MS
//-----------------------------------------------------------------------------
// Insert <ms> millisecond delays using an on-chip timer.
// 
void Wait_MS(U16 ms)
{
   while(ms--)
   {
      Wait_US(1000);
   }
}


#if ALLOW_STOPWATCH_CALLBACKS 

//-----------------------------------------------------------------------------
// Stopwatch_Reset
//-----------------------------------------------------------------------------
// 
// Resets the stopwatch
// 
void Stopwatch_Reset(void)
{
   
   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;
                                       // use SYSCLK/12 as timebase
   CKCON  &= ~0x30;                    // Timer2 clocked based on T2XCLK;

   TMR2RL = 0x0000;
   TMR2 = 0x0000;
}


//-----------------------------------------------------------------------------
// Stopwatch_Start
//-----------------------------------------------------------------------------
// 
// Starts the stopwatch
// 
void Stopwatch_Start(void)
{
   
   TR2 = 1;                            // Start Timer 2
 
}


//-----------------------------------------------------------------------------
// Stopwatch_Stop
//-----------------------------------------------------------------------------
// 
// Stops the stopwatch and returns the value
// 
U16 Stopwatch_Stop(void)
{
   
   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;

   return TMR2;
}

#endif

//-----------------------------------------------------------------------------
// SmaRTClock Fail Handler
//-----------------------------------------------------------------------------
void CP240x_RTC_Failure(void)
{  
   // Modify this to handle an RTC clock failure
   while(1);
   
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
