//-----------------------------------------------------------------------------
// T620_ADC0_WindowInterrupt.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// --------------------
//
// This example will interrupt if the potentiometer ADC result is below ADCMIN
// or above ADCMAX.  Text sent to the UART indicates when the ADC is outside
// the allowed range and indicates the measured ADC result.
//
// How To Test:
// ------------
// 1) Download code to a 'T620 device on a 'T62x mother board.
// 2) Connect the USB cable from the PC to the mother board (the USB ACTIVE
//    LED should light up if the CP210x drivers are properly installed).
// 3) Ensure the UART jumpers are populated on J11 between P0.4 and RX_COMM and
//    P0.5 and TX_COMM.
// 4) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1.
// 5) Connect the potentiometer to P2.5 by populating J12.
// 6) Populate J14 to connect the capacitors to VREF.
// 7) Run the code.
// 8) HyperTerminal will print the voltage measured by the device if
//    everything is working properly.
//
// Target:         C8051T620/1 or C8051T320/1/2/3
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
// Release 1.0
//    -Initial Version (TP)
//    -30 JUN 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"            // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       12000000          // SYSCLK frequency in Hz
#define BAUDRATE     115200            // Baud rate of UART in bps

#define ADCMAX       (1024*2)/3        // The Window Comparator interrupt
#define ADCMIN       (1024*1)/3        // will trigger if the ADC result is
                                       // not in the middle 1/3 range

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void ADC0_Init(void);
void UART0_Init (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init ();                 // Initialize system clock to
                                       // 12 MHz
   Port_Init ();                       // Initialize crossbar and GPIO
   UART0_Init ();                      // Initialize UART0 for printf's
   ADC0_Init ();                       // Initialize ADC0

   EA = 1;                             // Enable global interrupts

   while (1)                           // Continuously take ADC conversions
   {
      AD0INT = 0;
      AD0BUSY = 1;
      while (AD0INT != 1);
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
// This function initializes the system clock to use the internal oscillator
// at 12 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x83;                      // Set the internal oscillator to
                                       // 12 MHz
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configure the Crossbar and GPIO ports.
//
// P0.4 - UART TX (push-pull)
// P0.5 - UART RX
//
// P0.7 - VREF (analog)
//
// P2.5 - Potentiometer ADC0 analog input
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDOUT |= 0x10;                    // Set TX pin to push-pull

   P0MDIN &= ~0x80;                    // Set P0.7 as an analog pin
   P2MDIN &= ~0x20;                    // Set P2.5 as an analog input

   P0SKIP = 0x80;                      // Skip P0.7
   P2SKIP = 0x20;                      // Skip P2.5

   XBR0 = 0x01;                        // Enable UART0
   XBR1 = 0x40;                        // Enable crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configures ADC0 to make single-ended analog measurements on pin P2.5.  Note
// that the ADC conversion complete interrupt is not enabled.  ADC is in dual-
// tracking mode (default).
//
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   ADC0CN = 0x00;                      // ADC0 disabled, normal tracking,
                                       // conversion triggered on TMR2 overflow
   REF0CN = 0x03;                      // Use internal voltage reference (2.2V)

   AMX0P = 0x0D;                      // P2.5 selected as input

   // Set SAR clock to 3MHz
   ADC0CF = ((SYSCLK/3000000)-1)<<3;
   ADC0CF |= 0x00;                     // Right-justify results
   ADC0CF |= 0x01;                     // Gain = 1

   ADC0GT = ADCMAX;                    // Set the Window Comparator values
   ADC0LT = ADCMIN;

   EIE1 |= 0x04;                       // Enable the ADC Window Comparator
                                       // interrupt

   AD0EN = 1;                          // Enable ADC0
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configure the UART0 using Timer1 for <BAUDRATE> and 8-N-1.
//
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
      CKCON |=  0x08;                  // T1M = 1; SCA1:0 = xx
   } else if (SYSCLK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   } else if (SYSCLK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else if (SYSCLK/BAUDRATE/2/256 < 48) {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   } else {
      while (1);                       // Error.  Unsupported baud rate
   }

   TL1 = TH1;                          // Init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
   TI0 = 1;                            // Indicate TX0 ready
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// ADC0_Window_ISR
//-----------------------------------------------------------------------------
//
// This ISR triggers if the ADC result is above ADCMAX or below ADCMIN.
//
//-----------------------------------------------------------------------------
INTERRUPT(ADC0_Window_ISR, INTERRUPT_ADC0_WINDOW)
{
   AD0WINT = 0;                              // Clear ADC0 window flag

   printf("\nADC input outside valid range: 0x%04x", ADC0);
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
