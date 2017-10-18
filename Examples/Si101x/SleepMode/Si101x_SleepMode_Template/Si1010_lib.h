//-----------------------------------------------------------------------------
// Si1010_lib.h
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Driver for the  power management function.
//
// Target:         Si101x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    - 18 MAY 10 (FB)
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <smartclock.h>                // RTC Functionality
#include <power.h>                     // Power Management Functionality


//-----------------------------------------------------------------------------
// Global Configuration Options
//-----------------------------------------------------------------------------

#define SYSCLK  20000000

#define SMARTCLOCK_ENABLED   1