//-----------------------------------------------------------------------------
// F326_USB_Main.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This application note covers the implementation of a simple USB application 
// using the interrupt transfer type. This includes support for device
// enumeration, control and interrupt transactions, and definitions of 
// descriptor data. The purpose of this software is to give a simple working 
// example of an interrupt transfer application; it does not include
// support for multiple configurations or other transfer types.
//
// How To Test:    See Readme.txt
//
//
// FID:            326000019
// Target:         C8051F326
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F326_USB_Interrupt
//
//
// Release 1.1
//    -All changes by BW
//    -1 SEP 2010
//    -Updated USB0_Init() to write 0x89 to CLKREC instead of 0x80
//    -Updated USB0_Init() to write 0xA9 to CLKREC instead of 0xA0
// Release 1.0
//    -Initial Revision (DM)
//    -14 DEC 2005
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f326.h>
#include "F326_USB_Register.h"
#include "F326_USB_Main.h"
#include "F326_USB_Descriptor.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

sbit Led1 = P2^2;                      // LED='1' means ON
sbit Led2 = P2^3;

#define Sw1 0x01                       // These are the port2 bits for Sw1
#define Sw2 0x02                       // and Sw2 on the development board
BYTE Switch1State = 0;                 // Indicate status of switch
BYTE Switch2State = 0;                 // starting at 0 == off

BYTE Toggle1 = 0;                      // Variable to make sure each button
BYTE Toggle2 = 0;                      // press and release toggles switch

BYTE Potentiometer = 0x00;             // Last read potentiometer value
BYTE Temperature = 0x00;               // Last read temperature sensor value

idata BYTE Out_Packet[64];                   // Last packet received from host
idata BYTE In_Packet[64];                    // Next packet to sent to host


//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{
   Sysclk_Init();                      // Initialize oscillator
   Port_Init();                        // Initialize crossbar and GPIO
   Usb0_Init();                        // Initialize USB0

   while (1)
   {
    // It is possible that the contents of the following packets can change
    // while being updated.  This doesn't cause a problem in the sample
    // application because the bytes are all independent.  If data is NOT
    // independent, packet update routines should be moved to an interrupt
    // service routine, or interrupts should be disabled during data updates.

      Check_Switch();                     // Update switch state

      if (Out_Packet[0] == 1) Led1 = 1;   // Update status of LED #1
      else Led1 = 0;
      if (Out_Packet[1] == 1) Led2 = 1;   // Update status of LED #2
      else Led2 = 0;
      P0 = ((Out_Packet[2] & 0x0F) | 0xF0);  // Set Port 0 [0-3]


      In_Packet[0] = Switch1State;        // Send status of switch 1
      In_Packet[1] = Switch2State;        // and switch 2 to host
      In_Packet[2] = ((P0 & 0xF0) >> 4);  // Port 0 [4-7]
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Sysclk_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Initialize the system clock and USB clock
//
//-----------------------------------------------------------------------------
void Sysclk_Init(void)
{
#ifdef _USB_LOW_SPEED_

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKSEL  = SYS_INT_OSC;              // Select System clock
   CLKSEL |= USB_INT_OSC_DIV_2;        // Select USB clock
#else
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency

   CLKMUL  = 0x00;                     // Select internal oscillator as
                                       // input to clock multiplier

   CLKMUL |= 0x80;                     // Enable clock multiplier
   Delay();                            // Delay for clock multiplier to begin
   CLKMUL |= 0xC0;                     // Initialize the clock multiplier

   while(!(CLKMUL & 0x20));            // Wait for multiplier to lock
   CLKSEL  = SYS_INT_OSC;              // Select system clock as clock multiplier/2
   CLKSEL |= USB_4X_CLOCK;             // Select USB clock

   CLKSEL = 0x02;                      // Select system clock as clock multiplier/2
#endif  /* _USB_LOW_SPEED_ */
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
// P1.7   analog                  Potentiometer
// P2.2   digital   push-pull     LED
// P2.3   digital   push-pull     LED
//-----------------------------------------------------------------------------
void Port_Init(void)
{
   P0MDOUT = 0x0F;                     // Port 0 pins 0-3 set push-pull
   P2MDOUT = 0x0C;                     // Port 2 pins 2,3 set push-pull
   P0 = 0xFF;                          // Port 0 latches set high
   P2 = 0xFF;                          // Port 2 latches set high

   GPIOCN = 0x40;                      // Digital input enable
}

//-----------------------------------------------------------------------------
// Usb0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
// 
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//-----------------------------------------------------------------------------
void Usb0_Init(void)
{
   BYTE Count;

   // Set initial values of In_Packet and Out_Packet to zero
   // Initialize here as opposed to above main() to prevent WDT reset
   for (Count = 0; Count < 64; Count++)
   {
      Out_Packet[Count] = 0;
      In_Packet[Count] = 0;
   }

   POLL_WRITE_BYTE(POWER,  0x08);      // Force Asynchronous USB Reset
   POLL_WRITE_BYTE(IN1IE,  0x03);      // Enable Endpoint 0,1 in interrupts
   POLL_WRITE_BYTE(OUT1IE, 0x02);      // Enable Endpoint 1 out interrupts
   POLL_WRITE_BYTE(CMIE,   0x07);      // Enable Reset,Resume,Suspend interrupts
#ifdef _USB_LOW_SPEED_
   USB0XCN = 0xC0;                     // Enable transceiver; select low speed
   POLL_WRITE_BYTE(CLKREC, 0xA9);      // Enable clock recovery; single-step mode
                                       // disabled; low speed mode enabled
#else
   USB0XCN = 0xE0;                     // Enable transceiver; select full speed
   POLL_WRITE_BYTE(CLKREC, 0x89);      // Enable clock recovery, single-step mode
                                       // disabled
#endif // _USB_LOW_SPEED_

   EIE1 |= 0x02;                       // Enable USB0 Interrupts
   EA = 1;                             // Global Interrupt enable
                                       // Enable USB0 by clearing the USB 
                                       // Inhibit bit
   POLL_WRITE_BYTE(POWER,  0x01);      // and enable suspend detection
}

//-----------------------------------------------------------------------------
// Check_Switch
//-----------------------------------------------------------------------------
//
// Called from main loop, check to see if switch is pressed,
// then watch for release.
//
//-----------------------------------------------------------------------------

void Check_Switch(void)
{
   if (!(P2 & Sw1))                    // Check for switch #1 pressed
   {
      if (Toggle1 == 0)                // Toggle is used to debounce switch
      {                                // so that one press and release will
         Switch1State = ~Switch1State; // toggle the state of the switch sent
         Toggle1 = 1;                  // to the host
      }
   }
   else Toggle1 = 0;                   // Reset toggle variable

   if (!(P2 & Sw2))                    // Same as above, but Switch2
   {
      if (Toggle2 == 0)
      {
         Switch2State = ~Switch2State;
         Toggle2 = 1;
      }
   }
   else Toggle2 = 0;
}

//-----------------------------------------------------------------------------
// Delay
//-----------------------------------------------------------------------------
//
// Used for a small pause, approximately 80 us in Full Speed,
// and 1 ms when clock is configured for Low Speed
//
//-----------------------------------------------------------------------------

void Delay(void)
{
   int x;
   for(x = 0;x < 500;x)
      x++;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------