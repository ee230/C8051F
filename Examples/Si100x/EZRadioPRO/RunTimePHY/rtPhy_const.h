#ifndef  RT_PHY_CONST_H
#define  RT_PHY_CONST_H
//================================================================================================
// EZMacPro.h
//------------------------------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//
// Target:
//    Any Silicon Labs C8051 MCU.
//
// IDE:
//    Silicon Laboratories IDE   version 3.3
//
// Tool Chains:
//    Keil
//       c51.exe     version 8.0.8
//       bl51.exe    version 6.0.5
//    SDCC
//       sdcc.exe    version 2.8.0
//       aslink.exe  version 1.75
//
// Project Name:
//    Run Time PHY
//
// Release 2.0
//    - 04 NOV 2011
//
// This software must be used in accordance with the End User License Agreement.
//
//================================================================================================
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
//------------------------------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------------------------------
#define NUMBER_OF_INIT_REGISTERS                         7
#define NUMBER_OF_RX_BANDWIDTH_VALUES_LT2                54
#define NUMBER_OF_RX_BANDWIDTH_VALUES_LT10               54
#define NUMBER_OF_RX_BANDWIDTH_VALUES_GE10               53
//------------------------------------------------------------------------------------------------
// Structures
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// Public Variables (API)
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// Public code constants
//------------------------------------------------------------------------------------------------
extern SEGMENT_VARIABLE (rtPhyInitRegisters[NUMBER_OF_INIT_REGISTERS], U8, code);
extern SEGMENT_VARIABLE (rtPhyInitSettings[NUMBER_OF_INIT_REGISTERS], U8, code);
extern SEGMENT_VARIABLE (rtPhyTableRxBandwidthLT2[NUMBER_OF_RX_BANDWIDTH_VALUES_LT2], U16, code);
extern SEGMENT_VARIABLE (rtPhyTableIF_FilterSettingLT2[NUMBER_OF_RX_BANDWIDTH_VALUES_LT2], U8, code);
extern SEGMENT_VARIABLE (rtPhyTableRxBandwidthLT10[NUMBER_OF_RX_BANDWIDTH_VALUES_LT10], U16, code);
extern SEGMENT_VARIABLE (rtPhyTableIF_FilterSettingLT10[NUMBER_OF_RX_BANDWIDTH_VALUES_LT10], U8, code);
extern SEGMENT_VARIABLE (rtPhyTableRxBandwidthGE10[NUMBER_OF_RX_BANDWIDTH_VALUES_GE10], U16, code);
extern SEGMENT_VARIABLE (rtPhyTableIF_FilterSettingGE10[NUMBER_OF_RX_BANDWIDTH_VALUES_GE10], U8, code);
//------------------------------------------------------------------------------------------------
// Public function prototypes (API)
//------------------------------------------------------------------------------------------------
//=================================================================================================
//=================================================================================================
#endif // RT_PHY_CONST_H