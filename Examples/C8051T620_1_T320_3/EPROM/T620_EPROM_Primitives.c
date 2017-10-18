//-----------------------------------------------------------------------------
// T620_EPROM_Primitives.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// EPROM memory on the 'T620/1.
//
// Target:         C8051T620/1 or C8051T320/1/2/3
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (GP / TP)
//    -01 JUL 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"
#include "T620_EPROM_Primitives.h"

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void          EPROM_ByteWrite (EPROMADDR addr, char byte);
unsigned char EPROM_ByteRead  (EPROMADDR addr);

//-----------------------------------------------------------------------------
// EPROM_ByteWrite
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) EPROMADDR addr - address of the byte to write to
//                    valid range is from 0x0000 to 0x3DFF for 16K devices
//   2) char byte - byte to write to EPROM memory.
//
// This routine writes <byte> to the linear EPROM address <addr>.
//
// - NOTE: this routine requires that the capacitor on VPP be populated for
// the internal EPROM charge pump.
//-----------------------------------------------------------------------------
void EPROM_ByteWrite (EPROMADDR addr, char byte)
{
   bit EA_SAVE = EA;                   // Preserve EA
   char xdata * data pwrite;           // EPROM write pointer

   EA = 0;                             // Disable interrupts

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   pwrite = (char xdata *) addr;

   PFE0CN = 0x00;                      // Disable the Prefetch engine

   MEMKEY = 0xA5;                      // Key Sequence 1
   MEMKEY = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x01;                      // PSWE = 1 which enables writes

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
   IAPCN = 0x80;                       // Enable the internal EPROM charge pump

   *pwrite = byte;                     // Write the byte

   IAPCN = 0x00;                       // Disable the internal EPROM charge
                                       // pump
   PSCTL &= ~0x01;                     // PSWE = 0 which disable writes

   PFE0CN = 0x20;                      // Re-enable the Prefetch engine

   EA = EA_SAVE;                       // Restore interrupts
}

//-----------------------------------------------------------------------------
// EPROM_ByteRead
//-----------------------------------------------------------------------------
//
// Return Value :
//      unsigned char - byte read from EPROM memory
// Parameters   :
//   1) EPROMADDR addr - address of the byte to read to
//                    valid range is from 0x0000 to 0x3DFF for 16K devices
//
// This routine reads a <byte> from the linear EPROM address <addr>.
//-----------------------------------------------------------------------------
unsigned char EPROM_ByteRead (EPROMADDR addr)
{
   bit EA_SAVE = EA;                   // Preserve EA
   char code * data pread;             // EPROM read pointer
   unsigned char byte;

   EA = 0;                             // Disable interrupts

   pread = (char code *) addr;

   byte = *pread;                      // Read the byte

   EA = EA_SAVE;                       // Restore interrupts

   return byte;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------