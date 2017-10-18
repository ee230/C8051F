#ifndef TEST_VECTORS_H
#define TEST_VECTORS_H
//=============================================================================
// TestVectors.h
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
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
//=============================================================================
// Defines used with Table
//=============================================================================
#define NUM_MANCHESTER_VECTORS               16
#define NUM_MANCHESTER_UNENCODED_BYTES       (NUM_MANCHESTER_VECTORS)
#define NUM_MANCHESTER_ENCODED_BYTES         (NUM_MANCHESTER_VECTORS*2)
#define NUM_THREEOUTOFSIX_VECTORS            16
#define NUM_THREEOUTOFSIX_UNENCODED_BYTES    (NUM_THREEOUTOFSIX_VECTORS*2)
#define NUM_THREEOUTOFSIX_ENCODED_BYTES      (NUM_THREEOUTOFSIX_VECTORS*3)
//=============================================================================
// extern'ed Public variables
//=============================================================================
extern SEGMENT_VARIABLE (ManchesterUnencodedData[NUM_MANCHESTER_UNENCODED_BYTES], U8, SEG_CODE);
extern SEGMENT_VARIABLE (ManchesterEncodedData[NUM_MANCHESTER_ENCODED_BYTES], U8, SEG_CODE);
extern SEGMENT_VARIABLE (ThreeOutOfSixUnencodedData[NUM_THREEOUTOFSIX_UNENCODED_BYTES], U8, SEG_CODE);
extern SEGMENT_VARIABLE (ThreeOutOfSixEncodedData[NUM_THREEOUTOFSIX_ENCODED_BYTES], U8, SEG_CODE);
//=============================================================================
// End of file
//=============================================================================
#endif  // #ifdef TEST_VECTORS_H
//=============================================================================
