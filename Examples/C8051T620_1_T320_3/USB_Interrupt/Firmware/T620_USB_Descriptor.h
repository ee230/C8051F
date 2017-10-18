//-----------------------------------------------------------------------------
// T620_USB_Descriptor.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Header file for USB firmware.  Defines standard descriptor structures.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051T620/1 or C8051T320/1/2/3
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   T620_USB_Interrupt
//
//
// Release 1.1
//    -All changes by TP
//    -14 AUG 2008
//    -Updated for 'T620/1
//
// Release 1.0
//    -Initial Revision (GP)
//    -22 NOV 2005
//    -Ported from 'F320_USB_Bulk
//

#ifndef  T620_USB_DESCRIPTORS_H
#define  T620_USB_DESCRIPTORS_H

#include "compiler_defs.h"

//-----------------------------------------------------------------------------
// Standard Device Descriptor Type Defintion
//-----------------------------------------------------------------------------
typedef code struct
{
   U8 bLength;                         // Size of this Descriptor in Bytes
   U8 bDescriptorType;                 // Descriptor Type (=1)
   UU16 bcdUSB;                        // USB Spec Release Number in BCD
   U8 bDeviceClass;                    // Device Class Code
   U8 bDeviceSubClass;                 // Device Subclass Code
   U8 bDeviceProtocol;                 // Device Protocol Code
   U8 bMaxPacketSize0;                 // Maximum Packet Size for EP0
   UU16 idVendor;                      // Vendor ID
   UU16 idProduct;                     // Product ID
   UU16 bcdDevice;                     // Device Release Number in BCD
   U8 iManufacturer;                   // Index of String Desc for Manufacturer
   U8 iProduct;                        // Index of String Desc for Product
   U8 iSerialNumber;                   // Index of String Desc for SerNo
   U8 bNumConfigurations;              // Number of possible Configurations
} device_descriptor;                   // End of Device Descriptor Type

//-----------------------------------------------------------------------------
// Standard Configuration Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef code struct
{
   U8 bLength;                         // Size of this Descriptor in Bytes
   U8 bDescriptorType;                 // Descriptor Type (=2)
   UU16 wTotalLength;                  // Total Length of Data for this Conf
   U8 bNumInterfaces;                  // # of Interfaces supported by Conf
   U8 bConfigurationValue;             // Designator Value for *this* Conf
   U8 iConfiguration;                  // Index of String Desc for this Conf
   U8 bmAttributes;                    // Configuration Characteristics
   U8 bMaxPower;                       // Max. Power Consumption in Conf (*2mA)
} configuration_descriptor;            // End of Configuration Descriptor Type

//-----------------------------------------------------------------------------
// Standard Interface Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef code struct
{
   U8 bLength;                         // Size of this Descriptor in Bytes
   U8 bDescriptorType;                 // Descriptor Type (=4)
   U8 bInterfaceNumber;                // Number of *this* Interface (0..)
   U8 bAlternateSetting;               // Alternative for this Interface
   U8 bNumEndpoints;                   // No of EPs used by this IF (excl. EP0)
   U8 bInterfaceClass;                 // Interface Class Code
   U8 bInterfaceSubClass;              // Interface Subclass Code
   U8 bInterfaceProtocol;              // Interface Protocol Code
   U8 iInterface;                      // Index of String Desc for Interface
} interface_descriptor;                // End of Interface Descriptor Type

//-----------------------------------------------------------------------------
// Standard Endpoint Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef code struct
{
   U8 bLength;                         // Size of this Descriptor in Bytes
   U8 bDescriptorType;                 // Descriptor Type (=5)
   U8 bEndpointAddress;                // Endpoint Address (Number + Direction)
   U8 bmAttributes;                    // Endpoint Attributes (Transfer Type)
   UU16 wMaxPacketSize;                // Max. Endpoint Packet Size
   U8 bInterval;                       // Polling Interval (Interrupt) ms
} endpoint_descriptor;                 // End of Endpoint Descriptor Type


//-----------------------------------------------------------------------------
// Setup Packet Type Definition
//------------------------------------------------------------------------------
typedef struct
{
   U8 bmRequestType;                   // Request recipient, type, direction
   U8 bRequest;                        // Specific standard request number
   UU16 wValue;                        // varies according to request
   UU16 wIndex;                        // varies according to request
   UU16 wLength;                       // Number of bytes to transfer
} setup_buffer;                        // End of Setup Packet Type

#endif                                 // T620_USB_DESCRIPTORS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------