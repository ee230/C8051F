//-----------------------------------------------------------------------------
// Si102x_SMBus_Master_Multibyte.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Example software to demonstrate the SMBus interface in
// Master mode.
// - Interrupt-driven SMBus implementation
// - Only master states defined (no slave or arbitration)
// - multiple-byte SMBus data holders used for each transmit and receive
// - Timer1 used as SMBus clock source
// - Timer3 used by SMBus for SCL low timeout detection
// - SCL frequency defined by <SMB_FREQUENCY> constant
// - ARBLOST support included
// - supports multiple-byte writes and multiple-byte reads
// - Pinout:
//    P0.0 -> LED1
//    P0.1 -> SDA (SMBus)
//    P0.2 -> SCL (SMBus)
//    P2.0 -> C2D (debug interface)
//
//    all other port pins unused
//
// How To Test:
//
// 1) Download the code to an Si102x/3x device on a UPMP-F960-MLCD development
//    board that is connected as above to another board running the
//    SMBus_Slave_Multibyte code.
// 2) Run the code:
//         a) The test will indicate proper communication with the slave by
//            toggling LED1 on and off each time a value is sent and
//            received.
//         b) The best method to view the proper functionality is to run to
//            the indicated line of code in the TEST CODE section of main and
//            view the SMB_DATA_IN and SMB_DATA_OUT variable arrays in the
//            Watch Window.
//
// Target:         Si102x/3x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    - Initial Revision (MRF)
//    - 28 OCTOBER 2011
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1020_defs.h>               // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define  SYSCLK         24500000       // System clock frequency in Hz

#define  SMB_FREQUENCY  10000          // Target SCL clock rate
                                       // This example supports between 10kHz
                                       // and 100kHz

#define  WRITE          0x00           // SMBus WRITE command
#define  READ           0x01           // SMBus READ command

#define LED_ON           0             // Turns the LED on
#define LED_OFF          1             // Turns the LED off

// Device addresses (7 bits, lsb is a don't care)
#define  SLAVE_ADDR     0xF0           // Device address for slave target

// Status vector - top 4 bits only
#define  SMB_MTSTA      0xE0           // (MT) start transmitted
#define  SMB_MTDB       0xC0           // (MT) data byte transmitted
#define  SMB_MRDB       0x80           // (MR) data byte received
// End status vector definition

#define  NUM_BYTES_WR   3              // Number of bytes to write
                                       // Master -> Slave
#define  NUM_BYTES_RD   3              // Number of bytes to read
                                       // Master <- Slave

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

// Global holder for SMBus data
// All receive data is written here
U8 SMB_DATA_IN[NUM_BYTES_RD];

// Global holder for SMBus data.
// All transmit data is read from here
U8 SMB_DATA_OUT[NUM_BYTES_WR];


U8 TARGET;                             // Target SMBus slave address

volatile bit SMB_BUSY;                 // Software flag to indicate when the
                                       // SMB_Read() or SMB_Write() functions
                                       // have claimed the SMBus

volatile bit SMB_RW;                            // Software flag to indicate the
                                       // direction of the current transfer

U16 NUM_ERRORS;                        // Counter for the number of errors.

SBIT (LED1, SFR_P0, 0);          // LED1==LED_ON means ON

SBIT (SDA, SFR_P0, 1);                 // SMBus on P0.1
SBIT (SCL, SFR_P0, 2);                 // and P0.2

LOCATED_VARIABLE_NO_INIT (reserved, U8, SEG_XDATA, 0x0000);

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void SMBus_Init (void);
void Timer1_Init (void);
void Timer3_Init (void);
void Port_Init (void);

INTERRUPT_PROTO (TIMER3_ISR, INTERRUPT_TIMER3);
INTERRUPT_PROTO (SMBUS0_ISR, INTERRUPT_SMBUS0);

void SMB_Write (void);
void SMB_Read (void);
void T0_Waitms (U8 ms);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, then loops forever sending
// and receiving SMBus data to the slave.
//
//-----------------------------------------------------------------------------
void main (void)
{
   volatile U8 dat;         // Test counter
   volatile U8 data_count;  // SMB_DATA_IN and SMB_DATA_OUT counter
   U8 i;                    // Dummy variable counters

   PCA0MD &= ~0x40;                    // WDTE = 0 (watchdog timer enable bit)

   REG0CN |= 0x10;                     // Enable precision osc. bias
   OSCICN |= 0x80;                     // Enable precision internal oscillator
   CLKSEL =  0x00;                     // Select precision internal oscillator
                                       // divided by 1 as system clock

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

   Timer1_Init ();                     // Configure Timer1 for use as SMBus
                                       // clock source

   Timer3_Init();                      // Configure Timer3 for use with SMBus
                                       // low timeout detect

   SMBus_Init ();                      // Configure and enable SMBus

   EIE1 |= 0x01;                       // Enable the SMBus interrupt

   LED1 = LED_OFF;

   EA = 1;                             // Global interrupt enable

// TEST CODE-------------------------------------------------------------------

   dat = 0;                            // Output data counter
   NUM_ERRORS = 0;                     // Error counter
   while (1)
   {
      // SMBus Write Sequence
      for (data_count = 0; data_count < NUM_BYTES_WR; data_count++)
      {
         SMB_DATA_OUT[data_count] = dat; // Define next outgoing byte
         dat++;
      }
      TARGET = SLAVE_ADDR;             // Target the Slave for next SMBus
                                       // transfer
      SMB_Write();                     // Initiate SMBus write

      // SMBus Read Sequence
      TARGET = SLAVE_ADDR;             // Target the Slave for next SMBus
                                       // transfer
      SMB_Read();

      // Check transfer data
      for (data_count = 0; data_count < NUM_BYTES_RD; data_count++)
      {
         // Received data match transmit data?
         if(SMB_DATA_IN[data_count] != SMB_DATA_OUT[data_count])
         {
            NUM_ERRORS++;              // Increment error counter if no
                                       // match
         }
      }

      // Indicate that an error has occurred (LED no longer lit)
      if (NUM_ERRORS > 0) {
         LED1 = LED_OFF;
      }
      else {
         LED1 = !LED1;
      }

      // Run to here to view the SMB_DATA_IN and SMB_DATA_OUT variable arrays

      T0_Waitms (50);                  // Wait 50 ms until the next cycle
                                       // so that LED blinks slow enough to see
   }

// END TEST CODE---------------------------------------------------------------

}

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// SMBus configured as follows:
// - SMBus enabled
// - Slave mode inhibited
// - Timer1 used as clock source. The maximum SCL frequency will be
//   approximately 1/3 the Timer1 overflow rate
// - Setup and hold time extensions enabled
// - Bus Free and SCL Low timeout detection enabled
//
//-----------------------------------------------------------------------------
void SMBus_Init (void)
{
   SMB0CF = 0x5D;                      // Use Timer1 overflows as SMBus clock
                                       // source;
                                       // Disable slave mode;
                                       // Enable setup & hold time
                                       // extensions;
                                       // Enable SMBus Free timeout detect;
                                       // Enable SCL low timeout detect;

   SMB0CF |= 0x80;                     // Enable SMBus;
}

//-----------------------------------------------------------------------------
// Timer1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer1 configured as the SMBus clock source as follows:
// - Timer1 in 8-bit auto-reload mode
// - SYSCLK or SYSCLK / 4 as Timer1 clock source
// - Timer1 overflow rate => 3 * SMB_FREQUENCY
// - The resulting SCL clock rate will be ~1/3 the Timer1 overflow rate
// - Timer1 enabled
//
//-----------------------------------------------------------------------------
void Timer1_Init (void)
{

// Make sure the Timer can produce the appropriate frequency in 8-bit mode
// Supported SMBus Frequencies range from 10kHz to 100kHz.  The CKCON register
// settings may need to change for frequencies outside this range.
#if ((SYSCLK/SMB_FREQUENCY/3) < 255)
   #define SCALE 1
      CKCON |= 0x08;                   // Timer1 clock source = SYSCLK
#elif ((SYSCLK/SMB_FREQUENCY/4/3) < 255)
   #define SCALE 4
      CKCON |= 0x01;
      CKCON &= ~0x0A;                  // Timer1 clock source = SYSCLK / 4
#endif

   TMOD = 0x20;                        // Timer1 in 8-bit auto-reload mode

   // Timer1 configured to overflow at 1/3 the rate defined by SMB_FREQUENCY
   TH1 = (unsigned char) -(SYSCLK/SMB_FREQUENCY/SCALE/3);

   TL1 = TH1;                          // Init Timer1

   TR1 = 1;                            // Timer1 enabled
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
// - Timer3 in 16-bit auto-reload mode
// - SYSCLK/12 as Timer3 clock source
// - Timer3 reload registers loaded for a 25ms overflow period
// - Timer3 pre-loaded to overflow after 25ms
// - Timer3 enabled
//
//-----------------------------------------------------------------------------
void Timer3_Init (void)
{
   TMR3CN = 0x00;                      // Timer3 configured for 16-bit auto-
                                       // reload, low-byte interrupt disabled

   CKCON &= ~0x40;                     // Timer3 uses SYSCLK/12

   TMR3RL = (unsigned int) -(SYSCLK/12/40); // Timer3 configured to overflow after
   TMR3 = TMR3RL;                      // ~25ms (for SMBus low timeout detect):
                                       // 1/.025 = 40

   EIE1 |= 0x80;                       // Timer3 interrupt enable
   TMR3CN |= 0x04;                     // Start Timer3
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P0.0   digital   push-pull     LED1
// P0.1   digital   open-drain    SMBus SDA
// P0.2   digital   open-drain    SMBus SCL
//
//
// all other port pins unused
//
// Note: If the SMBus is moved, the SCL and SDA sbit declarations must also
// be adjusted.
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDOUT = 0x01;                    // Make the LED (P0.0) a push-pull
                                       // output
   P0SKIP = 0x01;

   XBR0 = 0x04;                        // Enable SMBus pins
   XBR2 = 0x40;                        // Enable crossbar and weak pull-ups
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
// - All incoming data is written to global variable array <SMB_DATA_IN>
// - All outgoing data is read from global variable array <SMB_DATA_OUT>
//
//-----------------------------------------------------------------------------
INTERRUPT(SMBUS0_ISR, INTERRUPT_SMBUS0)
{
   bit FAIL = 0;                       // Used by the ISR to flag failed
                                       // transfers

   static U8 sent_byte_counter;
   static U8 rec_byte_counter;

   if (ARBLOST == 0)                   // Check for errors
   {
      // Normal operation
      switch (SMB0CN & 0xF0)           // Status vector
      {
         // Master Transmitter/Receiver: START condition transmitted.
         case SMB_MTSTA:
            SMB0DAT = TARGET;          // Load address of the target slave
            SMB0DAT &= 0xFE;           // Clear the LSB of the address for the
                                       // R/W bit
            SMB0DAT |= (U8) SMB_RW;    // Load R/W bit
            STA = 0;                   // Manually clear START bit
            rec_byte_counter = 1;      // Reset the counter
            sent_byte_counter = 1;     // Reset the counter
            break;

         // Master Transmitter: Data byte transmitted
         case SMB_MTDB:
            if (ACK)                   // Slave ACK?
            {
               if (SMB_RW == WRITE)    // If this transfer is a WRITE,
               {
                  if (sent_byte_counter <= NUM_BYTES_WR)
                  {
                     // send data byte
                     SMB0DAT = SMB_DATA_OUT[sent_byte_counter-1];
                     sent_byte_counter++;
                  }
                  else
                  {
                     STO = 1;          // Set STO to terminate transfer
                     SMB_BUSY = 0;     // And free SMBus interface
                  }
               }
               else {}                 // If this transfer is a READ,
                                       // proceed with transfer without
                                       // writing to SMB0DAT (switch
                                       // to receive mode)


            }
            else                       // If slave NACK,
            {
               STO = 1;                // Send STOP condition, followed
               STA = 1;                // By a START
               NUM_ERRORS++;           // Indicate error
            }
            break;

         // Master Receiver: byte received
         case SMB_MRDB:
            if (rec_byte_counter < NUM_BYTES_RD)
            {
               SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT; // Store received
                                                         // byte
               ACK = 1;                 // Send ACK to indicate byte received
               rec_byte_counter++;      // Increment the byte counter
            }
            else
            {
               SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT; // Store received
                                                         // byte
               SMB_BUSY = 0;            // Free SMBus interface
               ACK = 0;                 // Send NACK to indicate last byte
                                       // of this transfer

               STO = 1;                 // Send STOP to terminate transfer
            }
            break;

         default:
            FAIL = 1;                  // Indicate failed transfer
                                       // and handle at end of ISR
            break;

      } // end switch
   }
   else
   {
      // ARBLOST = 1, error occurred... abort transmission
      FAIL = 1;
   } // end ARBLOST if

   if (FAIL)                           // If the transfer failed,
   {
      SMB0CF &= ~0x80;                 // Reset communication
      SMB0CF |= 0x80;
      STA = 0;
      STO = 0;
      ACK = 0;

      SMB_BUSY = 0;                    // Free SMBus

      FAIL = 0;
      LED1 = 0;

      NUM_ERRORS++;                    // Indicate an error occurred
   }

   SI = 0;                             // Clear interrupt flag
}

//-----------------------------------------------------------------------------
// Timer3 Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// A Timer3 interrupt indicates an SMBus SCL low timeout.
// The SMBus is disabled and re-enabled here
//
//-----------------------------------------------------------------------------
INTERRUPT(TIMER3_ISR, INTERRUPT_TIMER3)
{
   SMB0CF &= ~0x80;                    // Disable SMBus
   SMB0CF |= 0x80;                     // Re-enable SMBus
   TMR3CN &= ~0x80;                    // Clear Timer3 interrupt-pending
                                       // flag
   STA = 0;
   SMB_BUSY = 0;                       // Free SMBus
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
// 2) Write outgoing data to the <SMB_DATA_OUT> variable array
// 3) Call SMB_Write()
//
//-----------------------------------------------------------------------------
void SMB_Write (void)
{
   while (SMB_BUSY);                   // Wait for SMBus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 0;                         // Mark this transfer as a WRITE
   STA = 1;                            // Start transfer
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
// 3) Read input data from <SMB_DATA_IN> variable array
//
//-----------------------------------------------------------------------------
void SMB_Read (void)
{
   while (SMB_BUSY);               // Wait for bus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 1;                         // Mark this transfer as a READ

   STA = 1;                            // Start transfer

   while (SMB_BUSY);               // Wait for transfer to complete
}

//-----------------------------------------------------------------------------
// T0_Waitms
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) U8 ms - number of milliseconds to wait
//                        range is full range of character: 0 to 255
//
// Configure Timer0 to wait for <ms> milliseconds using SYSCLK as its time
// base.
//
//-----------------------------------------------------------------------------
void T0_Waitms (U8 ms)
{
   TCON &= ~0x30;                      // Stop Timer0; Clear TF0
   TMOD &= ~0x0f;                      // 16-bit free run mode
   TMOD |=  0x01;

   CKCON |= 0x04;                      // Timer0 counts SYSCLKs

   while (ms) {
      TR0 = 0;                         // Stop Timer0
      TH0 = ((-SYSCLK/1000) >> 8);     // Overflow in 1ms
      TL0 = ((-SYSCLK/1000) & 0xFF);
      TF0 = 0;                         // Clear overflow indicator
      TR0 = 1;                         // Start Timer0
      while (!TF0);                    // Wait for overflow
      ms--;                            // Update ms counter
   }

   TR0 = 0;                            // Stop Timer0
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
