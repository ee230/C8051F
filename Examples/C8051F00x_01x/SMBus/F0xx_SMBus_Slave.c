//-----------------------------------------------------------------------------
// F0xx_SMBus_Slave.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Example software to demonstrate the C8051F0xx ('F005 family) SMBus interface
// in Slave mode:
// - Interrupt-driven SMBus implementation
// - Only slave states defined
// - 1-byte SMBus data holder used for both transmit and receive
// - Timer3 used by SMBus for SCL low timeout detection
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
// 1) Download code to a 'F0xx device that is connected to a SMBus master.
// 2) Run the code.  The slave code will write data and read data from the
//    same data byte, so a successive write and read will effectively echo the
//    data written.  To verify that the code is working properly, verify on the
//    master that the data written is the same as the data received.
//
//
// FID:            00X000005
// Target:         C8051F0xx ('F005 TB)
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

#include <C8051F000.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define  SYSCLK         4000000L       // System clock frequency in Hz

#define  SMB_FREQUENCY  10000L         // Target SCL clock rate
                                       // This example supports between 10kHz
                                       // and 100kHz

#define  WRITE          0x00           // WRITE direction bit
#define  READ           0x01           // READ direction bit

// Device addresses (7 bits, lsb is a don't care)
#define  MY_ADDR        0xF0           // Address of this SMBus device

// SMBus Buffer Size
#define  SMB_BUFF_SIZE  0x08           // Defines the maximum number of bytes
                                       // that can be sent or received in a
                                       // single transfer

#define  SMB_BUS_ERROR  0x00           // (all modes) BUS ERROR
#define  SMB_SROADACK   0x60           // (SR) SMB's own slave address + W
                                       //    rec'vd; ACK transmitted
#define  SMB_SRGADACK   0x70           // (SR) general call address rec'vd;
                                       //    ACK transmitted
#define  SMB_SRODBACK   0x80           // (SR) data byte received under own
                                       //    slave address; ACK returned
#define  SMB_SRODBNACK  0x88           // (SR) data byte received under own
                                       //    slave address; NACK returned
#define  SMB_SRGDBACK   0x90           // (SR) data byte received under general
                                       //    call address; ACK returned
#define  SMB_SRGDBNACK  0x98           // (SR) data byte received under general
                                       //    call address; NACK returned
#define  SMB_SRSTOP     0xa0           // (SR) STOP or repeated START received
                                       //    while addressed as a slave
#define  SMB_STOADACK   0xa8           // (ST) SMB's own slave address + R
                                       //    rec'vd; ACK transmitted
#define  SMB_STDBACK    0xb8           // (ST) data byte transmitted; ACK
                                       //    rec'ed
#define  SMB_STDBNACK   0xc0           // (ST) data byte transmitted; NACK
                                       //    rec'ed
#define  SMB_STDBLAST   0xc8           // (ST) last data byte transmitted
                                       //    (AA=0); ACK received
#define  SMB_SCLHIGHTO  0xd0           // (ST & SR) SCL clock high timer per
                                       //    SMB0CR timed out (FTE=1)
#define  SMB_IDLE       0xf8           // (all modes) Idle

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

unsigned char SMB_DATA;                // Global holder for SMBus data.
                                       // All receive data is written
                                       // here;
                                       // all transmit data is read
                                       // from here

bit DATA_READY = 0;                    // Set to '1' by the SMBus ISR
                                       // when a new data byte has been
                                       // received.

// 16-bit SFR declarations
sfr16    TMR3RL   = 0x92;              // Timer3 reload registers
sfr16    TMR3     = 0x94;              // Timer3 counter registers

sbit LED = P1^6;                       // LED on P1.6

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void Port_Init(void);
void SMBus_Init(void);
void Timer3_Init(void);

void SMBus_ISR(void);
void Timer3_ISR(void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void MAIN (void)
{
   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   OSCICN = 0x05;                      // Set internal oscillator to run
                                       // at 4 MHz

   Port_Init ();                       // Initialize Crossbar and GPIO

   // Turn off the LED before the test starts
   LED = 0;

   SMBus_Init ();                      // Configure and enable SMBus

   Timer3_Init ();                     // Configure and enable Timer3

   EIE1 |= 0x02;                       // Enable the SMBus interrupt

   EA = 1;                             // Global interrupt enable

   SI = 0;

   SMB_DATA = 0xFD;                    // Initialize SMBus data holder

   while(1)
   {
      while(!DATA_READY);              // New SMBus data received?
      DATA_READY = 0;
      LED = ~LED;
   }
}

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

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
void PORT_Init (void)
{
   PRT0CF = 0x00;                      // All P0 pins open-drain output

   PRT1CF |= 0x40;                     // Make the LED (P1.6) a push-pull
                                       // output

   XBR0 = 0x01;                        // Enable SMBus on the crossbar
   XBR2 = 0x40;                        // Enable crossbar and weak pull-ups

   P0 = 0xFF;
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
   SMB0CN = 0x07;                      // Assert Acknowledge low (AA bit = 1b);
                                       // Enable SMBus Free timeout detect;
                                       // Enable SCL low timeout detect

   // SMBus clock rate (derived approximation from the Tlow and Thigh equations
   // in the SMB0CR register description)
   SMB0CR = 257 - (SYSCLK / (2 * SMB_FREQUENCY));

   SMB0ADR = MY_ADDR;                  // Set own slave address.

   SMB0CN |= 0x40;                     // Enable SMBus;
}

//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer3 configured for use by the SMBus low timeout detect feature as
// follows:
// - SYSCLK/12 as Timer3 clock source
// - Timer3 reload registers loaded for a 25ms overflow period
// - Timer3 pre-loaded to overflow after 25ms
// - Timer3 enabled
//
void Timer3_Init (void)
{
   TMR3CN = 0x00;                      // Timer3 uses SYSCLK/12

   TMR3RL = -(SYSCLK/12/40);           // Timer3 configured to overflow after
   TMR3 = TMR3RL;                      // ~25ms (for SMBus low timeout detect)

   EIE2 |= 0x01;                       // Timer3 interrupt enable
   TMR3CN |= 0x04;                     // Start Timer3
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// SMBus ISR state machine
// - Slave only implementation - no master states defined
// - All incoming data is written to global variable <SMB_DATA>
// - All outgoing data is read from global variable <SMB_DATA>
//
void SMBUS_ISR (void) interrupt 7
{
   bit FAIL = 0;                       // Used by the ISR to flag failed
                                       // transfers

   // Status code for the SMBus (SMB0STA register)
   switch (SMB0STA)
   {
      // Slave Receiver: Slave address + WRITE received.  ACK transmitted.
      // Fall through.
      case SMB_SROADACK:

      // Slave Receiver: General call address received.  ACK transmitted.
      case SMB_SRGADACK:

         break;

      // Slave Receiver: Data byte received after addressed by general
      // call address + WRITE.
      // ACK transmitted.  Fall through.
      case SMB_SRGDBACK:

      // Slave Receiver: Data byte received after addressed by own
      // slave address + WRITE.
      // ACK transmitted.
      case SMB_SRODBACK:

         SMB_DATA = SMB0DAT;           // Store incoming data
         DATA_READY = 1;               // Indicate new data received

         break;

      // Slave Receiver: Data byte received while addressed as slave.
      // NACK transmitted. Should not occur since AA will not be cleared
      // as slave.  Fall through to next state.
      case SMB_SRODBNACK:

      // Slave Receiver: Data byte received while addressed by general call.
      //  NACK transmitted.
      // Should not occur since AA will not be cleared as slave.
      case SMB_SRGDBNACK:
         AA = 1;

         break;

      // Slave Receiver: STOP or repeated START received while addressed as
      // slave.
      case SMB_SRSTOP:

         break;

      // Slave Transmitter: Own slave address + READ received.  ACK
      // transmitted.
      // Load SMB0DAT with data to be output.
      case SMB_STOADACK:
         SMB0DAT = SMB_DATA;

         break;

      // Slave Transmitter: Data byte transmitted.  ACK received.  Fall
      // through.
      case SMB_STDBACK:

      // Slave Transmitter: Data byte transmitted.  NACK received. Fall
      // through.
      case SMB_STDBNACK:

      // Slave Transmitter: Last data byte transmitted.  ACK received.
      // No action necessary.
      case SMB_STDBLAST:

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
      AA = 1;

      FAIL = 0;
   }

   SI = 0;                             // Clear interrupt flag
}

//-----------------------------------------------------------------------------
// Timer3 Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// A Timer3 interrupt indicates an SMBus SCL low timeout.
// The SMBus is disabled and re-enabled if a timeout occurs.
//
void Timer3_ISR (void) interrupt 14
{
   SMB0CN &= ~0x40;                    // Disable SMBus
   SMB0CN |= 0x40;                     // Re-enable SMBus
   TMR3CN &= ~0x80;                    // Clear Timer3 interrupt-pending flag
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
