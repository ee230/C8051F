//-----------------------------------------------------------------------------
// T620_USB_Descriptors.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This file includes descriptor data structures (device, configuration,
// interface, endpoint).
//
// This file may need to change depending on the application.  However, most of
// the data should stay the same.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051T620/1 or C8051T320/1/2/3
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   T620_USB_Bulk
//
//
// Release 1.4
//    -All changes by TP
//    -01 JUL 2008
//    -Updated for 'T620/1 by moving the file storage to XRAM and limiting the
//      size to 512 bytes
//
// Release 1.3
//    -All changes by GP
//    -21 NOV 2005
//    -Changed revision number to match project revision
//     No content changes to this file
//    -Modified file to fit new formatting guidelines
//    -Changed file name from usb_desc.c
//
// Release 1.2
//    -Initial Revision (JS/CS/JM)
//    -XX OCT 2003
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "T620_USB_Main.h"
#include "T620_USB_Structs.h"
#include "T620_USB_Registers.h"
#include "T620_USB_Request.h"
#include "T620_USB_Descriptors.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

// All descriptors are contained in the global structure <gDescriptorMap>.
// This structure contains BYTE arrays for the standard device descriptor
// and all configurations. The lengths of the configuration arrays are
// defined by the number of interface and endpoint descriptors required
// for the particular configuration (these constants are named
// CFG1_IF_DSC and CFG1_EP_DSC for configuration1).
//
// The entire gDescriptorMap structure is initialized below in
// codespace.

DESCRIPTORS code gDescriptorMap = {

//---------------------------
// Begin Standard Device Descriptor (structure element stddevdsc)
//---------------------------
   18,                        // bLength
   0x01,                      // bDescriptorType
   0x00, 0x02,                // bcdUSB (lsb first)
   0x00,                      // bDeviceClass
   0x00,                      // bDeviceSubClass
   0x00,                      // bDeviceProtocol
   64,                        // bMaxPacketSize0
   0xC4, 0x10,                // idVendor (lsb first)
   0x03, 0x00,                // idProduct (lsb first)
   0x00, 0x00,                // bcdDevice (lsb first)
   0x00,                      // iManufacturer
   0x00,                      // iProduct
   0x00,                      // iSerialNumber
   0x01,                      // bNumConfigurations

//---------------------------
// Begin Configuration 1 (structure element cfg1)
//---------------------------

   // Begin Descriptor: Configuration 1
   0x09,                      // Length
   0x02,                      // Type
   0x20, 0x00,                // TotalLength (lsb first)
   0x01,                      // NumInterfaces
   0x01,                      // bConfigurationValue
   0x00,                      // iConfiguration
   0x80,                      // bmAttributes (no remote wakeup)
   0x0F,                      // MaxPower (*2mA)

   // Begin Descriptor: Interface0, Alternate0
   0x09,                      // bLength
   0x04,                      // bDescriptorType
   0x00,                      // bInterfaceNumber
   0x00,                      // bAlternateSetting
   0x02,                      // bNumEndpoints
   0x00,                      // bInterfaceClass
   0x00,                      // bInterfaceSubClass
   0x00,                      // bInterfaceProcotol
   0x00,                      // iInterface

   // Begin Descriptor: Endpoint1, Interface0, Alternate0
   0x07,                      // bLength
   0x05,                      // bDescriptorType
   0x81,                      // bEndpointAddress (ep1, IN)
   0x02,                      // bmAttributes (Bulk)
   0x40, 0x00,                // wMaxPacketSize (lsb first)
   0x05,                      // bInterval

   // Begin Descriptor: Endpoint2, Interface0, Alternate0
   0x07,                      // bLength
   0x05,                      // bDescriptorType
   0x02,                      // bEndpointAddress (ep2, OUT)
   0x02,                      // bmAttributes (Bulk)
   0x40, 0x00,                // wMaxPacketSize (lsb first)
   0x05,                      // bInterval

//---------------------------
// End Configuration 1
//---------------------------

};

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------