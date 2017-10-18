//-----------------------------------------------------------------------------
// F580_FlashPrimitives.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// Target:         C8051F580
// Tool chain:     Raisonance / Keil / SDCC
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//    -Updated to meet new data sheet guidelines for writing/erasing
//
// Release 1.0 / 23 OCT 2008 (GP)
//    -Initial Revision

//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <c8051F580_defs.h>
#include "F580_FlashPrimitives.h"

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
U8   FLASH_ByteWrite (FLADDR addr, U8 byte, bit SFLE);
U8   FLASH_ByteRead  (FLADDR addr, bit SFLE);
U8   FLASH_PageErase (FLADDR addr, bit SFLE);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// This routine writes <byte> to the linear FLASH address <addr>.
// Linear map is decoded as follows:
// Linear Address       Bank     Bank Address
// ------------------------------------------------
// 0x00000 - 0x07FFF    0        0x0000 - 0x7FFF
// 0x08000 - 0x0FFFF    1        0x8000 - 0xFFFF
// 0x10000 - 0x17FFF    2        0x8000 - 0xFFFF
// 0x18000 - 0x1FFFF    3        0x8000 - 0xFFFF
//
// This routine conforms to the recommendations in the C8051F58x data sheet
// 
// If the MCU is operating from the internal voltage regulator, the VDD
// monitor should be set threshold and enabled as a reset source only when
// writing or erasing Flash. Otherwise, it should be set to the low threshold.
//
// If the MCU is operating from an external voltage regulator powering VDD
// directly, the VDD monitor can be set to the high threshold permanently.
//-----------------------------------------------------------------------------
U8 FLASH_ByteWrite (FLADDR addr, U8 byte, bit SFLE)
{
   S8 SFRPAGE_SAVE = SFRPAGE;          // Preserve SFRPAGE
   bit EA_SAVE = EA;                   // Preserve EA
   S8 PSBANK_SAVE = PSBANK;            // Preserve PSBANK
   S8 xdata * data pwrite;             // FLASH write pointer
   U8 i;

   EA = 0;                             // Disable interrupts

   SFRPAGE = ACTIVE_PAGE;

   if (addr < 0x10000) {               // 64K linear address
      pwrite = (S8 xdata *) addr;
   } else if (addr < 0x18000) {        // BANK 2
      addr |= 0x8000;
      pwrite = (S8 xdata *) addr;
      PSBANK &= ~0x30;                 // COBANK = 0x2
      PSBANK |=  0x20;
   } else {                            // BANK 3
      pwrite = (S8 xdata *) addr;
      PSBANK &= ~0x30;                 // COBANK = 0x3
      PSBANK |=  0x30;
   }

   RSTSRC = 0x00;                      // 1. Disable VDD monitor as a reset source

   VDM0CN = 0xA0;                      // 2. Enable VDD monitor and high threshold

   for (i = 0; i < 255; i++) {}        // 3. Wait for VDD monitor to stabilize

   if (!(VDM0CN & 0x40))               // 4. If the VDD voltage is not high
      return 0;                        //    enough don't write to Flash

   RSTSRC = 0x02;                      // 5. Safe to enable VDD Monitor as a 
                                       //    reset source

                                       // 6. Enable Flash Writes

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x01;                      // PSWE = 1

   if (SFLE) {
      PSCTL |= 0x04;                   // Set SFLE
   }

   VDM0CN = 0xA0;                      // 7. Enable VDD monitor and high threshold

   RSTSRC = 0x02;                      // 8. Enable VDD monitor as a reset source

   *pwrite = byte;                     // 9. Write the byte

   RSTSRC = 0x00;                      // 10. Disable the VDD monitor as reset 
                                       //     source
   VDM0CN = 0x80;                      // 11. Change VDD Monitor to low threshold
   RSTSRC = 0x02;                      // 12. Re-enable the VDD monitor as a 
                                       //     reset source
   if (SFLE) {
      PSCTL &= ~0x04;                  // Clear SFLE
   }
   PSCTL &= ~0x01;                     // PSWE = 0

   PSBANK  = PSBANK_SAVE;              // Restore PSBANK
   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
   EA      = EA_SAVE;                  // Restore interrupts

   return 1;
}

//-----------------------------------------------------------------------------
// FLASH_ByteRead
//-----------------------------------------------------------------------------
//
// This routine reads a <byte> from the linear FLASH address <addr>.
//
//-----------------------------------------------------------------------------
U8 FLASH_ByteRead (FLADDR addr, bit SFLE)
{
   S8 SFRPAGE_SAVE = SFRPAGE;          // Preserve SFRPAGE
   bit EA_SAVE = EA;                   // Preserve EA
   S8 PSBANK_SAVE = PSBANK;            // Preserve PSBANK
   S8 code * data pread;               // FLASH read pointer
   U8 byte;

   EA = 0;                             // Disable interrupts

   SFRPAGE = ACTIVE_PAGE;

   if (addr < 0x10000) {               // 64K linear address
      pread = (S8 code *) addr;
   } else if (addr < 0x18000) {        // BANK 2
      addr |= 0x8000;
      pread = (S8 code *) addr;
      PSBANK &= ~0x30;                 // COBANK = 0x2
      PSBANK |=  0x20;
   } else {                            // BANK 3
      pread = (S8 code *) addr;
      PSBANK &= ~0x30;                 // COBANK = 0x3
      PSBANK |=  0x30;
   }

   if (SFLE) {
      PSCTL |= 0x04;                   // Set SFLE
   }

   byte = *pread;                      // Read the byte

   if (SFLE) {
      PSCTL &= ~0x04;                  // Clear SFLE
   }

   PSBANK = PSBANK_SAVE;               // Restore PSBANK
   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
   EA = EA_SAVE;                       // Restore interrupts

   return byte;
}

//-----------------------------------------------------------------------------
// FLASH_PageErase
//-----------------------------------------------------------------------------
//
// This routine erases the FLASH page containing the linear FLASH address
// <addr>.
//
// This routine conforms to the recommendations in the C8051F50x data sheet
// 
// If the MCU is operating from the internal voltage regulator, the VDD
// monitor should be set threshold and enabled as a reset source only when
// writing or erasing Flash. Otherwise, it should be set to the low threshold.
//
// If the MCU is operating from an external voltage regulator powering VDD
// directly, the VDD monitor can be set to the high threshold permanently.
//-----------------------------------------------------------------------------
U8 FLASH_PageErase (FLADDR addr, bit SFLE)
{
   S8 SFRPAGE_SAVE = SFRPAGE;          // Preserve SFRPAGE
   bit EA_SAVE = EA;                   // Preserve EA
   S8 PSBANK_SAVE = PSBANK;            // Preserve PSBANK
   S8 xdata * data pwrite;             // FLASH write pointer
   U8 i;

   EA = 0;                             // Disable interrupts

   SFRPAGE = ACTIVE_PAGE;

   if (addr < 0x10000) {               // 64K linear address
      pwrite = (S8 xdata *) addr;
   } else if (addr < 0x18000) {        // BANK 2
      addr |= 0x8000;
      pwrite = (S8 xdata *) addr;
      PSBANK &= ~0x30;                 // COBANK = 0x2
      PSBANK |=  0x20;
   } else {                            // BANK 3
      pwrite = (S8 xdata *) addr;
      PSBANK &= ~0x30;                 // COBANK = 0x3
      PSBANK |=  0x30;
   }

   RSTSRC = 0x00;                      // 1. Disable VDD monitor as a reset source

   VDM0CN = 0xA0;                      // 2. Enable VDD monitor and high threshold

   for (i = 0; i < 255; i++) {}        // 3. Wait for VDD monitor to stabilize

   if (!(VDM0CN & 0x40))               // 4. If the VDD voltage is not high enough
      return 0;                        //    don't attempt to write to Flash

   RSTSRC = 0x02;                      // 5. Safe to enable VDD Monitor as a reset 
                                       //    source

                                       // 6. Enable Flash Writes

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1

   if (SFLE) {
      PSCTL |= 0x04;                   // Set SFLE
   }

   VDM0CN = 0xA0;                      // 7. Enable VDD monitor and high threshold

   RSTSRC = 0x02;                      // 8. Enable VDD monitor as a reset source

   *pwrite = 0;                        // 9. Initiate page erase

   RSTSRC = 0x00;                      // 10. Disable the VDD monitor as a reset
                                       //     source
   VDM0CN = 0x80;                      // 11. Change VDD Monitor to low threshold
   RSTSRC = 0x02;                      // 12. Re-enable the VDD monitor as a reset 

   if (SFLE) {
      PSCTL &= ~0x04;                  // Clear SFLE
   }

   PSCTL &= ~0x03;                     // PSWE = 0; PSEE = 0

   PSBANK = PSBANK_SAVE;               // Restore PSBANK
   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
   EA = EA_SAVE;                       // Restore interrupts

   return 1;
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------