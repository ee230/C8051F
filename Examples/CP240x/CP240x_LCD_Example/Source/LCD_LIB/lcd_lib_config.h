//-----------------------------------------------------------------------------
// lcd_lib_config.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// Contains global compile-time configuration options.
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
// Callback Configuration Options
//-----------------------------------------------------------------------------
//
// Please configure the following options
//

// Stopwatch Callbacks are required for the RTC_MeasureFrequency() routine to 
// measure the time between 8 consecutive RTC clock cycles.  The callbacks are
// defined in lcd_lib_callback.c.

#define ALLOW_STOPWATCH_CALLBACKS        0



//-----------------------------------------------------------------------------
// RTC Configuration Options
//-----------------------------------------------------------------------------
//
// Please configure the following options
//

#define CP240X_SMARTCLOCK_ENABLED        1

// RTC Clock Source -- can be: 
//  CP240X_SELFOSC
//  CP240X_CRYSTAL
//  CP240X_CMOSCLK
//
#define CP240X_RTC_CLKSRC   CP240X_CRYSTAL 

// Crystal Load Capacitance
#define CP240X_LOADCAP_VALUE  4        //  0 =  4.0 pf,  1 =  4.5 pf
                                       //  2 =  5.0 pf,  3 =  5.5 pf
                                       //  4 =  6.0 pf,  5 =  6.5 pf
                                       //  6 =  7.0 pf,  7 =  7.5 pf 
                                       //  8 =  8.0 pf,  9 =  8.5 pf
                                       // 10 =  9.0 pf, 11 =  9.5 pf
                                       // 12 = 10.5 pf, 13 = 11.5 pf
                                       // 14 = 12.5 pf, 15 = 13.5 pf

#if CP240X_RTC_CLKSRC == CP240X_SELFOSC
   #define CP240X_RTCCLK   31500
#else
   #define CP240X_RTCCLK   32768
#endif


//-----------------------------------------------------------------------------
// LCD Configuration Options
//-----------------------------------------------------------------------------
//

// Define the LCD. Can be { VIM878 / CUSTOMLCD }
#define LCD                        VIM878

// Define the LCD Refresh Rate in Hz
#define REFRESH_RATE_HZ             60

// Define the mux mode. Can be { STATIC, TWO_MUX, THREE_MUX, FOUR_MUX}
#define MUX_MODE                    FOUR_MUX

// Define the Bias. Can be { HALF_BIAS, ONETHIRD_BIAS }
#define BIAS   ONETHIRD_BIAS

//-----------------------------------------------------------------------------
// CP240x RAM Configuration
//-----------------------------------------------------------------------------
// 
// The CP240x has a 256 byte RAM that may be used as general purpose memory.

#define CP240X_RAM_ENABLED                0

// If the RAM is enabled, then choose from the following options:
#if (CP240X_RAM_ENABLED)

   // This option allows a single byte to be accessed at a time.  Otherwise,
   // the standard "Block Read/Write" routines can be used to access the 
   // RAM.  These single-byte operations are faster than the Block operations.
   #define SINGLE_BYTE_RAM_OPERATIONS            0

   // This option allows a block of RAM to be read in reverse order (from 
   // the largest address to the smallest address)
   #define AUTODECREMENT_BLOCK_RAM_OPERATIONS    0

#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
