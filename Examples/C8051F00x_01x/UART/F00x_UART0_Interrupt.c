//-----------------------------------------------------------------------------
// F00x_UART0_Interrupt.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the C8051F000 to write to and read
// from the UART interface. The program reads a word using the UART1 interrupts
// and outputs that word to the screen, with all characters in uppercase
//
// How To Test:
//
// 1) Download code to a 'F00x device that is connected to a UART transceiver
// 2) Connect serial cable from the transceiver to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 4) Download and execute code on an 'F00x target board.
//
//
// Target:         C8051F00x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (SM)
//    -6 JUN 2007
//
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F000.h>                 // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F0xx
//-----------------------------------------------------------------------------

sfr16 RCAP2    = 0xCA;                 // Timer2 capture/reload
sfr16 TMR2     = 0xCC;                 // Timer2

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define BAUDRATE     115200            // Baud rate of UART in bps

// SYSCLK = System clock frequency in Hz

#define SYSCLK       22118400L         // SYSCLK in Hz

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void UART0_Init (void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

#define UART_BUFFERSIZE 64
unsigned char UART_Buffer[UART_BUFFERSIZE];
unsigned char UART_Buffer_Size = 0;
unsigned char UART_Input_First = 0;
unsigned char UART_Output_First = 0;
unsigned char TX_Ready =1;
static char Byte;

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   WDTCN = 0xDE;                       // Disable watchdog timer
   WDTCN = 0xAD;

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init ();                       // Initialize crossbar and GPIO

   UART0_Init ();                      // Initialize UART0

   EA = 1;

   while (1)
   { 
      // If the complete word has been entered via the hyperterminal followed by
      // carriage return
      if((TX_Ready == 1) && (UART_Buffer_Size != 0) && (Byte == 13))
      {
         TX_Ready = 0;                  // Set the flag to zero
         TI = 1;                        // Set transmit flag to 1

      }
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use an external 22.1184MHz
// crystal.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   int i;                              // Software timer

   OSCICN |= 0x80;                     // Enable the missing clock detector

   // Initialize external crystal oscillator to use 22.1184 MHz crystal

   OSCXCN = 0x67;                      // Enable external crystal osc.

   for (i = 0; i < 256; i++);          // Wait at least 1 ms
   while (!(OSCXCN & 0x80));           // Wait for crystal osc to settle

   OSCICN |= 0x08;                     // Select external clock source
   OSCICN &= ~0x04;                    // Disable the internal osc.
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
// Pinout:
//
// P0.0   digital   push-pull     UART TX
// P0.1   digital   open-drain    UART RX
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   XBR0     = 0x04;                    // Enable UART0
   XBR1     = 0x00;
   XBR2     = 0x40;                    // Enable crossbar and weak pull-up

   PRT0CF |= 0x01;                     // Set TX pin to push-pull
}

//-----------------------------------------------------------------------------
// UART0_Init   Variable baud rate, Timer 2, 8-N-1
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure UART0 for operation at <baudrate> 8-N-1 using Timer2 as
// baud rate source.
//
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   CKCON = 0x20;                       // Timer2 uses the system clock
   T2CON = 0x34;                       // Timer2 used for TX and RX, enabled
   RCAP2 = - ((long) (SYSCLK/BAUDRATE)/32);
   TMR2 = RCAP2;
   TR2= 1;                             // Start Timer2

   SCON = 0x50;                        // 8-bit variable baud rate;
                                       // 9th bit ignored; RX enabled
                                       // clear all flags
   TX_Ready = 1;                       // Flag showing that UART can transmit
   ES  = 1;
}

//-----------------------------------------------------------------------------
// UART1_Interrupt
//-----------------------------------------------------------------------------
//
// This routine is invoked whenever a character is entered or displayed on the
// Hyperterminal.
//
//-----------------------------------------------------------------------------

void UART0_Interrupt (void) interrupt 4
{
   if ((SCON & 0x01) == 0x01)
   {
      // Check if a new word is being entered
      if( UART_Buffer_Size == 0)  {
         UART_Input_First = 0; } 

      SCON = (SCON & 0xFE);          //RI1 = 0;
      Byte = SBUF;                    // Read a character from Hyperterminal

      if (UART_Buffer_Size < UART_BUFFERSIZE)
      {
         UART_Buffer[UART_Input_First] = Byte;  // Store character

         UART_Buffer_Size++;            // Update array's size

         UART_Input_First++;            // Update counter
      }
   }

   if ((SCON & 0x02) == 0x02)         // Check if transmit flag is set
   {
      SCON = (SCON & 0xFD);
      if (UART_Buffer_Size != 1)        // If buffer not empty
      {

         // Check if a new word is being output
         if ( UART_Buffer_Size == UART_Input_First )  
         {
            UART_Output_First = 0;
         }

         // Store a character in the variable byte
         Byte = UART_Buffer[UART_Output_First];

         if ((Byte >= 0x61) && (Byte <= 0x7A)) { // If lower case letter
            Byte -= 32; }

         SBUF = Byte;

         UART_Output_First++;           // Update counter
         UART_Buffer_Size--;            // Decrease array size
      }
      else
      {
         UART_Buffer_Size = 0;           // Set the array size to 0
         TX_Ready = 1;                   // Indicate transmission complete
      }
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------