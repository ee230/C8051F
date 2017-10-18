//-----------------------------------------------------------------------------
// F326_USB_Main.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Source file for USB firmware. Includes main routine and
// all hardware initialization routines.
//
// This firmware is intended to work with the Silabs USB Bulk File Transfer example,
// implementing two Bulk pipes with 64-byte Maximum transfers. The endpoints
// used are as follows:
//
// Endpoint1 IN - BULK IN
// Endpoint1 OUT - BULK OUT
//
//
// How To Test:    See Readme.txt
//
//
// FID:            326000008
// Target:         C8051F326
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F326_USB_Bulk
//
//
// Release 1.1
//    -All changes by BW
//    -1 SEP 2010
//    -Updated USB0_Init() to write 0x89 to CLKREC instead of 0x80
//    -Removed RMW from RSTSRC write (RSTSRC = 0x02;)
// Release 1.0
//    -Initial Revision (DM)
//    -18 DEC 2005
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f326.h>                 // SFR declarations
#include "F326_USB_Registers.h"
#include "F326_USB_Structs.h"
#include "F326_USB_Main.h"
#include "F326_USB_Descriptors.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

sbit Led1 = P2^2;                      // LED='1' means ON
sbit Led2 = P2^3;                      // blink to indicate data transmission


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void USB0_Init (void);
void USB0_Enable (void);
void PORT_Init (void);
void SYSCLK_Init (void);
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
   PORT_Init ();                       // Initialize Crossbar and GPIO
   SYSCLK_Init ();                     // Initialize oscillator
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
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock and USB clock.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   unsigned char delay = 100;

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency

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

   UWRITE_BYTE(IN1IE, 0x03);           // Enable Endpoint0,1 Interrupts
   UWRITE_BYTE(OUT1IE, 0x02);          // Enable Endpoint 1 Interrupt
   UWRITE_BYTE(CMIE, 0x04);            // Enable Reset interrupt

   USB0XCN = 0xC0;                     // Enable transceiver
   USB0XCN |= FULL_SPEED;              // Select device speed

   UWRITE_BYTE(CLKREC, 0x89);          // Enable clock recovery,
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
// PORT_Init
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
void PORT_Init(void)
{  
   P2MDOUT	|=	0x0C;					// Port 2 pins 0,1 set high impedence
   Led1 = 0;							// Start with both Leds off
   Led2 = 0;			    
   GPIOCN = 0x40;                         // Enable Digital Inputs
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------