//-----------------------------------------------------------------------------
// Imeasure.h
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This header file sets the desired options to be used for measuring current.
//
//
// Target:         Si10xx
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (FB)
//    -27 APR 2010
//
//


//-----------------------------------------------------------------------------
// Location of Infinite Imeasure Loop 
//-----------------------------------------------------------------------------

#define BASE_ADDRESS  0x0100           // Base address in code space for 
                                       // placing the "while(1)" loop. 
                                       // Note: The minimum and typical current
                                       // are constant across various base 
                                       // addresses that are an integer 
                                       // multiple of 64 (0x40) (or 128 (0x80)
                                       // in devices that have 1024 byte Flash
                                       // pages). The maximum current increases 
                                       // with the base address up to 0x8000, 
                                       // then it starts to decrease again.

#define MIN_CURRENT_LOCATION           (BASE_ADDRESS + 4)
#define TYP_CURRENT_LOCATION           (BASE_ADDRESS)
#define MAX_CURRENT_LOCATION           (BASE_ADDRESS - 2)

#define IMEASURE_FUNCTION_LOCATION     MIN_CURRENT_LOCATION

