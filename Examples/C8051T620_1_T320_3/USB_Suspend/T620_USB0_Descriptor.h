//-----------------------------------------------------------------------------
// T620_USB0_Descriptor.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Source file for USB firmware. Includes descriptor data.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051T620/1 or 'T320/1/2/3
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   T620_USB0_Suspend
//
// Release 1.0
//    -Initial Revision (PD / TP)
//    -15 SEP 2008
//

#ifndef  _USB_DESC_H_
#define  _USB_DESC_H_

#include "compiler_defs.h"

// All words sent to and received from the host are little endian, this is
// switched by software when neccessary.  These sections of code have been
// marked with "Compiler Specific" as above for easier modification

//------------------------------------------
// Standard Device Descriptor Type Defintion
//------------------------------------------
typedef /*code*/ struct
{
   unsigned char bLength;              // Size of this Descriptor in Bytes
   unsigned char bDescriptorType;      // Descriptor Type (=1)
   UU16 bcdUSB;                        // USB Spec Release Number in BCD
   unsigned char bDeviceClass;         // Device Class Code
   unsigned char bDeviceSubClass;      // Device Subclass Code
   unsigned char bDeviceProtocol;      // Device Protocol Code
   unsigned char bMaxPacketSize0;      // Maximum Packet Size for EP0
   UU16 idVendor;                      // Vendor ID
   UU16 idProduct;                     // Product ID
   UU16 bcdDevice;                     // Device Release Number in BCD
   unsigned char iManufacturer;        // Index of String Desc for Manufacturer
   unsigned char iProduct;             // Index of String Desc for Product
   unsigned char iSerialNumber;        // Index of String Desc for SerNo
   unsigned char bNumConfigurations;   // Number of possible Configurations
} device_descriptor;                   // End of Device Descriptor Type

//--------------------------------------------------
// Standard Configuration Descriptor Type Definition
//--------------------------------------------------
typedef /*code*/ struct
{
   unsigned char bLength;              // Size of this Descriptor in Bytes
   unsigned char bDescriptorType;      // Descriptor Type (=2)
   UU16 wTotalLength;                  // Total Length of Data for this Conf
   unsigned char bNumInterfaces;       // No of Interfaces supported by this
                                       // Conf
   unsigned char bConfigurationValue;  // Designator Value for *this*
                                       // Configuration
   unsigned char iConfiguration;       // Index of String Desc for this Conf
   unsigned char bmAttributes;         // Configuration Characteristics (see below)
   unsigned char bMaxPower;            // Max. Power Consumption in this
                                       // Conf (*2mA)
} configuration_descriptor;            // End of Configuration Descriptor Type

//----------------------------------------------
// Standard Interface Descriptor Type Definition
//----------------------------------------------
typedef /*code*/ struct
{
   unsigned char bLength;              // Size of this Descriptor in Bytes
   unsigned char bDescriptorType;      // Descriptor Type (=4)
   unsigned char bInterfaceNumber;     // Number of *this* Interface (0..)
   unsigned char bAlternateSetting;    // Alternative for this Interface (if any)
   unsigned char bNumEndpoints;        // No of EPs used by this IF (excl. EP0)
   unsigned char bInterfaceClass;      // Interface Class Code
   unsigned char bInterfaceSubClass;   // Interface Subclass Code
   unsigned char bInterfaceProtocol;   // Interface Protocol Code
   unsigned char iInterface;           // Index of String Desc for this Interface
} interface_descriptor;                // End of Interface Descriptor Type

//------------------------------------------
// Standard Class Descriptor Type Definition
//------------------------------------------
typedef /*code */struct
{
   unsigned char bLength;              // Size of this Descriptor in Bytes (=9)
   unsigned char bDescriptorType;      // Descriptor Type (HID=0x21)
   UU16 bcdHID;                        // HID Class Specification
                                       // release number (=1.01)
   unsigned char bCountryCode;         // Localized country code
   unsigned char bNumDescriptors;      // Number of class descriptors to follow
   unsigned char bReportDescriptorType;// Report descriptor type (HID=0x22)
   unsigned int wItemLength;           // Total length of report descriptor table
} class_descriptor;                    // End of Class Descriptor Type

//---------------------------------------------
// Standard Endpoint Descriptor Type Definition
//---------------------------------------------
typedef /*code*/ struct
{
   unsigned char bLength;              // Size of this Descriptor in Bytes
   unsigned char bDescriptorType;      // Descriptor Type (=5)
   unsigned char bEndpointAddress;     // Endpoint Address (Number + Direction)
   unsigned char bmAttributes;         // Endpoint Attributes (Transfer Type)
   UU16 wMaxPacketSize;                // Max. Endpoint Packet Size
   unsigned char bInterval;            // Polling Interval (Interrupt) ms
} endpoint_descriptor;                 // End of Endpoint Descriptor Type

//---------------------------------------------
// HID Configuration Descriptor Type Definition
//---------------------------------------------
// From "USB Device Class Definition for Human Interface Devices (HID)".
// Section 7.1:
// "When a Get_Descriptor(Configuration) request is issued,
// it returns the Configuration descriptor, all Interface descriptors,
// all Endpoint descriptors, and the HID descriptor for each interface."
typedef code struct {
  configuration_descriptor   hid_configuration_descriptor;
  interface_descriptor       hid_interface_descriptor;
  class_descriptor        hid_descriptor;
  endpoint_descriptor     hid_endpoint_in_descriptor;
  endpoint_descriptor     hid_endpoint_out_descriptor;
}
hid_configuration_descriptor;

#define HID_REPORT_DESCRIPTOR_SIZE 0x0032
#define HID_REPORT_DESCRIPTOR_SIZE_LE 0x3200



typedef code unsigned char hid_report_descriptor[HID_REPORT_DESCRIPTOR_SIZE];

//-----------------------------
// SETUP Packet Type Definition
//-----------------------------
typedef struct
{
   unsigned char bmRequestType;        // Request recipient, type, and dir.
   unsigned char bRequest;             // Specific standard request number
   UU16 wValue;                        // varies according to request
   UU16 wIndex;                        // varies according to request
   UU16 wLength;                       // Number of bytes to transfer
} setup_buffer;                        // End of SETUP Packet Type

#endif  /* _USB_DESC_H_ */

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
