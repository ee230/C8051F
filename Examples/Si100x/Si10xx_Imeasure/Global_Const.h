//-----------------------------------------------------------------------------
// Global_Const.h
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This header file contains global constants.
//
//
// Target:         C8051F93x-C8051F90x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (FB)
//    -27 APR 2010
//

//-----------------------------------------------------------------------------
// Power Mode Selection
//-----------------------------------------------------------------------------

// Available Power Modes (Do Not Modify)
#define NORMAL         0x00
#define IDLE           0x01
#define SUSPEND        0x40
#define RTCSLEEP       0x88
#define SLEEP          0x80

#define STOP           0x02            // Note: Legacy Stop mode is not 
                                       // recommended since other low power
                                       // modes (such as Suspend and Sleep) 
                                       // place the device in a lower power
                                       // state and wake-up does not require
                                       // a system reset. Stop mode only 
                                       // disables the Precision Oscillator.
                                       // It will not reduce power consumption
                                       // any more than Idle mode if the 
                                       // low power oscillator is selected.



//-----------------------------------------------------------------------------
// System Clock Configuration
//-----------------------------------------------------------------------------

// Available Oscillators (Do Not Modify)
#define PRECISION_OSC  0x00
#define LOW_POWER_OSC  0x04
#define SMARTCLOCK_OSC  0x03
#define EXTERNAL_OSC    0x01

// Available Oscillator Divide Values (Do Not Modify)
#define OSCILLATOR_DIV_1    (0 << 4)
#define OSCILLATOR_DIV_2    (1 << 4)
#define OSCILLATOR_DIV_4    (2 << 4)
#define OSCILLATOR_DIV_8    (3 << 4)
#define OSCILLATOR_DIV_16   (4 << 4)
#define OSCILLATOR_DIV_32   (5 << 4)
#define OSCILLATOR_DIV_64   (6 << 4)
#define OSCILLATOR_DIV_128  (7 << 4)


//------------------------------------------------------------------------------
// Determine Compiler Type
//------------------------------------------------------------------------------
#define RAISONANCE 0x01
#define KEIL       0x02

#if defined __RC51__
   #define COMPILER RAISONANCE
#elif defined __C51__
   #define COMPILER KEIL
#else
   #error "Unsupported Compiler -- please use Raisonance or Keil"
#endif
   



