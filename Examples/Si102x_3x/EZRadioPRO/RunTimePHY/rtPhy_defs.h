#ifndef  RT_PHY_DEFS_H
#define  RT_PHY_DEFS_H
//================================================================================================
// rtPhy_defs.h
//------------------------------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
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
//    EZRadioPRO Run Time PHY
//
// Release 2.0
//    - 04 NOV 2011
//
// This software must be used in accordance with the End User License Agreement.
//
//================================================================================================
// defines
//------------------------------------------------------------------------------------------------
// XTAL_TOLERANCE - used for automatic RX bandwidth calculation
//------------------------------------------------------------------------------------------------
#define XTAL_TOLERANCE 20
//------------------------------------------------------------------------------------------------
// BUFFER_MSPACE - defines memory space for TX and RX buffer pointers.
//------------------------------------------------------------------------------------------------
#define BUFFER_MSPACE                  SEG_PDATA
//------------------------------------------------------------------------------------------------
// TRANSMITTER_ONLY/RECEIVER_ONLY - reduces code size for TX ot RX only nodes.
//------------------------------------------------------------------------------------------------
//#define TRANSMITTER_ONLY
//#define RECEIVER_ONLY
//------------------------------------------------------------------------------------------------
// HIGH_MODULATION_INDEX_TABLES - adds additional tables to support high modulation index
//------------------------------------------------------------------------------------------------
//#define HIGH_MODULATION_INDEX_TABLES
//-----------------------------------------------------------------------------
// build option to check 32 bit math for overflow
//-----------------------------------------------------------------------------
//#define CHECK_32_BIT_MATH
//=================================================================================================
//=================================================================================================
#endif // RT_PHY_DEFS_H