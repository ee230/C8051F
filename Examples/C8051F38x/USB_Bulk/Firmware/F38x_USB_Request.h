//-----------------------------------------------------------------------------
// F38x_USB_Request.h
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Header file for USB firmware. Includes device request
// constants and masks.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051F38x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F38x_USB_Bulk
//
//
// Release 1.3
//    -All changes by GP
//    -21 NOV 2005
//    -Changed revision number to match project revision
//     No content changes to this file
//    -Modified file to fit new formatting guidelines
//    -Changed file name from usb_request.h
//
//
// Release 1.2
//    -Initial Revision (JS)
//    -XX AUG 2003
//

#ifndef  F38x_USB_REQUEST_H
#define  F38x_USB_REQUEST_H

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

// Standard Request Codes
#define  GET_STATUS              0x00
#define  CLEAR_FEATURE           0x01
#define  SET_FEATURE             0x03
#define  SET_ADDRESS             0x05
#define  GET_DESCRIPTOR          0x06
#define  SET_DESCRIPTOR          0x07
#define  GET_CONFIGURATION       0x08
#define  SET_CONFIGURATION       0x09
#define  GET_INTERFACE           0x0A
#define  SET_INTERFACE           0x0B
#define  SYNCH_FRAME             0x0C

// bmRequestType Masks
#define  CMD_MASK_DIR            0x80     // Request direction bit mask
#define  CMD_MASK_TYPE           0x60     // Request type bit mask
#define  CMD_MASK_RECIP          0x1F     // Request recipient bit mask
#define  CMD_MASK_COMMON         0xF0     // Common request mask

// bmRequestType Direction Field
#define  CMD_DIR_IN              0x80     // IN Request
#define  CMD_DIR_OUT             0x00     // OUT Request

// bmRequestType Type Field
#define  CMD_TYPE_STD            0x00     // Standard Request
#define  CMD_TYPE_CLASS          0x20     // Class Request
#define  CMD_TYPE_VEND           0x40     // Vendor Request

// bmRequestType Recipient Field
#define  CMD_RECIP_DEV           0x00     // Device Request
#define  CMD_RECIP_IF            0x01     // Interface Request
#define  CMD_RECIP_EP            0x02     // Endpoint Request
#define  CMD_RECIP_OTHER         0x03     // Other Request

// bmRequestType Common Commands
#define  CMD_STD_DEV_OUT         0x00     // Standard Device Request OUT
#define  CMD_STD_DEV_IN          0x80     // Standard Device Request IN
#define  CMD_STD_IF_OUT          0x01     // Standard Interface Request OUT
#define  CMD_STD_IF_IN           0x81     // Standard Interface Request IN

// Standard Descriptor Types
#define  DSC_DEVICE              0x01     // Device Descriptor
#define  DSC_CONFIG              0x02     // Configuration Descriptor
#define  DSC_STRING              0x03     // String Descriptor
#define  DSC_INTERFACE           0x04     // Interface Descriptor
#define  DSC_ENDPOINT            0x05     // Endpoint Descriptor

#define  DSC_MASK_REMOTE         0x20     // Remote Wakeup Support Mask
                                          // (bmAttributes Config Desc field)

// Feature Selectors (used in set and clear feature commands)
#define  DEVICE_REMOTE_WAKEUP    0x01     // Remote Wakeup selector
#define  ENDPOINT_HALT           0x00     // Endpoint Halt selector

#endif                                 // F34X_USB_REQUEST_H 

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------