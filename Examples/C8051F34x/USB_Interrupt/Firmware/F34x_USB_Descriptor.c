//-----------------------------------------------------------------------------
// F34x_USB_Descriptor.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
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
// FID:            34X000016
// Target:         C8051F34x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F34x_USB_Interrupt
//
// Release 1.0
//    -Initial Revision (GP)
//    -22 NOV 2005
//    -Ported from 'F320_USB_Bulk
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F34x_USB_Register.h"
#include "F34x_USB_Main.h"
#include "F34x_USB_Descriptor.h"

//-----------------------------------------------------------------------------
// Descriptor Declarations
//-----------------------------------------------------------------------------

const device_descriptor DeviceDesc =
{
   18,                  // bLength
   0x01,                // bDescriptorType
   0x1001,              // bcdUSB
   0x00,                // bDeviceClass
   0x00,                // bDeviceSubClass
   0x00,                // bDeviceProtocol
   EP0_PACKET_SIZE,     // bMaxPacketSize0
   0xC410,              // idVendor
   0x0000,              // idProduct
   0x0000,              // bcdDevice
   0x01,                // iManufacturer
   0x02,                // iProduct
   0x00,                // iSerialNumber
   0x01                 // bNumConfigurations
}; //end of DeviceDesc

const configuration_descriptor ConfigDesc =
{
   0x09,                // Length
   0x02,                // Type
   0x2000,              // Totallength
   0x01,                // NumInterfaces
   0x01,                // bConfigurationValue
   0x00,                // iConfiguration
   0x80,                // bmAttributes
   0x0F                 // MaxPower
}; //end of ConfigDesc

const interface_descriptor InterfaceDesc =
{
   0x09,                // bLength
   0x04,                // bDescriptorType
   0x00,                // bInterfaceNumber
   0x00,                // bAlternateSetting
   0x02,                // bNumEndpoints
   0x00,                // bInterfaceClass
   0x00,                // bInterfaceSubClass
   0x00,                // bInterfaceProcotol
   0x00                 // iInterface
}; //end of InterfaceDesc

const endpoint_descriptor Endpoint1Desc =
{
   0x07,                // bLength
   0x05,                // bDescriptorType
   0x81,                // bEndpointAddress
   0x03,                // bmAttributes
   EP1_PACKET_SIZE_LE,  // MaxPacketSize (LITTLE ENDIAN)
   10                   // bInterval
}; //end of Endpoint1Desc

const endpoint_descriptor Endpoint2Desc =
{
   0x07,                // bLength
   0x05,                // bDescriptorType
   0x02,                // bEndpointAddress
   0x03,                // bmAttributes
   EP2_PACKET_SIZE_LE,  // MaxPacketSize (LITTLE ENDIAN)
   10                   // bInterval
}; //end of Endpoint2Desc

#define STR0LEN 4

code const BYTE String0Desc[STR0LEN] =
{
   STR0LEN, 0x03, 0x09, 0x04
}; //end of String0Desc

#define STR1LEN sizeof("SILICON LABORATORIES INC.")*2

code const BYTE String1Desc[STR1LEN] =
{
   STR1LEN, 0x03,
   'S', 0,
   'I', 0,
   'L', 0,
   'I', 0,
   'C', 0,
   'O', 0,
   'N', 0,
   ' ', 0,
   'L', 0,
   'A', 0,
   'B', 0,
   'O', 0,
   'R', 0,
   'A', 0,
   'T', 0,
   'O', 0,
   'R', 0,
   'I', 0,
   'E', 0,
   'S', 0,
   ' ', 0,
   'I', 0,
   'N', 0,
   'C', 0,
   '.', 0
  }; //end of String1Desc

#define STR2LEN sizeof("C8051F340 Development Board")*2

code const BYTE String2Desc[STR2LEN] =
{
   STR2LEN, 0x03,
   'C', 0,
   '8', 0,
   '0', 0,
   '5', 0,
   '1', 0,
   'F', 0,
   '3', 0,
   '4', 0,
   '0', 0,
   ' ', 0,
   'D', 0,
   'e', 0,
   'v', 0,
   'e', 0,
   'l', 0,
   'o', 0,
   'p', 0,
   'm', 0,
   'e', 0,
   'n', 0,
   't', 0,
   ' ', 0,
   'B', 0,
   'o', 0,
   'a', 0,
   'r', 0,
   'd', 0
}; //end of String2Desc

BYTE* const StringDescTable[] =
{
   String0Desc,
   String1Desc,
   String2Desc
};

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------