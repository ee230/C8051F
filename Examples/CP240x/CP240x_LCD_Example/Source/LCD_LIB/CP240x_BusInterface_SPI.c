//-----------------------------------------------------------------------------
// CP240x_BusInterface_SPI.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// This file contains the implemention for the low level SPI interface. 
//
// Target:         Multiple
// Tool chain:     Generic
//                 Silicon Laboratories IDE version 3.40 
// Project Name:   CP240x_Firmware
//
// 
//
// Release 0.0
//    -Initial Release (GRP / ADT / FB)
//    -03 APR 2009
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F930_defs.h>
#include "lcd_lib.h"

#if(BUS_INTERFACE == SPI)


//-----------------------------------------------------------------------------
// Required Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Interface Initialization
//-----------------------------------------------------------------------------
void CP240x_BusInterface_Init()
{
   #if SPI_N == 0

   SPI0CFG   = 0x40;
   SPI0CN    = 0x01;

   // Set the SPI Data Rate to F_SCK_MAX
   SPI0CKR   = (SYSCLK/(2*SPI_FREQUENCY))-1;
   
   #elif SPI_N == 1

   SPI1CFG   = 0x40;
   SPI1CN    = 0x01;

   // Set the SPI Data Rate to F_SCK_MAX
   SPI1CKR   = (SYSCLK/(2*SPI_FREQUENCY))-1;
   
   #endif
}
//-----------------------------------------------------------------------------
// Single-Byte Register Read/Write
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CP240x_RegWrite
//-----------------------------------------------------------------------------
//
// Return Value : 
//    unsigned char - contents of the CP240x at the location ADDR
//	  
// Parameters   : Unsigned 16 bit ADDR
//				      Unsigned char (8 bit)  value
//
//	Writes value to the CP240x at ADDR.
//-----------------------------------------------------------------------------
void CP240x_RegWrite (U8 addr, U8 value)
{  

   NSS = 0;                            // Activate Slave Select 
   
   // Write the SPI Command
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = REGWRITE;                  // Write data
   while (!SPIF);                      // Wait for SPI Interrupt Flag

   // Write the address high byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = 0x00;                      // Write data
   while (!SPIF);                      // Wait for SPI Interrupt Flag

   // Write the address low byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = addr;                      // Write data
   while (!SPIF);                      // Wait for SPI Interrupt Flag

   // Write the data byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = value;                     // Write data
   while (!SPIF);                      // Wait for SPI Interrupt Flag

   NSS = 1;                            // De-Activate Slave Select 

  
}

//-----------------------------------------------------------------------------
// CP240x_RegRead
//-----------------------------------------------------------------------------
//
// Return Value : 
//    unsigned char - contents of the CP240x at the location <addr>
//	  
// Parameters   : Unsigned 8 bit <addr>
//
//	Reads the value from the CP240x register at <addr> using EMIF and returns it.
//-----------------------------------------------------------------------------
U8 CP240x_RegRead(U8 addr)
{
   U8 value;

    
 
   NSS = 0;                            // Activate Slave Select 
   
 
   // Write the SPI Command
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = REGREAD;                   // Write data
   while (!SPIF);                      // Wait for SPI Interrupt Flag

   // Write the address high byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = 0x00;                      // Write data
   while (!SPIF);                      // Wait for SPI Interrupt Flag

   // Write the address low byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = addr;                      // Write data
   while (!SPIF);                      // Wait for SPI Interrupt Flag

   // Generate the wait state
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = 0xFF;                      // Write data
   while (!SPIF);                      // Wait for SPI Interrupt Flag

   // Read the data byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = 0xAA;                      // Write data
   while (!SPIF);                      // Wait for SPI Interrupt Flag
   value = SPIDAT;                     // Read the data
          
   NSS = 1;                            // De-Activate Slave Select 

   return value;
   
}


//-----------------------------------------------------------------------------
// Single-Byte RAM Read/Write
//-----------------------------------------------------------------------------

#if(SINGLE_BYTE_RAM_OPERATIONS)
//-----------------------------------------------------------------------------
// CP240x_RAMWrite
//-----------------------------------------------------------------------------
//
// Return Value : 
//    unsigned char - contents of the CP240x at the location <addr>
//	  
// Parameters   : Unsigned 16 bit <addr>
//						Unsigned 8-bit <value>
//
//	Writes value to the CP240x at ADDR.
//-----------------------------------------------------------------------------
void CP240x_RAMWrite(U8 addr, U8 value)
{

   NSS = 0;                            // Activate Slave Select 
   
   // Write the SPI Command
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = RAMWRITE;                  // Write data
   while(!SPIF);	                     // Wait for SPI Interrupt Flag

   // Write the address high byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = 0x04;                      // Write data
   #if(AUTODECREMENT_BLOCK_RAM_OPERATIONS)
   if((CP240x_MSCN_Local & 0x20) == BACKWARD)
   {  
      // Reverse Direction
      addr = ~addr;

   }
   #endif 
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag

   // Write the address low byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = (addr);                    // Write data
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag

   // Write the data byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = value;                     // Write data
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag

   NSS = 1;                            // De-Activate Slave Select 
   
}

//-----------------------------------------------------------------------------
// CP240x_RAMRead
//-----------------------------------------------------------------------------
//
// Return Value : 
//    unsigned char - contents of the CP240x register at the location <addr>
//	  
// Parameters   : Unsigned 16 bit <addr>
//
//	Reads the value from the CP240x at ADDR and returns it.
//-----------------------------------------------------------------------------
U8 CP240x_RAMRead(U8 addr)
{

   U8 value;   

   NSS = 0;                            // Activate Slave Select 
  
   // Write the SPI Command
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = RAMREAD;                   // Write data
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag

   // Write the address high byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = 0x04;                      // Write data
   #if(AUTODECREMENT_BLOCK_RAM_OPERATIONS)
   if((CP240x_MSCN_Local & 0x20) == BACKWARD)
   {  
      // Reverse Direction
      addr = ~addr;

   }
   #endif 
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag

   // Write the address low byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = (addr & 0x00FF);           // Write data
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag

   // Generate the wait state
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = 0x00;                      // Write data
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag

   // Read the data byte
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = 0xFF;                      // Write data
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag
   value = SPIDAT;                     // Read the data
   
   NSS = 1;                            // De-Activate Slave Select 

   return value;

}
#endif //(SINGLE_BYTE_RAM_OPERATIONS)

//-----------------------------------------------------------------------------
// Block Read/Write
//-----------------------------------------------------------------------------

#if(AUTODECREMENT_BLOCK_RAM_OPERATIONS)
//-----------------------------------------------------------------------------
// CP240x_SetRAMBlockDirection
//-----------------------------------------------------------------------------
//
// Return Value : 
//    None
//	  
// Parameters   : Unsigned 8-bit: dir
//
// Sets the RAM block direction to FORWARD (auto-increment) or BACKWARD 
// (auto-decrement).
//
//-----------------------------------------------------------------------------
void CP240x_SetRAMBlockDirection(U8 dir)
{
   // Clear all bits except for bit 5 (direction bit)
   dir &= 0x20;
   
   // Perform a read, modify, write on the MSCN register
   if(dir == FORWARD) CP240x_MSCN_ClearBits(~BACKWARD);
   else               CP240x_MSCN_SetBits(BACKWARD);
 
}
#endif //(AUTODECREMENT_BLOCK_RAM_OPERATIONS)

//-----------------------------------------------------------------------------
// CP240x_BlockOperation
//-----------------------------------------------------------------------------
//
// Return Value : 
//    None
//	  
// Parameters   : Unsigned 8-bit: cmd
//                Unsigned 8 bit: addr
//						Pointer to unsigned char: ptr
//						Unsigned 8 bit: len
//
//	Performs the comand on "len" values to the CP240x starting at ADDR. If len,
// has a value of zero, it assumes 256 bytes. The values to be read or
//	written are stored into the array starting at ptr.  
//
//-----------------------------------------------------------------------------
void CP240x_BlockOperation(U8 cmd, U8 addr, U8 *ptr, U8 len)
{

   U8 dat;
   U8 operation;
   U16 length;
    
   NSS = 0;                            // Activate Slave Select 
  
   //----------------------------- 
   // Write the SPI Command
   //-----------------------------
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = cmd;                       // Set the CP240x Command Byte
   
   // Preload the address high byte   
   if (cmd == RAMREAD || cmd == RAMWRITE)
   {  
      #if(AUTODECREMENT_BLOCK_RAM_OPERATIONS)
      if((CP240x_MSCN_Local & 0x20) == BACKWARD)
      {  
         // Reverse Direction
         addr = ~addr;

      }
      #endif 
      dat = 0x04;
   } else
   {
      dat = 0x00;                      // Register Access
   }
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag

   //-----------------------------
   // Write the address high byte
   //-----------------------------
   
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = dat;                       // Write data
   
   // Determine the operation: READ or WRITE
   if( cmd == REGREAD || cmd == RAMREAD || cmd == REGPOLL)
   {  
      operation = READ;
   } else
   {
      operation  = WRITE;
   }
   // Copy len into length
   if(len)
   { 
      length = len;
   } else
   { 
      length = 256;
   }
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag

   //-----------------------------
   // Write the address low byte
   //-----------------------------
 
   SPIF = 0;                           // Clear SPI Interrupt Flag
   SPIDAT = addr;                      // Write data

   // Preload the first data byte on a write
   if(operation == WRITE)
   {
      dat = *ptr++;                      
   } else
   {
      dat = 0x00;
   }
   
   while(!SPIF); 	                     // Wait for SPI Interrupt Flag
   
   //-----------------------------  
   // Wait State
   //-----------------------------
   if(operation == READ)
   {
      SPIF = 0;                        // Clear SPI Interrupt Flag
      SPIDAT = 0x00;                   // Initiate wait state
      while(!SPIF); 	                  // Wait for SPI Interrupt Flag
   }

   //-----------------------------  
   // Read or Write Data
   //-----------------------------
   while(length)
   {  
 
      SPIF = 0;                        // Clear SPI Interrupt Flag
      SPIDAT = dat;                    // Write data
      length--;                        // Decrement Remaining Bytes
            
      if(operation == WRITE)
      {
         // Write the data
         dat = *ptr++;                    // Preload next data
         while(!SPIF); 	                  // Wait for SPI Interrupt Flag
    
      } else
      {
         // Read the data
         while(!SPIF); 	                  // Wait for SPI Interrupt Flag
         *ptr++ = SPIDAT;                 // Write data to output array    
      }      
   }

   NSS = 1;                               // De-Activate Slave Select 
   
}


//-----------------------------------------------------------------------------
// ULP Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CP240x_EnterULP
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : mode - the ULP mode
//
// Places the CP240x into the selected ULP mode. A low frequency ULP clock 
// must be present for the device to enter ULP mode. 
//-----------------------------------------------------------------------------
void CP240x_EnterULP(U8 mode)
{
   UU16 INTEN_registers;

   // Enable the Port Match, SmaRTClock Alarm, and SmaRTClock Fail Interrupts
   // Disable all other interrupts   
   INTEN_registers.U16 = 0x1900;
   CP240x_RegBlockWrite(INT0EN, &INTEN_registers.U8[MSB], 2);

   //INTEN_registers.U16 = 0x0000;
   //CP240x_RegBlockWrite(INT0EN, &INTEN_registers.U8[MSB], 2);
   CP240x_ClearAllInterrupts();   

   // Check if all Port Masks need to be disabled
   if(mode == ULP_SHUTDOWN)
   {
      CP240x_MSCN_SetBits(0x02);          // Clear ULP RAM (disable port masks)
   }
   
   // Configure charge pump for loose regulstion
   CP240x_RegWrite(MSCF, 0x80);           // Loose Regulation

   // Enter the selected ULP Mode
   CP240x_RegWrite (ULPCN, mode);         // Enter ULP mode


}



//-----------------------------------------------------------------------------
// CP240x_ExitULP ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : none
//
// This function will exit ULP mode.
//  
//-----------------------------------------------------------------------------
void CP240x_ExitULP(void)
{
   U8 interrupt_flags[3];   
   U8 forced_wake;   

   // If the interrupt pin is high, then we are forcing the CP240x to wake up
   // If the interrupt pin is low, then the device is already awake
   forced_wake = INT;
   
   // If the device is not already awake, force it to wake up
   if(forced_wake)
   {
      NSS = 0;                            // Activate Slave Select 
      while (INT);                        // Wait for INT to go low
   }

   // Re-configure Port I/Os
   CP240x_PortIO_Configure();   

   // Read the Interrupt Registers and Wake-up Source Flags
   CP240x_RegBlockRead(INT0RD, interrupt_flags, 3);
   
   // Decode the interrupt flag and set global state variables
   
   // Clear all Flags and Interrupts
   CP240x_ClearAllFlags();
   CP240x_ClearAllInterrupts();

}



#endif //(BUS_INTERFACE == SPI)

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------



