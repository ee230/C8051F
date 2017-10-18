//-----------------------------------------------------------------------------
//F41x_CRC.c
//-----------------------------------------------------------------------------
// Copyright (C) 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program shows how to use the CRC engine in the automatic mode and the
// manual mode. The automatic mode can be used to calculate the CRC of Flash
// pages. The manual mode can be used to calculate the CRC of any number of
// bytes manually written to the CRC engine.
//
// This program calculates the 32-bit CRC of the number of Flash pages found in
// the global constant section. The first section uses the manual method to
// calculate the CRC of all bytes except the last four. The last four bytes
// store the resulting CRC value. The second section uses the automatic CRC
// feature to calculate the entire CRC, including the result stored in the last
// four bytes of the last Flash pages. The final resulting CRC should be 0.
//
// This example uses the 32-bit mode CRC, but functions for the 16-bit CRC are
// included.
//
// How To Test:
//
// 1) Download code to the target board
// 2) Ensure that P1.5 and P1.6 LED jumpers are populated on the J8 header
// 3) Run the program.  If the Yellow LED is ON, the program is working.
//                      If the Red LED is ON, the program successfully finished
//                      If both LEDs are off, the program completed with errors
//
//
// Target:         F41x
// Tool chain:     Keil ,Raisonance, SDCC
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (ES)
//    -10 JAN 2011
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F410_defs.h>               // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (GREEN_LED, SFR_P2, 1);          // '1' means ON, '0' means OFF

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000 / 64     // SYSCLK frequency in Hz

#define LED_ON            1
#define LED_OFF           0

UU32 CRC_Result;                       // Will store the CRC results

// The following three pointers are used to pass a pointer into the Byte CRC
// function. This allows for an array of bytes from any of the three memory
// space below to be fed into the CRC engine.
VARIABLE_SEGMENT_POINTER (Code_PTR, U8, SEG_CODE);
VARIABLE_SEGMENT_POINTER (Xdata_PTR, U8, SEG_XDATA);
VARIABLE_SEGMENT_POINTER (Data_PTR, U8, SEG_DATA);

U16 Number_Of_Bytes;                   // Number of consecutive bytes to CRC.
                                       // This is used with the three segment
                                       // pointers above.

#define START_ADDRESS          0x1000
#define NUMBER_BYTES_TO_CRC    0x0400

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void PORT_Init (void);

void Byte_CRC16 (U8* Start_Byte, U16 Num_Bytes);
void Byte_CRC32 (U8* Start_Byte, U16 Num_Bytes);
void Read_CRC16 (UU16* Result);
void Read_CRC32 (UU32* Result);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{

   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)
   CRC_Result.U32 = 0x00000000;
   PORT_Init ();

   GREEN_LED = LED_OFF;

   // Calculate arguments passed into the Byte_CRC32 or Byte_CRC16 function
   Code_PTR =(START_ADDRESS);
   Number_Of_Bytes = (NUMBER_BYTES_TO_CRC);

   // Manually calculate CRC
   Byte_CRC32 (Code_PTR, Number_Of_Bytes);
   Read_CRC32 (&CRC_Result);

   GREEN_LED = LED_ON;

   while (1) {}                        // Spin forever
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
// P2.1   digital   push-pull     Green LED
//-----------------------------------------------------------------------------
void PORT_Init (void)
{

   P2MDIN |= 0x02;                     // P2.1 are digital

   P2MDOUT |= 0x02;                    // P2.1 are push-pull

   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}

//-----------------------------------------------------------------------------
// Byte_CRC16
//-----------------------------------------------------------------------------
//
// Return Value:  U8 - returns value indicating if the App FW is valid.
//   1) U8* Start_Byte - Pointer to first byte of data to CRC
//   2) U16 Num_Bytes - Number of consecutive bytes to include in the CRC
//
//   After this function completes, the result is kept in CRC0DAT
//-----------------------------------------------------------------------------
void Byte_CRC16 (U8* Start_Byte, U16 Num_Bytes)
{

   U16 i;

   // set CRC0SEL to 1 (16-bit calc),
   CRC0CN |= 0x10;
   // initialize CRC result to 0x00 and clear the current result.
   // If the result should be initialized to 0xFF, CRC0CN |= 0x0C;
   CRC0CN |= 0x08;

   for (i = 0; i < Num_Bytes; i++)
   {
      CRC0IN = Start_Byte[i];
   }

}

//-----------------------------------------------------------------------------
// Byte_CRC32
//-----------------------------------------------------------------------------
//
// Return Value:  U8 - returns value indicating if the App FW is valid.
//   1) U8* Start_Byte - Pointer to first byte of data to CRC
//   2) U16 Num_Bytes - Number of consecutive bytes to include in the CRC
//
//   After this function completes, the result is kept in CRC0DAT
//-----------------------------------------------------------------------------
void Byte_CRC32 (U8* Start_Byte, U16 Num_Bytes)
{

   U16 i;

   // clear CRC0SEL to 0 (32-bit calc),
   CRC0CN &= ~0x10;
   // initialize CRC result to 0x00 and clear the current result.
   // If the result should be initialized to 0xFF, CRC0CN |= 0x0C;
   CRC0CN |= 0x08;

   for (i = 0; i < Num_Bytes; i++)
   {
      CRC0IN = Start_Byte[i];
   }

   CRC0CN &= ~0x03;

}

//-----------------------------------------------------------------------------
// Read_CRC16
//-----------------------------------------------------------------------------
//
// Return Value : None.
// Parameters   :
//   1) U8* Result - Stores CRC into 2 bytes starting at this location
//
// Copies CRC result to two bytes starting at location passed into function
//-----------------------------------------------------------------------------
void Read_CRC16 (UU16* Result)
{

   // Initialize CRC0PNT to access bits 7-0 of the CRC result
   CRC0CN &= ~0x03;

   // With each read of CRC0DAT, the value in CRC0PNT will auto-increment
   (*Result).U8[LSB] = CRC0DAT;
   (*Result).U8[MSB] = CRC0DAT;

}

//-----------------------------------------------------------------------------
// Read_CRC32
//-----------------------------------------------------------------------------
//
// Return Value : None.
// Parameters   :
//   1) U8* Result - Stores CRC into 4 bytes starting at this location
//
// Copies CRC result to four bytes starting at location passed into function
//-----------------------------------------------------------------------------
void Read_CRC32 (UU32* Result)
{

   // Initialize CRC0PNT to access bits 7-0 of the CRC result
   CRC0CN &= ~0x03;

   // With each read of CRC0DAT, the value in CRC0PNT will auto-increment
   (*Result).U8[b0] = CRC0DAT;
   (*Result).U8[b1] = CRC0DAT;
   (*Result).U8[b2] = CRC0DAT;
   (*Result).U8[b3] = CRC0DAT;

}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------