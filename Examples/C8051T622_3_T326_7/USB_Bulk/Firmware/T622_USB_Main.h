//-----------------------------------------------------------------------------
// T622_USB_Main.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Main header file for USB firmware. Includes function prototypes,
// standard constants, device and endpoint state definitions.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051T622/2, 'T326/7
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.42
// Command Line:   See Readme.txt
// Project Name:   T622_USB_Bulk
//
// Release 1.4
//    -All changes by TP
//    -10 SEP 2008
//    -Updated for 'T622/3
//    -Reorganized function prototypes
//
// Release 1.3
//    -All changes by GP
//    -21 NOV 2005
//    -Changed revision number to match project revision
//     No content changes to this file
//    -Modified file to fit new formatting guidelines
//    -Changed file name from usb_main.h
//
// Release 1.2
//    -Initial Revision (JS)
//    -XX AUG 2003
//

#ifndef  T622_USB_MAIN_H
#define  T622_USB_MAIN_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "T622_USB_Structs.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

// Define standard constants
#define TRUE   1
#define FALSE  0
#define ON     1
#define OFF    0
#define REMOTE_WAKE_ON     2
#define REMOTE_WAKE_OFF    0
#define SELF_POWER_ON      1
#define SELF_POWER_OFF     0
#ifndef NULL
#define NULL   0
#endif

#define SYSCLK       24000000          // SYSCLK frequency in Hz

// USB clock selections (SFR CLKSEL)
#define USB_4X_CLOCK       0
#define USB_INT_OSC_DIV_2  0x10
#define USB_EXT_OSC        0x20
#define USB_EXT_OSC_DIV_2  0x30
#define USB_EXT_OSC_DIV_3  0x40
#define USB_EXT_OSC_DIV_4  0x50
#define USB_LFO            0x60

// System clock selections (SFR CLKSEL)
#define SYS_INT_OSC        0
#define SYS_EXT_OSC        0x01
#define SYS_4X_DIV_2       0x02
#define SYS_4X             0x03
#define SYS_LFO            0x04

// USB device speed settings
#define FULL_SPEED         0x20
#define LOW_SPEED          0x00

// Define endpoint status values
#define  EP_IDLE           0
#define  EP_TX             1
#define  EP_ERROR          2
#define  EP_HALTED         3
#define  EP_RX             4

// Define device states
#define  DEV_DEFAULT       0
#define  DEV_ADDRESS       1
#define  DEV_CONFIG        2

#define  EP_MASK_DIR       0x80

// Endpoint addresses
#define  EP1_IN            0x81
#define  EP1_OUT           0x01
#define  EP2_IN            0x82
#define  EP2_OUT           0x02
#define  EP3_IN            0x83
#define  EP3_OUT           0x03

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

// Functions in T622_USB_ISR.c
void Endpoint0 (void);
void BulkOut(PEP_STATUS);
void BulkIn (PEP_STATUS, U8 *, U16);
void USBReset (void);

// Functions in T622_USB_Standard_Requests.c
void SetAddressRequest (void);
void SetFeatureRequest (void);
void ClearFeatureRequest (void);
void SetConfigurationRequest (void);
void SetDescriptorRequest (void);
void SetInterfaceRequest (void);
void GetStatusRequest (void);
void GetDescriptorRequest (void);
void GetConfigurationRequest (void);
void GetInterfaceRequest (void);
void SynchFrameRequest (void);

// Functions in T622_USB_Utilities.c
U8 HaltEndpoint (U16 uEp);
U8 EnableEndpoint (U16 uEp);
U8 GetEpStatus (U16 uEp);
U8 SetConfiguration(U8);
U8 SetInterface(PIF_STATUS);
void FIFORead (U8, U16, U8*);
void FIFOWrite (U8 bEp, U16 uNumBytes, U8 * pData);

//void StdReq (PEP_STATUS);

#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
