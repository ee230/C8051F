#ifndef HARDWARE_DEFS_H
#define HARDWARE_DEFS_H
//=============================================================================
// hardware_defs.h
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//    This file is an alias for the hardware specific header file.
//
// Target:
//    Si100x
//
// IDE:
//    Silicon Laboratories IDE   version 3.3
//
// Tool Chains:
//    Keil
//       c51.exe     version 8.0.8
//       bl51.exe    version 6.0.5
//
// Project Name:
//    Si100x Code Examples
//
// Release 1.1
//    - TBD
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
//-----------------------------------------------------------------------------
// Define si100x variant in compiler command line
//
// SI1000_DAUGHTER_CARD - Si1000 and Si1001
// SI1002_DAUGHTER_CARD - Si1002 and Si1003
// SI1004_DAUGHTER_CARD - Si1004 and Si1005
// SI1010_DAUGHTER_CARD - Si1010 and Si1011
// SI1012_DAUGHTER_CARD - Si1012 and Si1013
// SI1014_DAUGHTER_CARD - Si1014 and Si1015
//
//-----------------------------------------------------------------------------
//#define SI1000_DAUGHTER_CARD
//#define SI1002_DAUGHTER_CARD
//#define SI1004_DAUGHTER_CARD
//#define SI1010_DAUGHTER_CARD
//#define SI1012_DAUGHTER_CARD
//#define SI1014_DAUGHTER_CARD
//=============================================================================
//-----------------------------------------------------------------------------
// Include Si100x_defs.h or Si101x_defs.h to defined hardware connections
//-----------------------------------------------------------------------------
#ifdef   SI1000_DAUGHTER_CARD
#include <Si1000_defs.h>
#elif defined SI1002_DAUGHTER_CARD
#include <Si1000_defs.h>
#elif defined  SI1004_DAUGHTER_CARD
#include <Si1000_defs.h>
#elif defined  SI1010_DAUGHTER_CARD
#include <Si1010_defs.h>
#elif defined  SI1012_DAUGHTER_CARD
#include <Si1010_defs.h>
#elif defined  SI1014_DAUGHTER_CARD
#include <Si1010_defs.h>
#else
#error "Please define SI10xx hardware on compiler command line"
#endif
//-----------------------------------------------------------------------------
// Use header file def to select NSS1
//-----------------------------------------------------------------------------
#ifdef SI1000_DEFS_H
SBIT(NSS1, SFR_P1, 4);                 // SI100x Internal Connection
#endif
#ifdef SI1010_DEFS_H
SBIT(NSS1, SFR_P1, 3);                 // SI101x Internal Connection
#endif
//-----------------------------------------------------------------------------
// Use daughter card def to select SDN
//-----------------------------------------------------------------------------
#ifdef SI1000_DAUGHTER_CARD
SBIT(SDN, SFR_P2, 6);
#endif
#ifdef SI1002_DAUGHTER_CARD
SBIT(SDN, SFR_P2, 6);
#endif
#ifdef SI1004_DAUGHTER_CARD
SBIT(SDN, SFR_P1, 6);
#endif
#ifdef SI1010_DAUGHTER_CARD
SBIT(SDN, SFR_P1, 6);
#endif
#ifdef SI1012_DAUGHTER_CARD
SBIT(SDN, SFR_P1, 6);
#endif
#ifdef SI1014_DAUGHTER_CARD
SBIT(SDN, SFR_P1, 6);
#endif
//-----------------------------------------------------------------------------
// Enable RF switch on high-power Si1000 and Si1010
//-----------------------------------------------------------------------------
#ifdef SI1000_DAUGHTER_CARD
#define ENABLE_RF_SWITCH
#endif
#ifdef SI1010_DAUGHTER_CARD
#define ENABLE_RF_SWITCH
#endif
//-----------------------------------------------------------------------------
// IRQ same on all daughter cards
//-----------------------------------------------------------------------------
SBIT(IRQ, SFR_P0, 1);                  // SI100x Daughter Card Connection
//-----------------------------------------------------------------------------
// OSC load cap value for XTAL on Si1000 Daughtercard
//-----------------------------------------------------------------------------
#define EZRADIOPRO_OSC_CAP_VALUE 0xD7
//-----------------------------------------------------------------------------
// MCU Motherboard LEDS and Switches
//-----------------------------------------------------------------------------
SBIT(SW02, SFR_P0, 2);
SBIT(SW03, SFR_P0, 3);
SBIT(LED15, SFR_P1, 5);
SBIT(LED16, SFR_P1, 6);
//-----------------------------------------------------------------------------
// Active Low LEDs
//-----------------------------------------------------------------------------
#define ILLUMINATE      0
#define EXTINGUISH      1
//=============================================================================
// End
//=============================================================================
#endif //HARDWARE_DEFS_H