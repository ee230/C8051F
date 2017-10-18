//-----------------------------------------------------------------------------
// F326_USB_Descriptor.h
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Header file for USB firmware. Includes descriptor array index
// definitions.
//
//
// How To Test:    See Readme.txt
//
//
// FID:            326000006
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

#ifndef  F326_USB_DESCRIPTORS_H
#define  F326_USB_DESCRIPTORS_H

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

// Descriptor sizes
#define STD_DSC_SIZE 18                   // Device
#define CFG_DSC_SIZE 9                    // Configuration
#define IF_DSC_SIZE 9                     // Interface
#define EP_DSC_SIZE 7                     // Endpoint

// Descriptor indicies - these are used to access specific fields
// of a descriptor array.

// Standard Device Descriptor Array Indicies
#define std_bLength 0                     // Length of this descriptor
#define std_bDescriptorType 1             // Device desc type (const. 0x01)
#define std_bcdUSB 2                      // USB Specification used (BCD)
#define std_bDeviceClass 4                // Device Class
#define std_bDeviceSubClass 5             // Device SubClass
#define std_bDeviceProtocol 6             // Device Protocol
#define std_bMaxPacketSize0 7             // Maximum packet size for Endpoint0
#define std_idVendor 8                    // Vendor ID 
#define std_idProduct 10                  // Product ID
#define std_bcdDevice 12                  // Device revision number
#define std_iManufacturer 14              // Manufacturer name string index
#define std_std_iProduct 15               // Product name string index
#define std_iSerialNumber 16              // Serial number string index
#define std_bNumConfigurations 17         // Number of supported configurations

// Configuration Descriptor Array Indicies
#define cfg_bLength 0                     // Length of this desc
#define cfg_bDescriptorType 1             // Config desc type (const. 0x02)
#define cfg_wTotalLength_lsb 2            // Total length, including
#define cfg_wTotalLength_msb 3            // interface & endpoints
#define cfg_bNumInterfaces 4              // Number of supported int's
#define cfg_bConfigurationValue 5         // Config index
#define cfg_iConfiguration 6              // Index for string desc
#define cfg_bmAttributes 7                // Power, wakeup options
#define cfg_MaxPower 8                    // Max bus power consumed

// Interface Descriptor Array Indicies
#define if_bLength 0                      // Length of this desc
#define if_bDescriptorType 1              // Interface desc type (const. ??  )
#define if_bInterfaceNumber 2             // This interface index
#define if_bAlternateSetting 3            // Alternate index
#define if_bNumEndpoints 4                // Endpoints used by this interface
#define if_bInterfaceClass 5              // Device class
#define if_bInterfaceSubClass 6           // Device subclass
#define if_bInterfaceProcotol 7           // Class-specific protocol
#define if_iInterface 8                   // Index for string desc

// Endpoint Descriptor Array Indicies
#define ep_bLength 0                      // Length of this desc
#define ep_bDescriptorType 1              // Endpoint desc type
#define ep_bEndpointAddress 2             // This endpoint address
#define ep_bmAttributes 3                 // Transfer type
#define ep_wMaxPacketSize 4               // Max FIFO size
#define ep_bInterval 6                    // Polling interval (int only)

#endif   // F326_USB_DESCRIPTORS_H 

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------