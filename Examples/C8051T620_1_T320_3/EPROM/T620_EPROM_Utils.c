//-----------------------------------------------------------------------------
// T620_EPROM_Utils.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// EPROM memory.
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
#include "T620_EPROM_Primitives.h"
#include "T620_EPROM_Utils.h"

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

// EPROM read/write routines
void EPROM_Write (EPROMADDR dest, char *src, unsigned int numbytes);
char * EPROM_Read (char *dest, EPROMADDR src, unsigned int numbytes);

// EPROM copy routine
void EPROM_Copy (EPROMADDR dest, EPROMADDR src, unsigned int numbytes);

// EPROM test routines
void EPROM_Fill (EPROMADDR addr, U32 length, U8 fill);



//-----------------------------------------------------------------------------
// EPROM_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) EPROMADDR dest - starting address of the byte(s) to write to
//                    valid range is 0x0000 to 0x3DFF for 16K EPROM devices
//   2) char *src - pointer to source bytes
//   3) unsigned int numbytes - the number of bytes to write
//                              valid range is is range of integer
//
// This routine copies <numbytes> from <src> to the linear EPROM address
// <dest>.  The bytes must be unwritten (0xFF) before writing.
// <dest> + <numbytes> must be less than 0x3DFF.
//
//-----------------------------------------------------------------------------
void EPROM_Write (EPROMADDR dest, char *src, unsigned int numbytes)
{
   EPROMADDR i;

   for (i = dest; i < dest+numbytes; i++)
   {
      EPROM_ByteWrite (i, *src++);
   }
}

//-----------------------------------------------------------------------------
// EPROM_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
//   1) char *dest - pointer to destination bytes
// Parameters   :
//   1) char *dest - pointer to destination bytes
//   2) EPROMADDR src - address of source bytes in EPROM
//                    valid range is 0x0000 to 0x3DFF for 16K EPROM devices
//   3) unsigned int numbytes - the number of bytes to read
//                    valid range is range of integer
//
// This routine copies <numbytes> from the linear EPROM address <src> to
// <dest>.
// <src> + <numbytes> must be less than 0x3DFF.
//
//-----------------------------------------------------------------------------
char * EPROM_Read (char *dest, EPROMADDR src, unsigned int numbytes)
{
   EPROMADDR i;

   for (i = 0; i < numbytes; i++)
   {
      *dest++ = EPROM_ByteRead (src+i);
   }
   return dest;
}

//-----------------------------------------------------------------------------
// EPROM_Copy
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) EPROMADDR dest - pointer to destination bytes in EPROM
//                    valid range is 0x0000 to 0x3DFF for 16K EPROM devices
//   2) EPROMADDR src - address of source bytes in EPROM
//                    valid range is 0x0000 to 0x3DFF for 16K EPROM devices
//   3) unsigned int numbytes - the number of bytes to copy
//                    valid range is range of integer
//
// This routine copies <numbytes> from <src> to the linear EPROM address
// <dest>.  The destination bytes must be unwritten (0xFF) before writing.
// <src>/<dest> + <numbytes> must be less than 0x3DFF.
//
//-----------------------------------------------------------------------------
void EPROM_Copy (EPROMADDR dest, EPROMADDR src, unsigned int numbytes)
{
   EPROMADDR i;

   for (i = 0; i < numbytes; i++)
   {
      EPROM_ByteWrite ((EPROMADDR) dest+i, EPROM_ByteRead((EPROMADDR) src+i));
   }
}

//-----------------------------------------------------------------------------
// EPROM_Fill
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) EPROMADDR addr - starting address of bytes to fill in EPROM
//                    valid range is 0x0000 to 0x3DFF for 16K EPROM devices
//   2) U32 length - number of bytes to fill
//                    range is total EPROM size
//   3) U8 fill - the character used the EPROM should be filled with
//
// This routine fills the EPROM beginning at <addr> with <length> bytes.
// The target bytes must be unwritten (0xFF) before writing to them.
// <addr> + <length> must be less than 0x3DFF.
//
//-----------------------------------------------------------------------------
void EPROM_Fill (EPROMADDR addr, U32 length, U8 fill)
{
   EPROMADDR i;

   for (i = 0; i < length; i++)
   {
      EPROM_ByteWrite (addr+i, fill);
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------