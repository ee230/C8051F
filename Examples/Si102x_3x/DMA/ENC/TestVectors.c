//=============================================================================
// TestVectors.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
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
//-----------------------------------------------------------------------------
// uncomment pragma SRC to generate assembly code
//-----------------------------------------------------------------------------
//#pragma SRC
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1020_defs.h>
#include "TestVectors.h"
//-----------------------------------------------------------------------------
// Reference Manchester Unencoded Data
//    Input Vectors for Validating Manchester Encoding
//
// One byte in give two bytes out.
//
// Sequential rolling codes. Tests all symbols at least once in each position.
//
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (ManchesterUnencodedData[NUM_MANCHESTER_UNENCODED_BYTES], U8, SEG_CODE) =
{
   0x01,
   0x12,
   0x23,
   0x34,
   0x45,
   0x56,
   0x67,
   0x78,
   0x89,
   0x9A,
   0xAB,
   0xBC,
   0xCD,
   0xDE,
   0xEF,
   0xF0
};
//-----------------------------------------------------------------------------
// Reference Manchester Encoded Data
//    Output Vectors for Validating three Out Of Six Encoder
//
// One byte in give two bytes out.
//
// Sequential rolling codes. Tests all symbols at least once in each position.
//
// Each encoded byte represents one nibble of data.
//
// 0 is encoded as 10
// 1 is encoded as 01
//
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (ManchesterEncodedData[NUM_MANCHESTER_ENCODED_BYTES], U8, SEG_CODE) =
{

   0xAA, 0xA9, // 0x01 = 10 10 10 10 10 10 10 01
   0xA9, 0xA6, // 0x12 = 10 10 10 01 10 10 01 10
   0xA6, 0xA5, // 0x23 = 10 10 01 10 10 10 01 01
   0xA5, 0x9A, // 0x34 = 10 10 01 01 10 01 10 10
   0x9A, 0x99, // 0x45 = 10 01 10 10 10 01 10 01
   0x99, 0x96, // 0x56 = 10 01 10 01 10 01 01 10
   0x96, 0x95, // 0x67 = 10 01 01 10 10 01 01 01
   0x95, 0x6A, // 0x78 = 10 01 01 01 01 10 10 10
   0x6A, 0x69, // 0x89 = 01 10 10 10 01 10 10 01
   0x69, 0x66, // 0x9A = 01 10 10 01 01 10 01 10
   0x66, 0x65, // 0xAB = 01 10 01 10 01 10 01 01
   0x65, 0x5A, // 0xBC = 01 10 01 01 01 01 10 10
   0x5A, 0x59, // 0xCD = 01 01 10 10 01 01 10 01
   0x59, 0x56, // 0xDE = 01 01 10 01 01 01 01 10
   0x56, 0x55, // 0xEF = 01 01 01 10 01 01 01 01
   0x55, 0xAA, // 0xF0 = 01 01 01 01 10 10 10 10
};
//-----------------------------------------------------------------------------
// Reference Three Out Of Six Unencoded Data
//    Input Vectors for Validating Three Out Of Six Encoder
//
// Two bytes in give three bytes out.
//
// Sequential rolling codes. Tests all symbols at least once in each position.
//
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (ThreeOutOfSixUnencodedData[NUM_THREEOUTOFSIX_UNENCODED_BYTES], U8, SEG_CODE) =
{
   0x01, 0x23,
   0x12, 0x34,
   0x23, 0x45,
   0x34, 0x56,
   0x45, 0x67,
   0x56, 0x78,
   0x67, 0x89,
   0x78, 0x9A,
   0x89, 0xAB,
   0x9A, 0xBC,
   0xAB, 0xCD,
   0xBC, 0xDE,
   0xCD, 0xEF,
   0xDE, 0xF0,
   0xEF, 0x01,
   0xF0, 0x12
};
//-----------------------------------------------------------------------------
// ThreeOutOfSixEncodedData
//    Output Vectors for Validating three Out Of Six Encoder
//
// Two bytes in give three bytes out.
//
// Sequential rolling codes. Tests all symbols at least once in each position.
//
// Each hex byte includes more than one six-bit symbol.
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (ThreeOutOfSixEncodedData[NUM_THREEOUTOFSIX_ENCODED_BYTES], U8, SEG_CODE) =
{
   0x58, 0xD3, 0x8B, // 0x0123 = 010110 001101 001110 001011
   0x34, 0xE2, 0xDC, // 0x1234 = 001101 001110 001011 011100
   0x38, 0xB7, 0x19, // 0x2345 = 001110 001011 011100 011001
   0x2D, 0xC6, 0x5A, // 0x3456 = 001011 011100 011001 011010
   0x71, 0x96, 0x93, // 0x4567 = 011100 011001 011010 010011
   0x65, 0xA4, 0xEC, // 0x5678 = 011001 011010 010011 101100
   0x69, 0x3B, 0x25, // 0x6789 = 011010 010011 101100 100101
   0x4E, 0xC9, 0x66, // 0x789A = 010011 101100 100101 100110
   0xB2, 0x59, 0xA3, // 0x89AB = 101100 100101 100110 100011
   0x96, 0x68, 0xF4, // 0x9ABC = 100101 100110 100011 110100
   0x9A, 0x3D, 0x31, // 0xABCD = 100110 100011 110100 110001
   0x8F, 0x4C, 0x72, // 0xBCDE = 100011 110100 110001 110010
   0xD3, 0x1C, 0xA9, // 0xCDEF = 110100 110001 110010 101001
   0xC7, 0x2A, 0x56, // 0xDEF0 = 110001 110010 101001 010110
   0xCA, 0x95, 0x8D, // 0xEF01 = 110010 101001 010110 001101
   0xA5, 0x63, 0x4E, // 0xF012 = 101001 010110 001101 001110
};
//=============================================================================
// End of File
//=============================================================================
