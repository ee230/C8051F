//-----------------------------------------------------------------------------
// lcd_lib.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// Contains global lcd library defintions.
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
#include "..\sysclk.h"
#include "CP240x_defs.h"
#include "lcd_lib_const.h"
#include "lcd_lib_config.h"
#include "lcd_lib_portdefs.h"
#include "lcd_lib_preprocessor.h"

#include "CP240x_BusInterface.h"
#include "CP240x_Core.h"
#include "CP240x_lcd.h"

//-----------------------------------------------------------------------------
// Delay Routines
//-----------------------------------------------------------------------------
extern void Wait_US(U16 us);
extern void Wait_MS(U16 ms);

//-----------------------------------------------------------------------------
// Stopwatch Routines
//-----------------------------------------------------------------------------
// Implements a stopwatch function
// 
// 
#if (ALLOW_STOPWATCH_CALLBACKS)
extern void Stopwatch_Reset(void);
extern void Stopwatch_Start(void);
extern U16 Stopwatch_Stop(void);
#endif

//-----------------------------------------------------------------------------
// SmaRTClock Fail Handler
//-----------------------------------------------------------------------------
extern void CP240x_RTC_Failure(void);

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
