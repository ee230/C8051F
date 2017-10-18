//-----------------------------------------------------------------------------
// T620_USB0_InterruptServiceRoutine.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Header file for USB ISR, support routines, and variables.
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

#ifndef _USB_ISR_H_
#define _USB_ISR_H_

//#define _USB_LOW_SPEED_              // Change this comment to make
                                       // Full/Low speed

// All words sent to and received from the host are little endian, this is
// switched by software when neccessary.  These sections of code have been
// marked with "Compiler Specific" as above for easier modification

// Define Endpoint Packet Sizes
#ifdef _USB_LOW_SPEED_
#define  EP0_PACKET_SIZE         0x08  // This value can be 8,16,32,64
                                       // depending on device speed, see
                                       // USB spec
#else
#define  EP0_PACKET_SIZE         0x40
#endif /* _USB_LOW_SPEED_ */
#define  EP1_PACKET_SIZE         0x000A// Can range 0 - 1024 depending on data
                                       // and transfer type

#define  EP1_PACKET_SIZE_LE      0x0A00// IMPORTANT- this should be
                                       // Little-Endian version of
                                       //  EP1_PACKET_SIZE

#define  EP2_PACKET_SIZE         0x000A// Can range 0 - 1024 depending on data
                                       // and transfer type

#define  EP2_PACKET_SIZE_LE      0x0A00// IMPORTANT- this should be
                                       // Little-Endian version of
                                       // EP2_PACKET_SIZE

// Standard Descriptor Types
#define  DSC_DEVICE              0x01  // Device Descriptor
#define  DSC_CONFIG              0x02  // Configuration Descriptor
#define  DSC_STRING              0x03  // String Descriptor
#define  DSC_INTERFACE           0x04  // Interface Descriptor
#define  DSC_ENDPOINT            0x05  // Endpoint Descriptor

// HID Descriptor Types
#define DSC_HID              0x21   // HID Class Descriptor
#define DSC_HID_REPORT       0x22   // HID Report Descriptor

// Standard Request Codes
#define  GET_STATUS              0x00  // Code for Get Status
#define  CLEAR_FEATURE           0x01  // Code for Clear Feature
#define  SET_FEATURE             0x03  // Code for Set Feature
#define  SET_ADDRESS             0x05  // Code for Set Address
#define  GET_DESCRIPTOR          0x06  // Code for Get Descriptor
#define  SET_DESCRIPTOR          0x07  // Code for Set Descriptor(not used)
#define  GET_CONFIGURATION       0x08  // Code for Get Configuration
#define  SET_CONFIGURATION       0x09  // Code for Set Configuration
#define  GET_INTERFACE           0x0A  // Code for Get Interface
#define  SET_INTERFACE           0x0B  // Code for Set Interface
#define  SYNCH_FRAME             0x0C  // Code for Synch Frame(not used)

// HID Request Codes
#define GET_REPORT           0x01   // Code for Get Report
#define GET_IDLE          0x02   // Code for Get Idle
#define GET_PROTOCOL         0x03   // Code for Get Protocol
#define SET_REPORT           0x09   // Code for Set Report
#define SET_IDLE          0x0A   // Code for Set Idle
#define SET_PROTOCOL         0x0B   // Code for Set Protocol

// Define device states
#define  DEV_ATTACHED            0x00  // Device is in Attached State
#define  DEV_POWERED             0x01  // Device is in Powered State
#define  DEV_DEFAULT             0x02  // Device is in Default State
#define  DEV_ADDRESS             0x03  // Device is in Addressed State
#define  DEV_CONFIGURED          0x04  // Device is in Configured State
#define  DEV_SUSPENDED           0x05  // Device is in Suspended State

// Define bmRequestType bitmaps
#define  IN_DEVICE               0x00  // Request made to device,
                                       // direction is IN
#define  OUT_DEVICE              0x80  // Request made to device,
                                       // direction is OUT
#define  IN_INTERFACE            0x01  // Request made to interface,
                                       // direction is IN
#define  OUT_INTERFACE           0x81  // Request made to interface,
                                       // direction is OUT
#define  IN_ENDPOINT             0x02  // Request made to endpoint,
                                       // direction is IN
#define  OUT_ENDPOINT            0x82  // Request made to endpoint,
                                       // direction is OUT

// Define wIndex bitmaps
#define  IN_EP1                  0x81  // Index values used by Set and Clear
                                       // feature
#define  OUT_EP1                 0x01  // commands for Endpoint_Halt
#define  IN_EP2                  0x82
#define  OUT_EP2                 0x02

// Define wValue bitmaps for Standard Feature Selectors
#define  DEVICE_REMOTE_WAKEUP    0x01  // Remote wakeup feature(not used)
#define  ENDPOINT_HALT           0x00  // Endpoint_Halt feature selector

// Define Endpoint States
#define  EP_IDLE                 0x00  // This signifies Endpoint Idle State
#define  EP_TX                   0x01  // Endpoint Transmit State
#define  EP_RX                   0x02  // Endpoint Receive State
#define  EP_HALT                 0x03  // Endpoint Halt State (return stalls)
#define  EP_STALL                0x04  // Endpoint Stall (send procedural stall
                                       // next status phase)
#define  EP_ADDRESS              0x05  // Endpoint Address (change FADDR during
                                       // next status phase)
#define  EP_GetReport            0x06  // Special Control Endpoint State for
                                       // GetReport HID Request
#define  EP_SetReport            0x07  // Special Control Endpoint State for
                                       // SetReport HID Request

// Function prototypes
// USB Routines
void Force_Stall(void);                // Forces a procedural stall on
                                       // Endpoint 0
void Handle_In1(void);                 // used by SetConfiguration in
                                       // USB_STD_REQ to initialize
                                       // ReadyToTransfer

// Standard Requests
void Get_Status(void);                 // These are called for each specific
                                       // standard request
void Clear_Feature(void);
void Set_Feature(void);
void Set_Address(void);
void Get_Descriptor(void);
void Get_Configuration(void);
void Set_Configuration(void);
void Get_Interface(void);
void Set_Interface(void);

// HID Specific Requests
void Get_Report(void);
void Set_Report(void);
void Get_Idle(void);
void Set_Idle(void);
void Get_Protocol(void);
void Set_Protocol(void);
void SendPacket(unsigned char);

extern unsigned char EP_STATUS[];

extern void Usb_Suspend (void);

#endif      /* _USB_ISR_H_ */

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
