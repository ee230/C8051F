//-----------------------------------------------------------------------------
// T620_USB_Main.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This project implements a file transfer between host software and the USB
// firmware running on the device.  For the 'T620, the file is limited to
// 512 bytes and is stored in XRAM, so the file will be lost if power is
// removed.
//
// This firmware is intended to work with the Silabs USB Bulk File Transfer
// example, implementing two Bulk pipes with 64-byte Maximum transfers. The
// endpoints used are as follows:
//
// Endpoint1 IN - BULK IN
// Endpoint2 OUT - BULK OUT
//
// The areas that contain application-specific code are marked.  The bulk of
// the application-specific code is included in the "T620_USB_Main.c" and
// "T620_USB_ISR.c" files.
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
// Release 1.5
//    -All changes by ES
//    -4 OCT 2010
//    -Updated USB0_Init() to write 0x8F to CLKREC instead of 0x80
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
//    -Changed file name from main.c
//
// Release 1.2
//    -Initial Revision (JS/CS/JM)
//    -XX OCT 2003
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"            // SFR declarations
#include "T620_USB_Registers.h"
#include "T620_USB_Structs.h"
#include "T620_USB_Main.h"
#include "T620_USB_Descriptors.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

SBIT (LED1, SFR_P2, 2);                // LED='1' means ON
SBIT (LED2, SFR_P2, 3);                // Blink to indicate data transmission

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void USB0_Init (void);
void USB0_Enable (void);
void Port_Init (void);
void Oscillator_Init (void);
void VDD_MON_Init(void);               // Turn on VDD Monitor

//-----------------------------------------------------------------------------
// Globals Variables
//-----------------------------------------------------------------------------

DEVICE_STATUS    gDeviceStatus;
EP_STATUS        gEp0Status;
EP_STATUS        gEp1InStatus;
EP_STATUS        gEp2OutStatus;
EP0_COMMAND      gEp0Command;

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   Port_Init ();                       // Initialize Crossbar and GPIO
   Oscillator_Init ();                 // Initialize oscillator
   USB0_Init ();                       // Initialize USB0
   VDD_MON_Init();                     // Turn on VDD Monitor

   EA = 1;                             // Enable global interrupts

   USB0_Enable ();                     // Enable USB0

   while (1);
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock and USB clock.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   unsigned char delay = 100;

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // 12 MHz

   // This clock multipiler code is no longer necessary, but it is backwards
   // compatible with the 'F34x
   CLKMUL = 0x00;                      // Select internal oscillator as
                                       // input to clock multiplier

   CLKMUL |= 0x80;                     // Enable clock multiplier
   while (delay--);                    // Delay for >5us
   CLKMUL |= 0xC0;                     // Initialize the clock multiplier

   while(!(CLKMUL & 0x20));            // Wait for multiplier to lock

   CLKSEL |= USB_4X_CLOCK;             // Select USB clock
   CLKSEL |= SYS_4X_DIV_2;             // Select SYSCLK as Clock Multiplier/2
}

//-----------------------------------------------------------------------------
// VDD_MON_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the VDD monitor
//
//-----------------------------------------------------------------------------
void VDD_MON_Init(void)
{
   RSTSRC = 0x02;
}

//-----------------------------------------------------------------------------
// USB0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - USB0 left disabled
//
//-----------------------------------------------------------------------------
void USB0_Init (void)
{
   UWRITE_BYTE(POWER, 0x08);           // Asynch. reset

   UWRITE_BYTE(IN1IE, 0x0F);           // Enable Endpoint0 Interrupt
   UWRITE_BYTE(OUT1IE, 0x0F);
   UWRITE_BYTE(CMIE, 0x04);            // Enable Reset interrupt

   USB0XCN = 0xC0;                     // Enable transceiver
   USB0XCN |= FULL_SPEED;              // Select device speed

   UWRITE_BYTE(CLKREC, 0x8F);          // Enable clock recovery,
                                       // single-step mode disabled

   EIE1 |= 0x02;                       // Enable USB0 Interrupts
}

//-----------------------------------------------------------------------------
// USB0_Enable
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function enables the USB transceiver
//
//-----------------------------------------------------------------------------
void USB0_Enable (void)
{
   UWRITE_BYTE(POWER, 0x00);           // Enable USB0 by clearing the
                                       // USB Inhibit bit
                                       // Suspend mode disabled
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P2.2   digital   push-pull     LED
// P2.3   digital   push-pull     LED
//
//-----------------------------------------------------------------------------
void Port_Init(void)
{
   P2MDOUT |= 0x0C;                    // Port 2 pins 0,1 set high impedence

   LED1 = 0;                           // Start with both Leds off
   LED2 = 0;

   XBR0 = 0x00;
   XBR1 = 0x40;                        // Enable Crossbar
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------