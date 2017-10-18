//-----------------------------------------------------------------------------
// F35x_ADC0_ExternalInput.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// --------------------
//
// This example code for the C8051F350 takes measurements from input A1N0.2
// using ADC0 then prints the results to a terminal window via the UART.
//
// The system is clocked by the internal 24.5MHz oscillator. The completion of
// this conversion in turn triggers an interrupt service routine (ISR). The ISR
// calculates the ADC0 result into the equivalent mV and then prints the value
// to the terminal via printf before starting another conversion.
//
// The analog multiplexer selects A1N2 as the positive ADC0 input.  This
// port is configured as an analog input in the port initialization routine.
// The negative ADC0 input is connected via mux to ground, which provides
// for a single-ended ADC input.
//
// A 100kohm potentiometer may be connected as a voltage divider between
// VREF and AGND on the terminal strip as shown below:
//
// ---------
//          |
//         o| VREF ----|
//         o| GND   ---|<-|
//         o| AIN2     |  |
//         o|    |        |
//         o|     --------
//         o|
//          |
//----------
// C8051F350-TB
//
// Terminal output is done via printf, which directs the characters to
// UART0.
//
// F350 Delta-Sigma ADC
// --------------------
// Please see Silicon Labs Applicaton Note AN217 for more information
// on the C8051F35x Delta-Sigma ADC.  AN217 can be found on the Applications
// webpage by going to the Silicon Labs Microcontrollers homepage
// (www.silabs.com -> select Microcontrollers under "Products at the top) and
// clicking the gray link on the left.
//
// Direct link:
//   http://www.silabs.com/products/microcontroller/applications.asp
//
// F350 Resources:
// ---------------
// Timer1: clocks UART
//
// How To Test:
// ------------
// 1) Download code to a 'F350 device on a C8051F350-TB development board
// 2) Connect serial cable from the transceiver to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 4) Connect a variable voltage source (between 0 and Vref)
//    to AIN2, or a potentiometer voltage divider as shown above.
// 5) HyperTerminal will print the voltage measured by the device.
//
// FID:            35X000029
// Target:         C8051F350
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (SM / TP)
//    - 8 NOV 2006

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F350.h>                 // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F35x
//-----------------------------------------------------------------------------

sfr16 TMR2RL = 0xCA;                   // Timer2 reload value
sfr16 TMR2 = 0xCC;                     // Timer2 counter
sfr16 ADC0DEC = 0x9A;                  // ADC0 Decimation Ratio Register

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000          // SYSCLK frequency in Hz

#define MDCLK         2457600          // Modulator clock in Hz (ideal is
                                       // (2.4576 MHz)
#define OWR                20          // Desired Output Word Rate in Hz

#define BAUDRATE       115200          // Baud rate of UART in bps

sbit LED = P0^7;                       // LED='1' means ON
sbit SW2 = P1^0;                       // SW2='0' means switch pressed

typedef union LONGDATA{                // Access LONGDATA as an
   unsigned long result;               // unsigned long variable or
   unsigned char Byte[4];              // 4 unsigned byte variables
}LONGDATA;

// With the Keil compiler and union byte addressing:
// [0] = bits 31-24, [1] =  bits 23-16, [2] = bits 15-8, [3] = bits 7-0
#define Byte3 0
#define Byte2 1
#define Byte1 2
#define Byte0 3

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void UART0_Init (void);
void ADC0_Init(void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init();                  // Initialize system clock
   Port_Init();                        // Initialize Crossbar and GPIO
   UART0_Init();                       // Initialize UART0 for printf's
   ADC0_Init();                        // Initialize ADC0

   AD0INT = 0;
   ADC0MD = 0x83;                      // Start continuous conversions
   EA = 1;                             // Enable global interrupts

   while (1) {                         // Spin forever
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
// This routine initializes the system clock to use the internal 24.5MHz
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x83;                      // Configure internal oscillator for
                                       // its lowest frequency
   RSTSRC = 0x04;                      // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the GPIO and the Crossbar
//
// Pinout:
//
//   P0.4 - UART TX (digital, push-pull)
//   P0.5 - UART RX (digital, open-drain)
//
//   AIN0.2 - ADC0 input
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   XBR0 = 0x01;                        // UART0 Selected
   XBR1 = 0x40;                        // Enable crossbar and weak pull-ups
   P0MDOUT |= 0xD0;                    // TX, LEDs = Push-pull
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1, for <BAUDRATE> and 8-N-1.
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
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Initialize the ADC to use the temperature sensor. (non-differential)
//
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   REF0CN |= 0x03;                     // Enable internal Vref
   ADC0CN = 0x00;                      // Gain = 1, Unipolar mode
   ADC0CF = 0x00;                      // Interrupts upon SINC3 filter output
                                       // and uses internal VREF

   ADC0CLK = (SYSCLK/MDCLK)-1;         // Generate MDCLK for modulator.
                                       // Ideally MDCLK = 2.4576MHz

   // Program decimation rate for desired OWR
   ADC0DEC = ((unsigned long) MDCLK / (unsigned long) OWR /
              (unsigned long) 128) - 1;

   ADC0BUF = 0x00;                     // Turn off Input Buffers
   ADC0MUX = 0x28;                     // Select AIN0.2

   ADC0MD = 0x81;                      // Start internal calibration
   while(AD0CALC != 1);                // Wait until calibration is complete

   EIE1   |= 0x08;                     // Enable ADC0 Interrupts
   ADC0MD  = 0x80;                     // Enable the ADC0 (IDLE Mode)
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ADC0_ISR
//-----------------------------------------------------------------------------
//
// This ISR prints the result to the UART. The ISR is called after each ADC
// conversion.
//
//-----------------------------------------------------------------------------
void ADC0_ISR (void) interrupt 10
{
   static LONGDATA rawValue;
   unsigned long mV;

   while(!AD0INT);                     // Wait till conversion complete
   AD0INT = 0;                         // Clear ADC0 conversion complete flag

   // Copy the output value of the ADC
   rawValue.Byte[Byte3] = 0x00;
   rawValue.Byte[Byte2] = (unsigned char)ADC0H;
   rawValue.Byte[Byte1] = (unsigned char)ADC0M;
   rawValue.Byte[Byte0] = (unsigned char)ADC0L;

   //                           Vref (mV)
   //   measurement (mV) =   --------------- * result (bits)
   //                       (2^24)-1 (bits)
   //
   //   measurement (mV) =  result (bits) / ((2^24)-1 (bits) / Vref (mV))
   //
   //
   //   With a Vref (mV) of 2500:
   //
   //   measurement (mV) =  result (bits) / ((2^24)-1 / 2500)
   //
   //   measurement (mV) =  result (bits) / ((2^24)-1 / 2500)
   //
   //   measurement (mV) =  result (bits) / (16777215 / 2500)
   //
   //   measurement (mV) =  result (bits) / (6710)

   mV = rawValue.result / 6710;        // Because of bounds issues, this
                                       // calculation has been manipulated as
                                       // shown above
                                       // (i.e. 2500 (VREF) * 2^24 (ADC result)
                                       // is greater than 2^32)

   printf("AIN0.2 voltage: %4ld mV\n",mV);
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
