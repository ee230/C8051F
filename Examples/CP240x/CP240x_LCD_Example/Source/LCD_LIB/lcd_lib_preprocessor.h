//-----------------------------------------------------------------------------
// lcd_lib_preprocessor.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// Contains port definitions for the CP240x LCD controller
//
// Target:         Multiple
// Tool chain:     Generic
//                 Silicon Laboratories IDE version 3.40 
// Project Name:   CP240x_Firmware
//
// 
//
// Release 1.0
//    -26 OCT 2009
//

//-----------------------------------------------------------------------------
// Preprocessor computations when using SPI
//-----------------------------------------------------------------------------
#if (BUS_INTERFACE == SPI)

   #if SPI_N == 0
      #define  SPIDAT            SPI0DAT
      #define	SPIF              SPIF0
   #elif SPI_N == 1
      #define  SPIDAT            SPI1DAT
      #define	SPIF              SPIF1
   #endif
#endif 

//-----------------------------------------------------------------------------
// Preprocessor computations for RAM operations
//-----------------------------------------------------------------------------

#if (CP240X_RAM_ENABLED == 0)

   // This option allows a single byte to be accessed at a time.  Otherwise,
   // the standard "Block Read/Write" routines can be used to access the 
   // RAM.  These single-byte operations are faster than the Block operations.
   #define SINGLE_BYTE_RAM_OPERATIONS            0

   // This option allows a block of RAM to be read in reverse order (from 
   // the largest address to the smallest address)
   #define AUTODECREMENT_BLOCK_RAM_OPERATIONS    0

#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
