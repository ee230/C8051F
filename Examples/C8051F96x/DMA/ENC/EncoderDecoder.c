//=============================================================================
// EncoderDecoder.c
//=============================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This C file is a reusabe code module for the encoder/decoder module.
//
// Please see the API function description below for a detailed explaination
// of the reusable functions.
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
//    C8051F960 Code Examples
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// uncomment pragma SRC to generate assembly code
//-----------------------------------------------------------------------------
//#pragma SRC
//=============================================================================
// Includes
//=============================================================================
#include <compiler_defs.h>
#include <C8051F960_defs.h>
#include "DMA_defs.h"
#include "EncoderDecoder.h"
//=============================================================================
// API Functions
//=============================================================================
//-----------------------------------------------------------------------------
// EncodeDecode()
//
// parameters:
//    operation       - MANCHESTER_DECODE, THREEOUTOFSIX_DECODE,
//                      MANCHESTER_ENCODE, or THREEOUTOFSIX_ENCODE
//    inSize          - size for input data
//    unencodedData   - xdata pointer to unencoded data
//    encodedData     - xdata pointer to encoded data
//
// returns:
//    status         - 0 for SUCCESS
//                   - 1 for ENC0_ERROR_INVALID_OPERATION
//                   - 2 for ENC0_ERROR_ZERO_LENGTH
//                   - 3 for ENC0_ERROR_INVALID_INSIZE
//
// Description:
//
//
// A single function is provided for both encode and decode operations.
// The operation parameter specifies the type or operation and the
// type of encoded. The operation defines are in the header file.
//
// This function uses ENC0 encoder/decoder with the DMA. The DMA
// channel allocation is defined in DMA_defs.h.
//
// The encoded data pointer is the output for an encode operation and
// an input for a decode operation.
//
// This function puts the MCU code into idle mode while encode/decode
// is in process.
//
// A interrupt is needed to wake up from Idle mode. This requires that the
// global interrupts are enabled. Also, a small DMA ISR that disables EIE2
// bit 5 is required in the main module.
//
// This function will return an error for an invalid operation or and
// invalid size.
//
// Manchester encoding can use any size date input. The output will be
// twice as many bytes. For Manchester decoding the size must be an
// even multiple of 2 bytes. For Three out of Six Encoding the size
// must be an even multiple of two bytes. For Three out of Six Decoding,
// the size must be an even multiple of 3 bytes.
//
// Note that Big Endian mode should be used since the data is
// normally stored in XRAM with first byte to be transmitted
// at the lowest address. When encoding, the first byte at
// the lowest address is placed in the most significant
// byte of the ENCO sfrs.  When decoding, the most significant
// byte of the ENCO sfrs is placed at he lowest address
// in XRAM. It is important that both the DMA and the ENC0 module
// must be configured for the same mode.
//
// The Encoder module also supports little endian mode. But to use little
// endian mode, the data would have to be stored with the first byte to be
// transmitted in the highest address. This is not reccomended.
//
//-----------------------------------------------------------------------------
U8 EncodeDecode (U8 operation, U16 inSize,
   VARIABLE_SEGMENT_POINTER(unencodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(encodedData, U8, SEG_XDATA))
{
   // unions used for compiler independent endianness.
   UU16 size;
   UU16 addr;

   // first check for zero size
   if (inSize == 0)
   {
      return ENC0_ERROR_ZERO_LENGTH;
   }

   // Input size must be multiple of ENCODE or UNENCODED size.
   // The modulus, or remainder after division, must be zero.
   if(operation == MANCHESTER_DECODE)
   {
      // Check inSize modulus for Manchester Decoding
      if (inSize % MANCHESTER_ENCODED_SIZE != 0)
         return ENC0_ERROR_INVALID_INSIZE;
   }
   else if (operation == THREEOUTOFSIX_DECODE)
   {
      // Check inSize modulus for Three out of Six Decoding
      if(inSize % THREEOUTOFSIX_ENCODED_SIZE != 0)
         return ENC0_ERROR_INVALID_INSIZE;
   }
   else if (operation == MANCHESTER_ENCODE)
   {
      // any size OK for Manchester Encoding
   }
   else if (operation == THREEOUTOFSIX_ENCODE)
   {
      // Check inSize modulus for Three out of Six Encoding
      if(inSize % THREEOUTOFSIX_UNENCODED_SIZE != 0)
         return ENC0_ERROR_INVALID_INSIZE;
   }
   else
   {
      // if not one of the four cases above,
      // then the operation parameter is invalid.
      return ENC0_ERROR_INVALID_OPERATION;
   }

   SFRPAGE = DPPE_PAGE;

   ENC0CN   = 0;                        // disable ENC for now

   // Clear ENC0 bits (ENC0_IN & ENC0_OUT) bits in DMA0EN sfr using mask.
   DMA0EN &= ~ENC0_MASK;

   // Configure DMA ENC0 IN channel.
   // Select channel and peripheral request.
   // Wrapping not used for any ENC operations.
   DMA0SEL = ENC0_IN_CHANNEL;
   DMA0NCF = ENC0_IN_PERIPHERAL_REQUEST|DMA_BIG_ENDIAN;
   DMA0NMD = NO_WRAPPING;

   // The In channel address depends on if the operation is encode or decode.
   // For Encode operation unencodedData is input.
   // For Decode operation encodedData is input.
   if(operation & ENCODE_OPERATION)
      addr.U16 = (U16)unencodedData;
   else
      addr.U16 = (U16)encodedData;

   DMA0NBAL = addr.U8[LSB];
   DMA0NBAH = addr.U8[MSB];


   // Initialize DMA transfer size using input size parameter.
   size.U16 = inSize;
   DMA0NSZL = size.U8[LSB];
   DMA0NSZH = size.U8[MSB];

   // Clear Address Offset
   DMA0NAOL = 0;
   DMA0NAOH = 0;

   // Configure DMA ENC0 OUT channel
   // Select channel and peripheral request
   // Enable interrupt
   DMA0SEL = ENC0_OUT_CHANNEL;
   DMA0NCF = ENC0_OUT_PERIPHERAL_REQUEST|DMA_BIG_ENDIAN|DMA_INT_EN;
   DMA0NMD = NO_WRAPPING;

   // The OUT ch address depends on if the operation is encode or decode.
   // For Encode operation encodedData is output.
   // For Decode operation unencodedData is output.
   if(operation & ENCODE_OPERATION)
      addr.U16 = (U16)encodedData;
   else
      addr.U16 = (U16)unencodedData;

   DMA0NBAL = addr.U8[LSB];
   DMA0NBAH = addr.U8[MSB];

   // Calculate output size depending on mode.
   // Note that the modulus has already been checked.
   // So dividing inSize by the corresponding encoded or unencoded size
   // will give an integer.
   if(operation == MANCHESTER_DECODE)
   {
      size.U16 = (inSize/MANCHESTER_ENCODED_SIZE) * MANCHESTER_UNENCODED_SIZE;
   }
   else if (operation == THREEOUTOFSIX_DECODE)
   {
      size.U16 = (inSize/THREEOUTOFSIX_ENCODED_SIZE) * THREEOUTOFSIX_UNENCODED_SIZE;
   }
   else if(operation == MANCHESTER_ENCODE)
   {
      size.U16 = (inSize/MANCHESTER_UNENCODED_SIZE) * MANCHESTER_ENCODED_SIZE;
   }
   else if (operation == THREEOUTOFSIX_ENCODE)
   {
      size.U16 = (inSize/THREEOUTOFSIX_UNENCODED_SIZE) * THREEOUTOFSIX_ENCODED_SIZE;
   }

   DMA0NSZL = size.U8[LSB];
   DMA0NSZH = size.U8[MSB];

   // Clear Address Offset
   DMA0NAOL = 0;
   DMA0NAOH = 0;

   // Clear ENCO DMA channel bits in DMA0INT sfr using mask.
   DMA0INT &= ~ENC0_MASK;

   // Set ENCO DMA channel bits in DMA0EN sfr using mask.
   // This enables the DMA. But the encode/decode operation
   // won't start until the END0 block is enabled.
   DMA0EN  |=  ENC0_MASK;

   // Enable ENC0CN block using big endian mode.
   ENC0CN =  operation | BIG_ENDIAN_DMA_MODE;

   // enable DMA interrupt to terminate Idle mode
   EIE2 |= 0x20;

   // This do...while loop ensures that the CPU will remain in Idle mode
   // until ENC0_OUT DMA channel transfer is complete.
   do
   {
      #ifdef DMA_TRANSFERS_USE_IDLE
      PCON |= 0x01;
      #endif

   }  while((DMA0INT&ENC0_OUT_MASK)==0);


   // Clear ENC0 bits (ENC0_IN & ENC0_OUT) bits in DMA0EN sfr using mask.
   DMA0EN &= ~ENC0_MASK;
   // Clear ENC0 bits (ENC0_IN & ENC0_OUT) bits in DMA0INT sfr using mask.
   DMA0INT &= ~ENC0_MASK;

   if((operation & DECODE_OPERATION) == DECODE_OPERATION)
   {
      if((ENC0CN & DECODER_ERROR)==DECODER_ERROR)
      {
         ENC0CN = 0x00;                      // clear ENC0
         return ENC0_ERROR_DECODER;
      }
      else
      {
         ENC0CN = 0x00;                      // clear ENC0
         return SUCCESS;
      }
   }
   else
   {
      ENC0CN = 0x00;                      // clear ENC0
      return SUCCESS;
   }
}
//=============================================================================
// End of file
//=============================================================================

