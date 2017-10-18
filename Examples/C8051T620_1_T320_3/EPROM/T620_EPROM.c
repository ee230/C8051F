//-----------------------------------------------------------------------------
// T620_EPROM.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program uses all the functions available in T620_EPROM_Primitives.c
// and T620_EPROM_Utils.c.  It uses a test data array to mimic the changes
// made to the EPROM and test for correctness.  While the test is running, the
// LED is on.  If the test fails, the LED will turn off.  If the test passes,
// the LED will blink.
//
// This program uses the In-Application Programming functionality of the
// 'T620/1.  In order to use this functionality, the capacitor on the VPP pin,
// C7, must be populated on the 'T620 daughter card.
//
// Pinout:
//
//    P2.2 -> LED
//
//    all other port pins unused
//
// How To Test:
//
// Option 1:
// 1) Load the T620_EPROM.c, T620_EPROM_Primitives.c, and T620_EPROM_Utils.c
//    files in the Silicon Labs IDE.
// 2) Populate the C7 capacitor on the 'T620 daughter card.
// 3) Compile and download code to a 'T620/1 device using a 'T620 daughter card
//    that does not have J3 shorting block populated.
// 4) Verify the P1.x and P0.x LED pins (J10) and switch pins (J9) are
//    populated on the 'T62x mother board.
// 5) Populate the J3 shorting block.
// 6) Remove the J15 VPP shorting block on the 'T62x Mother Board.
// 7) Run the code.
// 8) Check that the LED is blinking, indicating a passing test.
//
// Option 2:
// 1) Load the T620_EPROM.c, T620_EPROM_Primitives.c, and T620_EPROM_Utils.c
//    files in the Silicon Labs IDE.
// 2) Populate the C7 capacitor on the 'T620 daughter card.
// 3) Compile and download code to a 'T620/1 device using a 'T620 daughter card
//    that does not have J3 shorting block populated.
// 4) Verify the P1.x and P0.x switch pins (J9) are populated on the 'T62x
//    mother board.
// 5) Populate the J3 shorting block.
// 6) Remove the J15 VPP shorting block on the 'T62x Mother Board.
// 7) Run the code.
// 8) Open the Code window in the IDE and verify the following data at
//    address <start_address>:
//
//        A5 41 42 43 44 45
//        46 47 00 41 42 43
//        44 45 46 47 00 5A
//        5A 5A 5A 5A 5A 5A
//        5A
//
//
// Target:         C8051T620/1 or C8051T320/1/2/3
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -01 JUL 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"            // SFR declarations
#include "T620_EPROM_Primitives.h"
#include "T620_EPROM_Utils.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK               12000000  // SYSCLK in Hz
                                       // The internal oscillator has a
                                       // tolerance of +/- 2%

SBIT (LED, SFR_P2, 2);                 // LED

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);           // Oscillator initialization routine
void VDDMon_Init (void);               // VDD Monitor initialization routine
void Port_Init (void);                 // Port initialization routine

void Timer0_Delay_ms (unsigned int ms);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   unsigned char temp_byte = 0x00;
   EPROMADDR start_address = 0x3000;

   char test_write_buff[8] = "ABCDEFG";
   char test_write_buff2[3] = "HIJ";
   char test_read_buff[8] = {0};
   char test_compare_buff[8] = "ABCDEFG";

   unsigned char i;

   bit error_flag = 0;

   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   if ((RSTSRC & 0x02) != 0x02)
   {
      if ((RSTSRC & 0x40) == 0x40)
      {
         LED = 0;
         while(1);                     // Last reset was caused by a EPROM
                                       // Error Device Reset
                                       // LED is off and loop forever to
                                       // indicate error
      }
   }

   Oscillator_Init();                  // Initialize the internal oscillator
                                       // to 12 MHz

   VDDMon_Init();

   Port_Init();

   LED = 1;

   // Check if that portion of the EPROM is not written (0xFF)
   if (EPROM_ByteRead(start_address) != 0xFF)
   {
      // Check if that portion of the EPROM was already written by the test
      // (0xA5)
      if (EPROM_ByteRead(start_address) != 0xA5)
      {
         error_flag = 1;               // Indicate error - EPROM not empty
      }
   }

   //BEGIN TEST================================================================

   // Check if able to Write and Read the EPROM--------------------------------
   EPROM_ByteWrite(start_address, 0xA5);

   temp_byte = EPROM_ByteRead(start_address);

   if (temp_byte != 0xA5)
   {
      error_flag = 1;
   }
   //--------------------------------------------------------------------------

   // Check if able to write and read a series of bytes------------------------
   EPROM_Write(start_address+1, test_write_buff, sizeof(test_write_buff));

   EPROM_Read(test_read_buff, start_address+1, sizeof(test_write_buff));

   for (i = 0; i < sizeof(test_write_buff); i++)
   {
      if (test_read_buff[i] != test_write_buff[i])
      {
         error_flag = 1;
      }
   }
   //--------------------------------------------------------------------------

   // Check if able to copy data in the EPROM----------------------------------
   EPROM_Copy (start_address+sizeof(test_write_buff)+1, start_address+1,
               sizeof(test_write_buff));

   EPROM_Read(test_read_buff, start_address+sizeof(test_write_buff)+1,
              sizeof(test_read_buff));

   for (i = 0; i < sizeof(test_write_buff); i++)
   {
      if (test_read_buff[i] != test_compare_buff[i])
      {
         error_flag = 1;
      }
   }
   //--------------------------------------------------------------------------

   // EPROM test routines------------------------------------------------------
   EPROM_Fill (start_address+sizeof(test_write_buff)*2+1, sizeof(test_write_buff),
               0x5A);

   EPROM_Read(test_read_buff, start_address+sizeof(test_write_buff)*2+1,
              sizeof(test_write_buff));

   for (i = 0; i < sizeof(test_write_buff); i++)
   {
      if (test_read_buff[i] != 0x5A)
      {
         error_flag = 1;
      }
   }
   //--------------------------------------------------------------------------

   for (i = 255; i > 0; i--);          // Delay for at least 1 us

   IAPCN = 0x40;                       // Disable the charge pump discharge
                                       // on the VPP pin so this pin can be
                                       // used as regular GPIO.  This must be
                                       // more than 1 us after the last write
                                       // to EPROM.

   //END OF TEST===============================================================

   while (1)                           // Loop forever
   {
      // Blink LED to indicate success
      if (error_flag == 0)
      {
         LED = ~LED;

         Timer0_Delay_ms (100);
      }
      else
      {
         LED = 0;
      }
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the internal oscillator to 12 MHz.
//-----------------------------------------------------------------------------
void Oscillator_Init(void)
{
   OSCICN |= 0x03;                     // Set the Internal Oscillator to run
                                       // at 12 MHz
}

//-----------------------------------------------------------------------------
// VDDMon_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the VDD Monitor and enables it as a reset source.
// The VDD Monitor is enabled following a power-on reset, but this routine
// explicitly enables the VDD Monitor.
//
//-----------------------------------------------------------------------------
void VDDMon_Init(void)
{
   // Step 1. Enable the VDD Monitor (VDMEN bit in VDM0CN = ‘1’).
   VDM0CN = 0x80;                      // Enable the VDD Monitor

   // Step 2. Wait for the VDD Monitor to stabilize (approximately 5 µs).
   // Note: This delay should be omitted if software contains routines which
   // write EPROM memory.
   // Omitted in this code

   // Step 3. Select the VDD Monitor as a reset source (PORSF bit in
   // RSTSRC = ‘1’).

   RSTSRC = 0x02;
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// Pinout:
//
//    P2.2 -> LED
//
//    all other port pins unused
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P2MDOUT = 0x04;                     // Set LED as push-pull

   P2SKIP |= 0x04;                     // Skip LED

   XBR1 = 0x40;                        // Enable crossbar
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer0_Delay_ms
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) unsigned int ms - number of milliseconds of delay
//                        range is full range of integer: 0 to 65335
//
// Configure Timer0 to delay <ms> milliseconds before returning.
//
// Note: This routine modifies the SCA0-1 bits in CKCON, which will affect
// the timing of Timer1.
//-----------------------------------------------------------------------------
void Timer0_Delay_ms (unsigned int ms)
{
   unsigned i;                         // Millisecond counter

   TCON  &= ~0x30;                     // STOP Timer0 and clear overflow flag
   TMOD  &= ~0x0F;                     // Configure Timer0 to 16-bit mode
   TMOD  |=  0x01;

   CKCON &=  ~0x04;                    // Timer0 counts SYSCLK / 48
   CKCON |= 0x02;

   for (i = 0; i < ms; i++)            // Count milliseconds
   {
      TCON &= ~0x10;                   // STOP Timer0
      TH0 = (-SYSCLK/48/1000) >> 8;    // Set Timer0 to overflow in 1ms
      TL0 = -SYSCLK/48/1000;
      TCON |= 0x10;                    // START Timer0
      while (TF0 == 0);                // Wait for overflow
      TCON &= ~0x20;                   // Clear overflow indicator
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------