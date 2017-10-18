//-----------------------------------------------------------------------------
// F02x_ADC0_ExternalInput.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program measures the voltage on an external ADC input and prints the 
// result to a terminal window via the UART.
//
// The system is clocked using the internal 24.5MHz oscillator multiplied
// up to 49MHz by the on-chip PLL.  Results are printed to the UART from a loop
// with the rate set by a delay based on Timer 2.  This loop periodically reads
// the ADC value from a global variable, Result.
//
// The ADC makes repeated measurements at a rate determined by SAMPLE_RATE using
// Timer 3.  The end of each ADC conversion initiates an interrupt which calls an 
// averaging function.  <INT_DEC> samples are averaged then the Result value updated.
//
// For each power of 4 of <INT_DEC>, you gain 1 bit of effective resolution.
// For example, <INT_DEC> = 256 gain you 4 bits of resolution: 4^4 = 256.
// 
// The ADC input multiplexer is set for a single-ended input at AIN0.1.  The input
// amplifier is set for unity gain so a voltage range of 0 to Vref (2.43V) may
// be measured.  Although voltages up to Vdd may be applied without damaging the
// device, only the range 0 to Vref may be measured by the ADC.  
// The input is available at the 8-position board-edge connector, J20, on the 
// C8051FX20-TB.
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
//----------   
//
// How To Test:
//
// 1) Download code to a 'F02x device that is connected to a UART transceiver
// 2) Connect serial cable from the transceiver to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 4) Connect a variable voltage source (between 0 and Vref) 
//    to AIN 0.1, or a potentiometer voltage divider as shown above.
// 5) HyperTerminal will print the voltage measured by the device if
//    everything is working properly
//
// FID:            02X000017
// Target:         C8051F02x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (PD)
//    -18-Jul-06
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f020.h>                 // SFR declarations
#include <stdio.h>                     

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F02x
//-----------------------------------------------------------------------------

sfr16 ADC0     = 0xbe;                 // ADC0 data
sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 RCAP3    = 0x92;                 // Timer3 capture/reload
sfr16 TMR2     = 0xcc;                 // Timer2
sfr16 TMR3     = 0x94;                 // Timer3

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define BAUDRATE     115200            // Baud rate of UART in bps
#define SYSCLK       22118400          // External crystal oscillator frequency
#define SAMPLE_RATE  50000             // Sample frequency in Hz
#define INT_DEC      256               // Integrate and decimate ratio
#define SAR_CLK      2500000           // Desired SAR clock speed

#define SAMPLE_DELAY 50                // Delay in ms before taking sample

sbit LED = P1^6;                       // LED='1' means ON
sbit SW1 = P3^7;                       // SW1='0' means switch pressed

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);           
void PORT_Init (void);
void UART0_Init (void);
void ADC0_Init (void);
void TIMER3_Init (int counts);
void ADC0_ISR (void);
void Wait_MS (unsigned int ms);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

long Result;                           // ADC0 decimated value

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   long measurement;                   // Measured voltage in mV

   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init ();                       // Initialize crossbar and GPIO
   UART0_Init ();                      // Initialize UART1
   TIMER3_Init (SYSCLK/SAMPLE_RATE);   // Initialize Timer3 to overflow at
                                       // sample rate

   ADC0_Init ();                       // Init ADC

   AD0EN = 1;                          // Enable ADC

   EA = 1;                             // Enable global interrupts

   while (1)
   {
      EA = 0;                          // Disable interrupts

      // The 12-bit ADC value is averaged across INT_DEC measurements.  
      // The result is then stored in Result, and is right-justified 
      // The measured voltage applied to AIN 0.1 is then:
      //
      //                           Vref (mV)
      //   measurement (mV) =   --------------- * Result (bits) 
      //                       (2^12)-1 (bits)

      measurement =  Result * 2430 / 4095;

      EA = 1;                          // Re-enable interrupts

      printf("AIN0.1 voltage: %ld mV\n",measurement);

      LED = ~SW1;                      // LED reflects state of switch

      Wait_MS(SAMPLE_DELAY);           // Wait 50 milliseconds before taking
                                       // another sample
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
// This routine initializes the system clock to use an 22.1184MHz crystal
// as its clock source.
//
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   int i;                              // delay counter

   OSCXCN = 0x67;                      // start external oscillator with
                                       // 22.1184MHz crystal

   for (i=0; i < 256; i++) ;           // wait for oscillator to start

   while (!(OSCXCN & 0x80)) ;          // Wait for crystal osc. to settle

   OSCICN = 0x88;                      // select external oscillator as SYSCLK
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
// P1.6   digital   push-pull     LED
// AIN0.1 analog                  Analog input (no configuration necessary)
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   XBR0    = 0x04;                     // Route UART0 to crossbar
   XBR2    |= 0x40;                    // Enable crossbar,  weak pull-ups
   P0MDOUT |= 0x01;                    // enable TX0 as a push-pull output
   P1MDOUT |= 0x40;                    // enable LED as push-pull output


   P0MDOUT |= 0x01;                    // Set TX1 pin to push-pull
   P1MDOUT |= 0x40;                    // Set P1.6(LED) to push-pull

}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART1 using Timer1, for <baudrate> and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   SCON0   = 0x50;                     // SCON0: mode 1, 8-bit UART, enable RX
   TMOD    = 0x20;                     // TMOD: timer 1, mode 2, 8-bit reload
   TH1    = -(SYSCLK/BAUDRATE/16);     // set Timer1 reload value for baudrate
   TR1    = 1;                         // start Timer1
   CKCON |= 0x10;                      // Timer1 uses SYSCLK as time base
   PCON  |= 0x80;                      // SMOD00 = 1
   TI0    = 1;                         // Indicate TX0 ready
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

   AMX0CF = 0x00;                      // AIN inputs are single-ended (default)

   AMX0SL = 0x01;                      // Select AIN0.1 pin as ADC mux input

   ADC0CF = (SYSCLK/SAR_CLK) << 3;     // ADC conversion clock = 2.5MHz
   ADC0CF |= 0x00;                     // PGA gain = 1 (default)

   EIE2 |= 0x02;                       // enable ADC interrupts

}

//-----------------------------------------------------------------------------
// TIMER3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1)  int counts - calculated Timer overflow rate
//                    range is postive range of integer: 0 to 32767
//
// Configure Timer3 to auto-reload at interval specified by <counts> (no
// interrupt generated) using SYSCLK as its time base.
//
//-----------------------------------------------------------------------------
void TIMER3_Init (int counts)
{

   TMR3CN = 0x02;                      // Stop Timer3; Clear TF3; set sysclk
                                       // as timebase

   RCAP3   = -counts;                  // Init reload values
   TMR3    = RCAP3;                    // Set to reload immediately
   EIE2   &= ~0x01;                    // Disable Timer3 interrupts
   TMR3CN |= 0x04;                     // start Timer3

}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ADC0_ISR
//-----------------------------------------------------------------------------
//
// Here we take the ADC0 sample, add it to a running total <accumulator>, and
// decrement our local decimation counter <int_dec>.  When <int_dec> reaches
// zero, we post the decimated result in the global variable <Result>.
//
//-----------------------------------------------------------------------------
void ADC0_ISR (void) interrupt 15
{
   static unsigned int_dec=INT_DEC;    // Integrate/decimate counter
                                       // we post a new result when
                                       // int_dec = 0
   static long accumulator=0L;         // Here's where we integrate the
                                       // ADC samples

   AD0INT = 0;                         // Clear ADC conversion complete
                                       // indicator

   accumulator += ADC0;                // Read ADC value and add to running
                                       // total
   int_dec--;                          // Update decimation counter

   if (int_dec == 0)                   // If zero, then post result
   {
      int_dec = INT_DEC;               // Reset counter
      Result = accumulator >> 8;
      accumulator = 0L;                // Reset accumulator
   }
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

   CKCON &= ~0x20;                     // use SYSCLK/12 as timebase

   RCAP2 = -(SYSCLK/1000/12);          // Timer 2 overflows at 1 kHz
   TMR2 = RCAP2;

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