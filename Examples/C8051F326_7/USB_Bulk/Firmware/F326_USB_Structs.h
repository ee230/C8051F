//-----------------------------------------------------------------------------
// F326_USB_Structs.h
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Header file for USB firmware. Includes all data structure definitions.
//
//
// How To Test:    See Readme.txt
//
//
// FID:            326000013
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

#include "F326_USB_Config.h"
#include "F326_USB_Descriptors.h"

#ifndef  F326_USB_STRUCTS_H
#define  F326_USB_STRUCTS_H

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#ifndef _BYTE_DEF_
#define _BYTE_DEF_
typedef unsigned char BYTE;
#endif   /* _BYTE_DEF_ */

#ifndef _UINT_DEF_
#define _UINT_DEF_
typedef unsigned int UINT;
#endif   /*_UINT_DEF_*/

#ifndef _WORD_DEF_
#define _WORD_DEF_
typedef union {unsigned int i; unsigned char c[2];} WORD;
#endif   /* _WORD_DEF_ */

typedef struct IF_STATUS {
   BYTE bNumAlts;             // Number of alternate choices for this
                              // interface
   BYTE bCurrentAlt;          // Current alternate setting for this interface
                              // zero means this interface does not exist
                              // or the device is not configured
   BYTE bIfNumber;            // Interface number for this interface
                              // descriptor
   } IF_STATUS;
typedef IF_STATUS * PIF_STATUS;


// Configuration status - only valid in configured state
// This data structure assumes a maximum of 2 interfaces for any given
// configuration, and a maximum of 4 interface descriptors (including
// all alternate settings).
typedef struct DEVICE_STATUS {
   BYTE bCurrentConfig;       // Index number for the selected config
   BYTE bDevState;            // Current device state
   BYTE bRemoteWakeupSupport; // Does this device support remote wakeup?
   BYTE bRemoteWakeupStatus;  // Device remote wakeup enabled/disabled
   BYTE bSelfPoweredStatus;   // Device self- or bus-powered
   BYTE bNumInterf;           // Number of interfaces for this configuration
   BYTE bTotalInterfDsc;      // Total number of interface descriptors for
                              // this configuration (includes alt.
                              // descriptors)
   BYTE* pConfig;             // Points to selected configuration desc
   IF_STATUS IfStatus[MAX_IF];// Array of interface status structures
   } DEVICE_STATUS;
typedef DEVICE_STATUS * PDEVICE_STATUS;

// Control endpoint command (from host)
typedef struct EP0_COMMAND {
   BYTE  bmRequestType;       // Request type
   BYTE  bRequest;            // Specific request
   WORD  wValue;              // Misc field
   WORD  wIndex;              // Misc index
   WORD  wLength;             // Length of the data segment for this request
  } EP0_COMMAND;

// Endpoint status (used for IN, OUT, and Endpoint0)
typedef struct EP_STATUS {
   BYTE  bEp;                 // Endpoint number (address)
   UINT  uNumBytes;           // Number of bytes available to transmit
   UINT  uMaxP;               // Maximum packet size
   BYTE  bEpState;            // Endpoint state
   void *pData;               // Pointer to data to transmit
   WORD  wData;               // Storage for small data packets
   } EP_STATUS;
typedef EP_STATUS * PEP_STATUS;

// Descriptor structure
// This structure contains all usb descriptors for the device.
// The descriptors are held in array format, and are accessed with the offsets
// defined in the header file "usb_desc.h". The constants used in the
// array declarations are also defined in header file "usb_desc.h".
typedef struct DESCRIPTORS {
   BYTE bStdDevDsc[STD_DSC_SIZE];
   BYTE bCfg1[CFG_DSC_SIZE + IF_DSC_SIZE*CFG1_IF_DSC + EP_DSC_SIZE*CFG1_EP_DSC];
} DESCRIPTORS;



#endif   // F326_USB_STRUCTS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
