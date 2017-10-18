//-----------------------------------------------------------------------------
// F04x_ADC0_ExternalInput_Mux.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This code example illustrates using the internal analog multiplexer to
// measure analog voltages on up to 8 different analog inputs.  Results are
// printed to a PC terminal program via the UART.
//
// The inputs are sequentially scanned, beginning with input 0 (AIN0.0), up
// to input number <ANALOG_INPUTS>-1 (maximum ANALOG_INPUTS = 8, which will
// scan all analog inputs AIN0.0 - AIN0.7).
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
// ISR:  Timer 2                   ^                                 ^                                 ^
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
// Timer 2 is set to change the mux input and start a conversion every 20uS.
//
// General:
// --------
//
// The system is clocked using the internal 24.5MHz oscillator. Results are
// printed to the UART from a loop with the rate set by a delay based on Timer 2.
// This loop periodically reads the ADC value from a global array, Result.
//
// The ADC makes repeated measurements at 20uS intervals based on Timer 2.
// The end of each ADC conversion initiates an interrupt which calls an
// averaging function.  <INT_DEC> samples are averaged then the Result
// values updated.
//
// For each power of 4 of <INT_DEC>, you gain 1 bit of effective resolution.
// For example, <INT_DEC> = 256 gain you 4 bits of resolution: 4^4 = 256.
//
// The ADC input multiplexer is set for a single-ended input.  The example
// sequentially scans through the inputs, starting at AIN0.0.  <ANALOG_INPUTS>
// inputs are read.  The amplifier is set for unity gain so a voltage range of
// 0 to Vref (2.43V) may be measured.  Although voltages up to Vdd may be
// applied without damaging the device, only the range 0 to Vref may be
// measured by the ADC.  The input is available at the 8-position board-edge
// connector, J20, on the C8051FX20-TB.
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
// 1) Download code to a 'F04x target board
// 2) Connect serial cable from the transceiver to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 4) Connect a variable voltage source (between 0 and Vref)
//    to AIN0.0 - AIN0.3, or a potentiometer voltage divider as shown above.
//    These input are available on the J20 and J11 headers
// 5) HyperTerminal will print the voltages measured by the device if
//    everything is working properly.  Note that some of the analog inputs are
//    floating and will return nonzero values.
//
// Target:         C8051F04x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision SM
//    -15 JUN 2007
//


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f040.h>                 // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F04x
//-----------------------------------------------------------------------------

sfr16 DP       = 0x82;                 // data pointer
sfr16 RCAP2    = 0xCA;                 // Timer2 reload/capture value
sfr16 RCAP3    = 0xCA;                 // Timer3 reload/capture value
sfr16 RCAP4    = 0xCA;                 // Timer4 reload/capture value
sfr16 TMR2     = 0xCC;                 // Timer2 counter/timer
sfr16 TMR3     = 0xCC;                 // Timer3 counter/timer
sfr16 TMR4     = 0xCC;                 // Timer4 counter/timer
sfr16 ADC0     = 0xBE;                 // ADC0 data
sfr16 ADC0GT   = 0xC4;                 // ADC0 greater than window
sfr16 ADC0LT   = 0xC6;                 // ADC0 less than window
sfr16 DAC0     = 0xD2;                 // DAC0 data
sfr16 DAC1     = 0xD2;                 // DAC1 data
sfr16 CAN0DAT  = 0xD8;                 // CAN data window

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define BAUDRATE     115200            // Baud rate of UART in bps
#define SYSCLK       24500000          // Output of PLL derived from (INTCLK*2)
#define INT_DEC      256               // Integrate and decimate ratio
#define SAR_CLK      2500000           // Desired SAR clock speed

#define SAMPLE_DELAY 2500              // Delay in ms before displaying sample

#define ANALOG_INPUTS 4                // Number of AIN pins to measure
                                       // (min=1, max=8)

sbit LED = P1^6;                       // LED='1' means ON
sbit SW1 = P3^7;                       // SW1='0' means switch pressed


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void UART1_Init (void);
void ADC0_Init (void);
void TIMER2_Init (void);
void ADC0_ISR (void);
void TIMER2_ISR (void);
void Wait_MS (unsigned int ms);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

long Result[ANALOG_INPUTS];            // ADC0 decimated value, one for each
                                       // analog input
unsigned char amux_input=0;            // index of analog MUX inputs
unsigned char amux_convert=0;


//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{

   unsigned char i;
   long measurement;                   // measured voltage in mV

   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init ();                       // Initialize crossbar and GPIO
   UART1_Init ();                      // Initialize UART1

   TIMER2_Init ();                     // Initialize Timer2 to overflow at 1 mS

   ADC0_Init ();                       // Init ADC

   SFRPAGE = ADC0_PAGE;
   AD0EN = 1;                          // Enable ADC

   EA = 1;                             // Enable global interrupts

   while (1)
   {
      EA = 0;                          // Disable interrupts

      SFRPAGE = UART1_PAGE;
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
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal oscillator
// at 24.5 MHz.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // set SFR page

   OSCICN = 0x83;                      // set internal oscillator to run
                                       // at its maximum frequency

   CLKSEL = 0x00;                      // Select the internal osc. as
                                       // the SYSCLK source

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine configures the crossbar and GPIO ports.
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // set SFR page

   XBR0     = 0x00;
   XBR1     = 0x00;
   XBR2     = 0x44;                    // Enable crossbar and weak pull-up
                                       // Enable UART1

   P0MDOUT |= 0x01;                    // Set TX1 pin to push-pull
   P1MDOUT |= 0x40;                    // Set P1.6(LED) to push-pull

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// UART1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART1 using Timer1, for <baudrate> and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART1_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = UART1_PAGE;
   SCON1   = 0x10;                     // SCON1: mode 0, 8-bit UART, enable RX

   SFRPAGE = TIMER01_PAGE;
   TMOD   &= ~0xF0;
   TMOD   |=  0x20;                    // TMOD: timer 1, mode 2, 8-bit reload


   if (SYSCLK/BAUDRATE/2/256 < 1) {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON |= 0x10;                   // T1M = 1; SCA1:0 = xx
   } else if (SYSCLK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x13;                  // Clear all T1 related bits
      CKCON |=  0x01;                  // T1M = 0; SCA1:0 = 01
   } else if (SYSCLK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x13;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x13;                  // Clear all T1 related bits
      CKCON |=  0x02;                  // T1M = 0; SCA1:0 = 10
   }

   TL1 = TH1;                          // initialize Timer1
   TR1 = 1;                            // start Timer1

   SFRPAGE = UART1_PAGE;
   TI1 = 1;                            // Indicate TX1 ready

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page

}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure ADC0 to use Timer2 overflows as conversion source, to
// generate an interrupt on conversion complete, and to use left-justified
// output mode.  Enables ADC end of conversion interrupt. Leaves ADC disabled.
//
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = ADC0_PAGE;

   ADC0CN = 0x0C;                      // ADC0 disabled; normal tracking
                                       // mode; ADC0 conversions are initiated
                                       // on overflow of Timer2; ADC0 data is
                                       // right-justified, low power tracking
                                       // mode

   REF0CN = 0x03;                      // Enable on-chip VREF and output buffer

   AMX0CF = 0x00;                      // AIN inputs are single-ended (default)

   AMX0SL = 0x00;                      // Select AIN0.0 pin as ADC mux input
                                       // ISR will change this to step through
                                       // inputs

   ADC0CF = (SYSCLK/SAR_CLK) << 3;     // ADC conversion clock = 2.5MHz
   ADC0CF |= 0x00;                     // PGA gain = 1 (default)

   EIE2 |= 0x02;                       // enable ADC interrupts

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// TIMER2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer2 to auto-reload at 20uS rate using SYSCLK as its timebase
//
//-----------------------------------------------------------------------------
void TIMER2_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TMR2_PAGE;

   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2, select
                                       // auto-reload;
   TMR2CF = 0x08;                      // use SYSCLK as timebase, count down
   RCAP2   = 65536 -(SYSCLK / 24500);  // Init reload values for 20uS
   TMR2    = RCAP2;                    // Set to reload immediately
   TR2     = 1;                        // start Timer2
   ET2    = 1;                               // enable timer 2 interrupt
   //IE |= 0x20;                       // enable timer 2 interrupt

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
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

   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = ADC0_PAGE;
   AD0INT = 0;                         //clear ADC conversion complete overflow

   accumulator[amux_convert] += ADC0;  // Read ADC value and add to running
                                       // total

   if(amux_convert == (ANALOG_INPUTS-1))// reset input index if the last input
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

   amux_convert = amux_input;          // now that conversion results are
                                       // stored, advance index to the analog
                                       // input currently selected on the mux

   LED = 1;

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// TIMER2_ISR
//-----------------------------------------------------------------------------
//
// The timer2 overflow triggers the ADC0 conversion on the analog MUX input
// previously selected.  It is permissable to change the analog MUX
// input once conversion has started, as the ADC has an internal sample
// and hold.
//
// This ISR routine will then select the next analog MUX input so as to
// maximize the settling time.
//
//-----------------------------------------------------------------------------

void TIMER2_ISR(void) interrupt 5
{
   SFRPAGE = TMR2_PAGE;
   TF2 = 0;

   amux_input ++;                      // step to the next analog mux input

   if(amux_input == ANALOG_INPUTS)     // reset input index if the last input
      {                                // was just read
      amux_input=0;                    // reset input index back to AIN0.0
      }

   SFRPAGE = ADC0_PAGE;
   AMX0SL = amux_input;                // select the next input on the analog
                                       // multiplexer

   LED = 0;
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// wait_ms
//-----------------------------------------------------------------------------
//
// This routine inserts a delay of <ms> milliseconds.
//
void Wait_MS(unsigned int ms)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TMR3_PAGE;

   TMR3CN = 0x00;                      // Stop Timer3; Clear TF3;
   TMR3CF = 0x00;                      // use SYSCLK/12 as timebase

   RCAP3 = -(SYSCLK/1000/12);          // Timer 3 overflows at 1 kHz
   TMR3 = RCAP3;

   TR3 = 1;                            // Start Timer 3

   while(ms)
   {
      TF3 = 0;                         // Clear flag to initialize
      while(!TF3);                     // Wait until timer overflows
      ms--;                            // Decrement ms
   }

   TR3 = 0;                            // Stop Timer 3

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
