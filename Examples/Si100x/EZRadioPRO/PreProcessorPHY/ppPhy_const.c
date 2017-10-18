//=============================================================================
// ppPhy_const.c
//=============================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Target:
//    Any Silicon Labs C8051 MCU.
//
// IDE:
//    Silicon Laboratories IDE   Version 3.6
//
// Tool Chains:
//    Keil        9
//    SDCC        3
//    Raisonance  7
//
// Project Name:
//    PreProcessorPHY
//
// Version 2.0 - See release notes in ppPhy.c
//    - 08NOV2011
//
// This software must be used in accordance with the End User License
// Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Use #pragma SRC to check PHY settings against Excel calculator
//-----------------------------------------------------------------------------
//#pragma SRC
//-----------------------------------------------------------------------------
// Includes
//
// These includes must be in a specific order. Dependencies listed in comments.
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include "ppPhy_defs.h"
#include "ppPhy_const.h"
#include "hardware_defs.h"
//-----------------------------------------------------------------------------
// Phy Radio Register to be set
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (ppPhyRegisters[NUMBER_OF_REGISTERS], U8, code) =
{
   // Packet Handler Registers
   EZRADIOPRO_DATA_ACCESS_CONTROL,                                          // 0x30
   EZRADIOPRO_HEADER_CONTROL_1,                                             // 0x32
   EZRADIOPRO_HEADER_CONTROL_2,                                             // 0x33
   EZRADIOPRO_PREAMBLE_LENGTH,                                              // 0x34
   EZRADIOPRO_PREAMBLE_DETECTION_CONTROL,                                   // 0x35

   // Modulation Mode Registers
   EZRADIOPRO_MODULATION_MODE_CONTROL_1,                                    // 0x70
   EZRADIOPRO_MODULATION_MODE_CONTROL_2,                                    // 0x71

   //TRX Frequency Registers
   EZRADIOPRO_FREQUENCY_DEVIATION,                                          // 0x72
   EZRADIOPRO_FREQUENCY_BAND_SELECT,                                        // 0x75
   EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_1,                                  // 0x76
   EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_0,                                  // 0x77
   EZRADIOPRO_FREQUENCY_HOPPING_STEP_SIZE,                                  // 0x7A

   //TX Modem Registers
   EZRADIOPRO_TX_POWER,                                                     // 0x6D
   EZRADIOPRO_TX_DATA_RATE_1,                                               // 0x6E
   EZRADIOPRO_TX_DATA_RATE_0,                                               // 0x6F


    // RX Modem Registers
   EZRADIOPRO_IF_FILTER_BANDWIDTH,                                          // 0x1C
   EZRADIOPRO_AFC_LOOP_GEARSHIFT_OVERRIDE,                                  // 0x1D
   EZRADIOPRO_AFC_TIMING_CONTROL,                                           // 0x1E
   EZRADIOPRO_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE,                            // 0x1F
   EZRADIOPRO_CLOCK_RECOVERY_OVERSAMPLING_RATIO,                            // 0x20
   EZRADIOPRO_CLOCK_RECOVERY_OFFSET_2,                                      // 0x21
   EZRADIOPRO_CLOCK_RECOVERY_OFFSET_1,                                      // 0x22
   EZRADIOPRO_CLOCK_RECOVERY_OFFSET_0,                                      // 0x23
   EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1,                            // 0x24
   EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0,                            // 0x25
   EZRADIOPRO_AFC_LIMITER,                                                  // 0x2A
   EZRADIOPRO_CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE,                         // 0x58
   EZRADIOPRO_AGC_OVERRIDE_1,                                               // 0x69
};
//--------------------------------------------------------------------------------
// Preprocessor Calculated Constant Radio Settings
//--------------------------------------------------------------------------------
const SEGMENT_VARIABLE (ppPhySettings[NUMBER_OF_REGISTERS], U8, code) =
{
   0x8D, // 0x30 - SI4432_DATA_ACCESS_CONTROL - enable TX & RX packet handler, enable CRC
   0x00, // 0x32 - SI4432_HEADER_CONTROL_1 - no header
   0x02, // 0x33 - SI4432_HEADER_CONTROL_2 - 2 byte sync word, variable packet length
   0x0A, // 0x34 - SI4432_PREAMBLE_LENGTH - 10 nibbles, 40 bits
   0x28, // 0x35 - SI4432_PREAMBLE_DETECTION_CONTROL -  5 nibbles, 20 bits

   MODULATION_MODE_CONTROL_1_VALUE,                                     // 0x70
   MODULATION_MODE_CONTROL_2_VALUE,                                     // 0x71

   //TX Frequency Settings
   TX_DEVIATION_0_VALUE,                                                // 0x72
   FREQUENCY_BAND_SELECT_VALUE,                                         // 0x75
   NOMINAL_CARRIER_FREQUENCY_1_VALUE,                                   // 0x76
   NOMINAL_CARRIER_FREQUENCY_0_VALUE,                                   // 0x77
   FREQUENCY_HOPPING_STEP_SIZE_VALUE,                                   // 0x7A

   //TX Modem Settings
   TX_POWER_VALUE,                                                      // 0x6D
   TX_DATA_RATE_1_VALUE,                                                // 0x6E
   TX_DATA_RATE_0_VALUE,                                                // 0x6F

   // RX Modem Settings
   IF_FILTER_BANDWIDTH,                                                 // 0x1C
   AFC_LOOP_GEARSHIFT_OVERRIDE_VALUE,                                   // 0x1D
   AFC_TIMING_CONTROL_VALUE,                                            // 0x1E
   CLOCK_RECOVERY_GEARSHIFT_OVERRIDE_VALUE,                             // 0x1F
   CLOCK_RECOVERY_OVERSAMPLING_RATIO_0_VALUE,                           // 0x20
   CLOCK_RECOVERY_OFFSET_2_VALUE,                                       // 0x21
   CLOCK_RECOVERY_OFFSET_1_VALUE,                                       // 0x22
   CLOCK_RECOVERY_OFFSET_0_VALUE,                                       // 0x23
   CLOCK_RECOVERY_TIMING_LOOP_GAIN_1_VALUE,                             // 0x24
   CLOCK_RECOVERY_TIMING_LOOP_GAIN_0_VALUE,                             // 0x25
   ACF_LIMIT_VALUE,                                                     // 0x2A
   CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE_VALUE,                          // 0x58
   AGC_OVERRIDE_1_VALUE,                                                // 0x69
};
//==============================================================================
// End
//==============================================================================

