//-----------------------------------------------------------------------------
// lcd_lib_const.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// This file contains global constants used throughout the application code
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
// Global Constants
//-----------------------------------------------------------------------------
		
// Return Codes
#define NO_ERROR                                0   
#define REGWRITE_VERIFY_FAILURE                -1  // FF
#define INCORRECT_BUS_INTERFACE_SELECTED       -2  // FE
#define RSTVAL_VERIFY_FAILURE                  -3  // FD
#define RAM_RW_ERROR                           -4  // FC
#define SMARTCLOCK_OSC_FAILURE                 -5  // FB
#define SMARTCLOCK_TIMEOUT                     -6  // FA
#define INVALID_ALARM_COUNT                    -7  
#define TIMER0_TIMEOUT                         -8
#define INVALID_OVERFLOW_COUNT                 -9
#define TIMER1_TIMEOUT                         -10
#define UNEXPECTED_SMARTCLOCK_EVENT            -11 

// Bus Interface Choices
#define SPI                                     1
#define SMBUS                                   2

// Development Board Choices
#define F930DK                                  0
#define F912DK                                  1
#define SI1000DK                                2
#define SI1050DK                                3
#define CUSTOM                                  4

// SPI choices
#define SPI0                                    0
#define SPI1                                    1

// SMBUS choices
#define TIMER0                                 0x00
#define TIMER1                                 0x01
#define TIMER2                                 0x02 
#define TIMER2_HB                              0x82
#define TIMER2_LB                              0x03

// LCD choices
#define VIM878                                 0x01
#define CUSTOMLCD                              0xFF

// SmaRTClock choices
#define CP240X_SELFOSC                          0
#define CP240X_CRYSTAL                          1
#define CP240X_CMOSCLK                          2

// Mux mode choices
#define STATIC                                 0x00
#define TWO_MUX                                0x02
#define THREE_MUX                              0x04
#define FOUR_MUX                               0x06

// Bias mode choices
#define HALF_BIAS                               1
#define ONETHIRD_BIAS                           0


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
