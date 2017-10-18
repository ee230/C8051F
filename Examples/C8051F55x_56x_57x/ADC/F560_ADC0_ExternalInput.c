//-----------------------------------------------------------------------------
// F560_ADC0_ExternalInput.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// --------------------
//
// This example code takes and averages 2048 analog measurements from input
// P1.2 using ADC0, then prints the results to a terminal window via the UART.
//
// The system is clocked by the internal 24 MHz oscillator.  Timer 2 triggers
// a conversion on ADC0 on each overflow.  The completion of this conversion
// in turn triggers an interrupt service routine (ISR).  The ISR averages
// 2048 measurements, then prints the value to the terminal via printf before
// starting another average cycle.
//
// The analog multiplexer selects P1.2 as the ADC0 input.  The C8051F50x
// ADC supports single-ended inputs and so the voltage on P1.2 is compared to
// GND.
//
// P1.2 on the C8051F560-TB is connected to a potentiometer.  When the
// potentiometer is set to the maximum setting, the voltage input is VIO,
// which is 5V.  In order to measure this signal using the on-chip 2.25V
// voltage reference, the on-chip Gain module is used to scale the signal.
//
// Terminal output is done via printf, which directs the characters to
// UART0.  A UART initialization routine is therefore necessary.
//
// F560 Resources:
// ---------------
// ADC0  : Uses on-chip VREF
// Timer2: overflow initiates ADC conversion
//
// How To Test:
// ------------
// 1) Download code to a 'F560 device on a C8051F560-TB development board
// 2) Connect USB cable from the development board to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the USB port (virtual com port) at <BAUDRATE>, 8 data bits, no parity,
//    1 stop bit and no flow control.
// 4) Ensure that a shorting block is put on J20 (potentiometer) and J22 (VREF)
// 5) HyperTerminal will print the voltage measured by the device if
//    everything is working properly
//
//
// Target:         C8051F568 (Side A of a C8051F560-TB)
// Tool chain:     Raisonance / Keil / SDCC
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0 / 15 JAN 2009 (GP)
//    -Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F560_defs.h>            // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24000000          // SYSCLK frequency in Hz
#define BAUDRATE       115200          // Baud rate of UART in bps

SBIT (LED, SFR_P1, 3);                 // LED==1 means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void PORT_Init (void);
void TIMER2_Init (void);
void ADC0_Init (void);
void UART0_Init (void);

INTERRUPT_PROTO (ADC_ISR, INTERRUPT_ADC0_EOC);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   SFRPAGE = ACTIVE_PAGE;              // Set for PCA0MD

   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   SYSCLK_Init ();                     // Initialize system clock
   PORT_Init ();                       // Initialize crossbar and GPIO
   TIMER2_Init ();                     // Init Timer2 for ADC conversion clock
   UART0_Init ();                      // Initialize UART0 for printf's
   ADC0_Init ();                       // Initialize ADC0

   EA = 1;                             // Enable global interrupts
   while (1);                          // Spin forever
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// This routine initializes the system clock to use the internal 24 MHz
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   OSCICN = 0xC7;                      // Configure internal oscillator for
                                       // maximum frequency

   RSTSRC = 0x04;                      // enable missing clock detector

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configure the Crossbar and GPIO ports.
//
// P0.0 - analog               VREF
// P0.4 - digital  push-pull   UART TX
// P0.5 - digital  open-drain  UART RX
//
// P1.2 - analog               ADC0 analog
// P1.3 - digital  push-pull   LED
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   P0SKIP |= 0x01;                     // Skip P0.0 (VREF)
   P1SKIP |= 0x04;                     // Skip P1.2 (ADC input)

   P0MDOUT |= 0x10;                    // Set TX pin to push-pull
   P0MDIN  &= ~0x01;                   // Set VREF to analog

   P1MDOUT |= 0x08;                    // Enable LED as a push-pull output
   P1MDIN  &= ~0x04;                   // Set P1.2 as an analog input

   XBR0     = 0x01;                    // Enable UART0
   XBR2     = 0x40;                    // Enable crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// TIMER2_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at 100uS
// intervals.  Timer 2 overflow automatically triggers ADC0 conversion.
//
//-----------------------------------------------------------------------------
void TIMER2_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = ACTIVE_PAGE;

   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;
                                       // use SYSCLK as timebase, 16-bit
                                       // auto-reload
   CKCON  |= 0x10;                     // Select SYSCLK for Timer 2 source
   TMR2RL  = 65535 - (SYSCLK / 10000); // Init reload value for 10uS
   TMR2    = 0xFFFF;                   // Set to reload immediately
   TR2     = 1;                        // Start Timer2

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configures ADC0 to make single-ended analog measurements on pin P1.2
// Also enabled the gain and the voltage reference
//
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = ACTIVE_PAGE;

   // Initialize the Gain to account for a 5V input and 2.25 VREF
   // Solve the equation provided in Section 9.3.1 of the Datasheet

   // The 5V input is scaled by a factor of 0.44 so that the maximum input
   // voltage seen by the pin is 2.2V

   // 0.44 = (GAIN/4096) + GAINADD * (1/64)

   // Set GAIN to 0x6CA and GAINADD to 1
   // GAIN = is the 12-bit word formed by ADC0GNH[7:0] ADC0GNL[7:4]
   // GAINADD is bit ADC0GNA.0

   ADC0CF |= 0x01;                     // Set GAINEN = 1
   ADC0H   = 0x04;                     // Load the ADC0GNH address
   ADC0L   = 0x6C;                     // Load the upper byte of 0x6CA to
                                       // ADC0GNH
   ADC0H   = 0x07;                     // Load the ADC0GNL address
   ADC0L   = 0xA0;                     // Load the lower nibble of 0x6CA to
                                       // ADC0GNL
   ADC0H   = 0x08;                     // Load the ADC0GNA address
   ADC0L   = 0x01;                     // Set the GAINADD bit
   ADC0CF &= ~0x01;                    // Set GAINEN = 0

   ADC0CN = 0x03;                      // ADC0 disabled, normal tracking,
                                       // conversion triggered on TMR2 overflow
                                       // Output is right-justified

   REF0CN = 0x33;                      // Enable on-chip VREF and buffer
                                       // Set voltage reference to 2.25V

   ADC0MX = 0x0A;                      // Set ADC input to P1.2

   ADC0CF = ((SYSCLK / 3000000) - 1) << 3;   // Set SAR clock to 3MHz

   EIE1 |= 0x04;                       // Enable ADC0 conversion complete int.

   AD0EN = 1;                          // Enable ADC0

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1, for <BAUDRATE> and 8-N-1.
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       // clear RI0 and TI0 bits

 // Baud Rate = [BRG Clock / (65536 - (SBRLH0:SBRLL0))] x 1/2 x 1 / Prescaler

#if   ((SYSCLK / BAUDRATE / 2 / 0xFFFF) < 1)
      SBRL0 = -(SYSCLK / BAUDRATE / 2);
      SBCON0 |= 0x03;                  // Set prescaler to 1
#elif ((SYSCLK / BAUDRATE / 2 / 0xFFFF) < 4)
      SBRL0 = -(SYSCLK / BAUDRATE / 2 / 4);
      SBCON0 &= ~0x03;
      SBCON0 |= 0x01;                  // Set prescaler to 4
#elif ((SYSCLK / BAUDRATE / 2 / 0xFFFF) < 12)
      SBRL0 = -(SYSCLK / BAUDRATE / 2 / 12);
      SBCON0 &= ~0x03;                 // Set prescaler to 12
#else
      SBRL0 = -(SYSCLK / BAUDRATE / 2 / 48);
      SBCON0 &= ~0x03;
      SBCON0 |= 0x02;                  // Set prescaler to 48
#endif

   SBCON0 |= 0x40;                     // Enable baud rate generator
   TI0 = 1;                            // Indicate TX0 ready

   SFRPAGE = SFRPAGE_save;
}

#ifdef SDCC

// SDCC does not include a definition for putchar(), which is used in printf()
// and so it is defined here.  The prototype does not need to be explicitly
// defined because it is provided in stdio.h

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : character to send to UART
//
// This function outputs a character to the UART.
//-----------------------------------------------------------------------------
void putchar (char output)
{
   if (output == '\n')
   {
      while (!TI0);
      TI0 = 0;
      SBUF0 = 0x0D;
   }
   while (!TI0);
   TI0 = 0;
   SBUF0 = output;
}

#endif

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// ADC0_ISR
//-----------------------------------------------------------------------------
//
// This ISR averages 2048 samples then prints the result to the terminal.  The
// ISR is called after each ADC conversion which is triggered by Timer2.
//
//-----------------------------------------------------------------------------
INTERRUPT (ADC_ISR, INTERRUPT_ADC0_EOC)
{

   static U32 accumulator = 0;         // Accumulator for averaging
   static U16 measurements = 2048;     // Measurement counter
   U32 result = 0;
   U32 mV;                             // Measured voltage in mV

   AD0INT = 0;                         // Clear ADC0 conv. complete flag

   accumulator += ADC0;                // Add most recent sample
   measurements--;                     // Subtract counter

   if (measurements == 0)
   {
      measurements = 2048;
      result = accumulator / 2048;
      accumulator = 0;

      // The 12-bit ADC value is averaged across 2048 measurements.
      // The measured voltage applied to P1.2 is then:
      //
      //                           Vref (mV)
      //   measurement (mV) =   --------------- * result (bits)
      //                       (2^12)-1 (bits)
      //
      // Then multiply the result by 2.27 to account for the 0.44 gain
      // applied earlier

      mV =  result * 2400 / 4095 * 227 / 100;

      // SFR page is already correct for printf (SBUF0 page)
      printf("P1.2 voltage: %ld mV\n", mV);
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
