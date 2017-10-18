#ifndef HARDWARE_DEFS_H
#define HARDWARE_DEFS_H
//=============================================================================
// hardware_defs.h
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//    This file is an alias for the hardware specific header file.
//
// Target:
//    Si102x/3x
//
// IDE:
//    Silicon Laboratories IDE
//
// Tool Chain:
//    Generic
//
// Project Name:
//    Si102x/3x Code Examples
//
// Release 1.0
//    - Initial release (MRF)
//    - 31 OCTOBER 2011
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif

//-----------------------------------------------------------------------------
// Define hardware
//=============================================================================
#define UDP_MLCD_SI1020
//#define UDP_EMIF_SI1020
//#define UDP_SLCD_SI1020
//#define UDP_MLCD_F960_PICO_SI4432
//#define UDP_EMIF_F960_PICO_SI4432
//#define UDP_SLCD_F960_PICO_SI4432

//-----------------------------------------------------------------------------
// Include Si102x_defs.h or EZRadioPRO_defs.h to defined hardware connections
//-----------------------------------------------------------------------------
#ifdef   UDP_MLCD_SI1020
#include <SI1020_defs.h>
#elif defined UDP_EMIF_SI1020
#include <SI1020_defs.h>
#elif defined UDP_SLCD_SI1020
#include <SI1020_defs.h>
#elif defined UDP_MLCD_F960_PICO_SI4432
#include <C8051F960_defs.h>
#include "EZRadioPRO_defs.h"
#elif defined UDP_EMIF_F960_PICO_SI4432
#include <C8051F960_defs.h>
#include "EZRadioPRO_defs.h"
#elif defined UDP_SLCD_F960_PICO_SI4432
#include <C8051F960_defs.h>
#include "EZRadioPRO_defs.h"
#else
#error "Please define F960/Si1020 hardware on compiler command line"
#endif

//-----------------------------------------------------------------------------
// Use header file def to select NSS1
//-----------------------------------------------------------------------------
#ifdef   UDP_MLCD_SI1020
SBIT(NSS1, SFR_P2, 3);
#elif defined UDP_EMIF_SI1020
SBIT(NSS1, SFR_P2, 3);
#elif defined UDP_SLCD_SI1020
SBIT(NSS1, SFR_P2, 3);
#elif defined UDP_MLCD_F960_PICO_SI4432
SBIT(NSS1, SFR_P2, 3);
#elif defined UDP_EMIF_F960_PICO_SI4432
SBIT(NSS1, SFR_P2, 3);
#elif defined UDP_SLCD_F960_PICO_SI4432
SBIT(NSS1, SFR_P2, 3);
#endif

//-----------------------------------------------------------------------------
// Use daughter card def to select SDN
//-----------------------------------------------------------------------------
#ifdef   UDP_MLCD_SI1020
SBIT(SDN, SFR_P1, 7);
#elif defined UDP_EMIF_SI1020
SBIT(SDN, SFR_P1, 7);
#elif defined UDP_SLCD_SI1020
SBIT(SDN, SFR_P1, 7);
#elif defined UDP_MLCD_F960_PICO_SI4432
SBIT(SDN, SFR_P1, 7);
#elif defined UDP_EMIF_F960_PICO_SI4432
SBIT(SDN, SFR_P1, 7);
#elif defined UDP_SLCD_F960_PICO_SI4432
SBIT(SDN, SFR_P1, 7);
#endif

//-----------------------------------------------------------------------------
// Enable RF switch on high-power
//
// This should be conditional if hardware definition doesn't use T/R switch.
// At this time, all hardware defined uses the switch.
//-----------------------------------------------------------------------------
#define ENABLE_RF_SWITCH

//-----------------------------------------------------------------------------
// IRQ
//-----------------------------------------------------------------------------
#ifdef   UDP_MLCD_SI1020
SBIT(IRQ, SFR_P1, 6);
#elif defined UDP_EMIF_SI1020
SBIT(IRQ, SFR_P1, 6);
#elif defined UDP_SLCD_SI1020
SBIT(IRQ, SFR_P1, 6);
#elif defined UDP_MLCD_F960_PICO_SI4432
SBIT(IRQ, SFR_P1, 6);
#elif defined UDP_EMIF_F960_PICO_SI4432
SBIT(IRQ, SFR_P1, 6);
#elif defined UDP_SLCD_F960_PICO_SI4432
SBIT(IRQ, SFR_P1, 6);
#endif

//-----------------------------------------------------------------------------
// OSC load cap value for XTAL on Si1000 Daughtercard
//-----------------------------------------------------------------------------
#define EZRADIOPRO_OSC_CAP_VALUE 0xD7

//-----------------------------------------------------------------------------
// MCU Motherboard LEDS and Switches
//-----------------------------------------------------------------------------
#ifdef   UDP_MLCD_SI1020
SBIT(SW1, SFR_P0, 0);
SBIT(SW2, SFR_P0, 1);
SBIT(SW3, SFR_P0, 2);
SBIT(SW4, SFR_P0, 3);
SBIT(LED1, SFR_P0, 0);
SBIT(LED2, SFR_P0, 1);
SBIT(LED3, SFR_P0, 2);
SBIT(LED4, SFR_P0, 3);
#elif defined UDP_EMIF_SI1020
SBIT(SW1, SFR_P3, 0);
SBIT(SW2, SFR_P3, 1);
SBIT(SW3, SFR_P3, 2);
SBIT(SW4, SFR_P3, 3);
SBIT(LED1, SFR_P3, 0);
SBIT(LED2, SFR_P3, 1);
SBIT(LED3, SFR_P3, 2);
SBIT(LED4, SFR_P3, 3);
#elif defined UDP_SLCD_SI1020
SBIT(SW1, SFR_P0, 0);
SBIT(SW2, SFR_P0, 1);
SBIT(SW3, SFR_P0, 2);
SBIT(SW4, SFR_P0, 3);
SBIT(LED1, SFR_P0, 0);
SBIT(LED2, SFR_P0, 1);
SBIT(LED3, SFR_P0, 2);
SBIT(LED4, SFR_P0, 3);
#elif defined UDP_MLCD_F960_PICO_SI4432
SBIT(SW1, SFR_P0, 0);
SBIT(SW2, SFR_P0, 1);
SBIT(SW3, SFR_P0, 2);
SBIT(SW4, SFR_P0, 3);
SBIT(LED1, SFR_P0, 0);
SBIT(LED2, SFR_P0, 1);
SBIT(LED3, SFR_P0, 2);
SBIT(LED4, SFR_P0, 3);
#elif defined UDP_EMIF_F960_PICO_SI4432
SBIT(SW1, SFR_P3, 0);
SBIT(SW2, SFR_P3, 1);
SBIT(SW3, SFR_P3, 2);
SBIT(SW4, SFR_P3, 3);
SBIT(LED1, SFR_P3, 0);
SBIT(LED2, SFR_P3, 1);
SBIT(LED3, SFR_P3, 2);
SBIT(LED4, SFR_P3, 3);
#elif defined UDP_SLCD_F960_PICO_SI4432
SBIT(SW1, SFR_P0, 0);
SBIT(SW2, SFR_P0, 1);
SBIT(SW3, SFR_P0, 2);
SBIT(SW4, SFR_P0, 3);
SBIT(LED1, SFR_P0, 0);
SBIT(LED2, SFR_P0, 1);
SBIT(LED3, SFR_P0, 2);
SBIT(LED4, SFR_P0, 3);
#endif

////-----------------------------------------------------------------------------
// Active Low LEDs
//-----------------------------------------------------------------------------
#define ILLUMINATE      0
#define EXTINGUISH      1

//=============================================================================
// End
//=============================================================================
#endif //HARDWARE_DEFS_H