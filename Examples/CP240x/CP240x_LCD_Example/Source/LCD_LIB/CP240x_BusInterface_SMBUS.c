//-----------------------------------------------------------------------------
// CP240x_BusInterface_SMBUS.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// This file contains the implemention for the low level SMBUS interface. 
//
// Target:         Multiple
// Tool chain:     Generic
//                 Silicon Laboratories IDE version 3.40 
// Project Name:   CP240x_Firmware
//
// 
//
// Release 1.0
//    -26 OCT 2009
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F930_defs.h>
#include "lcd_lib.h"

#if (BUS_INTERFACE == SMBUS)

//-----------------------------------------------------------------------------
// SMBUS Constants
//-----------------------------------------------------------------------------
#define  SMB_WRITE      0x00           // SMBus WRITE command
#define  SMB_READ       0x01           // SMBus READ command

// Status vector - top 4 bits only
#define  SMB_MTSTA      0xE0           // (MT) start transmitted
#define  SMB_MTDB       0xC0           // (MT) data byte transmitted
#define  SMB_MRDB       0x80           // (MR) data byte received
// End status vector definition

#define  CP240x_CMDHDR_LEN   3         // Number of bytes in the CP240x
                                       // header (CMD, ADDRH, ADDRL)


//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

// Global holder for SMBus data.
// All transmit data is read from here
U8 SMB_DATA_OUT[4];

// Global pointer to data array
U8 *DATA_OUT_PTR;
U8 *DATA_IN_PTR;

// Global length of send and receive data
U16 SEND_LEN;
U16 RECV_LEN;

U8 TARGET;                             // Target SMBus slave address

bit SMB_BUSY;                          // Software flag to indicate when the
                                       // SMB_Read() or SMB_Write() functions
                                       // have claimed the SMBus

bit SMB_RW;                            // Software flag to indicate the
                                       // direction of the current transfer

bit REPEAT_START;                      // Software flag to indicate that a 
                                       // repeated start and a change in 
                                       // transfer direction should occur

//-----------------------------------------------------------------------------
// Local Function Prototypes
//-----------------------------------------------------------------------------
INTERRUPT_PROTO (SMBUS0_ISR, INTERRUPT_SMBUS0);

#if(ENABLE_SCL_LOW_TIMEOUT)          
INTERRUPT_PROTO (TIMER3_ISR, INTERRUPT_TIMER3);
#endif


//-----------------------------------------------------------------------------
// Required Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Interface Initialization
//-----------------------------------------------------------------------------
void CP240x_BusInterface_Init()
{
   SMB0CF = 0x54;                      // Disable slave mode - (Master Only)
                                       // Enable setup & hold time extensions
                                       // Enable SMBus Free timeout detect

   SMB0CF |= (SMBUS_TIMER & 0x03);     // Select the SMBus Timer

   #if(ENABLE_SCL_LOW_TIMEOUT)          
      SMB0CF |= 0x08;                  // Enable SCL low timeout detect
   
      TMR3CN = 0x00;                   // Timer3 configured for 16-bit auto-
                                       // reload, low-byte interrupt disabled

      CKCON &= ~0x40;                  // Timer3 uses SYSCLK/12

      TMR3RL = (U16) -(SYSCLK/12/40);  // Timer3 configured to overflow after
      TMR3 = TMR3RL;                   // ~25ms (for SMBus low timeout detect):
                                       // 1/.025 = 40

      EIE1 |= 0x80;                    // Timer3 interrupt enable
      TMR3CN |= 0x04;                  // Start Timer3
      
   #endif
   

   // Configure the selected SMBus Timer
   #if(SMBUS_TIMER == TIMER0)

      CKCON |= 0x04;                   // Timer0 clocked from system clock
      TMOD &= ~0x0f;                   // TMOD: timer 0 in 8-bit autoreload
      TMOD |=  0x02;
      TH0 = -(SYSCLK/SMBUS_FREQUENCY/3); // Write the reload value
      TL0 = TH0;                       // Initialize timer
      TR0 = 1;                         // Start Timer
   
      #if(SYSCLK/SMBUS_FREQUENCY/3 > 255)
         #error "Increase SMBus Frequncy or use 16-bit Timer"
      #endif

   #elif (SMBUS_TIMER == TIMER1)
      
      CKCON |= 0x08;                   // Timer1 clocked from system clock
      TMOD &= ~0xf0;                   // TMOD: timer 1 in 8-bit autoreload
      TMOD |=  0x20;
      TH1 = -(SYSCLK/SMBUS_FREQUENCY/3); // Write the reload value
      TL1 = TH0;                       // Initialize timer
      TR1 = 1;                         // Start Timer

      #if(SYSCLK/SMBUS_FREQUENCY/3 > 255)
         #error "Increase SMBus Frequency, reduce SYSCLK, or use 16-bit Timer"
      #endif


   #elif (SMBUS_TIMER == TIMER2)
      
      CKCON |= 0x10;                   // Timer2 clocked from system clock
      T2SPLIT = 0;                     // Split Mode disabled (16-bit mode)
      TMR2RL = -(SYSCLK/SMBUS_FREQUENCY/3);  // Write the reload value
      TMR2 = TMR2RL;                   // Initialize timer
      TR2 = 1;                         // Start Timer

   #elif (SMBUS_TIMER == TIMER2_HB)

      CKCON |= 0x20;                   // Timer2 HB clocked from system clock
      T2SPLIT = 1;                     // Split Mode enabled (8-bit mode)
      TMR2RLH = -(SYSCLK/SMBUS_FREQUENCY/3); // Write the reload value
      TMR2H = TMR2RLH;                 // Initialize timer
      TR2 = 1;                         // Start Timer

      #if(SYSCLK/SMBUS_FREQUENCY/3 > 255)
         #error "Increase SMBus Frequency, reduce SYSCLK, or use 16-bit Timer"
      #endif


   #elif (SMBUS_TIMER == TIMER2_LB)

      CKCON |= 0x10;                   // Timer2 LB clocked from system clock
      T2SPLIT = 1;                     // Split Mode enabled (8-bit mode)
      TMR2RLL = -(SYSCLK/SMBUS_FREQUENCY/3);  // Write the reload value
      TMR2L = TMR2RLL;                 // Initialize timer
                                       // Timer always running in 8-bit mode

      #if(SYSCLK/SMBUS_FREQUENCY/3 > 255)
         #error "Increase SMBus Frequency, reduce SYSCLK, or use 16-bit Timer"
      #endif
   

   #else
      #error "No SMBus Timer Selected"
   #endif

   SMB0CF |= 0x80;                     // Enable SMBus
   EIE1 |= 0x01;                       // Enable the SMBus interrupt
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
void CP240x_RegWrite(U8 addr, U8 value)
{
   while (SMB_BUSY);                   // Wait for SMBus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)

   SMB_RW = 0;                         // Mark this transfer as a WRITE
   REPEAT_START = 0;                   // Mark this transfer to not generate a 
                                       // repeated start + READ after the
                                       // write operation is complete.

   TARGET = CP240X_SLAVE_ADDR;                // Set the Slave Address 
   
   SMB_DATA_OUT[0] = REGSET;           // Set the CP240x Command Byte
   SMB_DATA_OUT[1] = 0x00;             // Set the CP240x Address High Byte
   SMB_DATA_OUT[2] = addr;             // Set the CP240x Address Low Byte
   SMB_DATA_OUT[3] = value;            // Set the Data Byte

   DATA_OUT_PTR = &SMB_DATA_OUT[3];    // Initialize Data Pointer to the 
                                       // address of the first byte of data
   
   SEND_LEN = CP240x_CMDHDR_LEN + 1;   // Set total number of bytes to transmit
   RECV_LEN = 0;                       // Set number of bytes to receive
   
   STA = 1;                            // Start transfer

   while (SMB_BUSY);

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
//	Reads the value from the CP240x register at <addr> and returns it.
//-----------------------------------------------------------------------------
U8 CP240x_RegRead(U8 addr)
{
   U8 value;

   while (SMB_BUSY);                   // Wait for bus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)

   SMB_RW = 0;                         // Mark this transfer as a WRITE
   REPEAT_START = 1;                   // Mark this transfer to generate a 
                                       // repeated start + READ after the
                                       // write operation is complete.

   TARGET = CP240X_SLAVE_ADDR;                // Set the Slave Address 
   
   SMB_DATA_OUT[0] = REGREAD;          // Set the CP240x Command Byte
   SMB_DATA_OUT[1] = 0x00;             // Set the CP240x Address High Byte
   SMB_DATA_OUT[2] = addr;             // Set the CP240x Address Low Byte
   
   DATA_IN_PTR = &value;               // Initialize Data Pointer to the 
                                       // address of the receive buffer
   
   SEND_LEN = CP240x_CMDHDR_LEN + 0;   // Set total number of bytes to transmit
   RECV_LEN = 1;                       // Set number of bytes to receive

   STA = 1;                            // Start transfer

   while (SMB_BUSY);                   // Wait for transfer to complete

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

   while (SMB_BUSY);                   // Wait for SMBus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)

   SMB_RW = 0;                         // Mark this transfer as a WRITE
   REPEAT_START = 0;                   // Mark this transfer to not generate a 
                                       // repeated start + READ after the
                                       // write operation is complete.

   TARGET = CP240X_SLAVE_ADDR;                // Set the Slave Address 


   #if(AUTODECREMENT_BLOCK_RAM_OPERATIONS)
   if((CP240x_MSCN_Local & 0x20) == BACKWARD)
   {  
      // Reverse Direction
      addr = ~addr;

   }
   #endif 

   
   SMB_DATA_OUT[0] = RAMWRITE;         // Set the CP240x Command Byte
   SMB_DATA_OUT[1] = (0x04);           // Set the CP240x Address High Byte
   SMB_DATA_OUT[2] = (addr);           // Set the CP240x Address Low Byte
   SMB_DATA_OUT[3] = value;            // Set the Data Byte

   DATA_OUT_PTR = &SMB_DATA_OUT[3];    // Initialize Data Pointer to the 
                                       // address of the first byte of data
   
   SEND_LEN = CP240x_CMDHDR_LEN + 1;   // Set total number of bytes to transmit
   RECV_LEN = 0;                       // Set number of bytes to receive
   
   STA = 1;                            // Start transfer

   while (SMB_BUSY);
   
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

   while (SMB_BUSY);                   // Wait for bus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)

   SMB_RW = 0;                         // Mark this transfer as a WRITE
   REPEAT_START = 1;                   // Mark this transfer to generate a 
                                       // repeated start + READ after the
                                       // write operation is complete.

   TARGET = CP240X_SLAVE_ADDR;                // Set the Slave Address 
   
   #if(AUTODECREMENT_BLOCK_RAM_OPERATIONS)
   if((CP240x_MSCN_Local & 0x20) == BACKWARD)
   {  
      // Reverse Direction
      addr = ~addr;

   }
   #endif 

   SMB_DATA_OUT[0] = RAMREAD;          // Set the CP240x Command Byte
   SMB_DATA_OUT[1] = (0x04);      // Set the CP240x Address High Byte
   SMB_DATA_OUT[2] = (addr);    // Set the CP240x Address Low Byte
   
   DATA_IN_PTR = &value;               // Initialize Data Pointer to the 
                                       // address of the receive buffer
   
   SEND_LEN = CP240x_CMDHDR_LEN + 0;   // Set total number of bytes to transmit
   RECV_LEN = 1;                       // Set number of bytes to receive

   STA = 1;                            // Start transfer

   while (SMB_BUSY);                   // Wait for transfer to complete

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
   U8 operation;
   U8 dat;

   while (SMB_BUSY);                   // Wait for SMBus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)

   SMB_RW = 0;                         // Mark this transfer as a WRITE
                                       // Block transfers always start out as 
                                       // SMBus writes

   // Determine the operation: READ or WRITE
   if( cmd == REGREAD || cmd == RAMREAD || cmd == REGPOLL)
   {  
      operation = READ;

      REPEAT_START = 1;                // Mark this transfer to generate a 
                                       // repeated start + READ after the
                                       // write operation is complete.

      SEND_LEN = CP240x_CMDHDR_LEN + 0;// Set total number of bytes to transmit
      RECV_LEN = len;                  // Set number of bytes to receive
      
      if(len == 0)
      {
         RECV_LEN = 256;
      }      

   } else
   {
      operation  = WRITE;

      REPEAT_START = 0;                // Mark this transfer to not generate a 
                                       // repeated start + READ after the
                                       // write operation is complete.
      SEND_LEN = CP240x_CMDHDR_LEN + len;// Set total number of bytes to transmit
      RECV_LEN = 0;                    // Set number of bytes to receive

      if(len == 0)
      {
         SEND_LEN = CP240x_CMDHDR_LEN + 256;
      }      


   }

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


   TARGET = CP240X_SLAVE_ADDR;                // Set the Slave Address 

   SMB_DATA_OUT[0] = cmd;              // Set the CP240x Command Byte
   SMB_DATA_OUT[1] = dat;              // Set the CP240x Address High Byte
   SMB_DATA_OUT[2] = addr;             // Set the CP240x Address Low Byte
   
   DATA_IN_PTR = ptr;                  // Initialize Data Pointer to the 
                                       // address of the receive buffer
   DATA_OUT_PTR = ptr;                 // Initialize Data Pointer to the 
                                       // address of transmit buffer
   
   STA = 1;                            // Start transfer

   while (SMB_BUSY);

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

   PWR = 0;
      
   // Enter the selected ULP Mode
   CP240x_RegWrite (ULPCN, mode);         // Enter ULP mode
   
   PWR = 1;

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
   PWR = 0;                               // Activate Power Signal

   // If the device is not already awake, force it to wake up
   if(forced_wake)
   {
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

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// SMBus ISR state machine
// - Master only implementation - no slave or arbitration states defined
// - All incoming data is written to global variable array <SMB_DATA_IN>
// - All outgoing data is read from global variable array <SMB_DATA_OUT>
//
//-----------------------------------------------------------------------------
INTERRUPT(SMBUS0_ISR, INTERRUPT_SMBUS0)
{
   bit FAIL = 0;                       // Used by the ISR to flag failed
                                       // transfers

   static U16 sent_byte_counter;        // Slave address does not count
   static U16 rec_byte_counter;         // as a sent byte

   if (ARBLOST == 0)                   // Check for errors
   {
      // Normal operation
      switch (SMB0CN & 0xF0)           // Status vector
      {
         // Master Transmitter/Receiver: START condition transmitted.
         case SMB_MTSTA:
            SMB0DAT = TARGET;          // Load address of the target slave
            SMB0DAT &= 0xFE;           // Clear the LSB of the address for the
                                       // R/W bit
            SMB0DAT |= SMB_RW;         // Load R/W bit
            STA = 0;                   // Manually clear START bit
            rec_byte_counter = 0;      // Reset the counter
            sent_byte_counter = 0;     // Reset the counter
            break;

         // Master Transmitter: Data byte transmitted
         case SMB_MTDB:
            if (ACK)                   // Slave ACK?
            {
               if (SMB_RW == SMB_WRITE)// If this transfer is a WRITE,
               {
                  // Send the CP240x Header (CMD, ADDRH, ADDRL)
                  if (sent_byte_counter < CP240x_CMDHDR_LEN)
                  {
                     // send data byte
                     SMB0DAT = SMB_DATA_OUT[sent_byte_counter];
                     sent_byte_counter++;
                  } else
                  
                  // Send the Outgoing Data
                  if (sent_byte_counter < SEND_LEN)
                  {
                     // send data byte
                     SMB0DAT = *DATA_OUT_PTR;
                     DATA_OUT_PTR++;
                     sent_byte_counter++;
                  }

                  // All transmit data has been sent
                  else
                  {
                     // Check if a repeated start has been requested
                     if(REPEAT_START)
                     {
                        SMB_RW = SMB_READ;  // Change Direction
                        STA = 1;            // Send Repeated Start

                     } else
                     {
                        STO = 1;          // Set STO to terminate transfer
                        SMB_BUSY = 0;     // And free SMBus interface
                     }
                  }
               }
               else {}                 // If this transfer is a READ,
                                       // proceed with transfer without
                                       // writing to SMB0DAT (switch
                                       // to receive mode)


            }
            else                       // If slave NACK,
            {
               STO = 1;                // Send STOP condition, followed
               STA = 1;                // By a START
            }
            break;

         // Master Receiver: byte received
         case SMB_MRDB:
            if (rec_byte_counter < (RECV_LEN - 1))
            {
               *DATA_IN_PTR++ = SMB0DAT;// Store received byte
               rec_byte_counter++;      // Increment the byte counter
               ACK = 1;                 // Send ACK to indicate byte received
            }
            else
            {
               *DATA_IN_PTR++ = SMB0DAT;// Store received byte
               rec_byte_counter++;      // Increment the byte counter
               ACK = 0;                 // Send NACK to indicate last byte
                                        // of this transfer
               STO = 1;                 // Send STOP to terminate transfer
               SMB_BUSY = 0;            // Free SMBus interface
            }
            break;

         default:
            FAIL = 1;                  // Indicate failed transfer
                                       // and handle at end of ISR
            break;

      } // end switch
   }
   else
   {
      // ARBLOST = 1, error occurred... abort transmission
      FAIL = 1;
   } // end ARBLOST if

   if (FAIL)                           // If the transfer failed,
   {
      SMB0CF &= ~0x80;                 // Reset communication
      SMB0CF |= 0x80;
      STA = 0;
      STO = 0;
      ACK = 0;

      SMB_BUSY = 0;                    // Free SMBus

      FAIL = 0;

   }

   SI = 0;                             // Clear interrupt flag
}

#if(ENABLE_SCL_LOW_TIMEOUT) 
//-----------------------------------------------------------------------------
// Timer3 Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// A Timer3 interrupt indicates an SMBus SCL low timeout.
// The SMBus is disabled and re-enabled here
//
//-----------------------------------------------------------------------------
INTERRUPT(TIMER3_ISR, INTERRUPT_TIMER3)
{
   SMB0CF &= ~0x80;                    // Disable SMBus
   SMB0CF |= 0x80;                     // Re-enable SMBus
   TMR3CN &= ~0x80;                    // Clear Timer3 interrupt-pending
                                       // flag
   STA = 0;
   SMB_BUSY = 0;                       // Free SMBus
}
#endif // (ENABLE_SCL_LOW_TIMEOUT) 

#endif
//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
