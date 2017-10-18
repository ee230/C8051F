#ifndef  PP_PHY_H
#define  PP_PHY_H
//=============================================================================
// ppPhy.h
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//
// Target:
//    Si10xx.
//
// IDE:
//    Silicon Laboratories IDE
//
// Tool Chains:
//    Keil
//    SDCC
//    Raisonance
//
// Project Name:
//    PreProcessorPHY
//
// Version 2.0 - See release notes in ppPhy.c
//    - 08NOV2011
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
#ifndef PHY_DEFS_H
#include "ppPhy_defs.h"
#endif
//-----------------------------------------------------------------------------
// Expected RADIO_VERSION code for radio VERSION register
// Production version should be 0x06
//-----------------------------------------------------------------------------
#define MIN_RADIO_VERSION   0x05
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#define SYSCLK_HZ                (24500000L)
#define MILLISECONDS(t)          (((SYSCLK_HZ/1000)*(t))/48)
//-----------------------------------------------------------------------------
// ET0 inverted and used as timeout flag
//-----------------------------------------------------------------------------
#define TIMEOUT_T0   (!ET0)
//-----------------------------------------------------------------------------
// phy status values
//-----------------------------------------------------------------------------
#define PHY_STATUS U8
enum
{
   PHY_STATUS_SUCCESS = 0x00,
   PHY_STATUS_ERROR_SPI,
   PHY_STATUS_ERROR_UNSUPPORTED_RADIO,
   PHY_STATUS_ERROR_NO_IRQ,
   PHY_STATUS_ERROR_RADIO_XTAL,
   PHY_STATUS_ERROR_RADIO_SHUTDOWN,
   PHY_STATUS_TRANSMIT_ERROR,
   PHY_STATUS_ERROR_NO_PACKET
};
//-----------------------------------------------------------------------------
// Public variables (API)
//-----------------------------------------------------------------------------
extern bit RxPacketReceived;
extern SEGMENT_VARIABLE (RxPacketLength, U8, BUFFER_MSPACE);
extern SEGMENT_VARIABLE (RxErrors, U8, BUFFER_MSPACE);
//-----------------------------------------------------------------------------
// Public function prototypes (API)
//-----------------------------------------------------------------------------
PHY_STATUS  ppPhyInit (void);
PHY_STATUS  ppPhyInitRadio (void);
PHY_STATUS  ppPhyStandby (void);
PHY_STATUS  ppPhyIdle (void);
PHY_STATUS  ppPhyShutDown (void);
PHY_STATUS  ppPhyReStart (void);
PHY_STATUS  ppPhyTx (U8, VARIABLE_SEGMENT_POINTER(txBuffer, U8, BUFFER_MSPACE));
PHY_STATUS  ppPhyRxOn (void);
PHY_STATUS  ppPhyRxOff (void);
PHY_STATUS  ppPhyGetRxPacket (U8*, VARIABLE_SEGMENT_POINTER(rxBuffer, U8, BUFFER_MSPACE));
//-----------------------------------------------------------------------------
// Phy primitive readn and write - use with caution
//-----------------------------------------------------------------------------
void  phyWrite (U8, U8);
U8    phyRead (U8);
void  phyWriteFIFO (U8, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE));
void  phyReadFIFO (U8, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE));
//=============================================================================
// end PP_PHY_H
//=============================================================================
#endif //PP_PHY_H