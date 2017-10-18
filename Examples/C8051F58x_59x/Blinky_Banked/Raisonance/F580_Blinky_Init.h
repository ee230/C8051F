//-----------------------------------------------------------------------------
// F580_Blinky_Init.h
//-----------------------------------------------------------------------------
// Copyright (C) 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// How To Test:
//
// See F580_Blinky_Main.c
//
// Target:         C8051F500 (Side A of a C8051F580-TB)
// Tool chain:     Raisonance
// Command Line:   None
//
// Release 1.0 / 11 MAR 2010 (GP)
//    -Initial Revision

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>             // Compiler declarations
#include <C8051F580_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void TIMER2_Init (U16 counts);

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
