//-----------------------------------------------------------------------------
// F53xA_LIN_Master.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// This program's purpose is to communicate between the A and B sides of the
// C8051F53xA target board through the Local Interconnect Network (LIN).
//
// Pressing the P1.4_A switch will trigger the P1.3_B LED to light up.
// Releasing the P1.4_A switch will turn off the P1.3_B LED.
// Alternatively, pressing the P1.4_B switch will turn on the P1.3_A LED
// while releasing the P1.4_B switch will turn off the P1.3_A LED.
//
// This example is intended to be used with the LIN_Slave example.
//
// How To Test:
//
// 1) Verify the LED and switch pin jumpers are populated
//    (HDR4 for device A and HDR3 for device B).
//
// 2) If you wish to change the baud rate of the LIN, change the following
//    global constant: LIN_BAUD_RATE
//
// 3) Download the code to a F53xA-TB (either device A or device B) that is
//    connected as above to another device running the LIN0_Slave code.
//
// 4) Run the code.
//
// 5) If the communication passes, when a switch on a side of the target board
//    is pressed, the LED on the opposite side will light up. When said switch
//    is released, the LED will turn off.
//
// Target:         C8051F53x/52xA (C8051F530A TB)
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Revision History:
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0 / 30 OCT 2008 (ADT / GP / TP)
//    -Initial Revision
//
// Obs1: When the master sends a request frame, the ID sent from the master
//       is the same as the ID read by the master after a slave transmit frame,
//       even if the slave tries to edit the ID before transmitting.
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F520A_defs.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

SBIT (SW1, SFR_P1, 4);                 // SW1 = '0' means switch pressed
SBIT (LED, SFR_P1, 3);                 // LED = '1' means LED on

#define SYSCLK 24500000L               // Clock speed in Hz

#if (SYSCLK < 8000000)                 // For the slowest baud rate
#error Minimum system clock needed to operate LIN peripheral is 8Mhz.
#endif

#define TWENTY_MS (U16)((.02/(1.0/SYSCLK)) / 12.0) // Clock cycles for 20ms

#define  LIN_BAUD_RATE 19200           // Baud Rate to run LIN peripheral
                                       // 625 < LIN_BAUD_RATE < 20000

//-----------------------------------------------------------------------------
// LIN #defines
//-----------------------------------------------------------------------------

//------ Control register ------
#define  STREQ       0x01              // LIN Master Start transmission request
#define  WUPREQ      0x02              // Wake-Up request
#define  RSTERR      0x04              // Error reset request
#define  RSTINT      0x08              // Interrupt reset request
#define  DTACK       0x10              // Slave Data ACK
#define  TXRX        0x20              // Master selection of TX or RX mode
#define  SLEEP       0x40              // Slave flag set by master to inform
                                       // the peripheral that either a sleep
                                       // mode frame or bus idle timeout
                                       // was reached
#define  STOP        0x80              // Slave flag, to be set by the
                                       // application so no processing is to be
                                       // done until next SYNCH BREAK
//------ Status Register ------
#define  DONE        0x01              // Msg. processing completed
#define  WAKEUP      0x02              // Wake-Up signal
#define  ERROR       0x04              // Error detected
#define  LININTREQ   0x08              // Interrupt request
#define  DTREQ       0x10              // ID detected
#define  ABORT       0x20              // Abort requested
#define  IDLTOUT     0x40              // Time-out detected
#define  ACTIVE      0x80              // Interface active (communicating)

#define LIN_MULTIPLIER (U8)((20000.0 / LIN_BAUD_RATE) - 1)

#if (SYSCLK >= 16000000)
 #define LIN_PRESCALAR 1
 #define LIN_DIVIDER (U16) (SYSCLK / ((U32) 4 * (LIN_MULTIPLIER + 1) * (U32) LIN_BAUD_RATE))
#else
 #define LIN_PRESCALAR 0
 #define LIN_DIVIDER (U16) (SYSCLK / ((U32) 2 * (LIN_MULTIPLIER + 1) * (U32) LIN_BAUD_RATE))
#endif

// Instruction Set
#define ID_SW1_SLAVE_QUERY 0x01
#define ID_LED_SLAVE       0x02        //LED on the slave side
#define ID_LED_SLAVE_OFF   0x02
#define ID_LED_SLAVE_ON    0x03

#define LED_MASTER_OFF     0x04
#define LED_MASTER_ON      0x05

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

UU16 Next_Interrupt_Time;              // Contains the next interrupt time that
                                       // PCA module 2 will try to match

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);           // Configure the system clock
void Port_Init (void);                 // Configure the Crossbar and GPIO
void PCA0_Init (void);                 // Configure the PCA
void LIN0_Master_Init (void);          // Configure LIN for master

INTERRUPT_PROTO (PCA0_ISR, INTERRUPT_PCA0);
INTERRUPT_PROTO (LIN0_ISR, INTERRUPT_LIN);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   Oscillator_Init ();                 // Initialize the system clock
   Port_Init ();                       // Initialize crossbar and GPIO
   PCA0_Init ();                       // Initialize PCA0
   LIN0_Master_Init ();                // Initialize LIN0

   EA = 1;                             // Enable Interrupts

   LED = 0;                            // Initialize LED to be off

   while (1);
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configure the internal oscillator to maximum internal frequency of 24.5 Mhz
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN = 0x87;
}

//-----------------------------------------------------------------------------
// Port_Init *EDIT THIS SECTION*
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P0.1  -  TX (LIN), Push-Pull,  Digital
// P0.2  -  RX (LIN), Open-Drain, Digital
//
// P1.3  -  LED, Push-Pull, Digital
// P1.4  -  SW1, Open-Drain, Digital
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDOUT = 0x02;                     // Set TX (LIN) to push-pull
   P1MDOUT = 0x08;                     // Set LED to push-pull

   P0SKIP = 0x01;                      // Skip to place LIN on P0.1 and P0.2

   XBR0 = 0x04;                        // Enable LIN
   XBR1 = 0x40;                        // Enable crossbar and weak pullups
}


//-----------------------------------------------------------------------------
// LIN0_Master_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Intializes the LIN peripheral to master mode.
//
// LIN is an asynchronous, serial communications interface. All LIN registers
// besides the control mode register (LINCF) are indirectly accessed;
// the address is specified in the LINADDR register which points to a specific
// data register which can be accessed through the LINDATA register.
// Waveforms and additional specifications can be found in the LIN consortium
// (http:\\www.lin-subbus.com).
//
//-----------------------------------------------------------------------------
void LIN0_Master_Init (void)
{
   // Enable interface, configure the device for LIN Master, manual baud rate
   LINCF = 0x80;
   LINCF |= 0x40;

   // Load the least significant byte of the divider.
   LINADDR = LIN0DIV;
   LINDATA = (U8) LIN_DIVIDER;

   // Load the multiplier, prescalar, and most significant bit of the divider.
   LINADDR = LIN0MUL;
   LINDATA = (U8) ((LIN_PRESCALAR << 6) | (LIN_MULTIPLIER << 1)
               | (LIN_DIVIDER >> 8));

   LINADDR = LIN0SIZE;
   LINDATA = 0x80;                     // Enhanced checksum

   LINADDR = LIN0CTRL;
   LINDATA = 0x0C;                     // Reset LININT and LIN0ERR

   EIE1 |= 0x20;                       // Enable  LIN interrupts
}

//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Module 2 is configured as a match interrupt to be used as a periodic
// interrupt every 20 ms.  This allows the Master to send a LIN transmit or
// request frame periodically.
//
//-----------------------------------------------------------------------------
void PCA0_Init (void)
{
   // Configure PCA time base; overflow interrupt disabled
   PCA0CN = 0x00;                      // Stop counter; clear all flags
   PCA0MD = 0x00;                      // Use SYSCLK / 12 as time base

   PCA0CPM2 = 0x49;                    // Configure PCA0 Module 2 to interrupt
                                       // periodically

   PCA0L = 0x00;                       // Set PCA0 counter to 0
   PCA0H = 0x00;

   // Initialize PCA0 module 2 to interrupt in 20 ms
   Next_Interrupt_Time.U16 = TWENTY_MS;
   PCA0CPL2 = Next_Interrupt_Time.U8[LSB];
   PCA0CPH2 = Next_Interrupt_Time.U8[MSB];

   EIE1 |= 0x04;                       // Enable PCA interrupts

   CR = 1;                             // Start PCA counter
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PCA0_ISR
//-----------------------------------------------------------------------------
//
// This is the ISR for the PCA0. Module 2 will send a LIN transmit or request
// frame every 20ms to the slave. If there is a falling edge on the master
// switch, meaning that the switch was just pressed, send a LED on transfer.
// If there is a rising edige on the master switch, meaning that the switch
// was just released, send a LED off transfer. If the switch was in the same
// state as it was previously, meaning that the switch is either not pressed
// or is being held down, send a request frame to retreive the status of the
// slave switch.
//
//-----------------------------------------------------------------------------
INTERRUPT (PCA0_ISR, INTERRUPT_PCA0)
{
   static U8 sw1prev = 0;              // Status of the switch during the last
                                       // interrupt

   if (CCF2)
   {
      CCF2 = 0;                        // Clear other interrupt flags for PCA

      // Interrupt every 20 ms
      Next_Interrupt_Time.U16 = Next_Interrupt_Time.U16 + TWENTY_MS;

      PCA0CPL2 = Next_Interrupt_Time.U8[LSB];
      PCA0CPH2 = Next_Interrupt_Time.U8[MSB];

      if ((sw1prev == 0) && (SW1 == 1))
      {
         // Button was down and switch is now off = rising edge
         // Turn off LED
         LINADDR = LIN0CTRL;
         LINDATA |= TXRX;              // Select to transmit data

         LINADDR = LIN0ID;
         LINDATA = ID_LED_SLAVE_OFF;   // Tell slave to turn off LED

         // Data byte not used, but necessary to prevent slave error
         LINADDR = LIN0SIZE;
         LINDATA = (LINDATA & 0xF0) | 0x01;   // Transmitting 1 byte

         LINADDR = LIN0CTRL;
         LINDATA |= STREQ;             // Start Request
      }
      else if ((sw1prev == 1) && (SW1 == 0))
      {
         // Button was up and switch is now pressed = falling edge
         // Turn on LED
         LINADDR = LIN0CTRL;
         LINDATA |= TXRX;              // Select to transmit data

         LINADDR = LIN0ID;
         LINDATA = ID_LED_SLAVE_ON;    // Tell slave to turn on LED

         // Data byte not used, but necessary to prevent slave error
         LINADDR = LIN0SIZE;
         LINDATA = (LINDATA & 0xF0) | 0x01;   // Transmitting 1 byte

         LINADDR = LIN0CTRL;
         LINDATA |= STREQ;             // Start Request
      }
      else if ((sw1prev == SW1))
      {
         // Same state as last state, covers 00 and 11
         // Poll slave switch
         LINADDR = LIN0CTRL;
         LINDATA &= ~TXRX;             // Create a master receive operation

         LINADDR = LIN0ID;
         LINDATA = ID_SW1_SLAVE_QUERY; // Want to know status of slave switch

         LINADDR = LIN0SIZE;
         LINDATA = (LINDATA & 0xF0) | 0x01;   // Transmitting 1 byte

         LINADDR = LIN0CTRL;
         LINDATA |= STREQ;             // Start Request
      }
   }
   else
   {
      PCA0CN &= ~0xBB;                 // Clear other PCA interrupt flags
   }

   sw1prev = SW1;                      // Save value of switch
}

//-----------------------------------------------------------------------------
// LIN0_ISR
//-----------------------------------------------------------------------------
//
// This ISR triggers when completing/receiving a transmission from the slave.
// Depending on what message is sent, LED (P1.3) on the slave will turn on/off.
// When receiving a transmission from the slave, the information in data byte
// 1 will tell the master the state of the switch on the slave side. The master
// will then toggle the LED (P1.3) on the master side accordingly.
//
//-----------------------------------------------------------------------------
INTERRUPT (LIN0_ISR, INTERRUPT_LIN)
{
   U8 ctrl, ID, command, status, error;
   static U8 num_error = 0;

   LINADDR = LIN0ST;                   // Retrieve status
   status = LINDATA;

   LINADDR = LIN0CTRL;
   ctrl = LINDATA;

   LINADDR = LIN0ID;
   ID = LINDATA;

   if (status & LININTREQ)
   {
      if (status & ERROR)              // If an error was detected
      {
         LINADDR = LIN0ERR;
         error = LINDATA;              // Check the error register
         // Keep track of the number of errors received
         num_error++;
         // Handle your errors here, application dependent
      }
      else if(status & DONE)           // If transmission/reception completed
      {
         // If in recieve mode and ID is correct
         if (((ctrl & TXRX) == 0) && (ID == ID_SW1_SLAVE_QUERY))
         {
            LINADDR = LIN0DT1;
            command = LINDATA;         // Get info from transmission

            if (command == LED_MASTER_ON)
            {
               LED = 1;
            }
            if (command == LED_MASTER_OFF)
            {
               LED = 0;
            }
         }
      }
   }

   LINADDR = LIN0CTRL;
   LINDATA |= 0x0C;                    // Reset LININT and LIN0ERR
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------