//-----------------------------------------------------------------------------
// F380_DEFAULT_CustomApp.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Stub file for Firmware Template.
//
//
// How To Test:    See Readme.txt
//
//
// FID
// Target:         C8051F3xx
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_FirmwareTemplate
//
// Release 1.0
//    -Initial Revision (ES)
//    -18 JAN 2011
//

//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------
#include <c8051f3xx.h>
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Register.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define SYSCLK             12000000    // SYSCLK frequency in Hz

// USB clock selections (SFR CLKSEL)
#define USB_4X_CLOCK       0x00        // Select 4x clock multiplier, for USB
#define USB_INT_OSC_DIV_2  0x10        // Full Speed
#define USB_EXT_OSC        0x20
#define USB_EXT_OSC_DIV_2  0x30
#define USB_EXT_OSC_DIV_3  0x40
#define USB_EXT_OSC_DIV_4  0x50

// System clock selections (SFR CLKSEL)
#define SYS_INT_OSC        0x00        // Select to use internal oscillator
#define SYS_EXT_OSC        0x01        // Select to use an external oscillator
#define SYS_4X_DIV_2       0x02


//-----------------------------------------------------------------------------
// Local Function Prototypes
//-----------------------------------------------------------------------------
// Initialization Routines
void Sysclk_Init (void);               // Initialize the system clock
void Port_Init (void);                 // Configure ports
void Usb0_Init (void);                 // Configure USB core
void Delay (void);                     // Approximately 80 us/1 ms on
                                       // Full/Low Speed



//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DEFAULT_InitRoutine
//-----------------------------------------------------------------------------
// This function is declared in the header file DEFAULT_CustomApp and is
// called in the main(void) function.  It calls initialization routines
// local to this file.
//
//-----------------------------------------------------------------------------
void DEFAULT_InitRoutine (void)
{
   Sysclk_Init ();                     // Initialize oscillator
   Port_Init ();                       // Initialize crossbar and GPIO
}

//-----------------------------------------------------------------------------
// USB_Init
//-----------------------------------------------------------------------------
// USB Initialization performs the following:
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//
//-----------------------------------------------------------------------------
void USB_Init (void)
{

   POLL_WRITE_BYTE (POWER, 0x08);      // Force Asynchronous USB Reset
   POLL_WRITE_BYTE (IN1IE, 0x07);      // Enable Endpoint 0-1 in interrupts
   POLL_WRITE_BYTE (OUT1IE,0x07);      // Enable Endpoint 0-1 out interrupts
   POLL_WRITE_BYTE (CMIE, 0x07);       // Enable Reset, Resume, and Suspend
                                       // interrupts
   USB0XCN = 0xE0;                     // Enable transceiver; select full speed
   POLL_WRITE_BYTE (CLKREC,0x8F);      // Enable clock recovery, single-step
                                       // mode disabled

   EIE1 |= 0x02;                       // Enable USB0 Interrupts

                                       // Enable USB0 by clearing the USB
   POLL_WRITE_BYTE (POWER, 0x01);      // Inhibit Bit and enable suspend
                                       // detection

}


//-----------------------------------------------------------------------------
// Sysclk_Init(void)
//-----------------------------------------------------------------------------
// This function initializes the system clock and the USB clock.
//
//-----------------------------------------------------------------------------
void Sysclk_Init(void)
{
#ifdef _USB_LOW_SPEED_

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKSEL  = SYS_EXT_OSC;              // Select System clock
   CLKSEL |= USB_INT_OSC_DIV_2;        // Select USB clock
#else
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKMUL  = 0x00;                     // Select internal oscillator as
                                       // input to clock multiplier

   CLKMUL |= 0x80;                     // Enable clock multiplier
   Delay();                            // Delay for clock multiplier to begin
   CLKMUL |= 0xC0;                     // Initialize the clock multiplier
   Delay();                            // Delay for clock multiplier to begin

   while(!(CLKMUL & 0x20));            // Wait for multiplier to lock
   CLKSEL  = SYS_INT_OSC;              // Select system clock
   CLKSEL |= USB_4X_CLOCK;             // Select USB clock
#endif  // _USB_LOW_SPEED_
}

//-----------------------------------------------------------------------------
// Port_Init(void)
//-----------------------------------------------------------------------------
// Port Initialization routine that configures the Crossbar and GPIO ports.
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P2MDIN   = ~(0x20);                 // Port 2 pin 5 set as analog input
   P0MDOUT |= 0x0F;                    // Port 0 pins 0-3 set high impedence
   P1MDOUT |= 0x0F;                    // Port 1 pins 0-3 set high impedence
   P2MDOUT |= 0x0C;                    // Port 2 pins 0,1 set high empedence
   P2SKIP   = 0x20;                    // Port 2 pin 5 skipped by crossbar
   XBR0     = 0x00;
   XBR1     = 0x40;                    // Enable Crossbar
}



//-----------------------------------------------------------------------------
// Helper Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Delay(void)
//-----------------------------------------------------------------------------
// Used for a small pause, approximately 80 us in Full Speed,
// and 1 ms when clock is configured for Low Speed
//
//-----------------------------------------------------------------------------
void Delay (void)
{
   int x;
   for(x = 0;x < 500;x)
      x++;
}
