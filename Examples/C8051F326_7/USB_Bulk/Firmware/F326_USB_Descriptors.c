//-----------------------------------------------------------------------------
// F326_USB_Descriptors.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Source file for USB firmware. Includes descriptor data structures
// (device, configuration, interface, endpoint).
//
//
// How To Test:    See Readme.txt
//
//
// FID:            326000005
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

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F326_USB_Main.h"
#include "F326_USB_Structs.h"
#include "F326_USB_Registers.h"
#include "F326_USB_Request.h"
#include "F326_USB_Descriptors.h"

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
   0x01,                      // bEndpointAddress (ep1, OUT)
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