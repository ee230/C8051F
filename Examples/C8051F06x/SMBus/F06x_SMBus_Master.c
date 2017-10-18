//-----------------------------------------------------------------------------
// F06x_SMBus_Master.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Example software to demonstrate the C8051F06x SMBus interface in
// Master mode.
// - Interrupt-driven SMBus implementation
// - Only master states defined (no slave or arbitration)
// - 1-byte SMBus data holders used for each transmit and receive
// - Timer4 used by SMBus for SCL low timeout detection
// - SCL frequency defined by <SMB_FREQUENCY> constant
// - ARBLOST support included
// - Pinout:
//    P0.0 -> SDA (SMBus)
//    P0.1 -> SCL (SMBus)
//
//    P1.6 -> LED
//
//    all other port pins unused
//
// How To Test:
//
// 1) Download code to a 'F06x device that is connected to a SMBus slave.
// 2) Run the code:
//         a) The test will indicate proper communication with the slave by
//            toggling the LED on and off each time a value is sent and
//            received.
//         b) The best method to view the proper functionality is to run to
//            the indicated line of code in the TEST CODE section of main and
//            view the SMB_DATA_IN and SMB_DATA_OUT variables in the Watch
//            Window.
//
//
// FID:            06X000008
// Target:         C8051F06x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -20 APR 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F060.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define  SYSCLK         3062500L       // System clock frequency in Hz

#define  SMB_FREQUENCY  10000L         // Target SCL clock rate
                                       // This example supports between 10kHz
                                       // and 100kHz

#define  WRITE          0x00           // WRITE direction bit
#define  READ           0x01           // READ direction bit

// Device addresses (7 bits, lsb is a don't care)
#define  SLAVE_ADDR     0xF0           // Device address for slave target

#define  MY_ADDR        0x02           // Address of this SMBus device
                                       // (dummy value since this device does
                                       // not have any defined slave states)

#define  SMB_BUS_ERROR  0x00           // (all modes) BUS ERROR
#define  SMB_START      0x08           // (MT & MR) START transmitted
#define  SMB_RP_START   0x10           // (MT & MR) repeated START
#define  SMB_MTADDACK   0x18           // (MT) Slave address + W transmitted;
                                       //    ACK received
#define  SMB_MTADDNACK  0x20           // (MT) Slave address + W transmitted;
                                       //    NACK received
#define  SMB_MTDBACK    0x28           // (MT) data byte transmitted;
                                       //    ACK rec'vd
#define  SMB_MTDBNACK   0x30           // (MT) data byte transmitted;
                                       //    NACK rec'vd
#define  SMB_MTARBLOST  0x38           // (MT) arbitration lost
#define  SMB_MRADDACK   0x40           // (MR) Slave address + R transmitted;
                                       //    ACK received
#define  SMB_MRADDNACK  0x48           // (MR) Slave address + R transmitted;
                                       //    NACK received
#define  SMB_MRDBACK    0x50           // (MR) data byte rec'vd;
                                       //    ACK transmitted
#define  SMB_MRDBNACK   0x58           // (MR) data byte rec'vd;
                                       //    NACK transmitted

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

unsigned char SMB_DATA_IN;             // Global holder for SMBus data
                                       // All receive data is written here

unsigned char SMB_DATA_OUT;            // Global holder for SMBus data.
                                       // All transmit data is read from here

unsigned char TARGET;                  // Target SMBus slave address

bit SMB_BUSY;                          // Software flag to indicate when the
                                       // SMB_Read() or SMB_Write() functions
                                       // have claimed the SMBus

bit SMB_RW;                            // Software flag to indicate the
                                       // direction of the current transfer

unsigned long NUM_ERRORS;              // Counter for the number of errors.

// 16-bit SFR declarations
sfr16    RCAP4    = 0xCA;              // Timer4 reload registers
sfr16    TMR4     = 0xCC;              // Timer4 counter registers

sbit LED = P1^6;                       // LED on P1.6

sbit SDA = P0^0;                       // SMBus on P0.0
sbit SCL = P0^1;                       // and P0.1

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init(void);
void Port_Init(void);
void SMBus_Init(void);
void Timer4_Init(void);

void SMBus_ISR(void);
void Timer4_ISR(void);

void SMB_Write (void);
void SMB_Read (void);
void T0_Wait_ms (unsigned char ms);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void MAIN (void)
{
   volatile unsigned char dat;         // Test counter
   unsigned char i;                    // Dummy variable counters

   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   SYSCLK_Init ();                     // Set internal oscillator to a setting
                                       // of 3062500 Hz

   // If slave is holding SDA low because of an improper SMBus reset or error
   while(!SDA)
   {
      // Provide clock pulses to allow the slave to advance out
      // of its current state. This will allow it to release SDA.
      XBR1 = 0x40;                     // Enable Crossbar
      SCL = 0;                         // Drive the clock low
      for(i = 0; i < 255; i++);        // Hold the clock low
      SCL = 1;                         // Release the clock
      while(!SCL);                     // Wait for open-drain
                                       // clock output to rise
      for(i = 0; i < 10; i++);         // Hold the clock high
      XBR1 = 0x00;                     // Disable Crossbar
   }

   Port_Init ();                       // Initialize Crossbar and GPIO

   // Turn off the LED before the test starts
   LED = 0;

   SMBus_Init ();                      // Configure and enable SMBus

   Timer4_Init ();                     // Configure and enable Timer4

   EIE1 |= 0x02;                       // Enable the SMBus interrupt

   EA = 1;                             // Global interrupt enable


   SFRPAGE = SMB0_PAGE;
   SI = 0;

// TEST CODE-------------------------------------------------------------------

   dat = 0;                            // Output data counter
   NUM_ERRORS = 0;                     // Error counter
   while (1)
   {
      // SMBus Write Sequence
      SMB_DATA_OUT = dat;              // Define next outgoing byte
      TARGET = SLAVE_ADDR;             // Target the F3xx/Si8250 Slave for next
                                       // SMBus transfer
      SMB_Write();                     // Initiate SMBus write

      // SMBus Read Sequence
      TARGET = SLAVE_ADDR;             // Target the F3xx/Si8250 Slave for next
                                       // SMBus transfer
      SMB_Read();

      // Check transfer data
      if(SMB_DATA_IN != SMB_DATA_OUT)  // Received data match transmit data?
      {
         NUM_ERRORS++;                 // Increment error counter if no match
      }

      // Indicate that an error has occurred (LED no longer lit)
      if (NUM_ERRORS > 0)
      {
         LED = 0;
      }
      else
      {
         LED = ~LED;
      }

      // Run to here to view the SMB_DATA_IN and SMB_DATA_OUT variables

      dat++;

      T0_Wait_ms (1);                  // Wait 1 ms until the next cycle
   }

// END TEST CODE---------------------------------------------------------------

}

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal oscillator
// at 3.0625 MHz (24.5 / 8 MHz).
//
void SYSCLK_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x80;                      // Set internal oscillator to run
                                       // at 1/8 its maximum frequency

   CLKSEL = 0x00;                      // Select the internal osc. as
                                       // the SYSCLK source

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
                                       // detector
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P0.0   digital   open-drain    SMBus SDA
// P0.1   digital   open-drain    SMBus SCL
//
// P1.6   digital   push-pull     LED
//
// all other port pins unused
//
// Note: If the SMBus is moved, the SCL and SDA sbit declarations must also
// be adjusted.
//
void PORT_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;

   P0MDOUT = 0x00;                     // All P0 pins open-drain output

   P1MDOUT |= 0x40;                    // Make the LED (P1.6) a push-pull
                                       // output

   XBR0 = 0x01;                        // Enable SMBus on the crossbar
   XBR2 = 0x40;                        // Enable crossbar and weak pull-ups

   P0 = 0xFF;

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// SMBus_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// The SMBus peripheral is configured as follows:
// - SMBus enabled
// - Assert Acknowledge low (AA bit = 1b)
// - Free and SCL low timeout detection enabled
//
void SMBus_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = SMB0_PAGE;

   SMB0CN = 0x07;                      // Assert Acknowledge low (AA bit = 1b);
                                       // Enable SMBus Free timeout detect;
                                       // Enable SCL low timeout detect

   // SMBus clock rate (derived approximation from the Tlow and Thigh equations
   // in the SMB0CR register description)
   SMB0CR = 257 - (SYSCLK / (2 * SMB_FREQUENCY));

   SMB0ADR = MY_ADDR;                  // Set own slave address.

   SMB0CN |= 0x40;                     // Enable SMBus;

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// Timer4_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer4 configured for use by the SMBus low timeout detect feature as
// follows:
// - Timer4 in auto-reload mode
// - SYSCLK/12 as Timer4 clock source
// - Timer4 reload registers loaded for a 25ms overflow period
// - Timer4 pre-loaded to overflow after 25ms
// - Timer4 enabled
//
void Timer4_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TMR4_PAGE;

   TMR4CN = 0x00;                      // Timer4 external enable off;
                                       // Timer4 in timer mode;
                                       // Timer4 in auto-reload mode

   TMR4CF = 0x00;                      // Timer4 uses SYSCLK/12
                                       // Timer4 output not available
                                       // Timer4 counts up

   RCAP4 = -(SYSCLK/12/40);            // Timer4 configured to overflow after
   TMR4 = RCAP4;                       // ~25ms (for SMBus low timeout detect)

   EIE2 |= 0x04;                       // Timer4 interrupt enable
   TR4 = 1;                            // Start Timer4

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// SMBus ISR state machine
// - Master only implementation - no slave or arbitration states defined
// - All incoming data is written to global variable <SMB_DATA_IN>
// - All outgoing data is read from global variable <SMB_DATA_OUT>
//
void SMBUS_ISR (void) interrupt 7
{
   bit FAIL = 0;                       // Used by the ISR to flag failed
                                       // transfers

   // Status code for the SMBus (SMB0STA register)
   switch (SMB0STA)
   {
      // Master Transmitter/Receiver: START condition transmitted.
      // Load SMB0DAT with slave device address.
      case SMB_START:

      // Master Transmitter/Receiver: repeated START condition transmitted.
      // Load SMB0DAT with slave device address
      case SMB_RP_START:
         SMB0DAT = TARGET;             // Load address of the slave.
         SMB0DAT &= 0xFE;              // Clear the LSB of the address for the
                                       // R/W bit
         SMB0DAT |= SMB_RW;            // Load R/W bit
         STA = 0;                      // Manually clear STA bit

         break;

      // Master Transmitter: Slave address + WRITE transmitted.  ACK received.
      // For a READ: N/A
      //
      // For a WRITE: send the data to the slave.
      case SMB_MTADDACK:

         // send data byte
         SMB0DAT = SMB_DATA_OUT;

         break;

      // Master Transmitter: Slave address + WRITE transmitted.  NACK received.
      // Restart the transfer.
      case SMB_MTADDNACK:
         STA = 1;                      // Restart transfer
         break;

      // Master Transmitter: Data byte transmitted.  ACK received.
      // For a READ: N/A
      //
      // For a WRITE: Data byte was sent, so stop the transfer.
      case SMB_MTDBACK:

         STO = 1;                      // Set STO to terminte transfer
         SMB_BUSY = 0;                 // clear software busy flag

         break;

      // Master Transmitter: Data byte transmitted.  NACK received.
      // Restart the transfer.
      case SMB_MTDBNACK:
         STA = 1;                      // Restart transfer

         break;

      // Master Receiver: Slave address + READ transmitted.  ACK received.
      // For a READ: set the NACK after the data byte is received to end the
      //  transfer.
      //
      // For a WRITE: N/A
      case SMB_MRADDACK:
         AA = 0;                       // Only one byte in this transfer,
                                       // send NACK after byte is received
         break;

      // Master Receiver: Slave address + READ transmitted.  NACK received.
      // Restart the transfer.
      case SMB_MRADDNACK:
         STA = 1;                      // Restart transfer

         break;

      // Master Receiver: Data byte received.  NACK transmitted.
      // For a READ: Read operation has completed.  Read data register and
      //  send STOP.
      //
      // For a WRITE: N/A
      case SMB_MRDBNACK:
         SMB_DATA_IN = SMB0DAT;        // Store received byte
         STO = 1;
         SMB_BUSY = 0;
         AA = 1;                       // Set AA for next transfer

         break;

      // Master Transmitter: Arbitration lost.
      case SMB_MTARBLOST:

         FAIL = 1;                     // Indicate failed transfer
                                       // and handle at end of ISR

         break;

      // All other status codes invalid.  Reset communication.
      default:
         FAIL = 1;

         break;
   }

   if (FAIL)                           // If the transfer failed,
   {
      SMB0CN &= ~0x40;                 // Reset communication
      SMB0CN |= 0x40;
      STA = 0;
      STO = 0;
      AA = 0;

      SMB_BUSY = 0;                    // Free SMBus

      FAIL = 0;
   }

   SI = 0;                             // Clear interrupt flag
}

//-----------------------------------------------------------------------------
// Timer4 Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// A Timer4 interrupt indicates an SMBus SCL low timeout.
// The SMBus is disabled and re-enabled if a timeout occurs.
//
void Timer4_ISR (void) interrupt 16
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = SMB0_PAGE;

   SMB0CN &= ~0x40;                    // Disable SMBus
   SMB0CN |= 0x40;                     // Re-enable SMBus

   SFRPAGE = SFRPAGE_SAVE;             // Switch back to the Timer4 SFRPAGE

   TF4 = 0;                            // Clear Timer4 interrupt-pending flag
   SMB_BUSY = 0;                       // Free bus
}

//-----------------------------------------------------------------------------
// Support Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMB_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Writes a single byte to the slave with address specified by the <TARGET>
// variable.
// Calling sequence:
// 1) Write target slave address to the <TARGET> variable
// 2) Write outgoing data to the <SMB_DATA_OUT> variable
// 3) Call SMB_Write()
//
void SMB_Write (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = SMB0_PAGE;

   while (SMB_BUSY);                   // Wait for SMBus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 0;                         // Mark this transfer as a WRITE
   STA = 1;                            // Start transfer

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// SMB_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Reads a single byte from the slave with address specified by the <TARGET>
// variable.
// Calling sequence:
// 1) Write target slave address to the <TARGET> variable
// 2) Call SMB_Write()
// 3) Read input data from <SMB_DATA_IN> variable
//
void SMB_Read (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = SMB0_PAGE;

   while (SMB_BUSY);                   // Wait for bus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 1;                         // Mark this transfer as a READ

   STA = 1;                            // Start transfer

   while (SMB_BUSY);                   // Wait for transfer to complete

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// T0_Wait_ms
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) unsigned char ms - number of milliseconds to wait
//                        range is full range of character: 0 to 255
//
// Configure Timer0 to wait for <ms> milliseconds using SYSCLK as its time
// base.
//
void T0_Wait_ms (unsigned char ms)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TIMER01_PAGE;

   TCON &= ~0x30;                      // Stop Timer0; Clear TF0
   TMOD &= ~0x0f;                      // 16-bit free run mode
   TMOD |=  0x01;

   CKCON |= 0x08;                      // Timer0 counts SYSCLKs

   while (ms) {
      TR0 = 0;                         // Stop Timer0
      TH0 = -(SYSCLK/1000 >> 8);       // Overflow in 1ms
      TL0 = -(SYSCLK/1000);
      TF0 = 0;                         // Clear overflow indicator
      TR0 = 1;                         // Start Timer0
      while (!TF0);                    // Wait for overflow
      ms--;                            // Update ms counter
   }

   TR0 = 0;                            // Stop Timer0

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
