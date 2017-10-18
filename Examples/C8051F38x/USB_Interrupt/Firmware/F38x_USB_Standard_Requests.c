//-----------------------------------------------------------------------------
// F38x_USB_Standard_Requests.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This source file contains the subroutines used to handle incoming 
// setup packets. These are called by Handle_Setup in USB_ISR.c and used for 
// USB chapter 9 compliance.
//

// How To Test:    See Readme.txt
//
//
// Target:         C8051F38x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F38x_USB_Interrupt
//
//
// Release 1.1
//    -08 JAN 2008 (GP)
//    -Set rbDATAEND for various requests
//
// Release 1.0
//    -Initial Revision (GP)
//    -22 NOV 2005
//    -Ported from 'F320_USB_Bulk
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include "F38x_USB_Register.h"
#include "F38x_USB_Main.h"
#include "F38x_USB_Descriptor.h"

//-----------------------------------------------------------------------------
// Externs
//-----------------------------------------------------------------------------

// These are created in USB_DESCRIPTOR.h

extern device_descriptor DeviceDesc;            
extern configuration_descriptor ConfigDesc;
extern interface_descriptor InterfaceDesc;
extern endpoint_descriptor Endpoint1Desc;
extern endpoint_descriptor Endpoint2Desc;
extern BYTE* StringDescTable[];

extern setup_buffer Setup;             // Buffer for current device request
extern unsigned int DataSize;
extern unsigned int DataSent;
extern BYTE* DataPtr;

extern BYTE Ep_Status[];               // Contains status bytes for EP 0-2

extern BYTE USB_State;                 // Determines current usb device state

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

// These are response packets used for communication with host
code BYTE ONES_PACKET[2] = {0x01, 0x00};        
code BYTE ZERO_PACKET[2] = {0x00, 0x00};        

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Get_Status
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine returns a two byte status packet to the host
//
//-----------------------------------------------------------------------------

void Get_Status(void)                  
{                                      

   if (Setup.wValue.c[MSB] || Setup.wValue.c[LSB] ||
                                                
   // If non-zero return length or data length not  equal to 2 then send a stall
   // indicating invalid request
   Setup.wLength.c[MSB]    || (Setup.wLength.c[LSB] != 2))                                               
   {                                            
      Force_Stall();
   }

   // Determine if recipient was device, interface, or EP
   switch(Setup.bmRequestType)                  
   {
      // If recipient was device
      case OUT_DEVICE:                          
         if (Setup.wIndex.c[MSB] || Setup.wIndex.c[LSB])
         {
		    // Send stall if request is invalid
            Force_Stall();             
         }
         else
         {
		    // Otherwise send 0x00, indicating bus power and no
			// remote wake-up supported
            DataPtr = (BYTE*)&ZERO_PACKET;      
            DataSize = 2;                       
         }
         break;

      // See if recipient was interface
      case OUT_INTERFACE:                       
	     // Only valid if device is configured and non-zero index
         if ((USB_State != DEV_CONFIGURED) ||
              Setup.wIndex.c[MSB] || Setup.wIndex.c[LSB])                                                
         {
		    // Otherwise send stall to host
            Force_Stall();                      
         }
         else
         {
		    // Status packet always returns 0x00
            DataPtr = (BYTE*)&ZERO_PACKET;      
            DataSize = 2;
         }
         break;

      // See if recipient was an endpoint
      case OUT_ENDPOINT:                        
	     // Make sure device is configured and index msb = 0x00
         if ((USB_State != DEV_CONFIGURED) || Setup.wIndex.c[MSB])                   
         {                                     
            Force_Stall();              // otherwise return stall to host
         }
         else
         {
            // Handle case if request is directed to EP 1
            if (Setup.wIndex.c[LSB] == IN_EP1)  
            {
               if (Ep_Status[1] == EP_HALT)
               {                                
			      // If endpoint is halted, return 0x01,0x00
                  DataPtr = (BYTE*)&ONES_PACKET;
                  DataSize = 2;
               }
               else
               {
			      // Otherwise return 0x00,0x00 to indicate endpoint active
                  DataPtr = (BYTE*)&ZERO_PACKET;
                  DataSize = 2;
               }
            }
            else
            {
			   // If request is directed to endpoint 2, send either
			   // 0x01,0x00 if endpoint halted or 0x00,0x00 if endpoint is active
               if (Setup.wIndex.c[LSB] == OUT_EP2)
                                                
               {                                
                  if (Ep_Status[2] == EP_HALT)
                  {
                     DataPtr = (BYTE*)&ONES_PACKET;
                     DataSize = 2;
                  }
                  else
                  {
                     DataPtr = (BYTE*)&ZERO_PACKET;
                     DataSize = 2;
                  }
               }
               else
               {
                  Force_Stall();       // Send stall if unexpected data
               }
            }
         }
         break;

      default:
         Force_Stall();
         break;
   }
   if (Ep_Status[0] != EP_STALL)
   {
      // Set serviced Setup Packet, Endpoint 0 intransmit mode, 
	  // and reset DataSent counter
      POLL_WRITE_BYTE(E0CSR, rbSOPRDY);        
      Ep_Status[0] = EP_TX;                     
      DataSent = 0;
   }
}

//-----------------------------------------------------------------------------
// Clear_Feature
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine can clear Halt Endpoint features on endpoint 1 and 2.
//
//-----------------------------------------------------------------------------

void Clear_Feature()                            
{                                               

   if ((USB_State != DEV_CONFIGURED)          ||// Send procedural stall if device isn't configured
   (Setup.bmRequestType == IN_DEVICE)         ||// or request is made to host(remote wakeup not supported)
   (Setup.bmRequestType == IN_INTERFACE)      ||// or request is made to interface
   Setup.wValue.c[MSB]  || Setup.wIndex.c[MSB]||// or msbs of value or index set to non-zero value
   Setup.wLength.c[MSB] || Setup.wLength.c[LSB])// or data length set to non-zero.
   {
      Force_Stall();
   }

   else
   {
      if ((Setup.bmRequestType == IN_ENDPOINT)&&// Verify that packet was directed at an endpoint
      (Setup.wValue.c[LSB] == ENDPOINT_HALT)  &&// the feature selected was HALT_ENDPOINT
      ((Setup.wIndex.c[LSB] == IN_EP1) ||       // and that the request was directed at EP 1 in
      (Setup.wIndex.c[LSB] == OUT_EP2)))        // or EP 2 out
      {
         if (Setup.wIndex.c[LSB] == IN_EP1)
         {
            POLL_WRITE_BYTE (INDEX, 1);         // Clear feature endpoint 1 halt
            POLL_WRITE_BYTE (EINCSR1, rbInCLRDT);
            Ep_Status[1] = EP_IDLE;             // Set endpoint 1 status back to idle
         }
         else
         {
            POLL_WRITE_BYTE (INDEX, 2);         // Clear feature endpoint 2 halt
            POLL_WRITE_BYTE (EOUTCSR1, rbOutCLRDT);
            Ep_Status[2] = EP_IDLE;             // Set endpoint 2 status back to idle
         }
      }
      else
      {
         Force_Stall();                         // Send procedural stall
      }
   }
   POLL_WRITE_BYTE(INDEX, 0);                   // Reset Index to 0
   if (Ep_Status[0] != EP_STALL)
   {
      // Indicate setup packet has been serviced
      POLL_WRITE_BYTE(E0CSR, (rbSOPRDY | rbDATAEND));
   }
}

//-----------------------------------------------------------------------------
// Set_Feature
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine will set the EP Halt feature for endpoints 1 and 2
//
//-----------------------------------------------------------------------------

void Set_Feature(void)                          
{                                             

   if ((USB_State != DEV_CONFIGURED)          ||// Make sure device is configured, setup data
   (Setup.bmRequestType == IN_DEVICE)         ||// is all valid and that request is directed at
   (Setup.bmRequestType == IN_INTERFACE)      ||// an endpoint
   Setup.wValue.c[MSB]  || Setup.wIndex.c[MSB]||
   Setup.wLength.c[MSB] || Setup.wLength.c[LSB])
   {
      Force_Stall();                            // Otherwise send stall to host
   }

   else
   {
      if ((Setup.bmRequestType == IN_ENDPOINT)&&// Make sure endpoint exists and that halt
      (Setup.wValue.c[LSB] == ENDPOINT_HALT)  &&// endpoint feature is selected
      ((Setup.wIndex.c[LSB] == IN_EP1)        ||
      (Setup.wIndex.c[LSB] == OUT_EP2)))
      {
         if (Setup.wIndex.c[LSB] == IN_EP1)
         {
            POLL_WRITE_BYTE (INDEX, 1);         // Set feature endpoint 1 halt
            POLL_WRITE_BYTE (EINCSR1, rbInSDSTL);
            Ep_Status[1] = EP_HALT;
         }
         else
         {
            POLL_WRITE_BYTE (INDEX, 2);         // Set feature Ep2 halt
            POLL_WRITE_BYTE (EOUTCSR1, rbOutSDSTL);
            Ep_Status[2] = EP_HALT;
         }
      }
      else
      {
         Force_Stall();                         // Send procedural stall
      }
   }
   POLL_WRITE_BYTE(INDEX, 0);
   if (Ep_Status[0] != EP_STALL)
   {
      // Indicate setup packet has been serviced
      POLL_WRITE_BYTE(E0CSR, (rbSOPRDY | rbDATAEND));
   }
}

//-----------------------------------------------------------------------------
// Set_Address
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Set new function address
//
//-----------------------------------------------------------------------------

void Set_Address(void)                          
{
   if ((Setup.bmRequestType != IN_DEVICE)     ||// Request must be directed to device
   Setup.wIndex.c[MSB]  || Setup.wIndex.c[LSB]||// with index and length set to zero.
   Setup.wLength.c[MSB] || Setup.wLength.c[LSB]||
   Setup.wValue.c[MSB]  || (Setup.wValue.c[LSB] & 0x80))
   {
     Force_Stall();                             // Send stall if setup data invalid
   }

   Ep_Status[0] = EP_ADDRESS;                   // Set endpoint zero to update address next status phase
   if (Setup.wValue.c[LSB] != 0)
   {
      USB_State = DEV_ADDRESS;                  // Indicate that device state is now address
   }
   else
   {
      USB_State = DEV_DEFAULT;                  // If new address was 0x00, return device to default
   }                                            // state
   if (Ep_Status[0] != EP_STALL)
   {
      // Indicate setup packet has been serviced
      POLL_WRITE_BYTE(E0CSR, (rbSOPRDY | rbDATAEND));
   }
}

//-----------------------------------------------------------------------------
// Get_Descriptor
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine sets the data pointer and size to correct 
// descriptor and sets the endpoint status to transmit
//
//-----------------------------------------------------------------------------

void Get_Descriptor(void)                       
{                                              

   switch(Setup.wValue.c[MSB])                  // Determine which type of descriptor
   {                                            // was requested, and set data ptr and
      case DSC_DEVICE:                          // size accordingly
         DataPtr = (BYTE*) &DeviceDesc;
         DataSize = DeviceDesc.bLength;
         break;

      case DSC_CONFIG:
         DataPtr = (BYTE*) &ConfigDesc;
                                                // Compiler Specific - The next statement reverses the
                                                // bytes in the configuration descriptor for the compiler
         DataSize = ConfigDesc.wTotalLength.c[MSB] + 256*ConfigDesc.wTotalLength.c[LSB];
         break;

      case DSC_STRING:
         DataPtr = StringDescTable[Setup.wValue.c[LSB]];
                                                // Can have a maximum of 255 strings
         DataSize = *DataPtr;
         break;

      case DSC_INTERFACE:
         DataPtr = (BYTE*) &InterfaceDesc;
         DataSize = InterfaceDesc.bLength;
         break;

      case DSC_ENDPOINT:
         if ((Setup.wValue.c[LSB] == IN_EP1) ||
         (Setup.wValue.c[LSB] == OUT_EP2))
         {
            if (Setup.wValue.c[LSB] == IN_EP1)
            {
               DataPtr = (BYTE*) &Endpoint1Desc;
               DataSize = Endpoint1Desc.bLength;
            }
            else
            {
               DataPtr = (BYTE*) &Endpoint2Desc;
               DataSize = Endpoint2Desc.bLength;
            }
         }
         else
         {
            Force_Stall();
         }
         break;

      default:
         Force_Stall();                         // Send Stall if unsupported request
         break;
   }

   if (Setup.wValue.c[MSB] == DSC_DEVICE ||     // Verify that the requested descriptor is
   Setup.wValue.c[MSB] == DSC_CONFIG     ||     // valid
   Setup.wValue.c[MSB] == DSC_STRING     ||
   Setup.wValue.c[MSB] == DSC_INTERFACE  ||
   Setup.wValue.c[MSB] == DSC_ENDPOINT)
   {
      if ((Setup.wLength.c[LSB] < DataSize) &&
      (Setup.wLength.c[MSB] == 0))
      {
         DataSize = Setup.wLength.i;       // Send only requested amount of data
      }
   }
   if (Ep_Status[0] != EP_STALL)                // Make sure endpoint not in stall mode
   {
     POLL_WRITE_BYTE(E0CSR, rbSOPRDY);          // Service Setup Packet
     Ep_Status[0] = EP_TX;                      // Put endpoint in transmit mode
     DataSent = 0;                              // Reset Data Sent counter
   }
}

//-----------------------------------------------------------------------------
// Get_Configuration
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine returns current configuration value
//
//-----------------------------------------------------------------------------

void Get_Configuration(void)                   
{
   if ((Setup.bmRequestType != OUT_DEVICE)    ||// This request must be directed to the device
   Setup.wValue.c[MSB]  || Setup.wValue.c[LSB]||// with value word set to zero
   Setup.wIndex.c[MSB]  || Setup.wIndex.c[LSB]||// and index set to zero
   Setup.wLength.c[MSB] || (Setup.wLength.c[LSB] != 1))// and setup length set to one
   {
      Force_Stall();                            // Otherwise send a stall to host
   }

   else
   {
      if (USB_State == DEV_CONFIGURED)          // If the device is configured, then return value 0x01
      {                                         // since this software only supports one configuration
         DataPtr = (BYTE*)&ONES_PACKET;
         DataSize = 1;
      }
      if (USB_State == DEV_ADDRESS)             // If the device is in address state, it is not
      {                                         // configured, so return 0x00
         DataPtr = (BYTE*)&ZERO_PACKET;
         DataSize = 1;
      }
   }
   if (Ep_Status[0] != EP_STALL)
   {
      POLL_WRITE_BYTE(E0CSR, rbSOPRDY);         // Set Serviced Out Packet bit
      Ep_Status[0] = EP_TX;                     // Put endpoint into transmit mode
      DataSent = 0;                             // Reset Data Sent counter to zero
   }
}

//-----------------------------------------------------------------------------
// Set_Configuration
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine allows host to change current device configuration value
//
//-----------------------------------------------------------------------------

void Set_Configuration(void)                   
{                                               

   if ((USB_State == DEV_DEFAULT)             ||// Device must be addressed before configured
   (Setup.bmRequestType != IN_DEVICE)         ||// and request recipient must be the device
   Setup.wIndex.c[MSB]  || Setup.wIndex.c[LSB]||// the index and length words must be zero
   Setup.wLength.c[MSB] || Setup.wLength.c[LSB] ||
   Setup.wValue.c[MSB]  || (Setup.wValue.c[LSB] > 1))// This software only supports config = 0,1
   {
      Force_Stall();                            // Send stall if setup data is invalid
   }

   else
   {
      if (Setup.wValue.c[LSB] > 0)              // Any positive configuration request
      {                                         // results in configuration being set to 1
         USB_State = DEV_CONFIGURED;
         Ep_Status[1] = EP_IDLE;                // Set endpoint status to idle (enabled)
         Ep_Status[2] = EP_IDLE;
         POLL_WRITE_BYTE(INDEX, 1);             // Change index to endpoint 1
         POLL_WRITE_BYTE(EINCSR2, rbInDIRSEL);  // Set DIRSEL to indicate endpoint 1 is IN
         Handle_In1();                          // Put first data packet on fifo
         POLL_WRITE_BYTE(INDEX, 0);             // Set index back to endpoint 0
      }
      else
      {
         USB_State = DEV_ADDRESS;               // Unconfigures device by setting state to
         Ep_Status[1] = EP_HALT;                // address, and changing endpoint 1 and 2
         Ep_Status[2] = EP_HALT;                // status to halt
      }
   }
   if (Ep_Status[0] != EP_STALL)
   {
      // Indicate setup packet has been serviced
      POLL_WRITE_BYTE(E0CSR, (rbSOPRDY | rbDATAEND));
   }
}

//-----------------------------------------------------------------------------
// Get_Interface
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine returns 0x00, since only one interface is supported by 
// this firmware
//
//-----------------------------------------------------------------------------

void Get_Interface(void)                        
{                                              

   if ((USB_State != DEV_CONFIGURED)      ||    // If device is not configured
   (Setup.bmRequestType != OUT_INTERFACE) ||    // or recipient is not an interface
   Setup.wValue.c[MSB]  ||Setup.wValue.c[LSB] ||// or non-zero value or index fields
   Setup.wIndex.c[MSB]  ||Setup.wIndex.c[LSB] ||// or data length not equal to one
   Setup.wLength.c[MSB] ||(Setup.wLength.c[LSB] != 1))
   {
      Force_Stall();                            // Then return stall due to invalid request
   }

   else
   {
      DataPtr = (BYTE*)&ZERO_PACKET;            // Otherwise, return 0x00 to host
      DataSize = 1;
   }
   if (Ep_Status[0] != EP_STALL)
   {
      POLL_WRITE_BYTE(E0CSR, rbSOPRDY);         // Set Serviced Setup packet, put endpoint in transmit
      Ep_Status[0] = EP_TX;                     // mode and reset Data sent counter
      DataSent = 0;
   }
}

//-----------------------------------------------------------------------------
// Set_Interface
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine allows host to change current device configuration value
//
//-----------------------------------------------------------------------------

void Set_Interface(void)
{
   // Make sure request is directed at interface and all other packet values 
   // are set to zero

   if ((Setup.bmRequestType != IN_INTERFACE)  ||
   Setup.wLength.c[MSB] ||Setup.wLength.c[LSB]|| 
   Setup.wValue.c[MSB]  ||Setup.wValue.c[LSB] ||
   Setup.wIndex.c[MSB]  ||Setup.wIndex.c[LSB])
   {
      // Otherwise send a stall to host
      Force_Stall();                            
   }
   if (Ep_Status[0] != EP_STALL)
   {
      // Indicate setup packet has been serviced
      POLL_WRITE_BYTE(E0CSR, (rbSOPRDY | rbDATAEND));                                               
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------