//-----------------------------------------------------------------------------
// F0xx_ADC0_ExternalInput_Mux.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This code example illustrates using the internal analog multiplexer to
// measure analog voltages on up to 8 different analog inputs.  Results are
// printed to a PC terminal program via the UART.
//
// The inputs are sequentially scanned, beginning with input 0 (AIN0), up
// to input number <ANALOG_INPUTS>-1 (maximum ANALOG_INPUTS = 8, which will
// scan all analog inputs AIN0 - AIN7).
//
//
// ADC Settling Time Requirements, Sampling Rate:
// ----------------------------------------------
//
// The total sample time per intput is comprised of an input setting time
// (Tsettle), followed by a conversion time (Tconvert):
//
// Tsample  = Tsettle + Tconvert
//
// Settling and conversion times may overlap, as the ADC holds the value once
// conversion begins.  This code example takes advantage of this to increase
// the settling time above the minimum required.  In other words, when
// converting the value from analog input Ain(n), the input mux is switched
// over to the next input Ain(n+1) to begin settling.
//
// |--------Settling Ain(n)--------|=Conversion Ain(n)=|
//                                 |--------Settling Ain(n+1)--------|=Conversion Ain(n+1)=|
//                                                                   |--------Settling Ain(n+2)--------|
// ISR:  Timer 3                   ^                                 ^                                 ^
// ISR:  ADC0                                          ^                                   ^
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
//                   | 2^12 |
//    Tsettle =   ln | ---- | * 105e3 * 10e-12 = 10.2uS
//                   | 0.25 |
//
// In addition, one must allow at least 1.5uS after changing analog mux
// inputs or PGA settings.  The settling time in this example, then, is
// dictated by the large external source resistance.
//
// The conversion is 16 periods of the SAR clock <SAR_CLK>.  At 2.5 MHz,
// this time is 16 * 400nS = 6.4 uS.
//
//
// Tsample, minimum  = Tsettle + Tconvert
//                   = 10.2uS + 6.4uS
//                   = 16.6 uS
//
// Timer 3 is set to change the mux input and start a conversion every 20uS.
//
// General:
// --------
//
// The system is clocked using the external 22.1184.
// Results are printed to the UART from a loop
// with the rate set by a delay based on Timer 3.  This loop periodically reads
// the ADC value from a global array, Result.
//
// The ADC makes repeated measurements at 20uS intervals based on Timer 3.
// The end of each ADC conversion initiates an interrupt which calls an
// averaging function.  <INT_DEC> samples are averaged then the Result
// values updated.
//
// For each power of 4 of <INT_DEC>, you gain 1 bit of effective resolution.
// For example, <INT_DEC> = 256 gain you 4 bits of resolution: 4^4 = 256.
//
// The ADC input multiplexer is set for a single-ended input.  The example
// sequentially scans through the inputs, starting at AIN0.  <ANALOG_INPUTS>
// inputs are read.  The amplifier is set for unity gain so a voltage range of
// 0 to Vref (2.43V) may be measured.  Although voltages up to Vdd may be
// applied without damaging the device, only the range 0 to Vref may be
// measured by the ADC.  The input is available at the 8-position board-edge
// connector, J20, on the C8051F005-TB.
//
// A 100kohm potentiometer may be connected as a voltage divider between
// VREF and AGND as shown below:
//
// ---------
//          |
//       8 o| AGND ----|
//         o| VREF ----|<-|
//         o| AIN0.1   |  |
//         o|    |        |
//         o|     --------
//         o|
//         o|
//       1 o|
//          |
// ---------
//
// How To Test:
//
// 1) Download code to a 'F005 device that is connected to a UART transceiver
// 2) Connect serial cable from the transceiver to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 4) Connect a variable voltage source (between 0 and Vref)
//    to AIN0, or a potentiometer voltage divider as shown above.
// 5) HyperTerminal will print the voltages measured by the device if
//    everything is working properly.  Note that some of the analog inputs are
//    floating and will return nonzero values.
//
// Target:         C8051F005
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (PD)
//    -27-Jul-06
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f000.h>                 // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F00x
//-----------------------------------------------------------------------------

sfr16 DP       = 0x82;                 // Data pointer
sfr16 TMR3RL   = 0x92;                 // Timer3 reload value
sfr16 TMR3     = 0x94;                 // Timer3 counter
sfr16 ADC0     = 0xbe;                 // ADC0 data
sfr16 ADC0GT   = 0xc4;                 // ADC0 greater than window
sfr16 ADC0LT   = 0xc6;                 // ADC0 less than window
sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 T2       = 0xcc;                 // Timer2
sfr16 DAC0     = 0xd2;                 // DAC0 data
sfr16 DAC1     = 0xd5;                 // DAC1 data

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define BAUDRATE     115200            // Baud rate of UART in bps
#define SYSCLK       22118400          // External Crystal frequency
#define INT_DEC      256               // Integrate and decimate ratio
#define SAR_CLK      2500000           // Desired SAR clock speed

#define SAMPLE_DELAY 250               // Delay in ms before displaying sample

#define ANALOG_INPUTS 8                // Number of AIN pins to measure
                                       // (min=1, max=8)

sbit LED = P1^6;                       // LED='1' means ON
sbit SW1 = P3^7;                       // SW1='0' means switch pressed

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void UART0_Init (void);
void ADC0_Init (void);
void TIMER3_Init (void);
void ADC0_ISR (void);
void TIMER2_ISR (void);
void Wait_MS (unsigned int ms);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

long Result[ANALOG_INPUTS];            // ADC0 decimated value, one for each
                                       // analog input
unsigned char amux_input = 0;          // index of analog MUX inputs
unsigned char amux_convert = 0;

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   unsigned char i;
   long measurement;                   // Measured voltage in mV

   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init ();                       // Initialize crossbar and GPIO
   UART0_Init ();                      // Initialize UART1

   TIMER3_Init ();                     // Initialize Timer2 to overflow at 1 mS

   ADC0_Init ();                       // Init ADC

   ADCEN = 1;                          // Enable ADC

   EA = 1;                             // Enable global interrupts

   while (1)
   {
      EA = 0;                          // Disable interrupts

      printf("\f");

      for(i=0; i<ANALOG_INPUTS; i++)
      {
         // The 12-bit ADC value is averaged across INT_DEC measurements.
         // The result is then stored in Result, and is right-justified
         // The measured voltage applied to AIN 0.1 is then:
         //
         //                           Vref (mV)
         //   measurement (mV) =   --------------- * Result (bits)
         //                       (2^12)-1 (bits)

         measurement =  Result[i] * 2430 / 4095;
         printf("AIN0.%bu voltage: %ld\tmV\n",i,measurement);

      }

      EA = 1;                          // Re-enable interrupts
      Wait_MS(SAMPLE_DELAY);           // Wait before displaying new values
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
// This routine initializes the system clock to use an 22.1184 MHz crystal
// as its clock source.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   int i;                              // Delay counter

   OSCXCN = 0x67;                      // Start external oscillator with
                                       // 22.1184 MHz crystal

   for (i=0; i < 256; i++) ;           // XTLVLD blanking interval (>1ms)

   while (!(OSCXCN & 0x80)) ;          // Wait for crystal osc. to settle

   OSCICN = 0x88;                      // Select external oscillator as SYSCLK
                                       // source and enable missing clock
                                       // detector
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
// AIN0   analog                  Analog input (no configuration necessary)
//-----------------------------------------------------------------------------

void PORT_Init (void)
{
   XBR0    = 0x04;                     // Enable UART0
   XBR1    = 0x00;
   XBR2    = 0x40;                     // Enable crossbar and weak pull-ups
   PRT0CF |= 0x01;                     // Enable TX0 as a push-pull output
   PRT1CF |= 0x40;                     // Enable P1.6 (LED) as push-pull output
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART using Timer1, for <baudrate> and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   SCON    = 0x50;                     // SCON: mode 1, 8-bit UART, enable RX
   TMOD    = 0x20;                     // TMOD: timer 1, mode 2, 8-bit reload
   TH1    = -(SYSCLK/BAUDRATE/16);     // Set Timer1 reload value for baudrate
   TR1    = 1;                         // Start Timer1
   CKCON |= 0x10;                      // Timer1 uses SYSCLK as time base
   PCON  |= 0x80;                      // SMOD = 1
   TI     = 1;                         // Indicate TX ready
}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure ADC0 to use Timer3 overflows as conversion source, to
// generate an interrupt on conversion complete, and to use right-justified
// output mode.  Enables ADC end of conversion interrupt. Leaves ADC disabled.
//
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   ADC0CN = 0x04;                      // ADC0 disabled; normal tracking
                                       // mode; ADC0 conversions are initiated
                                       // on overflow of Timer3; ADC0 data is
                                       // right-justified
   REF0CN = 0x07;                      // Enable temp sensor, on-chip VREF,
                                       // and VREF output buffer
   AMX0SL = 0x00;                      // Select AIN0.0 pin as ADC mux input
                                       // ISR will change this to step through
                                       // inputs
   ADC0CF = 0x80;                      // ADC conversion clock = SYSCLK/16
   ADC0CF &= ~0x07;                    // PGA gain = 1

   EIE2 |= 0x02;                       // Enable ADC interrupts
}

//-----------------------------------------------------------------------------
// TIMER3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer3 to auto-reload at interval specified by <counts> (no
// interrupt generated) using SYSCLK as its time base.
//
//-----------------------------------------------------------------------------
void TIMER3_Init (void)
{
   TMR3CN = 0x02;                      // Stop Timer3; Clear TF3;
                                       // use SYSCLK as timebase
   TMR3RL  = 65535 -(SYSCLK / 50000);  // reload at 50 us
   TMR3    = 0xffff;                   // set to reload immediately
   EIE2   |= 0x01;                     // disable Timer3 interrupts
   TMR3CN |= 0x04;                     // start Timer3
}


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ADC0_ISR
//-----------------------------------------------------------------------------
//
// This ISR is called when the ADC0 completes a conversion.  Each value is
// added to a running total <accumulator>, and the local decimation counter
// <int_dec> decremented. When <int_dec> reaches zero, we post the decimated
// result in the global variable <Result[]>.
//
// The analog input is sampled, held, and converted on a Timer2 overflow.  To
// maximize input settling time, the analog mux is also advanced to the next
// input on the Timer2 overflow.  Two different indices are held globally:
//    amux_convert:  index of the analog input undergoing conversion
//    amux_input:    index of the analog input selected in the analog
//                   multiplexer
//
//
//-----------------------------------------------------------------------------
void ADC0_ISR (void) interrupt 15
{

   static unsigned int_dec=INT_DEC;    // Integrate/decimate counter
                                       // we post a new result when
                                       // int_dec = 0

   static long accumulator[ANALOG_INPUTS] ={0L};
                                       // Here's where we integrate the
                                       // ADC samples from input AIN0.0
   unsigned char i;

   ADCINT = 0;                         // clear ADC conversion complete
                                       // indicator

   accumulator[amux_convert] += ADC0;  // Read ADC value and add to running
                                       // total

   if(amux_convert == (ANALOG_INPUTS-1))// Reset input index if the last input
                                       //was just read
   {
      int_dec--;                       // Update decimation counter
                                       // when last of the analog inputs
                                       // sampled
   }

   if (int_dec == 0)                   // If zero, then post result
   {
      int_dec = INT_DEC;               // Reset counter

      for(i=0; i<ANALOG_INPUTS; i++)
      {
         Result[i] = accumulator[i] >> 8; //Copy decimated values into Result
         accumulator[i] = 0L;          // Reset accumulators
      }
   }

   amux_convert = amux_input;          // Now that conversion results are
                                       // stored, advance index to the analog
                                       // input currently selected on the mux

   LED = 1;

}

//-----------------------------------------------------------------------------
// TIMER3_ISR
//-----------------------------------------------------------------------------
//
// The timer3 overflow triggers the ADC0 conversion on the analog MUX input
// previously selected.  It is permissable to change the analog MUX
// input once conversion has started, as the ADC has an internal sample
// and hold.
//
// This ISR routine will then select the next analog MUX input so as to
// maximize the settling time.
//
//-----------------------------------------------------------------------------

void TIMER3_ISR(void) interrupt 14
{

   TMR3CN &= ~0x80;                    // Acknowledge interrupt

   amux_input ++;                      // Step to the next analog mux input

   if(amux_input == ANALOG_INPUTS)     // Reset input index if the last input
   {                                   // was just read
      amux_input=0;                    // reset input index back to AIN0.0
   }

   AMX0SL = amux_input;                // Select the next input on the analog
                                       // multiplexer

   LED = 0;
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Wait_MS
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters:
//   1) unsigned int ms - number of milliseconds of delay
//                        range is full range of integer: 0 to 65335
//
// This routine inserts a delay of <ms> milliseconds.
//
//-----------------------------------------------------------------------------
void Wait_MS(unsigned int ms)
{

   CKCON &= ~0x20;                     // Use SYSCLK/12 as timebase

   RCAP2 = -(SYSCLK/1000/12);          // Timer 2 overflows at 1 kHz
   T2 = RCAP2;

   ET2 = 0;                            // Disable Timer 2 interrupts

   TR2 = 1;                            // Start Timer 2

   while(ms)
   {
      TF2 = 0;                         // Clear flag to initialize
      while(!TF2);                     // Wait until timer overflows
      ms--;                            // Decrement ms
   }

   TR2 = 0;                            // Stop Timer 2

}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------