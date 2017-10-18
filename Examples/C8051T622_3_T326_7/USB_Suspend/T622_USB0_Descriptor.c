//-----------------------------------------------------------------------------
// T622_USB0_Descriptor.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
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
// Target:         C8051T622/3 or 'T326/7
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   T622_USB0_Suspend
//
// Release 1.0
//    -Initial Revision (PD / TP)
//    -15 SEP 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "T622_USB0_Register.h"
#include "T622_USB0_InterruptServiceRoutine.h"
#include "T622_USB0_Descriptor.h"

//-----------------------------------------------------------------------------
// Descriptor Declarations
//-----------------------------------------------------------------------------

const device_descriptor code DEVICEDESC =
{
   18,                                 // bLength
   0x01,                               // bDescriptorType
   0x1001,                             // bcdUSB
   0x00,                               // bDeviceClass
   0x00,                               // bDeviceSubClass
   0x00,                               // bDeviceProtocol
   EP0_PACKET_SIZE,                    // bMaxPacketSize0
   0xC410,                             // idVendor
   0xB981,                             // idProduct
   0x0000,                             // bcdDevice
   0x01,                               // iManufacturer
   0x02,                               // iProduct
   0x00,                               // iSerialNumber
   0x01                                // bNumConfigurations
}; //end of DEVICEDESC

// From "USB Device Class Definition for Human Interface Devices (HID)".
// Section 7.1:
// "When a Get_Descriptor(Configuration) request is issued,
// it returns the Configuration descriptor, all Interface descriptors,
// all Endpoint descriptors, and the HID descriptor for each interface."
const hid_configuration_descriptor code HIDCONFIGDESC =
{

{ // configuration_descriptor hid_configuration_descriptor
   0x09,                               // Length
   0x02,                               // Type
   0x2200,                             // Totallength (= 9+9+9+7)
   0x01,                               // NumInterfaces
   0x01,                               // bConfigurationValue
   0x00,                               // iConfiguration
   0x80,                               // bmAttributes
   0x20                                // MaxPower (in 2mA units)
},

{ // interface_descriptor hid_interface_descriptor
   0x09,                               // bLength
   0x04,                               // bDescriptorType
   0x00,                               // bInterfaceNumber
   0x00,                               // bAlternateSetting
   0x01,                               // bNumEndpoints
   0x03,                               // bInterfaceClass (3 = HID)
   0x01,                               // bInterfaceSubClass
   0x02,                               // bInterfaceProcotol
   0x00                                // iInterface
},

{ // class_descriptor hid_descriptor
  0x09,                                // bLength
  0x21,                                // bDescriptorType
  0x0101,                              // bcdHID
  0x00,                                // bCountryCode
  0x01,                                // bNumDescriptors
  0x22,                                // bDescriptorType
  HID_REPORT_DESCRIPTOR_SIZE_LE        // wItemLength (tot. len. of report
                                       // descriptor)
},

// IN endpoint (mandatory for HID)
{ // endpoint_descriptor hid_endpoint_in_descriptor
   0x07,                               // bLength
   0x05,                               // bDescriptorType
   0x81,                               // bEndpointAddress
   0x03,                               // bmAttributes
   EP1_PACKET_SIZE_LE,                 // MaxPacketSize (LITTLE ENDIAN)
   10                                  // bInterval
},

// OUT endpoint (optional for HID)
{ // endpoint_descriptor hid_endpoint_out_descriptor
   0x07,                               // bLength
   0x05,                               // bDescriptorType
   0x01,                               // bEndpointAddress
   0x03,                               // bmAttributes
   EP2_PACKET_SIZE_LE,                 // MaxPacketSize (LITTLE ENDIAN)
   10                                  // bInterval
}

};

const hid_report_descriptor code HIDREPORTDESC =
{
    0x05, 0x01,                        // Usage Page (Generic Desktop)
    0x09, 0x02,                        // Usage (Mouse)
    0xA1, 0x01,                        // Collection (Application)
    0x09, 0x01,                        //   Usage (Pointer)
    0xA1, 0x00,                        //   Collection (Physical)
    0x05, 0x09,                        //     Usage Page (Buttons)
    0x19, 0x01,                        //     Usage Minimum (01)
    0x29, 0x01,                        //     Usage Maximum (01)
    0x15, 0x00,                        //     Logical Minimum (0)
    0x25, 0x01,                        //     Logical Maximum (1)
    0x95, 0x01,                        //     Report Count (1)
    0x75, 0x01,                        //     Report Size (1)
    0x81, 0x02,                        //     Input (Data, Variable, Absolute)
    0x95, 0x01,                        //     Report Count (1)
    0x75, 0x07,                        //     Report Size (7)
    0x81, 0x01,                        //     Input (Constant) for padding
    0x05, 0x01,                        //     Usage Page (Generic Desktop)
    0x09, 0x30,                        //     Usage (X)
    0x09, 0x31,                        //     Usage (Y)
    0x15, 0x81,                        //     Logical Minimum (-127)
    0x25, 0x7F,                        //     Logical Maximum (127)
    0x75, 0x08,                        //     Report Size (8)
    0x95, 0x02,                        //     Report Count (2)
    0x81, 0x06,                        //     Input (Data, Variable, Relative)
    0xC0,                              //   End Collection (Physical)
    0xC0                               // End Collection (Application)
};

#define STR0LEN 4

code unsigned char String0Desc [STR0LEN] =
{
   STR0LEN, 0x03, 0x09, 0x04
}; //end of String0Desc

#define STR1LEN sizeof ("SILICON LABORATORIES") * 2

code unsigned char String1Desc [STR1LEN] =
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
   'S', 0
}; //end of String1Desc

#define STR2LEN sizeof ("C8051T622 Development Board") * 2

code unsigned char String2Desc [STR2LEN] =
{
   STR2LEN, 0x03,
   'C', 0,
   '8', 0,
   '0', 0,
   '5', 0,
   '1', 0,
   'T', 0,
   '6', 0,
   '2', 0,
   '2', 0,
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

unsigned char* const STRINGDESCTABLE [] =
{
   String0Desc,
   String1Desc,
   String2Desc
};

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
