//-----------------------------------------------------------------------------
// F326_USB_Main.h
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
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
// FID:            326000009
// Target:         C8051F326
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F326_USB_Bulk
//
//
// Release 1.0
//    -Initial Revision (DM)
//    -18 DEC 2005
//

#ifndef  F326_USB_MAIN_H
#define  F326_USB_MAIN_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F326_USB_Structs.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#ifndef _BYTE_DEF_
#define _BYTE_DEF_
typedef unsigned char BYTE;
#endif   /* _BYTE_DEF_ */

#ifndef _WORD_DEF_
#define _WORD_DEF_
typedef union {unsigned int i; unsigned char c[2];} WORD;
#endif   /* _WORD_DEF_ */

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

// System clock selections (SFR CLKSEL)
#define SYS_INT_OSC        0
#define SYS_EXT_OSC        0x01
#define SYS_4X_DIV_2       0x02

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

void USBReset (void);                     // usb_reset.c
void Endpoint0 ();                        // usb_endpoint.c
void BulkOrInterruptOut(PEP_STATUS);      //
void BulkOrInterruptIn (PEP_STATUS, BYTE *, UINT);
void StdReq (PEP_STATUS);                 // usb_stdreq.c
BYTE SetConfiguration(BYTE);              // usb_utils.c
BYTE SetInterface(PIF_STATUS);            //
BYTE HaltEndpoint (UINT uEp);             //
BYTE EnableEndpoint (UINT uEp);           //
BYTE GetEpStatus (UINT uEp);              //
void FIFORead (BYTE, UINT, BYTE*);        //
void FIFOWrite (BYTE bEp, UINT uNumBytes, BYTE * pData);

// Standard Device Request Routine prototypes
void SetAddressRequest (void);            // usb_stdreq.c
void SetFeatureRequest (void);            //
void ClearFeatureRequest (void);          //
void SetConfigurationRequest (void);      //
void SetDescriptorRequest (void);         //
void SetInterfaceRequest (void);          //
void GetStatusRequest (void);             //
void GetDescriptorRequest (void);         //
void GetConfigurationRequest (void);      //
void GetInterfaceRequest (void);          //
void SynchFrameRequest (void);            //

void Page_Erase(BYTE*);
void Page_Write(BYTE*);

#endif                                 // F326_USB_MAIN_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
