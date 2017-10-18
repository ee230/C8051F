//-----------------------------------------------------------------------------
// T622_UART0_Interrupt.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the C8051T622 to write to and
// read from the UART interface. The program reads a word using the UART
// interrupts and outputs that word to the screen, with all characters in
// uppercase.
//
// How To Test:
//
// 1) Ensure that jumpers are placed on J11 of the C8051T62x mother board
//    that connect the P0.4 pin to the RX_COMM signal and P0.5 to the TX_COMM
//    signal.
// 2) Specify the target baudrate in the constant <BAUDRATE>.
// 3) Connect a USB cable from the PC to the COMM connector (P4) on the mother
//    board (CP210x drivers must already be installed).
// 4) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE0>/<BAUDRATE1> and 8-N-1.
// 5) Download and execute code on an 'T622 device on a 'T62x mother board.
// 6) Type up to 64 characters into the Terminal and press Enter.  The MCU
//    will then echo back the characters that were typed.
//
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -02 SEP 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include <C8051T622_defs.h>            // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK      12000000           // SYSCLK frequency in Hz
#define BAUDRATE        9600           // Baud rate of UART in bps
#define UART_BUFFERSIZE   64

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void UART0_Init (void);
void Port_Init (void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

unsigned char UART_Buffer[UART_BUFFERSIZE];
unsigned char UART_Buffer_Size = 0;
unsigned char UART_Input_First = 0;
unsigned char UART_Output_First = 0;
unsigned char TX_Ready =1;
static char Byte;

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init ();                 // Initialize Oscillator
   Port_Init();                        // Initialize Port I/O
   UART0_Init();

   EA = 1;

   while(1)
   {
      // If the complete word has been entered via the terminal followed by
      // carriage return
      if(TX_Ready == 1 && UART_Buffer_Size != 0 && Byte == 0x0D)
      {
         TX_Ready = 0;                 // Set the flag to zero
         TI0 = 1;                      // Set transmit flag to 1
      }
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P0.4   digital   push-pull    UART TX
// P0.5   digital   open-drain   UART RX
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDOUT |= 0x10;                    // Enable UART TX as push-pull output

   XBR0     = 0x01;                    // Enable UART on P0.4(TX) and P0.5(RX)
   XBR1     = 0x40;                    // Enable crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal oscillator
// at 12 MHz.  Also enables the Missing Clock Detector.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // 12 MHz.
   RSTSRC  = 0x04;                     // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1 for <BAUDRATE> and 8-N-1.
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   if (SYSCLK/BAUDRATE/2/256 < 1) {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } else if (SYSCLK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   } else if (SYSCLK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // Init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
   TX_Ready = 1;                       // Flag showing that UART can transmit

   IP |= 0x10;                         // Make UART high priority
   ES0 = 1;                            // Enable UART0 interrupts
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// UART0_Interrupt
//-----------------------------------------------------------------------------
//
// This routine is invoked whenever a character is entered or displayed on the
// Hyperterminal.
//
//-----------------------------------------------------------------------------
INTERRUPT (UART0_ISR, INTERRUPT_UART0)
{
   // If a character was received
   if (RI0 == 1)
   {
      if( UART_Buffer_Size == 0)       // If new word is entered
      {
         UART_Input_First = 0;
      }

      RI0 = 0;                         // Clear interrupt flag

      Byte = SBUF0;                    // Read a character from UART

      // If the maximum character limit is not exceeded, store the character
      if (UART_Buffer_Size < UART_BUFFERSIZE)
      {
         UART_Buffer[UART_Input_First] = Byte; // Store in array

         UART_Buffer_Size++;           // Update array's size
         UART_Input_First++;           // Update counter
      }
   }

   // If a character was transmitted
   if (TI0 == 1)                       // Check if transmit flag is set
   {
      TI0 = 0;                         // Clear interrupt flag

      if (UART_Buffer_Size != 1)       // If buffer not empty
      {
         // If a new word is being output
         if (UART_Buffer_Size == UART_Input_First)
         {
            UART_Output_First = 0;
         }

         // Copy a character from the buffer so it can be transmitted
         Byte = UART_Buffer[UART_Output_First];

         // Convert the character to lower case
         if ((Byte >= 0x61) && (Byte <= 0x7A))
         {
            Byte -= 32;
         }

         SBUF0 = Byte;                 // Transmit the character to
                                       // Hyperterminal

         UART_Output_First++;          // Update counter
         UART_Buffer_Size--;           // Decrease array size
      }
      else
      {
         UART_Buffer_Size = 0;            // Set the array size to 0
         TX_Ready = 1;                    // Indicate transmission complete
      }
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------