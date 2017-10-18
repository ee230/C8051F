//-----------------------------------------------------------------------------
// app_config.h
//-----------------------------------------------------------------------------
// Copyright (C) 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Configures the desired application behavior
//
// Target:         C8051F93x-C8051F90x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    -26 OCT 2009
//

//-----------------------------------------------------------------------------
// Global Configuration Options
//-----------------------------------------------------------------------------

// The global <EXAMPLE> constant can be one of the following values:
// 
//    IMEASURE_SHUTDOWN  -- For measuring Shutdown Current
//    IMEASURE_ULP_SMARTCLOCK -- For measuring ULP SmaRTClock mode current
//    IMEASURE_ULP_LCD_NOTEXT -- For measuring ULP LCD mode current (Low)
//    IMEASURE_ULP_LCD_FULLTEXT -- For measuring ULP LCD mode current (High)
//    DISPLAY_DIGITS -- Displays a sequence of digits on the screen
//    DISPLAY_VOLTAGE -- Displays a voltage on the screen (VDD or P0.6 Voltage)

#define EXAMPLE  DISPLAY_DIGITS


// The global <CHANNEL_SELECT> constant can be one of the following values:
// This constant is only used when EXAMPLE is set to DISPLAY_VOLTAGE
//
//    POT -- For measuring the potentiometer (P0.6) voltage
//    VDD -- For measuring the supply voltage

#define CHANNEL_SELECT                POT
