//-----------------------------------------------------------------------------
// F06x_ADC0_ExternalInput.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program measures the voltage on an external ADC input and prints the 
// result to a terminal window via the UART.
//
// The system is clocked using the internal 24.5MHz oscillator.  
// Results are printed to the UART from a loop with the rate set by a delay 
// based on Timer 2.  This loop periodically reads the ADC value from a global 
// variable, Result.
//
// The ADC makes repeated measurements at a rate determined by SAMPLE_RATE 
// using Timer 3. The end of each ADC conversion initiates an interrupt which 
// calls an averaging function.  
// <INT_DEC> samples are averaged then the Result value updated.
//
// For each power of 4 of <INT_DEC>, you gain 1 bit of effective resolution.
// For example, <INT_DEC> = 256 gain you 4 bits of resolution: 4^4 = 256.
// 
// The ADC input multiplexer is set for a single-ended input at AIN2.1.  
// The input amplifier is set for unity gain so a voltage range of 0 to Vref 
// (2.43V) may be measured.  Although voltages up to Vdd may be applied without
// damaging the device, only the range 0 to Vref may be measured by the ADC.  
//
// A 100kohm potentiometer may be connected as a voltage divider between 
// VREF and AGND as shown below: (minimum value = 12Kohms as the maximum 
// recommended current through the Vref is of 200uA @2.4V)
//
// ---------
//          |        
//       8 o| AGND ----| 
//         o| VREF ----|<-|
//         o| AIN2.1   |  |
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
// 1) Download code to a 'F06x device that is connected to a UART transceiver
// 2) Connect serial cable from the transceiver to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 4) Connect a variable voltage source (between 0 and Vref) 
//    to AIN 2.1, or a potentiometer voltage divider as shown above.
// 5) HyperTerminal will print the voltage measured by the device if
//    everything is working properly
//
// FID:            06X000022
// Target:         C8051F06x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (CG)
//    -8-June-06
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051F060.h>                 // SFR declarations
#include <stdio.h>                     

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F06x
//-----------------------------------------------------------------------------

sfr16 ADC2     = 0xbe;                 // ADC2 data
sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 RCAP3    = 0xca;                 // Timer3 capture/reload
sfr16 TMR2     = 0xcc;                 // Timer2
sfr16 TMR3     = 0xcc;                 // Timer3

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define BAUDRATE     115200            // Baud rate of UART in bps
#define SYSCLK       24500000          // System Clock
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
void UART1_Init (void);
void ADC2_Init (void);
void TIMER3_Init (int counts);
void ADC2_ISR (void);
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
   UART1_Init ();                      // Initialize UART1
   TIMER3_Init (SYSCLK/SAMPLE_RATE);   // Initialize Timer3 to overflow at
                                       // sample rate

   ADC2_Init ();                       // Init ADC

   SFRPAGE = ADC2_PAGE;
   AD2EN = 1;                          // Enable ADC

   EA = 1;                             // Enable global interrupts

   while (1)
   {
      EA = 0;                          // Disable interrupts

      // The 10-bit ADC2 value is averaged across INT_DEC measurements.  
      // The result is then stored in Result, and is right-justified 
      // The measured voltage applied to AIN 2.1 is then:
      //
      //                           Vref (mV)
      //   measurement (mV) =   --------------- * Result (bits) 
      //                       (2^10)-1 (bits)

      measurement =  Result * 2430 / 1023;

      EA = 1;                          // Re-enable interrupts

      SFRPAGE = UART1_PAGE;

      printf("AIN2.1 voltage: %ld mV\n",measurement);

      SFRPAGE = CONFIG_PAGE;
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
// This function initializes the system clock to use the internal oscillator
// at 24.5 MHz.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x83;                      // Set internal oscillator to run
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
// This function configures the crossbar and GPIO ports.
//
// P0.4   digital   push-pull     UART TX
// P0.5   digital   open-drain    UART RX
// P1.6   digital   push-pull     LED
// AIN1.1 analog                  Analog input 
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

   P1MDIN = 0xFD;                      // P1.1 Analog Input, Open Drain

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

   TL1 = TH1;                          // Initialize Timer1
   TR1 = 1;                            // Start Timer1

   SFRPAGE = UART1_PAGE;
   TI1 = 1;                            // Indicate TX1 ready

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page

}

//-----------------------------------------------------------------------------
// ADC2_Init
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
void ADC2_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = ADC2_PAGE;

   ADC2CN = 0x04;                      // ADC2 disabled; normal tracking
                                       // mode; ADC2 conversions are initiated
                                       // on overflow of Timer3; ADC2 data is
                                       // right-justified

   REF2CN = 0x03;                      // Enable on-chip VREF,
                                       // and VREF output buffer
   AMX2CF = 0x00;                      // AIN inputs are single-ended (default)
   AMX2SL = 0x01;                      // Select AIN2.1 pin as ADC mux input
   ADC2CF = (SYSCLK/SAR_CLK) << 3;     // ADC conversion clock = 2.5MHz
   EIE2 |= 0x10;                       // enable ADC interrupts
   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
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
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TMR3_PAGE;

   TMR3CN = 0x00;                      // Stop Timer3; Clear TF3;
   TMR3CF = 0x08;                      // use SYSCLK as timebase

   RCAP3   = -counts;                  // Init reload values
   TMR3    = RCAP3;                    // Set to reload immediately
   EIE2   &= ~0x01;                    // Disable Timer3 interrupts
   TR3     = 1;                        // start Timer3

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ADC2_ISR
//-----------------------------------------------------------------------------
//
// Here we take the ADC2 sample, add it to a running total <accumulator>, and
// decrement our local decimation counter <int_dec>.  When <int_dec> reaches
// zero, we post the decimated result in the global variable <Result>.
//
//-----------------------------------------------------------------------------
void ADC2_ISR (void) interrupt 18
{
   static unsigned int_dec=INT_DEC;    // Integrate/decimate counter
                                       // we post a new result when
                                       // int_dec = 0
   static long accumulator=0L;         // Here's where we integrate the
                                       // ADC samples
   AD2INT = 0;                         // Clear ADC conversion complete
                                       // indicator
   accumulator += ADC2;                // Read ADC value and add to running
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
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TMR2_PAGE;

   TMR2CN = 0x00;                      // Stop Timer3; Clear TF3;
   TMR2CF = 0x00;                      // use SYSCLK/12 as timebase

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

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------