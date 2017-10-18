//-----------------------------------------------------------------------------
// F12x_SMBus_Multimaster.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Example software to demonstrate the C8051F12x SMBus interface in
// a Multi-Master environment.
// - Interrupt-driven SMBus implementation
// - Master and slave states defined
// - 1-byte SMBus data holders used for each transmit and receive
// - Timer3 used by SMBus for SCL low timeout detection
// - SCL frequency defined by <SMB_FREQUENCY> constant
// - ARBLOST support included
// - When another master attempts to transmit at the same time:
//      - the 'F12x SMBus master should detect any conflict using ARBLOST
//      - the 'F12x SMBus master should give up the bus, if necessary, and
//        switch to slave receiver mode
//      - the 'F12x SMBus master should transmit after the other master is done
// - Pinout:
//    P0.0 -> SDA (SMBus)
//    P0.1 -> SCL (SMBus)
//
//    P1.6 -> LED
//
//    P3.7 -> SW
//
//    all other port pins unused
//
// How To Test:
//
// 1) Download code to a 'F12x device that is connected to another C8051Fxxx
//    SMBus multimaster.
// 2) Run the code.  The test will indicate proper communication with the
//    other C8051Fxxx device by blinking the LED on the other device on and
//    off each time SW is pressed, even if the buttons on both boards are
//    pressed at the same time.
//
//      'F12x Multimaster                     Other C8051Fxxx Multimaster
//         LED (blinks) <------------------------------ Switch
//              SW --------------------------------> LED (blinks)
//
// NOTE: Before running the test, verify that MY_ADDR and MULTI_ADDR are
// defined to the proper values.
//
// FID:            12X000017
// Target:         C8051F12x
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

#include <C8051F120.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define  SYSCLK         6125000L       // System clock frequency in Hz

#define  SMB_FREQUENCY  10000L         // Target SCL clock rate
                                       // This example supports between 10kHz
                                       // and 100kHz

#define  WRITE          0x00           // WRITE direction bit
#define  READ           0x01           // READ direction bit

// Device addresses (7 bits, lsb is a don't care)
#define  MY_ADDR        0x02           // Device address for this device target
#define  MULTI_ADDR     0xF0           // Device address for other multimaster
                                       // target

// Master States
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

// Slave States
#define  SMB_SROADACK   0x60           // (SR) SMB's own slave address + W
                                       //    rec'vd; ACK transmitted
#define  SMB_SROARBLOST 0x68           // (SR) SMB's own slave address + W
                                       //    rec'vd; arbitration lost
#define  SMB_SRGADACK   0x70           // (SR) general call address rec'vd;
                                       //    ACK transmitted
#define  SMB_SRGARBLOST 0x78           // (SR) arbitration lost when
                                       //    transmitting slave addr + R/W as
                                       //    master; general call address
                                       //    rec'vd; ACK transmitted
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
#define  SMB_STOARBLOST 0xb0           // (ST) arbitration lost in transmitting
                                       //    slave address + R/W as master; own
                                       //    slave address rec'vd; ACK
                                       //    transmitted
#define  SMB_STDBACK    0xb8           // (ST) data byte transmitted; ACK
                                       //    rec'ed
#define  SMB_STDBNACK   0xc0           // (ST) data byte transmitted; NACK
                                       //    rec'ed
#define  SMB_STDBLAST   0xc8           // (ST) last data byte transmitted
                                       // (AA=0); ACK received
#define  SMB_SCLHIGHTO  0xd0           // (ST & SR) SCL clock high timer per
                                       //    SMB0CR timed out (FTE=1)
#define  SMB_IDLE       0xf8           // (all modes) Idle


// Data to indicate the switch was pressed or released
#define  SW_PRESSED     0x0A
#define  SW_RELEASED    0x50

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

// Global holder for SMBus master data. All receive data is written here
// while in master mode
// Slave->Master
unsigned char SMB_DATA_IN_MASTER = 0x00;

// Global holder for SMBus master data. All transmit data is read from here
// while in master mode
// Master->Slave
unsigned char SMB_DATA_OUT_MASTER = 0x00;

// Global holder for SMBus slave data. All receive data is written here
// while slave mode
// Master->Slave
unsigned char SMB_DATA_IN_SLAVE = 0x00;

// Global holder for SMBus slave data. All transmit data is read from here
// while in slave mode
// Slave->Master
unsigned char SMB_DATA_OUT_SLAVE = 0x00;

bit DATA_READY = 0;                    // Set to '1' by the SMBus ISR
                                       // when a new data byte has been
                                       // received in slave mode.

unsigned char TARGET;                  // Target SMBus slave address

bit SMB_BUSY;                          // Software flag to indicate when the
                                       // SMB_Read() or SMB_Write() functions
                                       // have claimed the SMBus

bit SMB_RW;                            // Software flag to indicate the
                                       // direction of the current transfer

// 16-bit SFR declarations
sfr16    RCAP3    = 0xCA;              // Timer3 reload registers
sfr16    TMR3     = 0xCC;              // Timer3 counter registers

sbit LED = P1^6;                       // LED on P1.6
sbit SW = P3^7;                        // Switch on P3.7

sbit SDA = P0^0;                       // SMBus on P0.0
sbit SCL = P0^1;                       // and P0.1

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init(void);
void Port_Init(void);
void SMBus_Init(void);
void Timer0_Init (void);
void Timer1_Init (void);
void Timer3_Init(void);

void SMBus_ISR(void);
void Timer3_ISR(void);

void SMB_Write (void);
void Blink_LED (void);
void Stop_LED (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void MAIN (void)
{
   bit switch_pressed_flag = 0;        // Used to detect when the switch is
                                       // released
   unsigned char i;                    // Dummy variable counters

   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   SYSCLK_Init ();                     // Set internal oscillator to a setting
                                       // of 6125000 Hz

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

   Timer3_Init ();                     // Configure and enable Timer3

   Timer0_Init ();                     // Configure Timer0 for the LED

   EIE1 |= 0x02;                       // Enable the SMBus interrupt

   ET0 = 1;                            // Enable the Timer0 interrupt
   PT0 = 1;                            // Make the Timer0 interrupt high
                                       // priority

   EA = 1;                             // Global interrupt enable


   SFRPAGE = SMB0_PAGE;
   SI = 0;

   while (1)
   {
      // Check if the switch was just pressed
      if ((switch_pressed_flag == 0) && (SW == 0))
      {
          // SMBus Write Sequence

          // Tell the other device the switch was pressed
          SMB_DATA_OUT_MASTER = SW_PRESSED;

          TARGET = MULTI_ADDR;         // Target the other multimaster device
          SMB_Write();                 // Initiate SMBus write

          switch_pressed_flag = 1;
      }

      // Determine if the switch was just released
      if ((switch_pressed_flag == 1) && (SW != 0))
      {
          // SMBus Write Sequence

          // Tell the other device the switch was released
          SMB_DATA_OUT_MASTER = SW_RELEASED;

          TARGET = MULTI_ADDR;         // Target the other multimaster device
          SMB_Write();                 // Initiate SMBus write

          switch_pressed_flag = 0;
      }

      // Check if data was sent to this device from the other multimaster
      // device
      if (DATA_READY == 1)
      {
         if (SMB_DATA_IN_SLAVE == SW_PRESSED)
         {
            Blink_LED();
         }
         else
         {
            if (SMB_DATA_IN_SLAVE == SW_RELEASED)
            {
               Stop_LED();
            }
         }

         DATA_READY = 0;
      }
   }  // Loop forever
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
// at 6.125 MHz (24.5 / 4 MHz).
//
void SYSCLK_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x81;                      // Set internal oscillator to run
                                       // at 1/4 its maximum frequency

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
// P3.7   digital   open-drain    SW (switch)
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
   SMB0CR = 257 - (SYSCLK / (8 * SMB_FREQUENCY));

   SMB0ADR = MY_ADDR;                  // Set own slave address.

   SMB0CN |= 0x40;                     // Enable SMBus;

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// Timer0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer0 is configured to blink the LED.
//
void Timer0_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TIMER01_PAGE;

   TMOD = 0x02;                        // Timer0 in 8-bit auto-reload mode

   CKCON = 0x02;                       // Timer0 uses SYSCLK/48

   TH0 = 0x00;                         // Make Timer0 wait the longest time
                                       // between LED blinks.
   TL0 = TH0;                          // Init Timer0

   // Keeper Timer0 off until the switch on the other multimaster device is
   // pushed

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
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
// - Timer3 in auto-reload mode
// - SYSCLK/12 as Timer3 clock source
// - Timer3 reload registers loaded for a 25ms overflow period
// - Timer3 pre-loaded to overflow after 25ms
// - Timer3 enabled
//
void Timer3_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TMR3_PAGE;

   TMR3CN = 0x00;                      // Timer3 external enable off;
                                       // Timer3 in timer mode;
                                       // Timer3 in auto-reload mode

   TMR3CF = 0x00;                      // Timer3 uses SYSCLK/12
                                       // Timer3 output not available
                                       // Timer3 counts up

   RCAP3 = -(SYSCLK/12/40);            // Timer3 configured to overflow after
   TMR3 = RCAP3;                       // ~25ms (for SMBus low timeout detect)

   EIE2 |= 0x01;                       // Timer3 interrupt enable
   TR3 = 1;                            // Start Timer3

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer0 Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// Timer0 blinks the LED when instructed by the other multimaster.
//
void Timer0_ISR (void) interrupt 1
{
   static unsigned char count = 0;     // Count up to 256 Timer0 interrupts
                                       // before switching the LED
                                       // otherwise, it won't be visible to
                                       // the eye
   if (count == 255)
   {
      LED = ~LED;                      // Turn the LED on and off

      count = 0;
   }
   else
   {
      count++;
   }
}

//-----------------------------------------------------------------------------
// SMBus Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// SMBus ISR state machine
// - Master only implementation - no slave states defined
// - All incoming data is written starting at the global pointer <pSMB_DATA_IN>
// - All outgoing data is read from the global pointer <pSMB_DATA_OUT>
//
void SMBUS_ISR (void) interrupt 7
{
   bit FAIL = 0;                       // Used by the ISR to flag failed
                                       // transfers

   // Status code for the SMBus (SMB0STA register)
   switch (SMB0STA)
   {
      // Master States --------------------------------------------------------

      // Master Transmitter/Receiver: START condition transmitted.
      // Load SMB0DAT with slave device (EEPROM) address.
      case SMB_START:

      // Master Transmitter/Receiver: repeated START condition transmitted.
      // Load SMB0DAT with slave device address
      case SMB_RP_START:
         SMB0DAT = TARGET;             // Load address of the target slave
         SMB0DAT &= 0xFE;              // Clear the LSB of the address for the
                                       // R/W bit
         SMB0DAT |= SMB_RW;            // Load R/W bit
         STA = 0;                      // Manually clear START bit

         break;

      // Master Transmitter: Slave address + WRITE transmitted.  ACK received.
      case SMB_MTADDACK:

         // send data byte
         SMB0DAT = SMB_DATA_OUT_MASTER;

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
         SMB_BUSY = 0;                 // Clear software busy flag

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

      // Master Receiver: Data byte received.  ACK transmitted.
      // For a READ: Read operation has completed.  Read data register and
      //  send STOP.
      //
      // For a WRITE: N/A
      case SMB_MRDBNACK:

         SMB_DATA_IN_MASTER = SMB0DAT; // Store received byte
         STO = 1;
         SMB_BUSY = 0;
         AA = 1;                       // Set AA for next transfer

         break;

      // Master Transmitter: Arbitration lost. (bus error)
      case SMB_MTARBLOST:

         FAIL = 1;                     // Indicate failed transfer
                                       // and handle at end of ISR

         break;

      // Slave States ---------------------------------------------------------

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

         SMB_DATA_IN_SLAVE = SMB0DAT;  // Store incoming data
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

         SMB0DAT = SMB_DATA_OUT_SLAVE;

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


      // Slave Receiver: Arbitration lost, general call address received.
      // ACK transmitted.
      // Fall through.
      case SMB_SRGARBLOST:

      // Slave Receiver: Arbitration lost, own slave address + WRITE received.
      // ACK transmitted.
      // Fall through.
      case SMB_SROARBLOST:

      // Slave Transmitter: Arbitration lost as master. Own address + READ
      // received.
      // ACK transmitted.
      // Set STA bit to retry the transfer when the bus is free.
      case SMB_STOARBLOST:

         STA = 1;                      // Retry transfer when bus is free.

         break;

      // SCL High Timeout: SCL detected high longer than calculated SMB0CR
      // time.
      // No action necessary.  Bus is now free.
      case SMB_SCLHIGHTO:

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
// Timer3 Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// A Timer3 interrupt indicates an SMBus SCL low timeout.
// The SMBus is disabled and re-enabled if a timeout occurs.
//
void Timer3_ISR (void) interrupt 14
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = SMB0_PAGE;

   SMB0CN &= ~0x40;                    // Disable SMBus
   SMB0CN |= 0x40;                     // Re-enable SMBus

   SFRPAGE = SFRPAGE_SAVE;             // Switch back to the Timer3 SFRPAGE

   TF3 = 0;                            // Clear Timer3 interrupt-pending flag
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
// 2) Write outgoing data to the <SMB_data_out> variable
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
// Blink_LED
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Starts the LED blinking.
//
void Blink_LED (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TIMER01_PAGE;

   TR0 = 1;

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// Stop_LED
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Stops the LED from blinking.
//
void Stop_LED (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TIMER01_PAGE;

   TR0 = 0;

   LED = 0;                            // Turn the LED off when it's not
                                       // blinking

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page detector
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
