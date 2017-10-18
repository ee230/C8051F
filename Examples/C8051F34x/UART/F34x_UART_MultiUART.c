//-----------------------------------------------------------------------------
// F34x_UART_MultiUART.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
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
// 1) Download code to a 'F34x device that is connected to a UART transceiver
// 2) Connect serial cable from the transceiver to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE0>/<BAUDRATE1> and 8-N-1
// 4) To test UART 0, place jumpers connecting P0.4 to TX and P0.5 to RX on 
//    C8051F340 Target Board header J12.  To test UART 1, run wires from
//    P0.0 on J2 to the TX pin of J12, and P0.1 on J2 to the RX pin of J12.
//
//
// FID:          34X000081  
// Target:       C8051F34x
// Tool chain:   KEIL C51 7.20 / KEIL EVAL C51
//               Silicon Laboratories IDE version 2.72
//
// Release 1.0
//    -Initial Revision (PD)
//    -17 JUL 2006
//    -Initial Release
//
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F340.h>                 // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F34x
//-----------------------------------------------------------------------------

sfr16 SBRL1 = 0xB4;

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       48000000          // SYSCLK frequency in Hz
#define BAUDRATE0     115200           // Baud rate of UART0 in bps
#define BAUDRATE1     115200           // Baud rate of UART1 in bps

sbit LED = P2^2;                       // LED = 1 means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSTEMCLOCK_Init (void);
void PORT_Init (void);
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

void main (void) {
   char input_char;

   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   SYSTEMCLOCK_Init ();                // initialize oscillator
   PORT_Init ();                       // initialize crossbar and GPIO
   UART0_Init ();                      // initialize UART0
   UART1_Init ();                      // initialize UART1

   // transmit example UART0
   UART = 0;   // select UART0
   printf ("Hello, from UART0!\n");


   // transmit example UART1
   UART = 1;                           // select UART1
   printf ("Hello, from UART1!\n");


   // receive example: a '1' turns LED on; a '0' turns LED off.
   // select which UART to receive on by changing the next line of code.

   UART = 1;                           // select UART: 0 = UART0, 1 = UART1

   while (1) {
      input_char = getchar();
      printf (" '%c', 0x%02x\n", (unsigned char) input_char, 
                                 (unsigned) input_char);

      switch (input_char) {
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
// SYSTEMCLOCK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None

// This routine initializes the system clock to use the internal system clock
// routed through the clock multiplier as its clock source.
//
//-----------------------------------------------------------------------------

void SYSTEMCLOCK_Init (void)
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
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None

// Configure the Crossbar and GPIO ports
//
// P0.0     TX1 (UART 1)
// P0.1     RX1 (UART 1)
// P0.2     
// P0.3
// P0.4     TX0 (UART 0)
// P0.5     RX0 (UART 0)
// P0.6
// P0.7

// P1       not used in this example

// P2.0
// P2.1
// P2.2     LED
// P2.3    
// P2.4
// P2.5
// P2.6
// P2.7
//-----------------------------------------------------------------------------

void PORT_Init (void)
{  
   XBR0 = 0x01;                        // route UART 0 to crossbar
   XBR2 = 0x01;                        // route UART 1 to crossbar
   XBR1 = 0x40;                        // enable crossbar
   P0MDOUT |= 0x11;                    // set P0.4 to push-pull output
   P2MDOUT |= 0x04;                    // set LED to push-pull
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1, for baudrate; and 8-N-1.
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

   TL1 = TH1;                          // init Timer1
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
// Configure UART1 for baudrate1 and 8-N-1.
//
//-----------------------------------------------------------------------------

void UART1_Init (void)
{
   SMOD1 = 0x0C;                       // set to disable parity, 8-data bit,
                                       // disable extra bit, 
                                       // stop bit 1 bit wide

   SCON1 = 0x10;                       // SCON1: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits

   if (SYSCLK/BAUDRATE1/2/0xFFFF < 1) {
      SBRL1 = -(SYSCLK/BAUDRATE1/2);
      SBCON1 |= 0x03;                  // set prescaler to 1
   } else if (SYSCLK/BAUDRATE1/2/0xFFFF < 4) {
      SBRL1 = -(SYSCLK/BAUDRATE1/2/4);
      SBCON1 &= ~0x03;
      SBCON1 |= 0x01;                  // set prescaler to 4

   } else if (SYSCLK/BAUDRATE1/2/0xFFFF < 12) {
      SBRL1 = -(SYSCLK/BAUDRATE1/2/12);
      SBCON1 &= ~0x03;                 // set prescaler to 12
   } else {
      SBRL1 = -(SYSCLK/BAUDRATE1/2/48);
      SBCON1 &= ~0x03;
      SBCON1 |= 0x02;                  // set prescaler to 4
   }

   SCON1 |= 0x02;                      // indicate ready for TX
   SBCON1 |= 0x40;                     // enable baud rate generator
}


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

char putchar (char c)  {

   if (UART == 0) {

      if (c == '\n')  {                // check for newline character
         while (!TI0);                 // wait until UART0 is ready to transmit
         TI0 = 0;                      // clear interrupt flag
         SBUF0 = 0x0d;                 // output carriage return command
      }
      while (!TI0);                    // wait until UART0 is ready to transmit
      TI0 = 0;                         // clear interrupt flag
      return (SBUF0 = c);              // output <c> using UART 0
   }

   else if (UART == 1) {
      if (c == '\n')  {                // check for newline character
         while (!(SCON1 & 0x02));      // wait until UART1 is ready to transmit
         SCON1 &= ~0x02;               // clear TI1 interrupt flag
         SBUF1 = 0x0d;                 // output carriage return
      }
      while (!(SCON1 & 0x02));         // wait until UART1 is ready to transmit
      SCON1 &= ~0x02;                  // clear TI1 interrupt flag
      return (SBUF1 = c);              // output <c> using UART 1
   }
}

//-----------------------------------------------------------------------------
// _getkey
//-----------------------------------------------------------------------------
//
// Return Value : byte received from UART0/1
// Parameters   : none

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
char _getkey ()  {
  char c;

  if (UART == 0) {
    while (!RI0);                      // wait until UART0 receives a character
    c = SBUF0;                         // save character to local variable
    RI0 = 0;                           // clear UART0 receive interrupt flag
    return (c);                        // return value received through UART0
  }

  else if (UART == 1) {
    while (!(SCON1 & 0x01));           // wait until UART1 receives a character
    c = SBUF1;                         // save character to local variable
    SCON1 &= ~0x01;                    // clear UART1 receive interrupt flag
    return (c);                        // return value received through UART1
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
   for(x = 0;x < 500;x)
      x++;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
