//-----------------------------------------------------------------------------
// T61x_SPI0_EEPROM_Polled_Mode.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program accesses a SPI EEPROM using polled mode access. The 'T61x MCU 
// is configured in 4-wire Single Master Mode, and the EEPROM is the only 
// slave device connected to the SPI bus. The read/write operations are 
// tailored to access a Microchip 4 kB EEPROM 25LC320. The relevant hardware 
// connections of the 'T61x MCU are shown here:
// 
// P0.0 - SPI SCK    (digital output, push-pull)
// P0.1 - SPI MISO   (digital input, open-drain)
// P0.2 - SPI MOSI   (digital output, push-pull)
// P0.3 - SPI NSS    (digital output, push-pull)
// P0.4 - UART TXD   (digital output, push-pull)
// P0.5 - UART RXD   (digital input, open-drain)
// P1.0 - LED        (digital output, push-pull)
//
//
// How To Test:
//
// Method1:
// 1) Download the code to a 'T61x device that is connected as above.
// 2) Run the code. The LED will blink fast during the write/read/verify 
//    operations.
// 3) If the verification passes, the LED will blink slowly. If it fails,
//    the LED will be OFF.
//
// Method2 (optional):
// 1) Download code to a 'T61x device that is connected as above, and 
//    also connected to a PC through the COMM USB port, P4.
// 2) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the USB port at <BAUDRATE> and 8-N-1
// 3) HyperTerminal will print the progress of the write/read operation, and in 
//    the end will print the test result as pass or fail. Additionally, if the 
//    verification passes, the LED will blink slowly. If it fails, the LED will 
//    be OFF.
//
//
// FID:            31X000012
// Target:         C8051T61x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (SM)
//    -16 AUG 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <c8051T610_defs.h>            // SFR declarations
#include <stdio.h>                     // printf is declared here


//-----------------------------------------------------------------------------
// User-defined types, structures, unions etc
//-----------------------------------------------------------------------------
#ifndef U8
#define U8 unsigned char
#endif

#ifndef U16
#define U16 unsigned int
#endif

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define BAUDRATE           115200      // Baud rate of UART in bps
#define SYSCLK             24500000    // Internal oscillator frequency in Hz

// Microchip 25AA320 Slave EEPROM Parameters
#define  F_SCK_MAX         2000000     // Max SCK freq (Hz)
#define  T_NSS_DISABLE_MIN 500         // Min NSS disable time (ns)
#define  EEPROM_CAPACITY   4096        // EEPROM capacity (U8s)

// EEPROM Instruction Set
#define  EEPROM_CMD_READ   0x03        // Read Command
#define  EEPROM_CMD_WRITE  0x02        // Write Command
#define  EEPROM_CMD_WRDI   0x04        // Reset Write Enable Latch Command
#define  EEPROM_CMD_WREN   0x06        // Set Write Enable Latch Command
#define  EEPROM_CMD_RDSR   0x05        // Read Status Register Command
#define  EEPROM_CMD_WRSR   0x01        // Write Status Register Command

SBIT (LED, SFR_P1, 0);                 // LED==1 means ON

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void PCA0_Init (void);
void OSCILLATOR_Init (void);
void PORT_Init (void);
void TIMER2_Init (void);
void UART0_Init (void);
void SPI0_Init (void);
void Init_Device (void);

void Delay_us (U8 time_us);
void Delay_ms (U8 time_ms);
void EEPROM_Write (U16 address, U8 value);
U8 EEPROM_Read (U16 address);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   U16  address;                      // EEPROM address
   U8  test_U8;                    // Used as a temporary variable
   
   Init_Device ();                     // Initializes hardware peripherals
   
   
   // The following code will test the EEPROM by performing write/read/verify
   // operations. The first test will write 0xFFs to the EEPROM, and the 
   // second test will write the LSBs of the EEPROM addresses.

   // Fill EEPROM with 0xFF's
   LED = 1;
   printf("Filling with 0xFF's...\n");
   for (address = 0; address < EEPROM_CAPACITY; address++)
   {
      test_U8 = 0xFF;
      EEPROM_Write (address, test_U8);

      // Print status to UART0
      if ((address % 16) == 0)
      {
         printf ("\nWriting 0x%04x: %02x ", address, (U16)test_U8);
         LED = ~LED;
      }
      else
         printf ("%02x ", (U16)test_U8);
   }

   // Verify EEPROM with 0xFF's
   printf("\n\nVerifying 0xFF's...\n");
   for (address = 0; address < EEPROM_CAPACITY; address++)
   {
      test_U8 = EEPROM_Read (address);

      // Print status to UART0
      if ((address % 16) == 0)
      {
         printf ("\nVerifying 0x%04x: %02x ", address, (U16)test_U8);
         LED = ~LED;
      }
      else
         printf ("%02x ", (U16)test_U8);
      if (test_U8 != 0xFF)
      {
         LED = 0;
         printf ("Error at %u\n", address);
         while (1);                    // Stop here on error (for debugging)
      }
   }

   // Fill EEPROM with LSB of EEPROM addresses
   printf("\n\nFilling with LSB of EEPROM addresses...\n");
   for (address = 0; address < EEPROM_CAPACITY; address++)
   {
      test_U8 = address & 0xFF;
      EEPROM_Write (address, test_U8);

      // Print status to UART0
      if ((address % 16) == 0)
      {
         printf ("\nWriting 0x%04x: %02x ", address, (U16)test_U8);
         LED = ~LED;
      }
      else
         printf ("%02x ", (U16)test_U8);
   }

   // Verify EEPROM with LSB of EEPROM addresses
   printf("\n\nVerifying LSB of EEPROM addresses...\n");
   for (address = 0; address < EEPROM_CAPACITY; address++)
   {
      test_U8 = EEPROM_Read (address);

      // print status to UART0
      if ((address % 16) == 0)
      {
         printf ("\nVerifying 0x%04x: %02x ", address, (U16)test_U8);
         LED = ~LED;
      }
      else
         printf ("%02x ", (U16)test_U8);
      
      if (test_U8 != (address & 0xFF))
      {
         LED = 0;
         printf ("Error at %u\n", address);
         while (1);                    // Stop here on error (for debugging)
      }
   }

   printf ("\n\nVerification success!\n");
   
   while (1)                           // Loop forever
   {                                   
      LED = ~LED;                      // Flash LED when done (all verified)
      Delay_ms (200);
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function disables the watchdog timer.
//
//-----------------------------------------------------------------------------
void PCA0_Init (void)
{
   PCA0MD   &= ~0x40;
   PCA0MD    = 0x00;
}

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 24.5 MHz.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN   = 0x83;
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P0.0  -  SCK  (SPI0), Push-Pull,  Digital
// P0.1  -  MISO (SPI0), Open-Drain, Digital
// P0.2  -  MOSI (SPI0), Push-Pull,  Digital
// P0.3  -  NSS  (SPI0), Push-Pull,  Digital
// P0.4  -  TX0 (UART0), Push-Pull,  Digital
// P0.5  -  RX0 (UART0), Open-Drain, Digital
// P0.6  -  Unassigned,  Open-Drain, Digital
// P0.7  -  Unassigned,  Open-Drain, Digital

// P1.0  -  LED,         Push-Pull,  Digital
// P1.1  -  Unassigned,  Open-Drain, Digital
// P1.2  -  Unassigned,  Open-Drain, Digital
// P1.3  -  Unassigned,  Open-Drain, Digital
// P1.4  -  Unassigned,  Open-Drain, Digital
// P1.5  -  Unassigned,  Open-Drain, Digital
// P1.6  -  Unassigned,  Open-Drain, Digital
// P1.7  -  Unassigned,  Open-Drain, Digital

// P2.0  -  Unassigned,  Open-Drain, Digital
// P2.1  -  Unassigned,  Open-Drain, Digital
// P2.2  -  Unassigned,  Open-Drain, Digital
// P2.3  -  Unassigned,  Open-Drain, Digital
//
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0MDOUT   = 0x1D;
   P1MDOUT   = 0x01;
   XBR0      = 0x03;
   XBR1      = 0x40;
}

//-----------------------------------------------------------------------------
// TIMER2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Initializes Timer2 to be clocked by SYSCLK for use as a delay timer.
//
//-----------------------------------------------------------------------------
void TIMER2_Init (void)
{
   CKCON    |= 0x10;
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configures the UART0 using Timer1, for <BAUDRATE> and 8-N-1. Once this is
// set up, the standard printf function can be used to output data.
//
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   if (SYSCLK/BAUDRATE/2/256 < 1)
   {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } 
   else if (SYSCLK/BAUDRATE/2/256 < 4)
   {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x09;
   }
   else if (SYSCLK/BAUDRATE/2/256 < 12)
   {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else
   {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
   TI0 = 1;                            // Indicate TX0 ready
}

//-----------------------------------------------------------------------------
// SPI0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configures SPI0 to use 4-wire Single-Master mode. The SPI timing is 
// configured for Mode 0,0 (data centered on first edge of clock phase and 
// SCK line low in idle state). The SPI clock is set to 1.75 MHz. The NSS pin 
// is set to 1.
//
//-----------------------------------------------------------------------------
void SPI0_Init()
{
   SPI0CFG   = 0x40;
   SPI0CN    = 0x0D;
   
   // The equation for SPI0CKR is (SYSCLK/(2*F_SCK_MAX))-1, but this yields
   // a SPI frequency that is slightly more than 2 MHz. But, 2 MHz is the max 
   // frequency spec of the EEPROM used here. So, the "-1" term is omitted 
   // in the following usage:
   SPI0CKR   = (SYSCLK/(2*F_SCK_MAX));
}

//-----------------------------------------------------------------------------
// Init_Device
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Calls all device initialization functions.
//
//-----------------------------------------------------------------------------
void Init_Device (void)
{
   PCA0_Init ();
   OSCILLATOR_Init ();
   PORT_Init ();
   TIMER2_Init ();
   UART0_Init ();
   SPI0_Init ();
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Delay_us
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : 1. time_us - time delay in microseconds
//                   range: 1 to 255
//
// Creates a delay for the specified time (in microseconds) using TIMER2. The 
// time tolerance is approximately +/-50 ns (1/SYSCLK + function call time).
//
//-----------------------------------------------------------------------------
void Delay_us (U8 time_us)
{
   TR2   = 0;                          // Stop timer
   TF2H  = 0;                          // Clear timer overflow flag
   TMR2  = -( (U16)(SYSCLK/1000000) * (U16)(time_us) );
   TR2   = 1;                          // Start timer
   while (!TF2H);                      // Wait till timer overflow occurs
   TR2   = 0;                          // Stop timer
}

//-----------------------------------------------------------------------------
// Delay_ms
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : 1. time_ms - time delay in milliseconds
//                   range: 1 to 255
//
// Creates a delay for the specified time (in milliseconds) using TIMER2. The 
// time tolerance is approximately +/-50 ns (1/SYSCLK + function call time).
//
//-----------------------------------------------------------------------------
void Delay_ms (U8 time_ms)
{
   U8 i;

   while(time_ms--)
      for(i = 0; i< 10; i++)           // 10 * 100 microsecond delay
         Delay_us (100);
}

//-----------------------------------------------------------------------------
// EEPROM_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : 1. address - the destination EEPROM address.
//                   range: 0 to EEPROM_CAPACITY
//                2. value - the value to write.
//                   range: 0x00 to 0xFF
//
// Writes one U8 to the specified address in the EEPROM. This function polls
// the EEPROM status register after the write operation, and returns only after
// the status register indicates that the write cycle is complete. This is to
// prevent from having to check the status register before a read operation.
//
//-----------------------------------------------------------------------------
void EEPROM_Write (U16 address, U8 value)
{
   // Writing a U8 to the EEPROM is a five-step operation.
   
   // Step1: Set the Write Enable Latch to 1
   NSSMD0   = 0;                       // Step1.1: Activate Slave Select
   SPI0DAT  = EEPROM_CMD_WREN;         // Step1.2: Send the WREN command
   while (!SPIF);                      // Step1.3: Wait for end of transfer
   SPIF     = 0;                       // Step1.4: Clear the SPI intr. flag
   NSSMD0   = 1;                       // Step1.5: Deactivate Slave Select
   Delay_us (1);                       // Step1.6: Wait for at least 
                                       //          T_NSS_DISABLE_MIN
   // Step2: Send the WRITE command
   NSSMD0   = 0;
   SPI0DAT  = EEPROM_CMD_WRITE;
   while (!SPIF);
   SPIF     = 0;
   
   // Step3: Send the EEPROM destination address (MSB first)
   SPI0DAT  = (U8)((address >> 8) & 0x00FF);
   while (!SPIF);
   SPIF     = 0;
   SPI0DAT  = (U8)(address & 0x00FF);
   while (!SPIF);
   SPIF     = 0;
   
   // Step4: Send the value to write
   SPI0DAT  = value;
   while (!SPIF);
   SPIF     = 0;
   NSSMD0   = 1;
   Delay_us (1);
   
   // Step5: Poll on the Write In Progress (WIP) bit in Read Status Register
   do
   {
      NSSMD0   = 0;                    // Activate Slave Select
      SPI0DAT  = EEPROM_CMD_RDSR;      // Send the Read Status Register command
      while (!SPIF);                   // Wait for the command to be sent out
      SPIF     = 0;
      SPI0DAT  = 0;                    // Dummy write to output serial clock
      while (!SPIF);                   // Wait for the register to be read
      SPIF     = 0;
      NSSMD0   = 1;                    // Deactivate Slave Select after read
      Delay_us (1);
   } while( (SPI0DAT & 0x01) == 0x01 );
}

//-----------------------------------------------------------------------------
// EEPROM_Read
//-----------------------------------------------------------------------------
//
// Return Value : The value that was read from the EEPROM
//                   range: 0x00 to 0xFF
// Parameters   : 1. address - the source EEPROM address.
//                   range: 0 to EEPROM_CAPACITY
//
// Reads one U8 from the specified EEPROM address.
//
//-----------------------------------------------------------------------------
U8 EEPROM_Read (U16 address)
{
   // Reading a U8 from the EEPROM is a three-step operation.
   
   // Step1: Send the READ command
   NSSMD0   = 0;                       // Activate Slave Select
   SPI0DAT  = EEPROM_CMD_READ;
   while (!SPIF);
   SPIF     = 0;
   
   // Step2: Send the EEPROM source address (MSB first)
   SPI0DAT  = (U8)((address >> 8) & 0x00FF);
   while (!SPIF);
   SPIF     = 0;
   SPI0DAT  = (U8)(address & 0x00FF);
   while (!SPIF);
   SPIF     = 0;
   
   // Step3: Read the value returned
   SPI0DAT  = 0;                       // Dummy write to output serial clock
   while (!SPIF);                      // Wait for the value to be read
   SPIF     = 0;
   NSSMD0   = 1;                       // Deactivate Slave Select
   Delay_us (1);
   
   return SPI0DAT;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------