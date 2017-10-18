#ifndef ENCODER_DECODER_H
#define ENCODER_DECODER_H
//=============================================================================
// EncoderDecoder.h
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This file is the API header file for the EncoderDecoder.c reusable C
// code module.
//
// Target:
//    Si102x/3x
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
//    Si102x/3x Code Examples
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
//=============================================================================
// Operations for use with the operation parameter
//=============================================================================
#define MANCHESTER_DECODE              0x10
#define THREEOUTOFSIX_DECODE           0x11
#define MANCHESTER_ENCODE              0x20
#define THREEOUTOFSIX_ENCODE           0x21
//-----------------------------------------------------------------------------
// Additional defines used with the ENC0CN sfr
//-----------------------------------------------------------------------------
#define MANCHESTER_ENCODING            0x00
#define THREEOUTOFSIX_ENCODING         0x01
#define DECODE_OPERATION               0x10
#define ENCODE_OPERATION               0x20
#define BIG_ENDIAN_DMA_MODE            0x06
#define DECODER_ERROR                  0x40
//-----------------------------------------------------------------------------
// Additional defines used to calculate output size
//-----------------------------------------------------------------------------
#define MANCHESTER_ENCODED_SIZE        2
#define MANCHESTER_UNENCODED_SIZE      1
#define THREEOUTOFSIX_ENCODED_SIZE     3
#define THREEOUTOFSIX_UNENCODED_SIZE   2
//-----------------------------------------------------------------------------
// Define SUCCESS if not already defined
//-----------------------------------------------------------------------------
#ifndef SUCCESS
#define SUCCESS 0
#elif(SUCCESS!=0)
#error  "SUCCESS definition conflict!"
#endif
//-----------------------------------------------------------------------------
// Define return error codes
//-----------------------------------------------------------------------------
enum ENC0_ERROR_CODES_Enum
{
   ENC0_ERROR_INVALID_OPERATION = 1,   // 0x01
   ENC0_ERROR_ZERO_LENGTH,             // 0x02
   ENC0_ERROR_INVALID_INSIZE,          // 0x03
   ENC0_ERROR_DECODER                  // 0x04
};
//=============================================================================
// Function Prototypes (API)
//=============================================================================
U8 EncodeDecode (U8 operation, U16 inSize,
   VARIABLE_SEGMENT_POINTER(unencodedData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(encodedData, U8, SEG_XDATA));
//=============================================================================
// End of file
//=============================================================================
#endif  // #ifdef ENCODER_DECODER_H
//=============================================================================
