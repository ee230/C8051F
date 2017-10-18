//-----------------------------------------------------------------------------
// F340_HIDtoUART.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Stub file for Firmware Template.
//
//
// How To Test:    See Readme.txt
//
//
// FID             
// Target:         C8051F32x/C8051F340
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   HIDtoUART
//
// Release 1.1
//    -All changes by BW
//    -1 SEP 2010
//    -Updated USB0_Init() to write 0x89 to CLKREC instead of 0x80
// Release 1.0
//    -Initial Revision (PD)
//    -04 JUN 2008
//

//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------
#include <c8051f3xx.h>
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Register.h"
#include "F3xx_HIDtoUART.h"
#include <intrins.h>
//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define SYSCLK             24000000    // SYSCLK frequency in Hz

// USB clock selections (SFR CLKSEL)
#define USB_4X_CLOCK       0x00        // Select 4x clock multiplier, for USB
#define USB_INT_OSC_DIV_2  0x10        // Full Speed
#define USB_EXT_OSC        0x20
#define USB_EXT_OSC_DIV_2  0x30
#define USB_EXT_OSC_DIV_3  0x40
#define USB_EXT_OSC_DIV_4  0x50

// System clock selections (SFR CLKSEL)
#define SYS_INT_OSC        0x00        // Select to use internal oscillator
#define SYS_EXT_OSC        0x01        // Select to use an external oscillator
#define SYS_4X_DIV_2       0x02


//-----------------------------------------------------------------------------
// Local Function Prototypes
//-----------------------------------------------------------------------------
// Initialization Routines
void Sysclk_Init (void);               // Initialize the system clock
void Port_Init (void);                 // Configure ports
void Usb0_Init (void);                 // Configure USB core
void Delay (void);                     // Approximately 80 us/1 ms on
                                       // Full/Low Speed
void UART0_Init(void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
#ifndef BAUDRATE_HARDCODED
unsigned long BaudRate;
#endif

// IN_PACKET and OUT_PACKEt buffer bytes immediately before and
// after they are transferred across USB inside the report handlers
unsigned char xdata IN_PACKET[64];
unsigned char xdata OUT_PACKET[64];

// UART_INPUT and UART_OUTPUT buffer bytes immediately before
// and after they are transferred across the UART interface
unsigned char xdata UART_INPUT[UART_INPUT_BUFFERSIZE];
unsigned char xdata UART_OUTPUT[UART_OUTPUT_BUFFERSIZE];

// UART_OUTPUT_OVERFLOW_BOUNDARY holds a calculation showing how
// many bytes can be transmitted out of the buffer in the span of time
// before another USB report is received
// If the buffer size crosses this boundary, USB communication should be
// suspended until the buffer shrinks below the boundary
unsigned char UART_OUTPUT_OVERFLOW_BOUNDARY;

unsigned char USB_OUT_SUSPENDED;       // Flag set when buffer size crosses
                                       // boundary

unsigned char UART_INPUT_SIZE = 0;     // Maintains size of input buffer
unsigned char UART_OUTPUT_SIZE = 0;    // Maintains size of output buffer

unsigned char UART_INPUT_LAST = 0;     // Points to oldest byte received
unsigned char UART_INPUT_FIRST = 0;    // Points to newest byte received

unsigned char UART_OUTPUT_LAST = 0;    // Points to oldest byte received
unsigned char UART_OUTPUT_FIRST = 0;   // Points to newest byte received
unsigned char TX_Ready;                // Flag used to initiate UART transfer


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// UART0 Interrupt Service Routine
//-----------------------------------------------------------------------------
// Routine transmits a byte by pulling LAST byte from UART_OUTPUT buffer.
// Routine receives a byte by pushing it as FIRST byte on UART_INPUT buffer.
//
void UART0_Interrupt (void) interrupt 4
{
//   DEBUG_UART = 1;
   if (RI0 == 1)                       // Received byte flag
   {
      RI0 = 0;                         // Acknowledge flag

      // Note: the code below only works correctly if the UART buffers
      // are 127 bytes (0x7F) in length
      // This allows buffer wrapping to be accomplished by simply
      // clearing the MSB
      UART_INPUT_FIRST++;              // Move pointer
      UART_INPUT_FIRST &= ~0x80;       // Wrap pointer if necessary
                   
      // Save received byte onto buffer
      UART_INPUT[UART_INPUT_FIRST] = SBUF0;

      UART_INPUT_SIZE++;               // Increment buffer size
   }

   if (TI0 == 1)                       // Transmit complete flag
   {
      TI0 = 0;                         // Acknowledge flag
      if (UART_OUTPUT_SIZE != 0)       // If buffer is not empty
      {  
         UART_OUTPUT_LAST++;           // Move buffer pointer

         UART_OUTPUT_LAST &= ~0x80;    // Wrap pointer

         // Transmit byte from buffer
         SBUF0 = UART_OUTPUT[UART_OUTPUT_LAST];
         UART_OUTPUT_SIZE--;           // Decrement buffer size
      }
      else
      {
         TX_Ready = 1;                 // Buffer is empty, signal that
                                       // UART TX is free and ready to
                                       // transmit
      }
   }

}
//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DEFAULT_InitRoutine
//-----------------------------------------------------------------------------
// This function is declared in the header file F3xx_HIDtoUART.h and is
// called in the main(void) function.  It calls initialization routines
// local to this file.
//
//-----------------------------------------------------------------------------
void System_Init (void)
{
   Sysclk_Init ();                     // Initialize oscillator
   Port_Init ();                       // Initialize crossbar and GPIO

#ifndef BAUDRATE_HARDCODED
   BaudRate = 115200;
#endif
   UART0_Init();

}

//-----------------------------------------------------------------------------
// USB_Init
//-----------------------------------------------------------------------------
// USB Initialization performs the following:
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//
//-----------------------------------------------------------------------------
void USB_Init (void)
{

   POLL_WRITE_BYTE (POWER,  0x08);     // Force Asynchronous USB Reset
   POLL_WRITE_BYTE (IN1IE,  0x07);     // Enable Endpoint 0-1 in interrupts
   POLL_WRITE_BYTE (OUT1IE, 0x07);     // Enable Endpoint 0-1 out interrupts
   POLL_WRITE_BYTE (CMIE,   0x07);     // Enable Reset, Resume, and Suspend
                                       // interrupts

   USB0XCN = 0xE0;                     // Enable transceiver; select full speed
   POLL_WRITE_BYTE (CLKREC, 0x89);     // Enable clock recovery, single-step
                                       // mode disabled

   EIE1 |= 0x02;                       // Enable USB0 Interrupts

                                       // Enable USB0 by clearing the USB
   POLL_WRITE_BYTE (POWER, 0x01);      // Inhibit Bit and enable suspend
                                       // detection

}


//-----------------------------------------------------------------------------
// Sysclk_Init(void)
//-----------------------------------------------------------------------------
// This function initializes the system clock and the USB clock.
//
//-----------------------------------------------------------------------------
void Sysclk_Init(void)
{
#ifdef _USB_LOW_SPEED_

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKSEL  = SYS_INT_OSC;              // Select System clock
   CLKSEL |= USB_INT_OSC_DIV_2;        // Select USB clock
#else
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
   CLKSEL  = 0x02;                     // Set sys clock to clkmul / 2
#endif  // _USB_LOW_SPEED_
}

//-----------------------------------------------------------------------------
// Port_Init(void)
//-----------------------------------------------------------------------------
// Port Initialization routine that configures the Crossbar and GPIO ports.
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDOUT |= 0x10;                    // Port 0 pins 0-3 set high impedence
   XBR0     = 0x01;                    // Enable UART output
   XBR1     = 0x40;                    // Enable Crossbar, 1 PCA channel
   P0SKIP = 0xCF;
   P1SKIP = 0xFF;
   P2SKIP = 0x07;
   P2MDOUT = 0x0F;                     // DEBUG
}


//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Configure the UART0 using Timer1, for <BAUDRATE> and 8-N-1.
//

//#ifndef BAUDRATE_HARDCODED
void UART0_Init (void)
{
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   if (SYSTEMCLOCK/BaudRate/2/256 < 1) {
      TH1 = -(SYSTEMCLOCK/BaudRate/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } else if (SYSTEMCLOCK/BaudRate/2/256 < 4) {
      TH1 = -(SYSTEMCLOCK/BaudRate/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01                  
      CKCON |=  0x01;
   } else if (SYSTEMCLOCK/BaudRate/2/256 < 12) {
      TH1 = -(SYSTEMCLOCK/BaudRate/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSTEMCLOCK/BaudRate/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;                       
   TR1 = 1;                            // START Timer1
   TX_Ready = 1;                       // Flag showing that UART can transmit
   IP |= 0x10;                         // Make UART high priority
   ES0 = 1;                            // Enable UART0 interrupts


   // The following code computes the size the UART_INPUT buffer, which 
   // stores bytes received from the USB, can reach before one more
   // USB packet will overflow the buffer.  
   // This is to guard against the case where the UART is transmitting
   // bytes out at a speed that is much lower than the speed at which
   // bytes are received.
   UART_OUTPUT_OVERFLOW_BOUNDARY = UART_OUTPUT_BUFFERSIZE - OUT_DATA_SIZE;

}

//-----------------------------------------------------------------------------
// Helper Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Delay(void)
//-----------------------------------------------------------------------------
// Used for a small pause, approximately 80 us in Full Speed,
// and 1 ms when clock is configured for Low Speed
//
//-----------------------------------------------------------------------------
void Delay (void)
{
   int x;
   for(x = 0;x < 500;x)
      x++;
}
