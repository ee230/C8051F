#ifndef  RT_PHY_H
#define  RT_PHY_H
//================================================================================================
// Phy.h
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
//-----------------------------------------------------------------------------
// EZRadioPRO Run Time PHY
// Version 2.0 Release Notes
//
// * Added automatic RX bandwidth calculator
// * Extended range to 256 kbps
// * Added optional 32-bit overflow checking
// * Added optional high modulation-index tables
// * Fully tested against known-good Test Vectors
//
// The parameters now must be set in a specific order.
// 1. Set the TRX_FREQUENCY
// 2. Set the TRX_DATA_RATE
// 3. Set the TRX_DEVIATION (optional)
// 4. Set the RX_BAND_WIDTH (optional)
// 5. Set the TRX_CHANNEL_SPACING (optional)
//
// Setting the data rate will automatically set the default
// deviation to one half the data rate (modulation index=1)
// and automatically calculate the required RX bandwidth.
//
// Manually setting the deviation will override default value
// and automatically calculate the required RX bandwidth.
// This must be done after setting the data rate.
//
// Manually setting the RX bandwidth will override the automatic
// RX bandwidth calculation. This must be done after setting the
// data rate and deviation.
//
//================================================================================================
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
#ifndef PHY_DEFS_H
#include "rtPhy_defs.h"
#endif
//-----------------------------------------------------------------------------
// Expected RADIO_VERSION code for radio VERSION register
// Production version should be 0x06
//-----------------------------------------------------------------------------
#define MIN_RADIO_VERSION   0x05
//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
#define SYSCLK_HZ                (24500000L)
#define MILLISECONDS(t)          (((SYSCLK_HZ/1000)*(t))/48)
//-----------------------------------------------------------------------------
// ET0 inverted and used as timeout flag
//-----------------------------------------------------------------------------
#define TIMEOUT_T0   (!ET0)
//------------------------------------------------------------------------------------------------
// PHY Variables accessed using phySet()/phyget()
//------------------------------------------------------------------------------------------------
enum
{
 TRX_FREQUENCY = 0,
 TRX_DATA_RATE,
 TRX_DEVIATION,
 RX_BAND_WIDTH,
 AFC_BAND_WIDTH,
 TRX_CHANNEL_SPACING
};
//------------------------------------------------------------------------------------------------
// phy data structure typedef - must match enum above
//------------------------------------------------------------------------------------------------
typedef struct rtPhySettingsStruct
{
 U32 TRxFrequency;
 U32 TRxDataRate;
 U32 TRxDeviation;
 U32 RxBandWidth;
 U32 AFCBandWidth;
 U32 TRxChannelSpacing;
} rtPhySettingsStruct;
//------------------------------------------------------------------------------------------------
// phy status values
//------------------------------------------------------------------------------------------------
#define PHY_STATUS U8
enum
{
   PHY_STATUS_SUCCESS = 0x00,
   PHY_STATUS_ERROR_SPI,
   PHY_STATUS_ERROR_UNSUPPORTED_RADIO,
   PHY_STATUS_ERROR_NO_IRQ,
   PHY_STATUS_ERROR_RADIO_XTAL,
   PHY_STATUS_ERROR_RADIO_SHUTDOWN,
   PHY_STATUS_ERROR_READ_ONLY_ADDRESS,
   PHY_STATUS_ERROR_INVALID_ADDRESS,
   PHY_STATUS_ERROR_INVALID_VALUE,
   PHY_STATUS_TRANSMIT_ERROR,
   PHY_STATUS_ERROR_NO_PACKET
};
//------------------------------------------------------------------------------------------------
// Public variables (API)
//------------------------------------------------------------------------------------------------
extern bit RxPacketReceived;
extern SEGMENT_VARIABLE (RxPacketLength, U8, BUFFER_MSPACE);
extern SEGMENT_VARIABLE (RxErrors, U8, BUFFER_MSPACE);
//------------------------------------------------------------------------------------------------
// Public Run Time PHY function prototypes (API)
//------------------------------------------------------------------------------------------------
PHY_STATUS    rtPhyInit (void);          // called once after MCU reset
PHY_STATUS    rtPhySet (U8, U32);
PHY_STATUS    rtPhyGet (U32 *);
PHY_STATUS    rtPhyInitRadio (void);
PHY_STATUS    rtPhyStandby (void);
PHY_STATUS    rtPhyIdle (void);
PHY_STATUS    rtPhyShutDown (void);
PHY_STATUS    rtPhyReStart (void);
PHY_STATUS    rtPhyTx (U8, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE));
PHY_STATUS    rtPhyRxOn (void);
PHY_STATUS    rtPhyRxOff (void);
PHY_STATUS    rtPhyGetRxPacket (U8*, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE));
//------------------------------------------------------------------------------------------------
// Public primitive phy function prototypes
//------------------------------------------------------------------------------------------------
void  phyWrite (U8, U8);
U8    phyRead (U8);
void  phyWriteFIFO (U8, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE));
//=================================================================================================
//=================================================================================================
#endif //RT_PHY_H