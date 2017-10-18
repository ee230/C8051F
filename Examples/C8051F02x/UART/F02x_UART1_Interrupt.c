//-----------------------------------------------------------------------------
// F02x_UART1_Interrupt.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the C8051F020 to write to and read
// from the UART interface. The program reads a word using the UART1 interrupts
// and outputs that word to the screen, with all characters in uppercase
//
// How To Test:
//
// 1) Download code to a 'F02x device that is connected to a UART transceiver
// 2) Verify jumpers J6 and J9 are populated on the 'F02x TB.
// 3) Connect serial cable from the transceiver to a PC
// 4) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 5) Download and execute code on an 'F02x target board.
//
//
// Target:         C8051F02x
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

#include <c8051f020.h>                 // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F02x
//-----------------------------------------------------------------------------

sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 TMR2     = 0xcc;                 // Timer2

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define BAUDRATE     115200            // Baud rate of UART in bps

// SYSTEMCLOCK = System clock frequency in Hz

#define SYSTEMCLOCK       (22118400L)

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void UART1_Init (void);

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
   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init ();                       // Initialize crossbar and GPIO

   UART1_Init ();                      // Initialize UART1

   EA = 1;
   while (1)
   {
      // If the complete word has been entered via the hyperterminal followed by
      // carriage return
      if((TX_Ready == 1) && (UART_Buffer_Size != 0) && (Byte == 13))
      {
         TX_Ready = 0;                  // Set the flag to zero
         SCON1 = (SCON1 | 0x02);        // Set transmit flag to 1

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
// This function initializes the system clock to use the  external 22.1184MHz
// crystal.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   int i;                              // Software timer

   OSCICN |= 0x80;                     // Enable the missing clock detector

   // Initialize external crystal oscillator to use 22.1184 MHz crystal

   OSCXCN = 0x67;                      // Enable external crystal osc.
   for (i=0; i < 256; i++);            // Wait at least 1ms
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
// P0.0   digital   push-pull     UART TX
// P0.1   digital   open-drain    UART RX
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   XBR0     = 0x00;
   XBR1     = 0x00;
   XBR2     = 0x44;                    // Enable crossbar and weak pull-up
                                       // Enable UART1

   P0MDOUT |= 0x01;                    // Set TX pin to push-pull
}

//-----------------------------------------------------------------------------
// UART1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART1 using Timer1, for <baudrate> and 8-N-1.
// This routine configures the UART1 based on the following equation:
//
// Baud = (2^SMOD1/32)*(SYSTEMCLOCK*12^(T1M-1))/(256-TH1)
//
// This equation can be found in the datasheet, Mode1 baud rate using timer1.
// The function select the proper values of the SMOD1 and T1M bits to allow
// for the proper baud rate to be reached.
//-----------------------------------------------------------------------------
void UART1_Init (void)
{
   SCON1   = 0x50;                     // SCON1: mode 1, 8-bit UART, enable RX

   TMOD   &= ~0xF0;
   TMOD   |=  0x20;                    // TMOD: timer 1, mode 2, 8-bit reload


   if ( (((SYSTEMCLOCK/BAUDRATE)/256)/16) < 1 )
   {
      PCON |= 0x10;                    // SMOD1 (PCON.4) = 1 --> UART1 baudrate
                                       // divide-by-two disabled
      CKCON |= 0x10;                   // Timer1 uses the SYSTEMCLOCK
      TH1 = - ((SYSTEMCLOCK/BAUDRATE)/16);
   }
   else if ( (((SYSTEMCLOCK/BAUDRATE)/256)/32) < 1 )
   {
      PCON &= ~0x10;                   // SMOD1 (PCON.4) = 0 --> UART1 baudrate
                                       // divide-by-two enabled
      CKCON |= 0x10;                   // Timer1 uses the SYSTEMCLOCK
      TH1 = - ((SYSTEMCLOCK/BAUDRATE)/32);
   }
   else if ( ((((SYSTEMCLOCK/BAUDRATE)/256)/16)/12) < 1 )
   {
      PCON |= 0x10;                    // SMOD1 (PCON.4) = 1 --> UART1 baudrate
                                       // divide-by-two disabled
      CKCON &= ~0x10;                  // Timer1 uses the SYSTEMCLOCK/12
      TH1 = - (((SYSTEMCLOCK/BAUDRATE)/16)/12);
   }
   else if ( ((((SYSTEMCLOCK/BAUDRATE)/256)/32)/12) < 1 )
   {
      PCON &= ~0x10;                   // SMOD1 (PCON.4) = 0 --> UART1 baudrate
                                       // divide-by-two enabled
      CKCON &= ~0x10;                  // Timer1 uses the SYSTEMCLOCK/12
      TH1 = - (((SYSTEMCLOCK/BAUDRATE)/32)/12);
   }

   TL1 = TH1;                          // init Timer1
   TR1 = 1;                            // START Timer1
   TX_Ready = 1;                       // Flag showing that UART can transmit
   EIE2     = 0x40;                    // Enable UART1 interrupts

   EIP2    = 0x40;                     // Make UART high priority

}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// UART1_Interrupt
//-----------------------------------------------------------------------------
//
// This routine is invoked whenever a character is entered or displayed on the
// Hyperterminal.
//
//-----------------------------------------------------------------------------

void UART1_Interrupt (void) interrupt 20
{
   if ((SCON1 & 0x01) == 0x01)
   {
      // Check if a new word is being entered
      if( UART_Buffer_Size == 0)  {
         UART_Input_First = 0; } 

      SCON1 = (SCON1 & 0xFE);          //RI1 = 0;
      Byte = SBUF1;                    // Read a character from Hyperterminal

      if (UART_Buffer_Size < UART_BUFFERSIZE)
      {
         UART_Buffer[UART_Input_First] = Byte;  // Store character

         UART_Buffer_Size++;            // Update array's size

         UART_Input_First++;            // Update counter
      }
   }

   if ((SCON1 & 0x02) == 0x02)         // Check if transmit flag is set
   {
      SCON1 = (SCON1 & 0xFD);
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

         SBUF1 = Byte;

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