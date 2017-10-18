//=============================================================================
// Validate_AES-Examples.c
//=============================================================================
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//    This file is the main module for the Encoder and provides validation of
// the encoder block. Please see the detailed comments in the main block.
//
// Target:
//    C8051F960
//
// IDE:
//    Silicon Laboratories IDE
//
// Tool Chains:
//    Keil
//    SDCC
//    Raisonance
//
// Project Name:
//    C8051F960 AES Code Examples
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// uncomment pragma SRC to generate assemble code
//-----------------------------------------------------------------------------
//#pragma SRC
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include "C8051F960_defs.h"
#include "EncoderDecoder.h"
#include "TestVectors.h"
//=============================================================================
// File global variables
//
// Reference Key and UnencodedData Copied to global variables located in XRAM.
//
//=============================================================================
SEGMENT_VARIABLE (UnencodedData[32], U8, SEG_XDATA);
SEGMENT_VARIABLE (EncodedData[16*3], U8, SEG_XDATA);
//=============================================================================
// Function Prototypes
//=============================================================================
//-----------------------------------------------------------------------------
// Interrupt proto (for SDCC compatibility)
//-----------------------------------------------------------------------------
INTERRUPT_PROTO(DMA_ISR, INTERRUPT_DMA0);
//-----------------------------------------------------------------------------
// Functions used to initialize and verify arrays.
//-----------------------------------------------------------------------------

void initUnencodedData (VARIABLE_SEGMENT_POINTER(unencodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(referenceUnencodedData, U8, SEG_CODE), U8 bytes);

U8 verifyUnencodedData (VARIABLE_SEGMENT_POINTER(unencodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(referenceUnencodedData, U8, SEG_CODE), U8 bytes);

void initEncodedData (VARIABLE_SEGMENT_POINTER(encodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(referenceEncodedData, U8, SEG_CODE), U8 bytes);

U8 verifyEncodedData (VARIABLE_SEGMENT_POINTER(encodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(referenceEncodedData, U8, SEG_CODE), U8 bytes);


//=============================================================================
// main
//=============================================================================
//-----------------------------------------------------------------------------
// main()
//
// Parameters: none
// Returns: none
//
// Description:
//
// This main module program serves as a code example for the ENCO reusable code
// modules and also validates all ENC0 operations.
//
// This reusable code modules for the ENC0 Encoder/Decoder is:
// EncoderDecoder.c
// AES_BlockCipher.c
// CBC_EncryptDecrypt.c
// CTR_EncryptDecrypt.c
//
// The test code in this module will test the four permutations of
// the two possible modes (encoding/decoding) and two possible
// encoding methods (Manchester/Three out of Six).
//
// The test vectors are located in the test Vectors.c module.
//
// Steps to use:
// Compile and download the code. Run code. If LED does not blink halt
// debugger and check code location.
//
// This simple test code will hang just after the respective test
// if any test fails. If all tests pass, it will run to the end of the
// program and blink an LED.
//-----------------------------------------------------------------------------
void main (void)
{
   U8 status;

   PCA0MD  &= ~0x40;                   // disable watchdog timer

   EA = 1;                             // enable global interrupts

   // Validate Manchester Encoding
   initUnencodedData(UnencodedData, ManchesterUnencodedData, 16);

   status = EncodeDecode(MANCHESTER_ENCODE, 16, UnencodedData, EncodedData);

   while(status);                      // code will hang here on error.

   status = verifyEncodedData(EncodedData, ManchesterEncodedData, 32);

   while(status);                      // code will hang here on error.

   // Validate Three Out of Six Encoding
   initUnencodedData(UnencodedData, ThreeOutOfSixUnencodedData, 32);

   status = EncodeDecode(THREEOUTOFSIX_ENCODE, 32, UnencodedData, EncodedData);

   while(status);                      // code will hang here on error.

   status = verifyEncodedData(EncodedData, ThreeOutOfSixEncodedData, 48);

   while(status);                      // code will hang here on error.

   // Validate Manchester Decoding
   initEncodedData(EncodedData, ManchesterEncodedData, 32);

   status = EncodeDecode(MANCHESTER_DECODE, 32, UnencodedData, EncodedData);

   while(status);                      // code will hang here on error.

   status = verifyUnencodedData(UnencodedData, ManchesterUnencodedData, 16);

   while(status);                      // code will hang here on error.

    // Validate Three Out of Six Decoding
   initEncodedData(EncodedData, ThreeOutOfSixEncodedData, 48);

   status = EncodeDecode(THREEOUTOFSIX_DECODE, 48, UnencodedData, EncodedData);

   while(status);                      // code will hang here on error.

   status = verifyUnencodedData(UnencodedData, ThreeOutOfSixUnencodedData, 32);

   while(status);                      // code will hang here on error.

   while(1);
}
//-----------------------------------------------------------------------------
// DMA_ISR
// description:
//
// This ISR is needed to support the DMA Idle mode wake up, which is used
// in the AES functions. Bit 5 of EIE2 should be enabled before going into
// idle mode. This ISR will disable further interrupts. EA must also be
// enabled.
//
//-----------------------------------------------------------------------------
INTERRUPT(DMA_ISR, INTERRUPT_DMA0)
{
  EIE2 &= ~0x20;                       // disable further interrupts
}
//-----------------------------------------------------------------------------
// initUnencodedData()
//
// parameters:
//   unencodedData          - xdata pointer to key
//   referenceUnencodedData - code pointer to reference key
//   blocks             - the number of 16-byte blocks to copy
//
// returns:
//
// description:
//
// Copies reference UnencodedData in Flash to unencodedtData in xdata RAM.
// The unencodedData must be copied to xdata for use by the DMA.
//
//-----------------------------------------------------------------------------
void initUnencodedData (VARIABLE_SEGMENT_POINTER(unencodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(referenceUnencodedData, U8, SEG_CODE),
   U8 bytes)
{
   while(bytes--)
   {
      *unencodedData++ = *referenceUnencodedData++;
   }
}
//-----------------------------------------------------------------------------
// verifyUnencodedData()
//
// parameters:
//   unencodedData          - xdata pointer to unencodedData
//   referenceUnencodedData - code pointer to reference UnencodedData
//   blocks             - the number of 16-byte blocks to copy
//
// returns:
//    badUnencodedDataBytes - the number of incorrect UnencodedData bytes
//
// description:
//
// Compares unencodedData in RAM to reference UnencodedData stored in Flash.
// The return value should be zero if the UnencodedDatas are identical.
//
//
//-----------------------------------------------------------------------------
U8 verifyUnencodedData (VARIABLE_SEGMENT_POINTER(unencodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(referenceUnencodedData, U8, SEG_CODE),
   U8 bytes)
{
   U8 badUnencodedDataBytes;

   badUnencodedDataBytes=0;

   while(bytes--)
   {
      if(*unencodedData++ != *referenceUnencodedData++)
         badUnencodedDataBytes++;
   }

   return badUnencodedDataBytes;
}
//-----------------------------------------------------------------------------
// initEncodedData()
//
// parameters:
//   cipherData          - xdata pointer to cipherData
//   referenceCipherData - code pointer to CipherData
//   blocks              - the number of 16-byte blocks to copy
//
// returns:
//
// description:
//
// Copies reference CipherData in Flash to cipherData in xdata RAM.
// The cipherData must be copied to xdata for use by the DMA.
//
//-----------------------------------------------------------------------------
void initEncodedData (VARIABLE_SEGMENT_POINTER(encodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(referenceEncodedData, U8, SEG_CODE),
   U8 bytes)
{
   while(bytes--)
   {
      *encodedData++ = *referenceEncodedData++;
   }
}
//-----------------------------------------------------------------------------
// verifyEncodedData()
//
// parameters:
//   cipherData          - xdata pointer to cipherData
//   referenceCipherData - code pointer to reference CipherData
//   blocks             - the number of 16-byte blocks to copy
//
// returns:
//    badCipherDataBytes - the number of incorrect CipherData bytes
//
// description:
//
// Compares cipherData in RAM to reference CipherData stored in Flash.
// The return value should be zero if the CipherDatas are identical.
//
//
//-----------------------------------------------------------------------------
U8 verifyEncodedData (VARIABLE_SEGMENT_POINTER(encodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(referenceEncodedData, U8, SEG_CODE),
   U8 bytes)
{
   U8 badEncodedDataBytes;

   badEncodedDataBytes=0;

   while(bytes--)
   {
      if(*encodedData++ != *referenceEncodedData++)
         badEncodedDataBytes++;
   }

   return badEncodedDataBytes;
}

//=============================================================================
// End of file
//=============================================================================

