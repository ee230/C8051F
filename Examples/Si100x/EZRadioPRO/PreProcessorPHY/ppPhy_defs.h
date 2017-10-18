#ifndef  PHY_DEFS_H
#define  PHY_DEFS_H
//=============================================================================
// ppPhyDefs.h
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
// Modulation enumeration
//-----------------------------------------------------------------------------
#define FSK_MODULATION     2
#define GFSK_MODULATION    3
//-----------------------------------------------------------------------------
// Select GFSK or FSK Modulation here...
//-----------------------------------------------------------------------------
#define MODULATION_MODE    GFSK_MODULATION
//-----------------------------------------------------------------------------
// Frequency and Data rate settings
//-----------------------------------------------------------------------------
#define TRX_FREQUENCY         (915000000L)
#define TRX_DATA_RATE         (40000L)
//#define TRX_DEVIATION         (20000L)
//-----------------------------------------------------------------------------
// Optionally specify RX bandwidth or leave undefined for automatic RXBW.
//-----------------------------------------------------------------------------
//#define RX_BANDWIDTH          (80000L)
//-----------------------------------------------------------------------------
// Optional TRX_CHANNEL_SPACING
//-----------------------------------------------------------------------------
//#define TRX_CHANNEL_SPACING   (1000000L)
//-----------------------------------------------------------------------------
// XTAL_TOLERANCE (ppm) used to RX bandwidth.
//-----------------------------------------------------------------------------
#define XTAL_TOLERANCE 20
//-----------------------------------------------------------------------------
// BUFFER_MSPACE defines memory space for functions using buffer pointers.
//-----------------------------------------------------------------------------
#define BUFFER_MSPACE         SEG_XDATA
//-----------------------------------------------------------------------------
// Error tests
//-----------------------------------------------------------------------------
#if     (TRX_DATA_RATE>256000L)
#error  "TRX_DATA_RATE too high"
#endif
#if     (TRX_DATA_RATE<1200L)
#error  "TRX_DATA_RATE too low"
#endif
#ifdef   TRX_DEVIATION
#if     (TRX_DEVIATION>319375L)
#error  "TRX_DEVIATION too high"
#endif
#if     (TRX_DEVIATION<625L)
#error  "TRX_DEVIATION too low"
#endif
#endif
#if     (TRX_FREQUENCY>959999999L)
#error  "TRX_FREQUENCY too high"
#endif
#if     (TRX_FREQUENCY<240000000L)
#error  "TRX_FREQUENCY too low"
#endif
//=============================================================================
// End
//=============================================================================
#endif //PHY_DEFS_H