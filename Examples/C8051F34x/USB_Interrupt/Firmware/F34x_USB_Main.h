//-----------------------------------------------------------------------------
// F34x_USB_Main.h
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Main header file for USB firmware. Includes function prototypes,
// standard constants, and configuration constants.
//
//
// How To Test:    See Readme.txt
//
//
// FID:            34X000020
// Target:         C8051F34x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F34x_USB_Interrupt
//
//
// Release 1.0
//    -Initial Revision (GP)
//    -22 NOV 2005
//    -Ported from 'F320_USB_Bulk
//
#ifndef  F34x_USB_MAIN_H
#define  F34x_USB_MAIN_H

//#define _USB_LOW_SPEED_                    // Change this comment to
                                             // make Full/Low speed

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK                   12000000    // SYSCLK frequency in Hz

// USB clock selections (SFR CLKSEL)
#define USB_4X_CLOCK             0x00        // Select 4x clock multiplier,
                                             // for USB Full Speed
#define USB_INT_OSC_DIV_2        0x10        // See Oscillators in Datasheet
#define USB_EXT_OSC              0x20
#define USB_EXT_OSC_DIV_2        0x30
#define USB_EXT_OSC_DIV_3        0x40
#define USB_EXT_OSC_DIV_4        0x50

// System clock selections (SFR CLKSEL)
#define SYS_INT_OSC              0x00        // Select to use internal osc.
#define SYS_EXT_OSC              0x01        // Select to use external osc.
#define SYS_4X_DIV_2             0x02

// BYTE type definition
#ifndef _BYTE_DEF_
#define _BYTE_DEF_
typedef unsigned char BYTE;
#endif   // _BYTE_DEF_

// WORD type definition, for KEIL Compiler
#ifndef _WORD_DEF_                     // Compiler Specific, for Little Endian
#define _WORD_DEF_
typedef union {unsigned int i; unsigned char c[2];} WORD;

// All words sent to and received from the host are little endian, this is
// switched by software when necessary.  These sections of code have been
// marked with "Compiler Specific" as above for easier modification

#define LSB 1
#define MSB 0

#endif   // _WORD_DEF_

// Define Endpoint Packet Sizes
#ifdef _USB_LOW_SPEED_
#define  EP0_PACKET_SIZE         0x08        // This value can be 8,16,32,64
                                             // depending on device speed
#else
#define  EP0_PACKET_SIZE         0x40
#endif // _USB_LOW_SPEED_ 

// Can range 0 - 1024 depending on data and transfer type
#define  EP1_PACKET_SIZE         0x0008

// IMPORTANT- this should be Little-Endian version of EP1_PACKET_SIZE
#define  EP1_PACKET_SIZE_LE      0x0800

// Can range 0 - 1024 depending on data and transfer type
#define  EP2_PACKET_SIZE         0x0040

// IMPORTANT- this should be Little-Endian version of EP2_PACKET_SIZE
#define  EP2_PACKET_SIZE_LE      0x4000

// Standard Descriptor Types
#define  DSC_DEVICE          0x01      // Device Descriptor
#define  DSC_CONFIG          0x02      // Configuration Descriptor
#define  DSC_STRING          0x03      // String Descriptor
#define  DSC_INTERFACE       0x04      // Interface Descriptor
#define  DSC_ENDPOINT        0x05      // Endpoint Descriptor

// Standard Request Codes
#define  GET_STATUS          0x00      // Code for Get Status
#define  CLEAR_FEATURE       0x01      // Code for Clear Feature
#define  SET_FEATURE         0x03      // Code for Set Feature
#define  SET_ADDRESS         0x05      // Code for Set Address
#define  GET_DESCRIPTOR      0x06      // Code for Get Descriptor
#define  SET_DESCRIPTOR      0x07      // Code for Set Descriptor(not used)
#define  GET_CONFIGURATION   0x08      // Code for Get Configuration
#define  SET_CONFIGURATION   0x09      // Code for Set Configuration
#define  GET_INTERFACE       0x0A      // Code for Get Interface
#define  SET_INTERFACE       0x0B      // Code for Set Interface
#define  SYNCH_FRAME         0x0C      // Code for Synch Frame(not used)

// Define device states
#define  DEV_ATTACHED        0x00      // Device is in Attached State
#define  DEV_POWERED         0x01      // Device is in Powered State
#define  DEV_DEFAULT         0x02      // Device is in Default State
#define  DEV_ADDRESS         0x03      // Device is in Addressed State
#define  DEV_CONFIGURED      0x04      // Device is in Configured State
#define  DEV_SUSPENDED       0x05      // Device is in Suspended State

// Define bmRequestType bitmaps
#define  IN_DEVICE           0x00      // Request made to device,
#define  OUT_DEVICE          0x80      // Request made to device,
#define  IN_INTERFACE        0x01      // Request made to interface,
#define  OUT_INTERFACE       0x81      // Request made to interface,
#define  IN_ENDPOINT         0x02      // Request made to endpoint,
#define  OUT_ENDPOINT        0x82      // Request made to endpoint,

// Define wIndex bitmaps
#define  IN_EP1              0x81      // Index values used by Set and Clear
#define  OUT_EP1             0x01      // commands for Endpoint_Halt
#define  IN_EP2              0x82
#define  OUT_EP2             0x02

// Define wValue bitmaps for Standard Feature Selectors
#define  DEVICE_REMOTE_WAKEUP 0x01     // Remote wakeup feature(not used)
#define  ENDPOINT_HALT        0x00     // Endpoint_Halt feature selector

// Define Endpoint States
#define  EP_IDLE             0x00      // This signifies Endpoint Idle State
#define  EP_TX               0x01      // Endpoint Transmit State
#define  EP_RX               0x02      // Endpoint Receive State
#define  EP_HALT             0x03      // Endpoint Halt State (return stalls)
// Endpoint Stall (send procedural stall next status phase)
#define  EP_STALL            0x04
// Endpoint Address (change FADDR during next status phase)
#define  EP_ADDRESS          0x05

//-----------------------------------------------------------------------------
// Global Prototypes
//-----------------------------------------------------------------------------

// USB Routines
void Usb_Resume(void);                 // This routine resumes USB operation
void Usb_Reset(void);                  // Called after USB bus reset
void Handle_Setup(void);               // Handle setup packet on Endpoint 0
void Handle_In1(void);                 // Handle in packet on Endpoint 1
void Handle_Out2(void);                // Handle out packet on Endpoint 2
void Usb_Suspend(void);                // This routine called when suspend
                                       // signalling on bus

// Standard Requests
// These are called for each specific standard request
void Get_Status(void);
void Clear_Feature(void);
void Set_Feature(void);
void Set_Address(void);
void Get_Descriptor(void);
void Get_Configuration(void);
void Set_Configuration(void);
void Get_Interface(void);
void Set_Interface(void);

// Initialization Routines
void OSCILLATOR_Init(void);            // Initialize the system clock
void PORT_Init(void);                  // Configure ports
void USB0_Init(void);                  // Configure USB for Full/Low speed
void Timer2_Init(void);                // Timer 2 for use by ADC and swtiches
void ADC0_Init(void);                  // Configure ADC for continuous
                                       // conversion low-power mode

// Other Routines
void Timer2_ISR(void);                 // Checks if switches are pressed
void ADC0_ConvComplete_ISR(void);      // Upon Conversion, switch ADC MUX
void USB0_ISR(void);                   // Determines type of USB interrupt
void Force_Stall(void);                // Forces procedural stall on Endpoint 0
void Delay(void);                      // About 80 us/1 ms on Full/Low Speed

// Used for multiple byte reads of Endpoint fifos
void Fifo_Read (BYTE, unsigned int, BYTE *);
// Used for multiple byte writes of Endpoint fifos
void Fifo_Write (BYTE, unsigned int, BYTE *);

#endif                                 // F340_USB_Interrupt

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------