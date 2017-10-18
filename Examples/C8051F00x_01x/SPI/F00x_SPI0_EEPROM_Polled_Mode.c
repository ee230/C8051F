//-----------------------------------------------------------------------------
// F00x_SPI0_EEPROM_Polled_Mode.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program accesses a SPI EEPROM using polled mode access. The 'F00x MCU 
// is configured in 4-wire Single Master Mode, and the EEPROM is the only 
// slave device connected to the SPI bus. The read/write operations are 
// tailored to access a Microchip 4 kB EEPROM 25LC320. The relevant hardware 
// connections of the 'F00x MCU are shown here:
// 
// P0.0 - SPI SCK    (digital output, push-pull) (J2 - pin 17)
// P0.1 - SPI MISO   (digital input, open-drain) (J2 - pin 18)
// P0.2 - SPI MOSI   (digital output, push-pull) (J2 - pin 15)
// P0.3 - SPI NSS    (digital input, open-drain) (unused in single master mode)
//                   (J2 - pin 16)
// P0.7 - GPIO (NSS) (digital output, push-pull) (used as slave select output)
//                   (J2 - pin 12)
// P1.6 - LED        (digital output, push-pull) (J2 - pin 3)
//
//
// How To Test:
//
// 1) Download the code to a 'F00x device that is connected as above.
// 2) Run the code. The LED will blink fast during the write/read/verify 
//    operations.
// 3) If the verification passes, the LED will blink slowly. If it fails,
//    the LED will be OFF.
//
//
// Target:         C8051F00x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (PKC)
//    -31 MAY 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <C8051F000.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for the 'F00x
//-----------------------------------------------------------------------------
sfr16 TMR2     = 0xCC;                 // Timer2 low and high bytes together

//-----------------------------------------------------------------------------
// User-defined types, structures, unions etc
//-----------------------------------------------------------------------------
#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef UINT
#define UINT unsigned int
#endif

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define SYSCLK             16000000    // Internal oscillator frequency in Hz

// Microchip 25AA320 Slave EEPROM Parameters
#define  F_SCK_MAX         2000000     // Max SCK freq (Hz)
#define  T_NSS_DISABLE_MIN 500         // Min NSS disable time (ns)
#define  EEPROM_CAPACITY   4096        // EEPROM capacity (bytes)

// EEPROM Instruction Set
#define  EEPROM_CMD_READ   0x03        // Read Command
#define  EEPROM_CMD_WRITE  0x02        // Write Command
#define  EEPROM_CMD_WRDI   0x04        // Reset Write Enable Latch Command
#define  EEPROM_CMD_WREN   0x06        // Set Write Enable Latch Command
#define  EEPROM_CMD_RDSR   0x05        // Read Status Register Command
#define  EEPROM_CMD_WRSR   0x01        // Write Status Register Command

sbit LED = P1^6;                       // LED='1' means ON
sbit EEPROM_CS = P0^7;                 // Active low chip select for EEPROM

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Reset_Sources_Init (void);
void OSCILLATOR_Init (void);
void PORT_Init (void);
void TIMER2_Init (void);
void SPI0_Init (void);
void Init_Device (void);

void Delay_us (BYTE time_us);
void Delay_ms (BYTE time_ms);
void EEPROM_Write (UINT address, BYTE value);
BYTE EEPROM_Read (UINT address);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   UINT  address;                      // EEPROM address
   BYTE  test_byte;                    // Used as a temporary variable
   
   Init_Device ();                     // Initializes hardware peripherals
   
   
   // The following code will test the EEPROM by performing write/read/verify
   // operations. The first test will write 0xFFs to the EEPROM, and the 
   // second test will write the LSBs of the EEPROM addresses.

   // Fill EEPROM with 0xFF's
   LED = 1;
   for (address = 0; address < EEPROM_CAPACITY; address++)
   {
      test_byte = 0xFF;
      EEPROM_Write (address, test_byte);

      // Print status to UART0
      if ((address % 32) == 0)
         LED = ~LED;
   }

   // Verify EEPROM with 0xFF's
   for (address = 0; address < EEPROM_CAPACITY; address++)
   {
      test_byte = EEPROM_Read (address);

      // Print status to UART0
      if ((address % 32) == 0)
         LED = ~LED;
      if (test_byte != 0xFF)
      {
         LED = 0;
         while (1);                    // Stop here on error (for debugging)
      }
   }

   // Fill EEPROM with LSB of EEPROM addresses
   for (address = 0; address < EEPROM_CAPACITY; address++)
   {
      test_byte = address & 0xFF;
      EEPROM_Write (address, test_byte);

      // Print status to UART0
      if ((address % 32) == 0)
         LED = ~LED;
   }

   // Verify EEPROM with LSB of EEPROM addresses
   for (address = 0; address < EEPROM_CAPACITY; address++)
   {
      test_byte = EEPROM_Read (address);

      // print status to UART0
      if ((address % 32) == 0)
         LED = ~LED;
      
      if (test_byte != (address & 0xFF))
      {
         LED = 0;
         while (1);                    // Stop here on error (for debugging)
      }
   }

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
void Reset_Sources_Init (void)
{
   WDTCN = 0xDE;                       // Disable WDT
   WDTCN = 0xAD;
}

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 16 MHz (nominal). See the device datasheet for oscillator specifications.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN = 0x07;
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
// Port 0
// P0.0 = SPI Bus SCK     (Push-Pull Output)
// P0.1 = SPI Bus MISO    (Open-Drain Output/Input)
// P0.2 = SPI Bus MOSI    (Push-Pull Output)
// P0.3 = SPI Bus NSS     (Open-Drain Output/Input; unused-single master mode)
// P0.4 = GP I/O          (Open-Drain Output/Input)
// P0.5 = GP I/O          (Open-Drain Output/Input)
// P0.6 = GP I/O          (Open-Drain Output/Input)
// P0.7 = GP I/O          (Push-Pull Output) (Used as NSS slave select output)

// Port 1
// P1.0 = GP I/O          (Open-Drain Output/Input)
// P1.1 = GP I/O          (Open-Drain Output/Input)
// P1.2 = GP I/O          (Open-Drain Output/Input)
// P1.3 = GP I/O          (Open-Drain Output/Input)
// P1.4 = GP I/O          (Open-Drain Output/Input)
// P1.5 = GP I/O          (Open-Drain Output/Input)
// P1.6 = GP I/O          (Push-Pull Output) (LED D3 - Target Board)
// P1.7 = GP I/O          (Open-Drain Output/Input) (Switch SW2 - Target Board)
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   PRT0CF = 0x85;
   PRT1CF = 0x40;
   XBR0 = 0x02;
   XBR1 = 0x00;
   XBR2 = 0x40;
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
   CKCON    |= 0x20;
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
// SCK line low in idle state). The SPI clock is set to 1.6 MHz (nominal). 
// The slave select pin is set to 1.
//
//-----------------------------------------------------------------------------
void SPI0_Init()
{
   SPI0CFG     = 0x07;                 // Data sampled on 1st SCK rising edge
                                       // 8-bit data words

   SPI0CN      = 0x03;                 // Master mode; SPI enabled; flags
                                       // cleared

   // Note: This example uses the internal oscillator as SYSCLK. This 
   // oscillator is specified with a tolerance of 16 MHz +/- 20%

   // The equation for SPI0CKR is (SYSCLK/(2*F_SCK_MAX))-1, but this yields
   // a SPI frequency that could be more than 2 MHz if the internal oscillator 
   // frequency is 16 MHz + 20% (19.2 MHz). But, 2 MHz is the max frequency 
   // spec of the EEPROM used here. So, the "-1" term is omitted in the 
   // following usage:   
   SPI0CKR     = (SYSCLK/(2*F_SCK_MAX));
   
   EEPROM_CS   = 1;                    // Deactivate Slave Select
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
   Reset_Sources_Init ();
   OSCILLATOR_Init ();
   PORT_Init ();
   TIMER2_Init ();
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
void Delay_us (BYTE time_us)
{
   TR2   = 0;                          // Stop timer
   TF2   = 0;                          // Clear timer overflow flag
   TMR2  = -( (UINT)(SYSCLK/1000000) * (UINT)(time_us) );
   TR2   = 1;                          // Start timer
   while (!TF2);                       // Wait till timer overflow occurs
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
void Delay_ms (BYTE time_ms)
{
   BYTE i;

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
// Writes one byte to the specified address in the EEPROM. This function polls
// the EEPROM status register after the write operation, and returns only after
// the status register indicates that the write cycle is complete. This is to
// prevent from having to check the status register before a read operation.
//
//-----------------------------------------------------------------------------
void EEPROM_Write (UINT address, BYTE value)
{
   // Writing a byte to the EEPROM is a five-step operation.
   
   // Step1: Set the Write Enable Latch to 1
   EEPROM_CS   = 0;                    // Step1.1: Activate Slave Select
   SPI0DAT  = EEPROM_CMD_WREN;         // Step1.2: Send the WREN command
   while (!SPIF);                      // Step1.3: Wait for end of transfer
   SPIF     = 0;                       // Step1.4: Clear the SPI intr. flag
   EEPROM_CS   = 1;                    // Step1.5: Deactivate Slave Select
   Delay_us (1);                       // Step1.6: Wait for at least 
                                       //          T_NSS_DISABLE_MIN
   // Step2: Send the WRITE command
   EEPROM_CS   = 0;
   SPI0DAT  = EEPROM_CMD_WRITE;
   while (!SPIF);
   SPIF     = 0;
   
   // Step3: Send the EEPROM destination address (MSB first)
   SPI0DAT  = (BYTE)((address >> 8) & 0x00FF);
   while (!SPIF);
   SPIF     = 0;
   SPI0DAT  = (BYTE)(address & 0x00FF);
   while (!SPIF);
   SPIF     = 0;
   
   // Step4: Send the value to write
   SPI0DAT  = value;
   while (!SPIF);
   SPIF     = 0;
   EEPROM_CS   = 1;
   Delay_us (1);
   
   // Step5: Poll on the Write In Progress (WIP) bit in Read Status Register
   do
   {
      EEPROM_CS   = 0;                 // Activate Slave Select
      SPI0DAT  = EEPROM_CMD_RDSR;      // Send the Read Status Register command
      while (!SPIF);                   // Wait for the command to be sent out
      SPIF     = 0;
      SPI0DAT  = 0;                    // Dummy write to output serial clock
      while (!SPIF);                   // Wait for the register to be read
      SPIF     = 0;
      EEPROM_CS   = 1;                 // Deactivate Slave Select after read
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
// Reads one byte from the specified EEPROM address.
//
//-----------------------------------------------------------------------------
BYTE EEPROM_Read (UINT address)
{
   // Reading a byte from the EEPROM is a three-step operation.
   
   // Step1: Send the READ command
   EEPROM_CS   = 0;                    // Activate Slave Select
   SPI0DAT  = EEPROM_CMD_READ;
   while (!SPIF);
   SPIF     = 0;
   
   // Step2: Send the EEPROM source address (MSB first)
   SPI0DAT  = (BYTE)((address >> 8) & 0x00FF);
   while (!SPIF);
   SPIF     = 0;
   SPI0DAT  = (BYTE)(address & 0x00FF);
   while (!SPIF);
   SPIF     = 0;
   
   // Step3: Read the value returned
   SPI0DAT  = 0;                       // Dummy write to output serial clock
   while (!SPIF);                      // Wait for the value to be read
   SPIF     = 0;
   EEPROM_CS   = 1;                    // Deactivate Slave Select
   Delay_us (1);
   
   return SPI0DAT;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------