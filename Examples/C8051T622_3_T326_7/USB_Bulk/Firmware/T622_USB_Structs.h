//-----------------------------------------------------------------------------
// T622_USB_Structs.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
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
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.42
// Command Line:   See Readme.txt
// Project Name:   T622_USB_Bulk
//
//
// Release 1.4
//    -All changes by TP
//    -10 SEP 2008
//    -Updated for 'T622/3
//
// Release 1.3
//    -All changes by GP
//    -21 NOV 2005
//    -Changed revision number to match project revision
//     No content changes to this file
//    -Modified file to fit new formatting guidelines
//    -Changed file name from usb_structs.h
//
// Release 1.2
//    -Initial Revision (JS)
//    -XX AUG 2003
//

#include "compiler_defs.h"
#include "T622_USB_Config.h"
#include "T622_USB_Descriptors.h"

#ifndef  T622_USB_STRUCTS_H
#define  T622_USB_STRUCTS_H

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

typedef struct IF_STATUS
{
   U8 bNumAlts;                        // Number of alternate choices for this
                                       // interface
   U8 bCurrentAlt;                     // Current alternate setting for this
                                       // interface:
                                       // Zero means this interface does not
                                       // exist or the device is not configured
   U8 bIfNumber;                       // Interface number for this interface
                                       // descriptor
} IF_STATUS;
typedef IF_STATUS * PIF_STATUS;


// Configuration status - only valid in configured state
// This data structure assumes a maximum of 2 interfaces for any given
// configuration, and a maximum of 4 interface descriptors (including
// all alternate settings).
typedef struct DEVICE_STATUS
{
   U8 bCurrentConfig;                  // Index number for the selected config
   U8 bDevState;                       // Current device state
   U8 bRemoteWakeupSupport;            // Does this device support remote
                                       // wakeup?
   U8 bRemoteWakeupStatus;             // Device remote wakeup enabled/disabled
   U8 bSelfPoweredStatus;              // Device self- or bus-powered
   U8 bNumInterf;                      // Number of interfaces for this
                                       // configuration
   U8 bTotalInterfDsc;                 // Total number of interface descriptors
                                       // for this configuration (includes alt.
                                       // descriptors)
   U8* pConfig;                        // Points to selected configuration desc
   IF_STATUS IfStatus[MAX_IF];         // Array of interface status structures
} DEVICE_STATUS;
typedef DEVICE_STATUS * PDEVICE_STATUS;

// Control endpoint command (from host)
typedef struct EP0_COMMAND
{
   U8 bmRequestType;                   // Request type
   U8 bRequest;                        // Specific request
   UU16 wValue;                        // Misc field
   UU16 wIndex;                        // Misc index
   UU16 wLength;                       // Length of the data segment for this
                                       // request
} EP0_COMMAND;

// Endpoint status (used for IN, OUT, and Endpoint0)
typedef struct EP_STATUS
{
   U8 bEp;                             // Endpoint number (address)
   U16 uNumBytes;                      // Number of bytes available to transmit
   U16 uMaxP;                          // Maximum packet size
   U8 bEpState;                        // Endpoint state
   void *pData;                        // Pointer to data to transmit
   UU16 wData;                         // Storage for small data packets
} EP_STATUS;
typedef EP_STATUS * PEP_STATUS;

// Descriptor structure
// This structure contains all usb descriptors for the device.
// The descriptors are held in array format, and are accessed with the offsets
// defined in the header file "usb_desc.h". The constants used in the
// array declarations are also defined in header file "usb_desc.h".
typedef struct DESCRIPTORS
{
   U8 bStdDevDsc[STD_DSC_SIZE];
   U8 bCfg1[CFG_DSC_SIZE + IF_DSC_SIZE*CFG1_IF_DSC + EP_DSC_SIZE*CFG1_EP_DSC];
} DESCRIPTORS;



#endif   // T622_USB_STRUCTS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
