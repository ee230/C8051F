//-----------------------------------------------------------------------------
// F96x_CRC0.c
//-----------------------------------------------------------------------------
// Copyright (C) 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program shows how to use the CRC engine in the automatic mode and the
// manual mode. The automatic mode can be used to calculate the CRC of Flash
// pages. The manual mode can be used to calculate the CRC of any number of
// bytes manually written to the CRC engine.
//
// This program calculates the 16-bit CRC of the number of Flash pages found in
// the global constant section. The first section uses the manual method to
// calculate the CRC of all bytes except the last four. The last four bytes
// store the resulting CRC value. The second section uses the automatic CRC
// feature to calculate the entire CRC, including the result stored in the last
// four bytes of the last Flash pages. The final resulting CRC should be 0.
//
// NOTE: The block size of the CRC module is equal to the Flash page size.
//       The CRC block size is 1024 bytes and the Flash page size is 1024 bytes.
//
// How To Test:
//
// 1) Check that hardware definitions match the actual hardware.
// 2)  Download code to the target board
// 3) Run the program.  If the  LED1 is ON, the program is working.
//                      If the  LED2 is ON, a CRC error was detected.
//                      If both LEDs are off,
//                        the program did not run to completion,
//                         or the hardware is not defined correctly.
//
//
// Target:         F96x
// Tool chain:     Keil ,Raisonance, SDCC
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (ES)
//    -03 JAN 2011
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F960_defs.h>               // SFR declarations
//-----------------------------------------------------------------------------
// Define Hardware
//-----------------------------------------------------------------------------
#define UDP_F960_MCU_MUX_LCD
//#define UDP_F960_MCU_EMIF
//-----------------------------------------------------------------------------
// Hardware Dependent definitions
//-----------------------------------------------------------------------------
#ifdef UDP_F960_MCU_MUX_LCD
SBIT (LED1, SFR_P0, 0);
SBIT (LED2, SFR_P0, 1);
#endif

#ifdef UDP_F960_MCU_EMIF
SBIT (LED1, SFR_P3, 0);
SBIT (LED2, SFR_P3, 1);
#endif

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK            20000000/8   // SYSCLK frequency in Hz

#define LED_ON            0
#define LED_OFF           1

#define FLASH_PAGE_SIZE   1024         // Flash page size in Bytes
#define START_PAGE        2            // Index of the Flash page to start the
                                       // auto calculation. The starting
                                       // address can be calculated by
                                       // FLASH_PAGE_SIZE * START_PAGE

#define PAGES_TO_CRC   10              // defines the number of pages to CRC

UU16 CRC_Result;                       // Will store the CRC results

// The following three pointers are used to pass a pointer into the Byte CRC
// function. This allows for an array of bytes from any of the three memory
// space below to be fed into the CRC engine.
VARIABLE_SEGMENT_POINTER (Code_PTR, U8, SEG_CODE);
VARIABLE_SEGMENT_POINTER (Xdata_PTR, U8, SEG_XDATA);
VARIABLE_SEGMENT_POINTER (Data_PTR, U8, SEG_DATA);

U16 Number_Of_Bytes;                   // Number of consecutive bytes to CRC.
                                       // This is used with the three segment
                                       // pointers above.

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void PORT_Init (void);

void Auto_CRC16 (U8 Start_Page, U8 Pages_to_CRC, U8 codeBank);
void Byte_CRC16 (U8* Start_Byte, U16 Num_Bytes);
void Read_CRC16 (UU16* Result);

void Write_CRC_to_Flash (void);
void FLASH_PageErase (U16 addr);
void FLASH_ByteWrite (U16 addr, U8 byte);
void FLASH_SetKeys (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)
   CRC_Result.U16 = 0x0000;
   PORT_Init ();

   LED1 = LED_OFF;
   LED2 = LED_OFF;

   // Calculate arguments passed into the Byte_CRC16 function
   Code_PTR =(FLASH_PAGE_SIZE*START_PAGE);
   Number_Of_Bytes = ((FLASH_PAGE_SIZE*PAGES_TO_CRC)-2);

   // Manually calculate CRC
   Byte_CRC16 (Code_PTR, Number_Of_Bytes);
   Read_CRC16 (&CRC_Result);

   // Write CRC to last bytes of last page that is used in CRC calculation
   Write_CRC_to_Flash ();

   // Calculate CRC including the CRC value written to the last bytes of last page
   Auto_CRC16 ((START_PAGE), (PAGES_TO_CRC), 0);
   Read_CRC16 (&CRC_Result);

   // If the CRC was calculated correct, the resulting CRC will be 0.
   // If the CRC is not 0, then the CRC that was written in the Byte_CRC is incorrect
   if (CRC_Result.U16 == 0x00)
   {
      LED1 = LED_ON;
      LED2 = LED_OFF;
   }
   else
   {
      LED1 = LED_OFF;
      LED2 = LED_ON;
   }

   while (1) {}                        // Spin forever
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   SFRPAGE = LEGACY_PAGE;

   XBR2    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}

//-----------------------------------------------------------------------------
// Auto_CRC16
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters   :
//   1) U8 Start_Page - Starting page of Flash memory to CRC. The starting
//         address of the first page is (FLASH_PAGE_SIZE * Start_Page)
//   2) U8 Pages_to_CRC - Number of pages in Flash to CRC. This determines the
//         total numbero of pages to CRC. If this is set to 1, the only Flash
//         page that will be included in the CRC is the Start_Page.
//   3) U8 codeBank - Code bank required to caculate CRC for data in bank 2
//      or bank 3 of 128 kB MCUs. This function itself should be located in
//      the common area.
//
//   After this function completes, the result is kept in CRC0DAT
//-----------------------------------------------------------------------------
void Auto_CRC16 (U8 Start_Page, U8 Pages_to_CRC, U8 codeBank)
{
   U8 restoreSFRPAGE;
   U8 restorePSBANK;
   bit restoreEA;

   restoreSFRPAGE = SFRPAGE;
   restorePSBANK = PSBANK;
   restoreEA=EA;
   EA = 0;

   if(codeBank>0)
   {
      PSBANK = ((PSBANK & ~0x03) | (codeBank & 0x03));
   }

   SFRPAGE = CONFIG_PAGE;

   // set CRC0SEL to 1 (16-bit calc),
   CRC0CN |= 0x10;
   // initialize CRC result to 0x00 and clear the current result.
   // If the result should be initialized to 0xFF, CRC0CN |= 0x0C;
   CRC0CN |= 0x08;

   // set starting page
   CRC0AUTO = Start_Page;
   // set # pages
   CRC0CNT = Pages_to_CRC;

   // set AUTOEN to enable automatic computation
   CRC0AUTO |= 0x80;

   // write to CRC0CN to start computation
   CRC0CN &= ~0x03;

   // follow AUTO CRC with benign 3-byte opcode
   CRC0FLIP = 0xFF;                    // benign 3-byte opcode
   CRC0AUTO &= ~0x80;                  // clear AUTOEN

   PSBANK = restorePSBANK;
   SFRPAGE = restoreSFRPAGE;
   EA = restoreEA;
}

//-----------------------------------------------------------------------------
// Byte_CRC16
//-----------------------------------------------------------------------------
//
// Return Value:  U8 - returns value indicating if the App FW is valid.
//   1) U8* Start_Byte - Pointer to first byte of data to CRC
//   2) U16 Num_Bytes - Number of consecutive bytes to include in the CRC
//
//   After this function completes, the result is kept in CRC0DAT
//-----------------------------------------------------------------------------
void Byte_CRC16 (U8* Start_Byte, U16 Num_Bytes)
{
   U16 i;

   U8 restore;

   restore = SFRPAGE;

   SFRPAGE = CONFIG_PAGE;


   // set CRC0SEL to 1 (16-bit calc),
   CRC0CN |= 0x10;
   // initialize CRC result to 0x00 and clear the current result.
   // If the result should be initialized to 0xFF, CRC0CN |= 0x0C;
   CRC0CN |= 0x08;

   for (i = 0; i < Num_Bytes; i++)
   {
      CRC0IN = Start_Byte[i];
   }

   SFRPAGE = restore;
}

//-----------------------------------------------------------------------------
// Read_CRC16
//-----------------------------------------------------------------------------
//
// Return Value : None.
// Parameters   :
//   1) U8* Result - Stores CRC into 2 bytes starting at this location
//
// Copies CRC result to two bytes starting at location passed into function
//-----------------------------------------------------------------------------
void Read_CRC16 (UU16* Result)
{
   U8 restore;

   restore = SFRPAGE;

   SFRPAGE = CONFIG_PAGE;


   // Initialize CRC0PNT to access bits 7-0 of the CRC result
   CRC0CN &= ~0x03;

   // With each read of CRC0DAT, the value in CRC0PNT will auto-increment
   (*Result).U8[LSB] = CRC0DAT;
   (*Result).U8[MSB] = CRC0DAT;

   SFRPAGE = restore;
}

//-----------------------------------------------------------------------------
// Write_CRC_to_Flash
//-----------------------------------------------------------------------------
//
// Return Value:  None.
// Parameters:    None
//
//-----------------------------------------------------------------------------
void Write_CRC_to_Flash (void)
{
   U16 end_address;
   end_address = FLASH_PAGE_SIZE * (START_PAGE + PAGES_TO_CRC);

   // First erase last page where CRC will be written to
   FLASH_SetKeys ();
   FLASH_PageErase(FLASH_PAGE_SIZE * ((START_PAGE + PAGES_TO_CRC)-1));

   end_address -= 2;                   // The resulting CRC will be stored in
                                       // last 2 bytes of the last page of
                                       // Flash (for 16-bit mode).
   FLASH_SetKeys();
   FLASH_ByteWrite (end_address++, CRC_Result.U8[MSB]);
   FLASH_SetKeys();
   FLASH_ByteWrite (end_address++, CRC_Result.U8[LSB]);
}

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) U16 addr - address of the byte to write to
//   2) U8 byte - byte to write to Flash.
//
// This routine writes <byte> to the linear FLASH address <addr>.
//-----------------------------------------------------------------------------

void FLASH_ByteWrite (U16 addr, U8 byte)
{

   bit EA_save;                    // Preserve EA
   U8 SFRPAGE_save;
   SEGMENT_VARIABLE_SEGMENT_POINTER (pwrite, U8, SEG_XDATA, SEG_DATA);

   EA_save = EA;
   EA = 0;                             // Disable interrupts

   SFRPAGE_save = SFRPAGE;
   SFRPAGE = LEGACY_PAGE;

   VDM0CN = 0x80;                      // Enable VDD monitor


   RSTSRC = 0x06;                      // Enable VDD monitor as a reset source
                                       // Leave missing clock detector enabled

   pwrite = (char xdata *) addr;

   // Keys are set in Flash_SetKeys();

   PSCTL |= 0x01;                      // PSWE = 1

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   *pwrite = byte;                     // Write the byte

   PSCTL &= ~0x05;                     // SFLE = 0; PSWE = 0

   SFRPAGE = SFRPAGE_save;             // Restore SFRPAGE
   EA = EA_save;                       // Restore EA
}

//-----------------------------------------------------------------------------
// FLASH_PageErase
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) U16 addr - address of any byte in the page to erase
//
// This routine erases the FLASH page containing the linear FLASH address
// <addr>.  Note that the page of Flash containing the Lock Byte cannot be
// erased from application code.
//-----------------------------------------------------------------------------

void FLASH_PageErase (U16 addr)
{

   bit EA_save;                    // Preserve EA
   U8 SFRPAGE_save;

   SEGMENT_VARIABLE_SEGMENT_POINTER (pwrite, U8, xdata, data);

   EA_save = EA;
   EA = 0;                             // Disable interrupts

   SFRPAGE_save = SFRPAGE;
   SFRPAGE = LEGACY_PAGE;

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x06;                      // Enable VDD monitor as a reset source
                                       // Leave missing clock detector enabled

   pwrite = (char xdata *) addr;

   // Keys are set in Flash_SetKeys();

   PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
   *pwrite = 0;                        // Initiate page erase

   PSCTL &= ~0x07;                     // SFLE = 0; PSWE = 0; PSEE = 0

   SFRPAGE = SFRPAGE_save;             // Restore SFRPAGE
   EA = EA_save;                       // Restore EA
}

//-----------------------------------------------------------------------------
// FLASH_SetKeys
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine erases sets the two required Flash keys for writes and erases.
//-----------------------------------------------------------------------------

void FLASH_SetKeys (void)
{
   U8 SFRPAGE_save;

   SFRPAGE_save = SFRPAGE;
   SFRPAGE = LEGACY_PAGE;
   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2

   SFRPAGE = SFRPAGE_save;             // Restore SFRPAGE
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------