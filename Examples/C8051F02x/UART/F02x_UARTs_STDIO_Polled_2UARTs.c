//-----------------------------------------------------------------------------
// F02x_UARTs_STDIO_Polled_2UARTs.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates configuration of UART0 and UART1 and includes
// an example of how to overload the STDIO library functions to enable
// selection of either UART0 or UART1 in code, via the "UART" global variable.
// This way both UARTs can use the same pins and present messages.
//
// Pinout
//
//    P0.0   digital   push-pull     UART TX
//    P0.1   digital   open-drain    UART RX
//    P1.6   digital   push-pull     LED
//
// How To Test:
//
// 1) Download code to a 'F02x device that is connected to a UART transceiver
// 2) Connect serial cable from the transceiver to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 4) HyperTerminal will print a message if everything is working properly
//
// Obs: The designer must check that the required baud rate is reached and
//    that the error is within range for the application.
//
//
// FID:            02X000005
// Target:         C8051F02x
// Tool chain:     KEIL C51 7.50 / KEIL EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (CG)
//    -24 JUL 2006
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

#define BAUDRATE     1200              // Baud rate of UART in bps

// SYSCLK = System clock frequency in Hz

#define SYSCLK       (22118400L)

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init_UART0 (void);
void PORT_Init_UART1 (void);
void UART0_Init (void);
void UART1_Init (void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

unsigned char UART;                    // Global variable -- when '0', UART0
                                       // is used for stdio; when '1', UART1
                                       // is used for stdio

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init_UART0 ();                 // Initialize crossbar and GPIO

   UART0_Init ();                      // Initialize UART0
   UART1_Init ();                      // Initialize UART1


   while (1)
   {
      // print something to UART0
      PORT_Init_UART0 ();              // Configure Crossbar to pinout
                                       // UART0 TX and RX to P0.0 and P0.1
      UART = 0;
      printf ("UART 0\n\n");

      // now print something to UART1
      PORT_Init_UART1 ();              // Configure Crossbar to pinout
                                       // UART1 TX and RX to P0.0 and P0.1
      UART = 1;
      printf ("UART 1\n\n");
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
   OSCICN |= 0x08;                  // Select external clock source
   OSCICN &= ~0x04;                 // Disable the internal osc.
}

//-----------------------------------------------------------------------------
// PORT_Init_UART0
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
void PORT_Init_UART0 (void)
{
   XBR0     = 0x04;                    // Enable UART0
   XBR1     = 0x00;
   XBR2     = 0x40;                    // Enable crossbar and weak pull-up

   P0MDOUT |= 0x01;                    // Set TX pin to push-pull
}

//-----------------------------------------------------------------------------
// PORT_Init_UART1
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
void PORT_Init_UART1 (void)
{
   XBR0     = 0x00;
   XBR1     = 0x00;
   XBR2     = 0x44;                    // Enable crossbar and weak pull-up
                                       // Enable UART1

   P0MDOUT |= 0x01;                    // Set TX pin to push-pull
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

   SCON0 = 0x50;                       // 8-bit variable baud rate;
                                       // 9th bit ignored; RX enabled
                                       // clear all flags
   TI0     = 1;                        // Indicate TX0 ready
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
// Baud = (2^SMOD1/32)*(SYSCLK*12^(T1M-1))/(256-TH1)
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


   if ( (((SYSCLK/BAUDRATE)/256)/16) < 1 )
   {
      PCON |= 0x10;                    // SMOD1 (PCON.4) = 1 --> UART1 baudrate
                                       // divide-by-two disabled
      CKCON |= 0x10;                   // Timer1 uses the SYSCLK
      TH1 = - ((SYSCLK/BAUDRATE)/16);
   }
   else if ( (((SYSCLK/BAUDRATE)/256)/32) < 1 )
   {
      PCON &= ~0x10;                   // SMOD1 (PCON.4) = 0 --> UART1 baudrate
                                       // divide-by-two enabled
      CKCON |= 0x10;                   // Timer1 uses the SYSCLK
      TH1 = - ((SYSCLK/BAUDRATE)/32);
   }
   else if ( ((((SYSCLK/BAUDRATE)/256)/16)/12) < 1 )
   {
      PCON |= 0x10;                    // SMOD1 (PCON.4) = 1 --> UART1 baudrate
                                       // divide-by-two disabled
      CKCON &= ~0x10;                  // Timer1 uses the SYSCLK/12
      TH1 = - (((SYSCLK/BAUDRATE)/16)/12);
   }
   else if ( ((((SYSCLK/BAUDRATE)/256)/32)/12) < 1 )
   {
      PCON &= ~0x10;                   // SMOD1 (PCON.4) = 0 --> UART1 baudrate
                                       // divide-by-two enabled
      CKCON &= ~0x10;                  // Timer1 uses the SYSCLK/12
      TH1 = - (((SYSCLK/BAUDRATE)/32)/12);
   }
   TL1 = TH1;                          // Initialize Timer1
   TR1 = 1;                            // Start Timer1
   SCON1 |= 0x02;                      // Indicate TX1 ready (TI1=1)
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART1 using Timer1, for <baudrate> and 8-N-1.
// This routine overloads the standard putchar() library function to support
// either UART0 or UART1, depending on the state of the global variable
// <UART>.
//
//-----------------------------------------------------------------------------
char putchar (char c)
{
   if (UART == 0)
   {
      if (c == '\n')                      // If carriage return
      {
         while (!TI0);
         TI0 = 0;
         SBUF0 = 0x0d;                    // Output CR
      }
      while (!TI0);                       // Wait for transmit complete
      TI0 = 0;
      SBUF0 = c;                          // Send character
   }
   else if (UART == 1)
   {
      if (c == '\n')                      // If carriage return
      {
         while ((SCON1 & 0x02) == 0);     // Wait for the transmit to complete
         SCON1 &= ~0x02;                  // Clear TI1
         SBUF1 = 0x0d;                    // Output CR
      }
      while ((SCON1 & 0x02) == 0);        // Wait for the transmit to complete
      SCON1 &= ~0x02;                     // Clear TI1
      SBUF1 = c;                          // Send character
   }
   return c;
}

//-----------------------------------------------------------------------------
// _getkey
//-----------------------------------------------------------------------------
//
// Return Value : Char received throught serial port (UART0 or UART1)
// Parameters   : None
//
// This routine overloads the standard _getkey() library function to support
// either UART0 or UART1, depending on the state of the global variable
// <UART>.
//-----------------------------------------------------------------------------
char _getkey ()
{
   char c;

   if (UART == 0)
   {
      while (!RI0);                       // Wait for byte to be received
      c = SBUF0;                          // Read the byte
   }
   else if (UART == 1)
   {
      while ((SCON1 & 0x01) == 0);        // Wait for the reception to complete
      c = SBUF1;                          // Read the byte
   }
   return (c);
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------