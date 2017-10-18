//-----------------------------------------------------------------------------
// F560_FlashPrimitives.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// Target:         C8051F56x
// Tool chain:     Raisonance / Keil / SDCC
// Command Line:   None
//
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0 / 15 JAN 2009 (GP)
//    -Initial Revision
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <c8051F560_defs.h>
#include "F560_FlashPrimitives.h"

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

U8  FLASH_ByteWrite (FLADDR addr, U8 byte);
U8  FLASH_ByteRead  (FLADDR addr);
U8  FLASH_PageErase (FLADDR addr);

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of the byte to write to
//                    valid range is 0x0000 to 0x7BFF for 32K Flash devices
//                    valid range is 0x0000 to 0x3FFF for 16K Flash devices
//   2) U8 byte - byte to write to Flash.
//
// This routine writes <byte> to the linear FLASH address <addr>.
//
// This routine conforms to the recommendations in the C8051F56x data sheet
// 
// If the MCU is operating from the internal voltage regulator, the VDD
// monitor should be set threshold and enabled as a reset source only when
// writing or erasing Flash. Otherwise, it should be set to the low threshold.
//
// If the MCU is operating from an external voltage regulator powering VDD
// directly, the VDD monitor can be set to the high threshold permanently.
//-----------------------------------------------------------------------------

U8 FLASH_ByteWrite (FLADDR addr, U8 byte)
{
   bit EA_SAVE = EA;                   // Preserve EA
   U8 xdata * data pwrite;             // FLASH write pointer
   U8 i;

   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = ACTIVE_PAGE;

   EA = 0;                             // Disable interrupts

   RSTSRC = 0x00;                      // 1. Disable VDD monitor as a reset source

   VDM0CN = 0xA0;                      // 2. Enable VDD monitor and high threshold

   for (i = 0; i < 255; i++) {}        // 3. Wait for VDD monitor to stabilize

   if (!(VDM0CN & 0x40))               // 4. If the VDD voltage is not high
      return 0;                        //    enough don't write to Flash

   RSTSRC = 0x02;                      // 5. Safe to enable VDD Monitor as a 
                                       //    reset source

   pwrite = (U8 xdata *) addr;

                                       // 6. Enable Flash Writes

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x01;                      // PSWE = 1 which enables writes

   VDM0CN = 0xA0;                      // 7. Enable VDD monitor and high threshold

   RSTSRC = 0x02;                      // 8. Enable VDD monitor as a reset source

   *pwrite = byte;                     // 9. Write the byte

   RSTSRC = 0x00;                      // 10. Disable the VDD monitor as reset 
                                       //     source
   VDM0CN = 0x80;                      // 11. Change VDD Monitor to low threshold
   RSTSRC = 0x02;                      // 12. Re-enable the VDD monitor as a 
                                       //     reset source

   PSCTL &= ~0x01;                     // PSWE = 0 which disable writes

   EA = EA_SAVE;                       // Restore interrupts

   SFRPAGE = SFRPAGE_save;

   return 1;                           // Write completed successfully
}

//-----------------------------------------------------------------------------
// FLASH_ByteRead
//-----------------------------------------------------------------------------
//
// Return Value :
//      U8 - byte read from Flash
// Parameters   :
//   1) FLADDR addr - address of the byte to read to
//                    valid range is 0x0000 to 0x7BFF for 32K Flash devices
//                    valid range is 0x0000 to 0x3FFF for 16K Flash devices
//
// This routine reads a <byte> from the linear FLASH address <addr>.
//-----------------------------------------------------------------------------

U8 FLASH_ByteRead (FLADDR addr)
{
   bit EA_SAVE = EA;                   // Preserve EA
   U8 code * data pread;               // FLASH read pointer
   U8 byte;

   EA = 0;                             // Disable interrupts

   pread = (U8 code *) addr;

   byte = *pread;                      // Read the byte

   EA = EA_SAVE;                       // Restore interrupts

   return byte;
}

//-----------------------------------------------------------------------------
// FLASH_PageErase
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of any byte in the page to erase
//                    valid range is 0x0000 to 0x79FF for 32K Flash devices
//                    valid range is 0x0000 to 0x3DFF for 16K Flash devices
//
// This routine erases the FLASH page containing the linear FLASH address
// <addr>.  Note that the page of Flash containing the Lock Byte cannot be
// erased from application code.
//
//// This routine conforms to the recommendations in the C8051F56x data sheet
//
// If the MCU is operating from the internal voltage regulator, the VDD
// monitor should be set threshold and enabled as a reset source only when
// writing or erasing Flash. Otherwise, it should be set to the low threshold.
//
// If the MCU is operating from an external voltage regulator powering VDD
// directly, the VDD monitor can be set to the high threshold permanently.
//-----------------------------------------------------------------------------

U8 FLASH_PageErase (FLADDR addr)
{
   bit EA_SAVE = EA;                   // Preserve EA
   U8 xdata * data pwrite;             // FLASH write pointer
   U8 i;

   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = ACTIVE_PAGE;

   EA = 0;                             // Disable interrupts

   RSTSRC = 0x00;                      // 1. Disable VDD monitor as a reset source

   VDM0CN = 0xA0;                      // 2. Enable VDD monitor and high threshold

   for (i = 0; i < 255; i++) {}        // 3. Wait for VDD monitor to stabilize

   if (!(VDM0CN & 0x40))               // 4. If the VDD voltage is not high enough
      return 0;                        //    don't attempt to write to Flash

   RSTSRC = 0x02;                      // 5. Safe to enable VDD Monitor as a reset 
                                       //    source

   pwrite = (U8 xdata *) addr;

                                       // 6. Enable Flash Writes

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1

   VDM0CN = 0xA0;                      // 7. Enable VDD monitor and high threshold

   RSTSRC = 0x02;                      // 8. Enable VDD monitor as a reset source

   *pwrite = 0;                        // 9. Initiate page erase

   RSTSRC = 0x00;                      // 10. Disable the VDD monitor as a reset
                                       //     source
   VDM0CN = 0x80;                      // 11. Change VDD Monitor to low threshold
   RSTSRC = 0x02;                      // 12. Re-enable the VDD monitor as a reset 
                                       //     source

   PSCTL &= ~0x03;                     // PSWE = 0; PSEE = 0

   EA = EA_SAVE;                       // Restore interrupts

   SFRPAGE = SFRPAGE_save;

   return 1;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------