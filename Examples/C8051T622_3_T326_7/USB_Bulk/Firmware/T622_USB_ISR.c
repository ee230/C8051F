//-----------------------------------------------------------------------------
// T622_USB_ISR.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This file contains most of the application-specific USB firmware that
// handles the file transfer and each endpoint as needed.  The following
// routines are included:
//
// - USB_ISR(): Top-level USB interrupt handler. All USB handler
//    routines are called from here.
// - Endpoint0(): Endpoint0 interrupt handler.
// - BulkOut(): Places the data read from the Host in FileStorage.
// - BulkIn(): Places the data in FileStorage in the FIFO to send to the Host.
// - USBReset(): USB Reset event handler.
// - State_Machine(): USB state machine - handles the RX and TX states.
// - Receive_Setup(): Determine whether a read or write request has been
//    received and initialize variables accordingly.
// - Receive_File(): Handles the handshaking and File upkeep for data sent from
//    the host to the device.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   T622_USB_Bulk
//
// Release 1.4
//    -All changes by TP
//    -10 SEP 2008
//    -Updated for 'T622/3 by moving the file storage to XRAM and limiting the
//      size to 512 bytes
//
// Release 1.3
//    -All changes by GP
//    -21 NOV 2005
//    -Changed revision number to match project revision
//     No content changes to this file
//    -Modified file to fit new formatting guidelines
//    -Changed file name from usb_isr.c
//
// Release 1.2
//    -Initial Revision (JS/CS/JM)
//    -XX OCT 2003
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T622_defs.h"
#include "T622_USB_Registers.h"
#include "T622_USB_Structs.h"
#include "T622_USB_Main.h"
#include "T622_USB_Descriptors.h"
#include "T622_USB_Config.h"
#include "T622_USB_Request.h"

//-----------------------------------------------------------------------------
// Extern Global Variables
//-----------------------------------------------------------------------------

extern DEVICE_STATUS  gDeviceStatus;
extern EP0_COMMAND    gEp0Command;
extern EP_STATUS      gEp0Status;
extern EP_STATUS      gEp1InStatus;
extern EP_STATUS      gEp2OutStatus;

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//-Application-Specific Code---------------------------------------------------

//  Constants Definitions
#define MAX_BLOCK_SIZE  64             // Use the maximum block size of 64
#define FILE_SIZE       512            // Maximum file size
#define BLOCKS_PR_FILE  FILE_SIZE/MAX_BLOCK_SIZE

//  Message Types
#define READ_MSG    0x00               // Message types for communication with
#define WRITE_MSG   0x01               // host
#define SIZE_MSG    0x02

//  Machine States
#define ST_WAIT_DEV 0x01               // Wait for application to open a device
                                       // instance
#define ST_IDLE_DEV 0x02               // Device is open, wait for Setup
                                       // Message from host
#define ST_RX_SETUP 0x04               // Received Setup Message, decode and
                                       // wait for data
#define ST_RX_FILE  0x08               // Receive file data from host
#define ST_TX_FILE  0x10               // Transmit file data to host
#define ST_TX_ACK   0x20               // Transmit ACK 0xFF to host after every
                                       // 8 packets
#define ST_ERROR    0x80               // Error state


typedef struct                         // Structure definition of a block of
{                                      // data
    U8 Piece[MAX_BLOCK_SIZE];
}   BLOCK;

xdata   BLOCK FileStorage[BLOCKS_PR_FILE] _at_ 0x200; // File storage

data    U16   BytesToWrite;            // Total number of bytes to write to
                                       // the host
data    U16   BytesToRead;             // Total number of bytes to read from
                                       // host
data    U8    Buffer[3];               // Buffer for Setup messages
data    U8    NumBlocks;               // Number of Blocks for this transfer
data    U8    M_State;                 // Current Machine State
data    U8    BlockIndex;              // Index of Current Block in Page
data    U8    BlocksRead;              // Total Number of Blocks Read
data    U8    BlocksWrote;             // Total Number of Blocks Written
U8 xdata* data ReadIndex;

// code const   U8    Serial1[0x0A] = {0x0A,0x03,'A',0,'B',0,'C',0,'D',0};
// Serial Number Defintion

xdata    U8    LengthFile[3]   _at_    0x1FD;
//  {Length(Low Byte), Length(High Byte), Number of Blocks}

SBIT (LED1, SFR_P0, 6); // LED='1' means ON
SBIT (LED2, SFR_P1, 2); // These blink to indicate data transmission

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void State_Machine(void);
void Receive_Setup(void);
void Receive_File(void);

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// USB_ISR
//-----------------------------------------------------------------------------
//
// This is the top level USB ISR. All endpoint interrupt/request
// handlers are called from this function.
//
// Handler routines for any configured interrupts should be
// added in the appropriate endpoint handler call slots.
//
//-----------------------------------------------------------------------------
INTERRUPT(USB_ISR, INTERRUPT_USB0)
{
   U8 bCommonInt, bInInt, bOutInt;

   // Read interrupt registers
   UREAD_BYTE(CMINT, bCommonInt);
   UREAD_BYTE(IN1INT, bInInt);
   UREAD_BYTE(OUT1INT, bOutInt);

   // Check for reset interrupt
   if (bCommonInt & rbRSTINT)
   {
      // Call reset handler
      USBReset();
      M_State = ST_WAIT_DEV;
   }

   // Check for Endpoint0 interrupt
   if (bInInt & rbEP0)
   {
      // Call Endpoint0 handler
      Endpoint0();
   }

   // Endpoint1 IN
   if (bInInt & rbIN1)
   {
      //-Application-Specific Code---------------------------------------------

      if (M_State == ST_RX_FILE)       // Ack Transmit complete, go to RX state
      {
         M_State = (ST_TX_ACK);
      }
      if (M_State == ST_TX_FILE)       // File block transmit complete,
      {                                // go to TX state
         // Go to Idle when done
         M_State = (BlocksWrote == NumBlocks) ? ST_IDLE_DEV : ST_TX_FILE;
      }

      //-----------------------------------------------------------------------
   }

   // Endpoint2 OUT
   if (bOutInt & rbOUT2)
   {
      //-Application-Specific Code---------------------------------------------

      // Call Endpoint2 OUT handler
      BulkOut(&gEp2OutStatus);

      M_State = (M_State == ST_IDLE_DEV) ? ST_RX_SETUP : ST_RX_FILE;

      //-----------------------------------------------------------------------
   }

   //-Application-Specific Code------------------------------------------------

   State_Machine();

   //--------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Endpoint0
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine handles all the Control endpoint requests.  This code does not
// need to be modified for most applications.
//
//-----------------------------------------------------------------------------
void Endpoint0 (void)
{
   U8 bTemp = 0;
   U8 bCsr1, uTxBytes;

   UWRITE_BYTE(INDEX, 0);                 // Target Ep0
   UREAD_BYTE(E0CSR, bCsr1);

   // Handle Setup End
   if (bCsr1 & rbSUEND)                   // Check for setup end
   {                                      // Indicate setup end serviced
      UWRITE_BYTE(E0CSR, rbSSUEND);
      gEp0Status.bEpState = EP_IDLE;      // Ep0 state to idle
      M_State = ST_IDLE_DEV;              // Ported from usb_file.c
   }

   // Handle sent stall
   if (bCsr1 & rbSTSTL)                   // If last state requested a stall
   {                                      // Clear Sent Stall bit (STSTL)
      UWRITE_BYTE(E0CSR, 0);
      gEp0Status.bEpState = EP_IDLE;      // Ep0 state to idle
      M_State = ST_IDLE_DEV;              // ported from usb_file.c
   }

   // Handle incoming packet
   if (bCsr1 & rbOPRDY)
   {
      // Read the 8-byte command from Endpoint0 FIFO
      FIFORead(0, 8, (U8*)&gEp0Command);

      // Byte-swap the wIndex field
      bTemp = gEp0Command.wIndex.U8[LSB];
      gEp0Command.wIndex.U8[LSB] = gEp0Command.wIndex.U8[MSB];
      gEp0Command.wIndex.U8[MSB] = bTemp;

      // Byte-swap the wValue field
      bTemp = gEp0Command.wValue.U8[LSB];
      gEp0Command.wValue.U8[LSB] = gEp0Command.wValue.U8[MSB];
      gEp0Command.wValue.U8[MSB] = bTemp;

      // Byte-swap the wLength field
      bTemp = gEp0Command.wLength.U8[LSB];
      gEp0Command.wLength.U8[LSB] = gEp0Command.wLength.U8[MSB];
      gEp0Command.wLength.U8[MSB] = bTemp;

      // Decode received command
      switch (gEp0Command.bmRequestType & CMD_MASK_COMMON)
      {
         case  CMD_STD_DEV_OUT:           // Standard device requests
            // Decode standard OUT request
            switch (gEp0Command.bRequest)
            {
               case SET_ADDRESS:
                  SetAddressRequest();
                  break;
               case SET_FEATURE:
                  SetFeatureRequest();
                  break;
               case CLEAR_FEATURE:
                  ClearFeatureRequest();
                  break;
               case SET_CONFIGURATION:
                  SetConfigurationRequest();
                  break;
               case SET_INTERFACE:
                  SetInterfaceRequest();
                  break;
               // All other OUT requests not supported
               case SET_DESCRIPTOR:
               default:
                  gEp0Status.bEpState = EP_ERROR;
                  break;
            }
            break;

         // Decode standard IN request
         case CMD_STD_DEV_IN:
            switch (gEp0Command.bRequest)
            {
               case GET_STATUS:
                  GetStatusRequest();
                  break;
               case GET_DESCRIPTOR:
                  GetDescriptorRequest();
                  break;
               case GET_CONFIGURATION:
                  GetConfigurationRequest();
                  break;
               case GET_INTERFACE:
                  GetInterfaceRequest();
                  break;
               // All other IN requests not supported
               case SYNCH_FRAME:
               default:
                  gEp0Status.bEpState = EP_ERROR;
                  break;
            }
            break;
         // All other requests not supported
         default:
            gEp0Status.bEpState = EP_ERROR;
      }

      // Write E0CSR according to the result of the serviced out packet
      bTemp = rbSOPRDY;
      if (gEp0Status.bEpState == EP_ERROR)
      {
         bTemp |= rbSDSTL;                // Error condition handled
                                          // with STALL
         gEp0Status.bEpState = EP_IDLE;   // Reset state to idle
      }

      UWRITE_BYTE(E0CSR, bTemp);
   }

   bTemp = 0;                             // Reset temporary variable

   // If state is transmit, call transmit routine
   if (gEp0Status.bEpState == EP_TX)
   {
      // Check the number of bytes ready for transmit
      // If less than the maximum packet size, packet will
      // not be of the maximum size
      if (gEp0Status.uNumBytes <= EP0_MAXP)
      {
         uTxBytes = gEp0Status.uNumBytes;
         gEp0Status.uNumBytes = 0;        // Update byte counter
         bTemp |= rbDATAEND;              // This will be the last
                                          // packet for this transfer
         gEp0Status.bEpState = EP_IDLE;   // Reset endpoint state
      }

      // Otherwise, transmit maximum-length packet
      else
      {
         uTxBytes = EP0_MAXP;
         gEp0Status.uNumBytes -= EP0_MAXP;// update byte counter
      }

      // Load FIFO
      FIFOWrite(0, uTxBytes, (U8*)gEp0Status.pData);

      // Update data pointer
      gEp0Status.pData = (U8*)gEp0Status.pData + uTxBytes;

      // Update Endpoint0 Control/Status register
      bTemp |= rbINPRDY;                  // Always transmit a packet
                                          // when this routine is called
                                          // (may be zero-length)

      UWRITE_BYTE(E0CSR, bTemp);          // Write to Endpoint0 Control/Status
   }
}

//-----------------------------------------------------------------------------
// BulkOut
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
// 1) PEP_STATUS pEpOutStatus
//
// This routine handles the transmission of data from the host to the device.
// The transferred bytes are handled by this routine and the File variable
// upkeep is handled by Receive_File().
//
//-----------------------------------------------------------------------------
void BulkOut (PEP_STATUS pEpOutStatus)
{
   U16 uBytes;
   U8 bTemp = 0;
   U8 bCsrL, bCsrH;

   UWRITE_BYTE(INDEX, pEpOutStatus->bEp); // Index to current endpoint
   UREAD_BYTE(EOUTCSRL, bCsrL);
   UREAD_BYTE(EOUTCSRH, bCsrH);

   // Make sure this endpoint is not halted
   if (pEpOutStatus->bEpState != EP_HALTED)
   {
      // Handle STALL condition sent
      if (bCsrL & rbOutSTSTL)
      {
         // Clear Send Stall, Sent Stall, and data toggle
         UWRITE_BYTE(EOUTCSRL, rbOutCLRDT);
      }

      // Read received packet
      if(bCsrL & rbOutOPRDY)
      {
         // Get packet length
         UREAD_BYTE(EOUTCNTL, bTemp);     // Low byte
         uBytes = (U16)bTemp & 0x00FF;

         UREAD_BYTE(EOUTCNTH, bTemp);     // High byte
         uBytes |= (U16)bTemp << 8;

         //-Application-Specific Code------------------------------------------

         // If the device is IDLE (i.e. this is the first packet of a file
         // transfer), read the number of bytes of the transfer.  Otherwise, if
         // a transmission is already in progress, read the file data to
         // FileStorage.
         if (M_State == ST_IDLE_DEV)
         {
            FIFORead(0x02, uBytes, &Buffer);
         }
         else
         {
            FIFORead(0x02, uBytes, (U8*)(&FileStorage[BlockIndex]));
         }

         //--------------------------------------------------------------------

         // Clear out-packet-ready
         UWRITE_BYTE(INDEX, pEpOutStatus->bEp);
         UWRITE_BYTE(EOUTCSRL, 0);

         // Read updated status register
         //UWRITE_BYTE(INDEX, pEpOutStatus->bEp); // Index to current endpoint
         //UREAD_BYTE(EOUTCSRL, bCsrL);
      }
   }
}

//-----------------------------------------------------------------------------
// BulkIn
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
// 1) PEP_STATUS pEpOutStatus
// 2) U8 * DataToWrite
// 3) U16 NumBytes
//
// This routine handles the transmission of data from the device to the host.
// It automatically modifies the BytesToWrite, ReadIndex, and BlocksWrote global
// variables, so these variables do not have to be modified by the calling
// function.
//
//-----------------------------------------------------------------------------
void BulkIn (PEP_STATUS pEpInStatus, U8 * DataToWrite, U16 NumBytes)
{
   U8 bCsrL, bCsrH;

   UWRITE_BYTE(INDEX, pEpInStatus->bEp);  // Index to current endpoint
   UREAD_BYTE(EINCSRL, bCsrL);
   UREAD_BYTE(EINCSRH, bCsrH);

   // Make sure this endpoint is not halted
   if (pEpInStatus->bEpState != EP_HALTED)
   {
      // Handle STALL condition sent
      if (bCsrL & rbInSTSTL)
      {
         UWRITE_BYTE(EINCSRL, rbInCLRDT); // Clear Send Stall and Sent Stall,
                                          // and clear data toggle
      }

      // If a FIFO slot is open, write a new packet to the IN FIFO
      if (!(bCsrL & rbInINPRDY))
      {
         //-Application-Specific Code------------------------------------------

         pEpInStatus->uNumBytes = NumBytes;
         pEpInStatus->pData = (U8*)DataToWrite;

         // Write <uNumBytes> bytes to the <bEp> FIFO
         FIFOWrite(pEpInStatus->bEp, pEpInStatus->uNumBytes,
            (U8*)pEpInStatus->pData);

         BytesToWrite -= NumBytes;
         ReadIndex += NumBytes;
         BlocksWrote++;

         //--------------------------------------------------------------------

         // Set Packet Ready bit (INPRDY)
         UWRITE_BYTE(EINCSRL, rbInINPRDY);

         // Check updated endopint status
         //UREAD_BYTE(EINCSRL, bCsrL);
      }
   }
}


//-----------------------------------------------------------------------------
// USBReset
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Initialize the global Device Status structure (all zeros) and resets all
// endpoints.  This function does not need to be modified for most
// applications.
//
//-----------------------------------------------------------------------------
void USBReset (void)
{
   U8 i, bPower = 0;
   U8 * pDevStatus;

   // Reset device status structure to all zeros (undefined)
   pDevStatus = (U8 *)&gDeviceStatus;
   for (i=0;i<sizeof(DEVICE_STATUS);i++)
   {
      *pDevStatus++ = 0x00;
   }

   // Set device state to default
   gDeviceStatus.bDevState = DEV_DEFAULT;

   // REMOTE_WAKEUP_SUPPORT and SELF_POWERED_SUPPORT
   // defined in file "usb_desc.h"
   gDeviceStatus.bRemoteWakeupSupport = REMOTE_WAKEUP_SUPPORT;
   gDeviceStatus.bSelfPoweredStatus = SELF_POWERED_SUPPORT;

   // Reset all endpoints

   // Reset Endpoint0
   gEp0Status.bEpState = EP_IDLE;         // Reset Endpoint0 state
   gEp0Status.bEp = 0;                    // Set endpoint number
   gEp0Status.uMaxP = EP0_MAXP;           // Set maximum packet size

   // Reset Endpoint1 IN
   gEp1InStatus.bEpState = EP_HALTED;     // Reset state
   gEp1InStatus.uNumBytes = 0;            // Reset byte counter

   // Reset Endpoint2 OUT
   gEp2OutStatus.bEpState = EP_HALTED;    // Reset state
   gEp2OutStatus.uNumBytes = 0;           // Reset byte counter

   // Get Suspend enable/disable status. If enabled, prepare temporary
   // variable bPower.
   if (SUSPEND_ENABLE)
   {
      bPower = 0x01;                      // Set bit0 (Suspend Enable)
   }

   // Get ISO Update enable/disable status. If enabled, prepare temporary
   // variable bPower.
   if (ISO_UPDATE_ENABLE)
   {
      bPower |= 0x80;                     // Set bit7 (ISO Update Enable)
   }

   UWRITE_BYTE(POWER, bPower);
}

//-----------------------------------------------------------------------------
// State_Machine
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Determine the new state and act on current state of the file transfer.  This
// function is application-specific code.
//
//-----------------------------------------------------------------------------
void State_Machine(void)
{
   //-Application-Specific Code------------------------------------------------

   switch (M_State)
   {
      case
         ST_WAIT_DEV:                  // Stay in Wait State
         break;

      case
         ST_IDLE_DEV:                  // Stay in Idle State
         break;

      case ST_RX_SETUP:
         Receive_Setup();              // Decode host Setup Message
         break;

      case ST_RX_FILE:
         Receive_File();               // Receive File data from host
         break;

      case ST_TX_ACK:
         M_State = ST_RX_FILE;         // Ack complete, continue RX data
         break;

      case ST_TX_FILE:                 // Send file data to host
         if(BytesToWrite > MAX_BLOCK_SIZE)
         {
            // Write a full packet, if possible
            BulkIn(&gEp1InStatus, (U8*)(ReadIndex),MAX_BLOCK_SIZE);

            // Try to write a second packet to the FIFO here to send data
            // as quickly as possible.
            if(BytesToWrite > MAX_BLOCK_SIZE)
            {
               // Write a full packet, if possible
               BulkIn(&gEp1InStatus,
                                 (U8*)(ReadIndex),MAX_BLOCK_SIZE);
            }
            else
            {
               // If a full packet isn't available, send a partial packet
               BulkIn(&gEp1InStatus,
                                 (U8*)(ReadIndex),BytesToWrite);
            }
         }
         else
         {
            // If a full packet isn't available, send a partial packet
            BulkIn(&gEp1InStatus, (U8*)(ReadIndex),BytesToWrite);
         }

         // Update LED2
         if ((BlocksWrote%8) == 0)
         {
            LED2 = !LED2;
         }
         if (BlocksWrote == NumBlocks)
         {
            LED2 = 0;
         }
         break;

      default:
         M_State = ST_ERROR;           // Unknown State, stay in Error State
         break;
   }

   //--------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
// Receive_Setup
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Determines whether a read or write request has been received.  Initializes
// variables for either a read or write operation.
//
// This function contains application-specific code.
//
//-----------------------------------------------------------------------------
void Receive_Setup(void)
{
   //-Application-Specific Code------------------------------------------------

   if (Buffer[0] == READ_MSG)          // Check See if Read File Setup
   {
      // Read request received... transfer data from device to host

      // Only transfer the maximum file size
      NumBlocks = LengthFile[2];       // Read NumBlocks
      NumBlocks = (NumBlocks > BLOCKS_PR_FILE)? BLOCKS_PR_FILE: NumBlocks;

      // Send host size of transfer message
      Buffer[0] = SIZE_MSG;
      Buffer[1] = LengthFile[1];
      Buffer[2] = LengthFile[0];
      BulkIn(&gEp1InStatus, &Buffer, 3);

      M_State = ST_TX_FILE;            // Go to TX data state

      // Initialize file transfer variables
      BlocksWrote = 0;
      BytesToWrite = BytesToRead;
      ReadIndex = &FileStorage[0].Piece[0];
      LED2 = 1;
   }
   else                                // Otherwise assume Write Setup Packet
   {
      // Write request received... transfer data from host to device

      LengthFile[0] = Buffer[2];
      LengthFile[1] = Buffer[1];

      // Calculate the number of bytes to be received
      BytesToRead = LengthFile[1] + 256*LengthFile[0];

      // Find the maximum NumBlocks
      NumBlocks = (U8)(BytesToRead/MAX_BLOCK_SIZE);

      // State Error if transfer too big
      if (NumBlocks > BLOCKS_PR_FILE)
      {
         M_State = ST_ERROR;
      }
      else
      {
         if (BytesToRead % MAX_BLOCK_SIZE)
         {
            NumBlocks++;               // Increment NumBlocks
                                       // for last partial block
         }

         // Save the file length and number of blocks
         // This variable is stored in XRAM so power loss will lose this file
         // information.
         LengthFile[2] = NumBlocks;

         // Initialize the variables
         BlockIndex = 0;
         BlocksRead = 0;
         LED1 = 1;
         M_State = ST_RX_FILE;         // Go to RX data state
      }
   }

   //--------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
// Receive_Setup
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine handles the file variables and upkeep.  After every 8 packets
// or at the end of the transfer, a handshake signal (0xFF) is sent to the
// host.
//
// This routine contains application-specific code.
//
//-----------------------------------------------------------------------------
void Receive_File(void)
{
   //-Application-Specific Code------------------------------------------------

   BlocksRead++;       // Increment
   BlockIndex++;

   // If multiple of 8 or last packet, send handshake packet 0xFF to host
   if ((BlockIndex == (BLOCKS_PR_FILE)) || (BlocksRead == NumBlocks))
   {
      LED1 = !LED1;
      BlockIndex = 0;
      Buffer[0] = 0xFF;

      // Place Handshake packet (0xFF) on the OUT FIFO
      BulkIn (&gEp1InStatus, (U8*)&Buffer, 1);
   }

   // Go to Idle state if last packet has been received
   if (BlocksRead == NumBlocks)
   {
      M_State = ST_IDLE_DEV;
      LED1 = 0;
   }

   //--------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------