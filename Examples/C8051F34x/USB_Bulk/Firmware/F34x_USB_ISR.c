//-----------------------------------------------------------------------------
// F34x_USB_ISR.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Source file for USB firmware. Includes the following routines:
//
// - USB_ISR(): Top-level USB interrupt handler. All USB handler
//    routines are called from here.
// - Endpoint0(): Endpoint0 interrupt handler.
// - BulkOrInterruptOut(): Bulk or Interrupt OUT interrupt
//    handler.
// - BulkOrInterruptIn(): Places DataToWrite on the IN FIFO.
// - USBReset (): USB Reset event handler.
// - State_Machine(): USB state machine
// - Receive_Setup(): Determine whether a read or write request
//    been received and initializes variables accordingly.
// - Receive_File(): Receives and saves data
// - Page_Erase(): Erases a page of FLASH
// - Page_Write(): Writes to a page of FLASH
//
//
// How To Test:    See Readme.txt
//
//
// FID:            34X000007
// Target:         C8051F34x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F34x_USB_Bulk
//
//
// Release 1.3
//    -All changes by GP
//    -21 NOV 2005
//    -Changed revision number to match project revision
//     No content changes to this file
//    -Modified file to fit new formatting guidelines
//    -Changed file name from usb_isr.c
//
//
// Release 1.2
//    -Initial Revision (JS/CS/JM)
//    -XX OCT 2003
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "c8051F340.h"
#include "F34x_USB_Registers.h"
#include "F34x_USB_Structs.h"
#include "F34x_USB_Main.h"
#include "F34x_USB_Descriptors.h"
#include "F34x_USB_Config.h"
#include "F34x_USB_Request.h"

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

//  Constants Definitions
#define NUM_STG_PAGES   20             // Total number of flash pages to
                                       // be used for file storage
#define MAX_BLOCK_SIZE  64             // Use the maximum block size of 64
#define FLASH_PAGE_SIZE 512            // Size of each flash page
#define BLOCKS_PR_PAGE  FLASH_PAGE_SIZE/MAX_BLOCK_SIZE
#define MAX_NUM_BLOCKS  BLOCKS_PR_PAGE*NUM_STG_PAGES

// UINT type definition
#ifndef _UINT_DEF_
#define _UINT_DEF_
typedef unsigned int UINT;
#endif                                 // _UINT_DEF_

// BYTE type definition
#ifndef _BYTE_DEF_
#define _BYTE_DEF_
typedef unsigned char BYTE;
#endif                                 // _BYTE_DEF_ 

//  Message Types
#define READ_MSG    0x00    // Message types for communication with host
#define WRITE_MSG   0x01
#define SIZE_MSG    0x02

//  Machine States
#define ST_WAIT_DEV 0x01    // Wait for application to open a device instance
#define ST_IDLE_DEV 0x02    // Device is open, wait for Setup Message from host
#define ST_RX_SETUP 0x04    // Received Setup Message, decode and wait for data
#define ST_RX_FILE  0x08    // Receive file data from host
#define ST_TX_FILE  0x10    // Transmit file data to host
#define ST_TX_ACK   0x20    // Transmit ACK 0xFF to host after every 8 packets
#define ST_ERROR    0x80    // Error state


typedef struct {            //  Structure definition of a block of data
    BYTE Piece[MAX_BLOCK_SIZE];
}   BLOCK;

typedef struct {            //  Structure definition of a flash memory page
    BYTE FlashPage[FLASH_PAGE_SIZE];
}   PAGE;

xdata   BLOCK   TempStorage[BLOCKS_PR_PAGE];    // Temporary storage of between 
                                                // flash writes

code    BYTE    Pg0 _at_    0x1400;
code    BYTE    Pg1 _at_    0x1600;
code    BYTE    Pg2 _at_    0x1800;
code    BYTE    Pg3 _at_    0x1A00;
code    BYTE    Pg4 _at_    0x1C00;
code    BYTE    Pg5 _at_    0x1E00;
code    BYTE    Pg6 _at_    0x2000;
code    BYTE    Pg7 _at_    0x2200;
code    BYTE    Pg8 _at_    0x2400;
code    BYTE    Pg9 _at_    0x2600;

code    BYTE    Pg10    _at_    0x2800;
code    BYTE    Pg11    _at_    0x2A00;
code    BYTE    Pg12    _at_    0x2C00;
code    BYTE    Pg13    _at_    0x2E00;
code    BYTE    Pg14    _at_    0x3000;
code    BYTE    Pg15    _at_    0x3200;
code    BYTE    Pg16    _at_    0x3400;
code    BYTE    Pg17    _at_    0x3600;
code    BYTE    Pg18    _at_    0x3800;
code    BYTE    Pg19    _at_    0x3A00;

idata   BYTE*   PageIndices[20] =   {&Pg0,  &Pg1,   &Pg2,   &Pg3,   &Pg4,   
                                     &Pg5,  &Pg6,   &Pg7,   &Pg8,   &Pg9,
                                     &Pg10, &Pg11,  &Pg12,  &Pg13,  &Pg14,  
									 &Pg15,  &Pg16,  &Pg17,  &Pg18,  &Pg19};

data    UINT    BytesToWrite;   //  Total number of bytes to write to the host
data    UINT    BytesToRead;    //  Total number of bytes to read from host
data    BYTE    Buffer[3];      //  Buffer for Setup messages
data    BYTE    NumBlocks;      //  Number of Blocks for this transfer
data    BYTE    M_State;        //  Current Machine State
data    BYTE    BlockIndex;     //  Index of Current Block in Page
data    BYTE    PageIndex;      //  Index of Current Page in File
data    BYTE    BlocksRead;     //  Total Number of Blocks Read
data    BYTE    BlocksWrote;    //  Total Number of Blocks Written
data    BYTE*   ReadIndex;

// code const   BYTE    Serial1[0x0A] = {0x0A,0x03,'A',0,'B',0,'C',0,'D',0};
// Serial Number Defintion

code    BYTE    LengthFile[3]   _at_    0x1200;
//  {Length(Low Byte), Length(High Byte), Number of Blocks}

sbit Led1 = P2^2; // LED='1' means ON
sbit Led2 = P2^3; // These blink to indicate data transmission

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void    State_Machine(void);        
void    Receive_Setup(void);        
void    Receive_File(void);        

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// USB_ISR
//-----------------------------------------------------------------------------
//
//
// This is the top level USB ISR. All endpoint interrupt/request
// handlers are called from this function.
//
// Handler routines for any configured interrupts should be
// added in the appropriate endpoint handler call slots.
//
//-----------------------------------------------------------------------------
void USB_ISR () interrupt 8
{
   BYTE bCommonInt, bInInt, bOutInt;

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
         if (M_State == ST_RX_FILE)    // Ack Transmit complete, go to RX state
         {
           M_State = (ST_TX_ACK);
         }
         if (M_State == ST_TX_FILE)    // File block transmit complete, 
		                               // go to TX state
         {
           // Go to Idle when done
           M_State = (BlocksWrote == NumBlocks) ? ST_IDLE_DEV : ST_TX_FILE;      
         }
   }

   // Endpoint2 OUT
   if (bOutInt & rbOUT2)
   {
      // Call Endpoint2 OUT handler
      BulkOrInterruptOut(&gEp2OutStatus);

      M_State = (M_State == ST_IDLE_DEV) ? ST_RX_SETUP : ST_RX_FILE;
   }

   State_Machine();
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
//-----------------------------------------------------------------------------
void Endpoint0 ()
{
   BYTE bTemp = 0;
   BYTE bCsr1, uTxBytes;

   UWRITE_BYTE(INDEX, 0);                 // Target ep0
   UREAD_BYTE(E0CSR, bCsr1);

   // Handle Setup End
   if (bCsr1 & rbSUEND)                   // Check for setup end
   {                                      // Indicate setup end serviced
      UWRITE_BYTE(E0CSR, rbSSUEND);
      gEp0Status.bEpState = EP_IDLE;      // ep0 state to idle
      M_State = ST_IDLE_DEV;              // ported from usb_file.c
   }

   // Handle sent stall
   if (bCsr1 & rbSTSTL)                   // If last state requested a stall
   {                                      // Clear Sent Stall bit (STSTL)
      UWRITE_BYTE(E0CSR, 0);
      gEp0Status.bEpState = EP_IDLE;      // ep0 state to idle
      M_State = ST_IDLE_DEV;              // ported from usb_file.c
   }

   // Handle incoming packet
   if (bCsr1 & rbOPRDY)
   {
      // Read the 8-byte command from Endpoint0 FIFO
      FIFORead(0, 8, (BYTE*)&gEp0Command);

      // Byte-swap the wIndex field
      bTemp = gEp0Command.wIndex.c[1];
      gEp0Command.wIndex.c[1] = gEp0Command.wIndex.c[0];
      gEp0Command.wIndex.c[0] = bTemp;

      // Byte-swap the wValue field
      bTemp = gEp0Command.wValue.c[1];
      gEp0Command.wValue.c[1] = gEp0Command.wValue.c[0];
      gEp0Command.wValue.c[0] = bTemp;

      // Byte-swap the wLength field
      bTemp = gEp0Command.wLength.c[1];
      gEp0Command.wLength.c[1] = gEp0Command.wLength.c[0];
      gEp0Command.wLength.c[0] = bTemp;

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
         gEp0Status.uNumBytes = 0;        // update byte counter
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
      FIFOWrite(0, uTxBytes, (BYTE*)gEp0Status.pData);

      // Update data pointer
      gEp0Status.pData = (BYTE*)gEp0Status.pData + uTxBytes;

      // Update Endpoint0 Control/Status register
      bTemp |= rbINPRDY;                  // Always transmit a packet
                                          // when this routine is called
                                          // (may be zero-length)

      UWRITE_BYTE(E0CSR, bTemp);          // Write to Endpoint0 Control/Status
   }

}

//-----------------------------------------------------------------------------
// BulkOrInterruptOut
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
// 1) PEP_STATUS pEpOutStatus
//
//-----------------------------------------------------------------------------
void BulkOrInterruptOut(PEP_STATUS pEpOutStatus)
{
   UINT uBytes;
   BYTE bTemp = 0;
   BYTE bCsrL, bCsrH;

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
         uBytes = (UINT)bTemp & 0x00FF;

         UREAD_BYTE(EOUTCNTH, bTemp);     // High byte
         uBytes |= (UINT)bTemp << 8;

         if (M_State == ST_IDLE_DEV) 
         {
            FIFORead(0x02, uBytes, &Buffer);
         }			 
         else
         {
            FIFORead(0x02, uBytes, (BYTE*)(&TempStorage[BlockIndex]));
         }

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
// BulkOrInterruptIn
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
// 1) PEP_STATUS pEpOutStatus
// 2) BYTE * DataToWrite
// 3) UINT NumBytes
//
// - Places DataToWrite on the IN FIFO
// - Sets Packet Ready Bit
//-----------------------------------------------------------------------------
void BulkOrInterruptIn (PEP_STATUS pEpInStatus, BYTE * DataToWrite,
                        UINT NumBytes)
{
   BYTE bCsrL, bCsrH;

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
         pEpInStatus->uNumBytes = NumBytes;
         pEpInStatus->pData = (BYTE*)DataToWrite;

         // Write <uNumBytes> bytes to the <bEp> FIFO
         FIFOWrite(pEpInStatus->bEp, pEpInStatus->uNumBytes,
            (BYTE*)pEpInStatus->pData);

         BytesToWrite -= NumBytes;
         ReadIndex += NumBytes;
         BlocksWrote++;

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
// - Initialize the global Device Status structure (all zeros)
// - Resets all endpoints
//-----------------------------------------------------------------------------
void USBReset ()
{
   BYTE i, bPower = 0;
   BYTE * pDevStatus;

  // Reset device status structure to all zeros (undefined)
   pDevStatus = (BYTE *)&gDeviceStatus;
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
// Page_Erase
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
// 1) BYTE* Page_Address
//
// Erases the page of FLASH located at Page_Address
//-----------------------------------------------------------------------------
void Page_Erase (BYTE* Page_Address)  small
{
   BYTE EA_Save;                        // Used to save state of global
                                        // interrupt enable
   BYTE xdata *pwrite;                  // xdata pointer used to generate movx

   EA_Save = EA;                        // Save current EA
   EA = 0;                              // Turn off interrupts
   pwrite = (BYTE xdata *)(Page_Address); // Set write pointer to Page_Address
   PSCTL =  0x03;                       // Enable flash erase and writes

   FLKEY =  0xA5;                       // Write flash key sequence to FLKEY
   FLKEY =  0xF1;
   *pwrite = 0x00;                      // Erase flash page using a
                                        // write command

   PSCTL = 0x00;                        // Disable flash erase and writes
   EA = EA_Save;                        // Restore state of EA
}

//-----------------------------------------------------------------------------
// USBReset
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
// 1) BYTE* PageAddress
//
// Writes data to the page of FLASH located at PageAddress
//-----------------------------------------------------------------------------
void Page_Write (BYTE* PageAddress)  small
{
   BYTE EA_Save;                           // Used to save state of global
                                           // interrupt enable
   BYTE xdata *pwrite;                     // Write Pointer
   BYTE xdata *pread;                      // Read Pointer
   UINT x;                                 // Counter for 0-512 bytes

   pread = (BYTE xdata *)(TempStorage);
   EA_Save = EA;                           // Save EA
   EA = 0;                                 // Turn off interrupts
   pwrite = (BYTE xdata *)(PageAddress);
   PSCTL = 0x01;                           // Enable flash writes
   for(x = 0; x<FLASH_PAGE_SIZE; x++)      // Write 512 bytes
   {
      FLKEY = 0xA5;                        // Write flash key sequence
      FLKEY = 0xF1;
      *pwrite = *pread;                    // Write data byte to flash

      pread++;                             // Increment pointers
      pwrite++;
   }
   PSCTL = 0x00;                           // Disable flash writes
   EA = EA_Save;                           // Restore EA
}

//-----------------------------------------------------------------------------
// State_Machine
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Determine new state and act on current state
//-----------------------------------------------------------------------------
void State_Machine(void)
{
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

      case ST_TX_FILE:                     // Send file data to host
         if(BytesToWrite > MAX_BLOCK_SIZE)
         {
            BulkOrInterruptIn(&gEp1InStatus, (BYTE*)(ReadIndex),MAX_BLOCK_SIZE);
            //BytesToWrite -= MAX_BLOCK_SIZE;
            //ReadIndex += MAX_BLOCK_SIZE;

            //  Try to write a second packet to the fifo here
            if(BytesToWrite > MAX_BLOCK_SIZE)
            {
               BulkOrInterruptIn(&gEp1InStatus, 
                                 (BYTE*)(ReadIndex),MAX_BLOCK_SIZE);
            }
            else
            {
               BulkOrInterruptIn(&gEp1InStatus, 
                                 (BYTE*)(ReadIndex),BytesToWrite);
            }
         }
         else
         {
            BulkOrInterruptIn(&gEp1InStatus, (BYTE*)(ReadIndex),BytesToWrite);
            //BytesToWrite = 0;
            //ReadIndex += BytesToWrite;
         }
         //BlocksWrote++;
         if ((BlocksWrote%8) == 0) 
         {
            Led2 = ~Led2;
         }
         if (BlocksWrote == NumBlocks)
         {
            Led2 = 0;
         }
         break;

      default:
         M_State = ST_ERROR;           // Unknown State, stay in Error State
         break;
   }
}

//-----------------------------------------------------------------------------
// Receive_Setup
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Determines whether a read or write request has been received
// Initializes variables for either a read or write operation
//
//-----------------------------------------------------------------------------

void Receive_Setup(void)
{

   if (Buffer[0] == READ_MSG)          // Check See if Read File Setup
   {
      PageIndex = 0;                   // Reset Index
      NumBlocks = LengthFile[2];       // Read NumBlocks from flash stg
      NumBlocks = (NumBlocks > MAX_NUM_BLOCKS)? MAX_NUM_BLOCKS: NumBlocks;
      Buffer[0] = SIZE_MSG;            // Send host size of transfer message
      Buffer[1] = LengthFile[1];
      Buffer[2] = LengthFile[0];
      BulkOrInterruptIn(&gEp1InStatus, &Buffer, 3);
      M_State = ST_TX_FILE;            // Go to TX data state
      BlocksWrote = 0;
      BytesToWrite = BytesToRead;
      ReadIndex = PageIndices[0];
      Led2 = 1;
   }
   else                                // Otherwise assume Write Setup Packet
   {
      BytesToRead = Buffer[1] + 256*Buffer[2];
      NumBlocks = (BYTE)(BytesToRead/MAX_BLOCK_SIZE);    // Find NumBlocks

      if (NumBlocks > MAX_NUM_BLOCKS)  // State Error if transfer too big
      {
         M_State = ST_ERROR;
      }
      else
      {

         if (BytesToRead % MAX_BLOCK_SIZE) 
         {
            NumBlocks++;                // Increment NumBlocks 
			                            // for last partial block
         }                  

         TempStorage[0].Piece[0] = Buffer[2];
         TempStorage[0].Piece[1] = Buffer[1];
         TempStorage[0].Piece[2] = NumBlocks;

         // Write Values to Flash
         Page_Erase(LengthFile);    // Store file data to flash
         Page_Write(LengthFile);

         PageIndex = 0;             // Reset Index
         BlockIndex = 0;
         BlocksRead = 0;
         Led1 = 1;
         M_State = ST_RX_FILE;      // Go to RX data state
      }
   }
}

//-----------------------------------------------------------------------------
// Receive_Setup
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Increments BlockRead and BlockIndex
// After every 8 packets or at the end of the transfer, sends a handshake
//   signal to the host (0xFF)
// Sets the state of the device
//
//-----------------------------------------------------------------------------

void Receive_File(void)
{
   BlocksRead++;       // Increment
   BlockIndex++;
   // If multiple of 8 or last packet, disable interrupts,
   // write page to flash, reset packet index, enable interrupts
   // Send handshake packet 0xFF to host after FLASH write
   if ((BlockIndex == (BLOCKS_PR_PAGE)) || (BlocksRead == NumBlocks))
   {
      Page_Erase((BYTE*)(PageIndices[PageIndex]));
      Page_Write((BYTE*)(PageIndices[PageIndex]));
      PageIndex++;
      Led1 = ~Led1;
      BlockIndex = 0;
      Buffer[0] = 0xFF;

      // Place Handshake packet (0xFF) on the OUT FIFO
      BulkOrInterruptIn (&gEp1InStatus, (BYTE*)&Buffer, 1);
   }

   // Go to Idle state if last packet has been received
   if (BlocksRead == NumBlocks)
   {
      M_State = ST_IDLE_DEV;
      Led1 = 0;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------