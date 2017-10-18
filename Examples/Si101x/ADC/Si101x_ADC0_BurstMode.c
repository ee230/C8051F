//-----------------------------------------------------------------------------
// Si101x_ADC0_BurstMode.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// --------------------
//
// This example code uses the Burst Mode with Auto-Averaging capability to 
// generate 12-bit samples from the ADC. No CPU intervention is required 
// to generate the 12-bit samples. Measurements are taken from P0.6
// (Potentiometer input) configured as an analog input and printed to a
// terminal window via the UART.
//
// The system is clocked by the internal 24.5MHz oscillator.  Timer 2 triggers
// a burst of 16 ADC0 conversions on each overflow.  The completion of 
// these conversions triggers an interrupt service routine (ISR). 
//
// When J16 is shorted, the potentiometer (R14) wiper is connected to P0.6.
// When J16 is open, P0.6 may also be supplied with a voltage from the H1 
// Terminal block. 
//
// ADC Settling Time Requirements, Sampling Rate:
// ----------------------------------------------
//
// The total sample time per input is comprised of an input setting time
// (Tsettle), followed by a conversion time (Tconvert):
//
// Tsample  = Tsettle + Tconvert
//
// |--------Settling-------|==Conversion==|----Settling--- . . .
// Timer 2 overflow        ^
// ADC0 ISR                               ^
//
// The ADC input voltage must be allowed adequate time to settle before the
// conversion is made.  This settling depends on the external source
// impedance, internal mux impedance, and internal capacitance.
// Settling time is given by:
//
//                   | 2^n |
//    Tsettle =   ln | --- | * Rtotal * Csample
//                   | SA  |
//
// In this application, assume a 100kohm potentiometer as the voltage divider.
// The expression evaluates to:
//
//                   | 2^10 |
//    Tsettle =   ln | ---- | * 105e3 * 5e-12 = 4.4uS
//                   | 0.25 |
//
// In addition, one must allow at least 1.7uS after changing analog mux
// inputs or PGA settings.  The settling time in this example, then, is
// dictated by the large external source resistance.
//
// The conversion is 10 periods of the SAR clock <SAR_CLK>.  At 8.33 MHz,
// this time is 10 * 120nS = 1.2 uS.
//
//
// Tsample, minimum  = Tsettle + Tconvert
//                   = 4.4uS + 1.2uS
//                   = 5.6 uS
//
// Timer 2 is set to start a burst of 16 conversions every 10 ms (100 Hz).
// Results are printed to the screen every 10 samples (10 Hz)
//
// Resources:
// ---------------
// Timer1: clocks UART
// Timer2: overflow initiates ADC conversion
//
// How To Test:
// ------------
// 1) Download code to a  device on a  development board
// 2) Ensure that jumpers are placed on J12 of the target board
//    that connect P0.4/TX to the RXD signal, P0.5/RX to the TXD signal,
//    and VDD/DC+ to VIO.
// 3) Connect a USB cable from the development board to a PC
// 4) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the USB port (virtual com port) at <BAUDRATE> and 8-N-1
// 5) Connect a variable voltage source to P0.6 or verify that J15 and J16 are
//    installed. J15 should be connected to the side labeled GND.
// 6) HyperTerminal will print the voltage measured by the device if
//    everything is working properly
//
// Target:         Si101x
// Tool chain:     Generic
// Command Line:   None
//
//
// Release 1.0
//    - Initial Revision (FB)
//    - 18 MAY 2010

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1010_defs.h>               // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000          // SYSCLK frequency in Hz
#define BAUDRATE     230400            // Baud rate of UART in bps

#define SAMPLERATE   100               // ADC word rate in Hz

SBIT (R15_ENABLE, SFR_P1, 4);          // Open-Drain output. Write '0' to 
                                       // enable the potentiometer.

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void PORT_Init (void);
void Timer2_Init(void);
void ADC0_Init(void);
void UART0_Init (void);

INTERRUPT_PROTO (ADC_ISR, INTERRUPT_ADC0_EOC);

//-----------------------------------------------------------------------------
// Generic UART definitions to support both Keil and SDCC
//-----------------------------------------------------------------------------

#ifdef SDCC
#include <sdcc_stdio.h>
#endif

char *GETS (char *, unsigned int);

#ifdef __C51__
#define GETS gets
#endif

#ifdef SDCC
#include <sdcc_stdio.c>
#endif


//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)
   
   PORT_Init ();                       // Initialize crossbar and GPIO
   SYSCLK_Init ();                     // Initialize system clock to
                                       // 24.5MHz
   
   Timer2_Init();                      // Init Timer2 to generate
                                       // overflows to trigger ADC
   UART0_Init();                       // Initialize UART0 for printf's
   ADC0_Init();                        // Initialize ADC0
   
   R15_ENABLE = 0;                     // Enable the Potentiometer

   EA = 1;                             // enable global interrupts
   while (1) {                         // spin forever
   }
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
// This routine initializes the system clock to use the internal 24.5MHz / 8
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   OSCICN |= 0x80;                     // Enable the precision internal osc.
   
   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor enabled.

   CLKSEL = 0x00;                      // Select precision internal osc. 
                                       // divided by 1 as the system clock
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
// P0.4 - UART TX (push-pull)
// P0.5 - UART RX
//
// P0.6 - ADC0 analog input
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   // Analog Input
   P0MDIN   &= ~0x40;                  // Set P0.6 as an analog input
   P0MDOUT  &= ~0x40;                  // Set P0.6 to open-drain
   P0       |=  0x40;                  // Set P0.6 latch to '1'
   P0SKIP   |=  0x40;                  // Skip P0.6 in the Crossbar
   
   // UART TX 
   P0MDOUT |= 0x10;                    // Set TX pin to push-pull
   
   // Crossbar Configuration
   XBR0     = 0x01;                    // Enable UART0
   XBR2     = 0x40;                    // Enable crossbar and weak pull-ups
      
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at 100uS
// intervals.  Timer 2 overflow automatically triggers ADC0 conversion.
//
//-----------------------------------------------------------------------------
void Timer2_Init (void)
{
   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;
   CKCON  &= ~0x30;                    // use SYSCLK/12 as timebase, 
                                       // 16-bit auto-reload

   TMR2RL  = 65535 - (SYSCLK / 12 / SAMPLERATE); // init reload value 
   TMR2    = TMR2RL;                   // init timer value
   TR2     = 1;                        // start Timer2
}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configures ADC0 to make single-ended analog measurements on pin P0.6
//
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   ADC0CN = 0x42;                      // ADC0 disabled, Burst Mode enabled,
                                       // conversion triggered on TMR2 overflow
  
   REF0CN = 0x18;                      // Select internal high speed voltage
                                       // reference

   ADC0MX = 0x06;                      // Select P0.6 as the ADC input pin

   ADC0CF = ((SYSCLK/8300000))<<3;     // Set SAR clock to 8.3MHz

   ADC0CF |= 0x00;                     // Select Gain of 0.5
   
   ADC0AC = 0x13;                      // Right-justify results, shifted right
                                       // by 2 bits. Accumulate 16 samples for
                                       // an output word of 12-bits.

   EIE1 |= 0x08;                       // Enable ADC0 conversion complete int.

   
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
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   #if (SYSCLK/BAUDRATE/2/256 < 1) 
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   #elif (SYSCLK/BAUDRATE/2/256 < 4) 
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   #elif (SYSCLK/BAUDRATE/2/256 < 12) 
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   #else 
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   #endif

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
// ADC0_ISR
//-----------------------------------------------------------------------------
//
// This ISR averages 2048 samples then prints the result to the terminal.  The
// ISR is called after each ADC conversion which is triggered by Timer2.
//
//-----------------------------------------------------------------------------
INTERRUPT (ADC_ISR, INTERRUPT_ADC0_EOC)
{

   unsigned long result;
   unsigned long mV;                   // measured voltage in mV
   static unsigned char sample_count = 0;   

   AD0INT = 0;                         // clear ADC0 conv. complete flag
   
   result = ADC0;                      // copy ADC0 into result
 
   // The 12-bit ADC value represents the voltage applied
   // to P0.6 according to the following equation:
   //
   //                           Vref (mV)
   //   measurement (mV) =   --------------- * result (bits)
   //                       ((2^10)-1)*2^2 (bits)

   mV =  result * 3300 / 4092;
   
   // Print the results every 10 samples
   if(sample_count >= 10)
   {
      printf("\f\nP0.6 voltage: %d mV", (unsigned int) mV);
      sample_count = 0;
   
   } else
   {
      sample_count++;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
