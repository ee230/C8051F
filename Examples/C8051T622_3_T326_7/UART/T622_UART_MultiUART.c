//-----------------------------------------------------------------------------
// T622_UART_MultiUART.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program configures UART0 and UART1 to operate in polled mode, suitable
// for use with the stdio.h functions printf() and scanf().
//
// Test code is included for printf() and getchar(), and shows an example
// of how the putchar() and _getkey() library routines can be overridden to
// allow for multiple UARTs to be selected by the functions (in this example,
// the global variable UART is used to select between UART0 and UART1).
//
// The example sets system clock to maximum frequency of 48 MHz.
//
// The system clock frequency is stored in a global constant SYSCLK.  The
// target UART0 baud rate is stored in a global constant BAUDRATE0, and the
// target UART1 baud rate is stored in a global constant BAUDRATE1.
//
//
// How To Test:
//
// 1) Download code to a 'T622 device on a 'T62x mother board.
// 2) Verify the P1.2 LED pin of J10 is populated on the 'T62x MB.
// 3) Verify the P0.1 switch jumper is removed (J9).
// 4) Connect a USB cable from the PC to the COMM connector (P4) on the mother
//    board (CP210x drivers must already be installed).
// 5) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE0>/<BAUDRATE1> and 8-N-1.
// 6) To test UART 0, place jumpers on J11 connecting P0.4 to RX_COMM and P0.5
//    to TX_COMM.  To test UART 1, run wires from P0.0 on J2 to the RX_COMM pin
//    of J12 and P0.1 on J2 to the TX_COMM pin of J12.
// 7) Run the code.  Once connected to the correct UART (UART0 by default),
//    pressing '1' on the keyboard should turn on the P2.2 LED and pressing '0'
//    on the keyboard should turn off the P2.2 LED.
//
//
// Target:       C8051T622/3, 'T326/7
// Tool chain:   KEIL C51 7.20 / KEIL EVAL C51
//
// Release 1.0
//    -Initial Revision (TP)
//    -02 SEP 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T622_defs.h"            // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       48000000          // SYSCLK frequency in Hz
#define BAUDRATE0      115200          // Baud rate of UART0 in bps
#define BAUDRATE1      115200          // Baud rate of UART1 in bps

SBIT (LED, SFR_P1, 2);                 // LED = 1 means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void UART0_Init (void);
void UART1_Init (void);

void Delay (void);

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

bit UART = 0;                          // '0 is UART0; '1' is UART1

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   char input_char;

   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   Oscillator_Init ();                // Initialize oscillator
   Port_Init ();                       // Initialize crossbar and GPIO
   UART0_Init ();                      // Initialize UART0
   UART1_Init ();                      // Initialize UART1

   // Transmit example UART0
   UART = 0;                           // Select UART0
   printf ("Hello, from UART0!\n");


   // Transmit example UART1
   UART = 1;                           // Select UART1
   printf ("Hello, from UART1!\n");

   // Receive example: a '1' turns LED on; a '0' turns LED off.
   // Select which UART to receive on by changing the next line of code.

   UART = 0;                           // Select UART: 0 = UART0, 1 = UART1

   while (1)
   {
      input_char = getchar();
      printf (" '%c', 0x%02x\n", (unsigned char) input_char,
                                 (unsigned) input_char);

      switch (input_char)
      {
         case '0':
            LED = 0;
            break;
         case '1':
            LED = 1;
            break;
         default:
            break;
      }
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal system clock
// running at 48 MHz.
//
// The 'T622/1 does not have a clock multiplier, but this function is
// compatible with the 'F322 development platform.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKMUL  = 0x00;                     // Select internal oscillator as
                                       // input to clock multiplier

   CLKMUL |= 0x80;                     // Enable clock multiplier
   Delay();                            // Delay for clock multiplier to begin
   CLKMUL |= 0xC0;                     // Initialize the clock multiplier
   Delay();                            // Delay for clock multiplier to begin

   while(!(CLKMUL & 0x20));            // Wait for multiplier to lock

   CLKSEL  = 0x03;                     // Select system clock
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports
//
// P0.0     TX1 (UART 1), digital push-pull
// P0.1     RX1 (UART 1), digital open-drain
//
// P0.4     TX0 (UART 0), digital push-pull
// P0.5     RX0 (UART 0), digital open-drain
//
// P1.2     LED, digital push-pull
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDOUT |= 0x11;                    // Set TX (UART1) and TX (UART0) to
                                       // push-pull output
   P1MDOUT |= 0x04;                    // Set LED to push-pull output

   XBR0 = 0x01;                        // Route UART 0 to crossbar
   XBR2 = 0x01;                        // Route UART 1 to crossbar
   XBR1 = 0x40;                        // Enable crossbar
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1 for BAUDRATE0 and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits

   if (SYSCLK/BAUDRATE0/2/256 < 1) {
      TH1 = -(SYSCLK/BAUDRATE0/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } else if (SYSCLK/BAUDRATE0/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE0/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x09;
   } else if (SYSCLK/BAUDRATE0/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE0/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSCLK/BAUDRATE0/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // Init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
   TI0 = 1;                            // Indicate TX0 ready
}

//-----------------------------------------------------------------------------
// UART1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART1 for BAUDRATE1 and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART1_Init (void)
{
   SMOD1 = 0x0C;                       // Set to disable parity, 8-data bit,
                                       // disable extra bit,
                                       // stop bit 1 bit wide

   SCON1 = 0x10;                       // SCON1: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits

   if (SYSCLK/BAUDRATE1/2/0xFFFF < 1) {
      SBRL1 = -(SYSCLK/BAUDRATE1/2);
      SBCON1 |= 0x03;                  // Set prescaler to 1
   } else if (SYSCLK/BAUDRATE1/2/0xFFFF < 4) {
      SBRL1 = -(SYSCLK/BAUDRATE1/2/4);
      SBCON1 &= ~0x03;
      SBCON1 |= 0x01;                  // Set prescaler to 4
   } else if (SYSCLK/BAUDRATE1/2/0xFFFF < 12) {
      SBRL1 = -(SYSCLK/BAUDRATE1/2/12);
      SBCON1 &= ~0x03;                 // Set prescaler to 12
   } else {
      SBRL1 = -(SYSCLK/BAUDRATE1/2/48);
      SBCON1 &= ~0x03;
      SBCON1 |= 0x02;                  // Set prescaler to 4
   }

   SCON1 |= 0x02;                      // Indicate ready for TX
   SBCON1 |= 0x40;                     // Enable baud rate generator
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
// Return Value : UART0/1 buffer value
// Parameters   : character to be transmitted across UART0/1
//
// This is an overloaded fuction found in the stdio library.  When the
// function putchar is called, either by user code or through calls to stdio
// routines such as printf, the following routine will be executed instead
// of the function located in the stdio library.
//
// The function checks the UART global variable to determine which UART to
// use to receive a character.
//
// The routine expands '\n' to include a carriage return as well as a
// new line character by first checking to see whether the character
// passed into the routine equals '\n'.  If it is, the routine waits for
// TI0/TI1 to be set, indicating that UART 0/1 is ready to transmit another
// byte.  The routine then clears TI0/TI1 bit, and sets the UART0/1 output
// buffer to '0x0d', which is the ASCII character for carriage return.
//
// The routine the waits for TI0/TI1 to be set, clears TI0/TI1, and sets
// the UART output buffer to <c>.
//
//-----------------------------------------------------------------------------
char putchar (char c)
{
   if (UART == 0)
   {
      if (c == '\n')                   // Check for newline character
      {
         while (!TI0);                 // Wait until UART0 is ready to transmit
         TI0 = 0;                      // Clear interrupt flag
         SBUF0 = 0x0D;                 // Output carriage return command
      }
      while (!TI0);                    // Wait until UART0 is ready to transmit
      TI0 = 0;                         // Clear interrupt flag
      return (SBUF0 = c);              // Output <c> using UART 0
   }
   else if (UART == 1)
   {
      if (c == '\n')                   // Check for newline character
      {
         while (!(SCON1 & 0x02));      // Wait until UART1 is ready to transmit
         SCON1 &= ~0x02;               // Clear TI1 interrupt flag
         SBUF1 = 0x0D;                 // Output carriage return
      }
      while (!(SCON1 & 0x02));         // Wait until UART1 is ready to transmit
      SCON1 &= ~0x02;                  // Clear TI1 interrupt flag
      return (SBUF1 = c);              // Output <c> using UART 1
   }
}

//-----------------------------------------------------------------------------
// _getkey
//-----------------------------------------------------------------------------
//
// Return Value : byte received from UART0/1
// Parameters   : none
//
// This is an overloaded fuction found in the stdio library.  When the
// function _getkey is called, either by user code or through calls to stdio
// routines such as scanf, the following routine will be executed instead
// of the function located in the stdio library.
//
// The function checks the UART global variable to determine which UART to
// use to receive a character.
//
// The routine waits for RI0/RI1 to be set, indicating that a byte has
// been received across the UART0/UART1 RX line.  The routine saves the
// received character into a local variable, clears the RI0/RI1 interrupt
// flag, and returns the received character value.
//
//-----------------------------------------------------------------------------
char _getkey (void)
{
   char c;

   if (UART == 0)
   {
      while (!RI0);                    // Wait until UART0 receives a character
      c = SBUF0;                       // Save character to local variable
      RI0 = 0;                         // Clear UART0 receive interrupt flag
      return (c);                      // Return value received through UART0
   }
   else if (UART == 1)
   {
      while (!(SCON1 & 0x01));         // Wait until UART1 receives a character
      c = SBUF1;                       // Save character to local variable
      SCON1 &= ~0x01;                  // Clear UART1 receive interrupt flag
      return (c);                      // Return value received through UART1
   }
}

//-----------------------------------------------------------------------------
// Delay
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : none
//
// Used for a small pause of approximately 40 us.
//
//-----------------------------------------------------------------------------
void Delay(void)
{
   int x;
   for(x = 0; x < 500; x)
      x++;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
