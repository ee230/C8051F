//-----------------------------------------------------------------------------
// Si10xx_Config.h
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This header file sets the desired options to be used for measuring current.
//
//
// Target:         Si10xx
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (FB)
//    -27 APR 2010
//
//
//-----------------------------------------------------------------------------
// Start of Configuration Options
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Power Mode Selection
//-----------------------------------------------------------------------------
// 
// Select a Power Mode:
//  
//    NORMAL      - Clock Frequency has a significant impact on power
//    IDLE        - Clock Frequency has a significant impact on power
//    STOP        - See Note Below     
//    SUSPEND     - Use with Low Power Oscillator for best performance 
//    RTCSLEEP    - Same as Sleep, with SmaRTClock Enabled   
//    SLEEP       - Power gated off from all digital circuits. RAM preserved.   
//                         
// Note: Legacy Stop mode is not recommended since other low power modes 
//       (such as Suspend and Sleep) place the device in a lower power
//       state and wake-up does not require a system reset. Stop mode only 
//       disables the Precision Oscillator. Power will be identical to 
//       Suspend Mode when the Precision Oscillator is used. Power will be
//       identical to IDLE mode when the Low Power Oscillator is used.

#define POWER_MODE     NORMAL
 

// Set the constant below to '1' to disable the VBAT supply monitor on
// 'F912 and 'F902 devices
#define DISABLE_VBAT_SUPPLY_MONITOR  0

// Set the constant below to '1' to disable the VDD/DC+ supply monitor
// This can affect Normal, Idle, Stop, and Suspend mode current by
// approximately 10 to 15 uA.  Note: Once disabled, the VDD Monitor does
// not turn back on unless explicitly enabled or after a POR. It is not
// recommended to turn off the VDD Supply Monitor
#define DISABLE_VDD_SUPPLY_MONITOR   0

// Set the constant below to use the low frequency oscillator (instead of 
// the crystal on 16KB devices). On devices that do not have an
// LFO, the SmaRTClock oscillator will be configured into self oscillate mode

#define USE_SMARTCLOCK_LFO           0

//-----------------------------------------------------------------------------
// System Clock Configuration
//-----------------------------------------------------------------------------
//
// Choose a system clock configuration: 
//
// Available Oscillators:
//    PRECISION_OSC 
//    LOW_POWER_OSC 
//    SMARTCLOCK_OSC 
//    EXTERNAL_OSC
//
// Available Clock Divide Values:
//    OSCILLATOR_DIV_1
//    OSCILLATOR_DIV_2    
//    OSCILLATOR_DIV_4    
//    OSCILLATOR_DIV_8    
//    OSCILLATOR_DIV_16   
//    OSCILLATOR_DIV_32   
//    OSCILLATOR_DIV_64   
//    OSCILLATOR_DIV_128  
//
// Add one oscillator and one clock divide value. 
// For example:
//    #define CLOCK_SELECTION (LOW_POWER_OSC + OSCILLATOR_DIV_1) 
//    #define CLOCK_SELECTION (PRECISION_OSC + OSCILLATOR_DIV_1)    
// 
#define CLOCK_SELECTION  (PRECISION_OSC + OSCILLATOR_DIV_1)


// Additional Clocking Options:
// '0' for DISABLE, '1' for ENABLE
#define ENABLE_MISSING_CLOCK_DETECTOR      0 

// Specify the external CMOS clock frequency in Hz
// CMOS clock must be applied to P0.3 if using the EXTERNAL_OSC selection
#define EXTERNAL_OSC_FREQ 1000000

//-----------------------------------------------------------------------------
// End of F9xx Configuration Options
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Calculations and Error Checking (Do Not Modify This Section)
//-----------------------------------------------------------------------------
#define PRECISION_OSC_FREQ  24500000
#define LOW_POWER_OSC_FREQ  20000000
#define SMARTCLOCK_OSC_FREQ  32768
#if   ((CLOCK_SELECTION & 0x0F) == PRECISION_OSC)
   #define OSCILLATOR_BASE_FREQUENCY PRECISION_OSC_FREQ
#elif ((CLOCK_SELECTION & 0x0F) == LOW_POWER_OSC)
   #define OSCILLATOR_BASE_FREQUENCY LOW_POWER_OSC_FREQ
#elif ((CLOCK_SELECTION & 0x0F) == SMARTCLOCK_OSC)
   #define OSCILLATOR_BASE_FREQUENCY SMARTCLOCK_OSC_FREQ
#elif ((CLOCK_SELECTION & 0x0F) == EXTERNAL_OSC)
   #define OSCILLATOR_BASE_FREQUENCY EXTERNAL_OSC_FREQ
#else
   #error "INVALID OSCILLATOR SELECTED"
#endif
#if   ((CLOCK_SELECTION & 0xF0) == OSCILLATOR_DIV_1)
   #define CLOCK_DIVIDER 1
#elif ((CLOCK_SELECTION & 0xF0) == OSCILLATOR_DIV_2)
   #define CLOCK_DIVIDER 2
#elif ((CLOCK_SELECTION & 0xF0) == OSCILLATOR_DIV_4)
   #define CLOCK_DIVIDER 4
#elif ((CLOCK_SELECTION & 0xF0) == OSCILLATOR_DIV_8)
   #define CLOCK_DIVIDER 8
#elif ((CLOCK_SELECTION & 0xF0) == OSCILLATOR_DIV_16)
   #define CLOCK_DIVIDER 16
#elif ((CLOCK_SELECTION & 0xF0) == OSCILLATOR_DIV_32)
   #define CLOCK_DIVIDER 32
#elif ((CLOCK_SELECTION & 0xF0) == OSCILLATOR_DIV_64)
   #define CLOCK_DIVIDER 64
#elif ((CLOCK_SELECTION & 0xF0) == OSCILLATOR_DIV_128)
   #define CLOCK_DIVIDER 128
#endif
#define SYSCLK (OSCILLATOR_BASE_FREQUENCY/CLOCK_DIVIDER)
