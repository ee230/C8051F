//-----------------------------------------------------------------------------
// F3xx_USB_Main.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
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
// Target:         C8051F32x, C8051F34x, C8051T622/3 or 'T326/7
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   HIDtoUART
//
// Release 1.1
//    -11 SEP 2008 (TP)
//    -Updated for 'T622/3
//
// Release 1.0
//    -Initial Revision (PD)
//    -04 JUN 2008
//
//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------

#include "c8051f3xx.h"
#include "F3xx_USB0_Register.h"
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Descriptor.h"
#include "F3xx_HIDtoUART.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Local Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{
   PCA0MD &= ~0x40;
   System_Init ();
   Usb_Init ();

   CKCON = 0x10;
   TMR2CN = 0x04;

   EA = 1;

   while (1)
   {
      // If bytes have been received across UART interface, initiate a USB
      // transfer to send them to the host
      if ((UART_INPUT_SIZE != 0) && (!SendPacketBusy))
      {
         SendPacket (IN_DATA);
      }

      // If bytes have been received across the USB interface, transmit
      // them across UART
      if ((UART_OUTPUT_SIZE != 0) && (TX_Ready == 1))
      {
         TX_Ready = 0;
         TI0 = 1;                      // Start UART transmit
      }

      // This code functions when the firmware has suspended reception of
      // USB OUT reports because the UART buffer size is above the
      // critical boundary
      EA = 0;
      if (USB_OUT_SUSPENDED == 1)
      {
         // Check to see if the buffer size has fallen back below the
         // boundary
         if (UART_OUTPUT_SIZE < UART_OUTPUT_OVERFLOW_BOUNDARY)
         {
            // If it has, resume USB communication
            USB_OUT_SUSPENDED = 0;
            POLL_WRITE_BYTE (INDEX, 1);// Set index to endpoint 1 registers
            // Clear Out Packet ready bit
            POLL_WRITE_BYTE (EOUTCSR1, 0);
         }
      }
      EA = 1;
   }
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
