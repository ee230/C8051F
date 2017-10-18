//-----------------------------------------------------------------------------
// T620_ADC_TemperatureSensor.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program shows the use of the temperature sensor along with the ADC.
//
// How To Test:
//
// 1) Add T620_ADC_TemperatureSensor.c to a project in the Silicon Labs IDE.
// 2) Open up the Tools menu and select Erase Code Space.
// 3) Compile, link and download the code to a 'T620 on a 'T62x mother board.
// 4) Run the code.
//
// 5) The designer can see the temperature by attaching a serial cable and
//    opening a terminal type program with the following setup:
//
//    <BAUDRATE> baud, no parity, 8 bits, 1 stop bit, no control
//
// 6) The format of the message is as follows:
//
//    T = 24.3(C)
//
//    T - the temperature in Celcius
//
// 7) The designer can see the temperature change by simply touching the
//    IC with any warm object including a finger (please be careful to
//    avoid ESD issues!).
//
// Target:         C8051T620/1 or 'T320/1/2/3
// Tool chain:     Keil C51 8 / Keil EVAL C51
// Command Line:   None
//
// Revision History:
//
// Release 1.0 / 30 JUN 2008 (TP)
//    -Initial Revision
//
// Obs1: The ADC is set to left-justified mode. When in this mode, the ADC
//       uses bits 6 through 15 to output the ADC readings with the decimal
//       point being to the left of bit 15 instead of to the right of bit 0 in
//       right-justified mode. The ADC0 reading lookings like:
//
//          Left-justified:     .xxxxxxxxxx000000
//          Right-justified:     000000xxxxxxxxxx.
//
//          where x's represent the readings from the ADC, and 0's are always 0
//
//       This mode's output is fractional since the decimal point is on the
//       left side. Since the ADC output is fractional, we can directly
//       multiply the ADC times our VREF voltage and find exactly what voltage
//       the temp sensor reading is.
//
//       The only caveat of this is that we have to keep track of where the
//       decimal point is after multiplication.
//
// Obs2: This program uses the following equation from the datasheet, Table
//       4.11:  Vtemp = xT(C) + xmV
//
//       Moving T(C) to the left-hand side we have:
//
//          T(C) = (1 / x)Vtemp - (xmV / x)
//
//       In this instance the equivalent linear equation is given by:
//
//          T(C) = xVtemp - x (V in millivolts)
//
//       Converting from V to ADC codes gives us:
//
//          T(C) = (x*VDD*ADC) - x;
//
//       Sampling 256 times:
//
//          T(C) = (x*VDD)*(ADC_sum/256) - x;
//
//       Assuming a VDD = 3300 mV leads to:
//
//          T(C) = x*ADC_sum - x;
//
//       To allow for precise calculations using fixed point math these
//       factors are going to be scaled up 1000x
//
// Remarks:
//
//       SLOPE - coefficient of the line that multiplies the ADC_sum value
//       OFFSET - the coefficient to be added or subtracted to the line
//       T(C) - Temperature in Celcius
//       ADC_sum - Sum of the ADC conversions (Oversampled)
//       OVER_ROUND - This variable keeps track of where the decimal place is.
//                    We get right-justified by shifting right OVER_ROUND times.
//       Oversampling - For more information on oversampling to improve ADC
//                      resolution, please refer to Application Note AN118.
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051T620_defs.h>
#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define TRUE                 1
#define FALSE                0

#define SCALE            1000L         // Scale for temp calculations
#define SAMPLING_NUMBER    256         // Number of samples per calculation
#define OVER_ROUND          10         // Number of shifts (>>N) to reach the
                                       // correct number of bits of precision
#define SLOPE             4507         // Slope of the temp transfer function
#define OFFSET         271329L         // Offset for the temp transfer function
#define SYSCLK        12000000         // SYSCLK frequency in Hz
#define BAUDRATE          9600         // Baud rate of UART in bps
#define COMP_ADDRESS    0x3FFA         // Location of TOFFH and TOFFL

U16 code COMPENSATION _at_ COMP_ADDRESS; // TOFFH and TOFFL stored in EPROM
                                         // memory

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

unsigned long ADC_Sum = 0;             // Accumulates the ADC samples
bit conv_complete = FALSE;             // ADC_Sum result ready flag

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void Timer2_Init(void);
void UART0_Init (void);
void ADC0_Init (void);

INTERRUPT_PROTO (ADC0_ISR, INTERRUPT_ADC0_EOC);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   long temp_scaled;                   // Scaled temperature value
   long temp_whole;                    // Stores integer portion for output
   long temp_comp;                     // Scaled and compensated temp
   int temp_frac = 0;                  // Stores fractional portion of temp

   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)
   VDM0CN = 0x80;                      // Enable Vdd monitor

   Oscillator_Init ();                 // Initialize Oscillator
   ADC0_Init ();                       // Initialize ADC0
   Timer2_Init ();                     // Initialize timer2
   Port_Init ();                       // Initialize crossbar and GPIO
   UART0_Init ();                      // Initialize UART

   RSTSRC = 0x02;                      // Vdd monitor enabled as a reset source

   EA = 1;                             // Enable all interrupts

   printf("\f");                       // Clear screen before output begins

   while (1)                           // Spin forever
   {
      if ( conv_complete )             // If ADC acquired SAMPLING_NUMBER
      {                                // samples
         // Calculate the temperature with rounding

         // Round sum to proper precision first
         temp_scaled = (long) ADC_Sum;

         temp_scaled *= SLOPE;         // Calculate rounded temperature

         // With a left-justified ADC, we have to shift the decimal place
         // of temp_scaled to the right so we can match the format of
         // OFFSET. Once the formats are matched, we can subtract OFFSET.
         temp_scaled = temp_scaled >> OVER_ROUND;

         temp_scaled -= OFFSET;        // Apply offset to temp
         temp_comp = temp_scaled - COMPENSATION; // Apply TOFFH and TOFFL

         // Take whole number portion unscaled
         temp_whole = temp_comp/SCALE;

         // temp_frac is the unscaled decimal portion of temperature
         temp_frac = (int)(( temp_comp - temp_whole * SCALE ) / (SCALE/10));

         // If negative, remove negative from temp_frac portion for output.
         if( temp_frac < 0 )
         {
            temp_frac *= -1;
         }

         printf ("\r  T = %ld.%d(C)   ",temp_whole,temp_frac);
         conv_complete = FALSE;
      }
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
// This routine initializes the system clock to use the internal oscillator
// at 12 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // 12 MHz.
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initialized the GPIO and the Crossbar
//
// P0.4   digital   push-pull    UART TX
// P0.5   digital   open-drain   UART RX
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDOUT |= 0x10;                    // UART-TX push-pull

   XBR0 = 0x01;                        // Enable UART I/O
   XBR1 = 0x40;                        // Enable crossbar
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
   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // use SYSCLK as timebase, 16-bit
                                       // auto-reload
   CKCON |= 0x10;                      // Select SYSCLK for timer 2 source
   TMR2RL = 65535 - (SYSCLK / 10000);  // Init reload value for 10uS
   TMR2 = 0xffff;                      // Set to reload immediately
   TR2 = 1;                            // Start Timer2
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
   if (SYSCLK/BAUDRATE/2/256 < 1)
   {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   }
   else if (SYSCLK/BAUDRATE/2/256 < 4)
   {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   }
   else if (SYSCLK/BAUDRATE/2/256 < 12)
   {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   }
   else
   {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
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
// Initialize the ADC to use the temperature sensor.
//
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   REF0CN = 0x0E;                      // VREF is VDD, Temp. Sensor ON, Bias ON
   AMX0P = 0x1E;                       // Selects Temp. Sensor
   ADC0CF = ((SYSCLK/3000000)-1)<<3;   // Set SAR clock to 3MHz
   ADC0CF |= 0x04;                     // ADC0 is left justified

   ADC0CN = 0x82;                      // ADC ON, starts on TMR2 overflow

   EIE1 |= 0x08;                       // Enable ADC0 conversion complete int.
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ADC0_ISR
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This ISR averages 256 samples then copies the result to ADC_Sum.
//
//-----------------------------------------------------------------------------
INTERRUPT (ADC_ISR, INTERRUPT_ADC0_EOC)
{
   static unsigned long accumulator = 0; // Accumulator for averaging
   static unsigned int measurements = SAMPLING_NUMBER; // Measurement counter

   AD0INT = 0;                         // Clear ADC0 conv. complete flag

   // Checks if SAMPLING_NUMBER of samples have been acquired
   if(measurements == 0)
   {
      ADC_Sum = accumulator;           // Copy total into ADC_Sum
      measurements = 256;              // Reset counter
      accumulator = 0;                 // Reset accumulator
      conv_complete = TRUE;            // Set result ready flag
   }
   else
   {
      accumulator += ADC0 >> 6;        // If not, keep adding while shifting
                                       // out unused bits in ADC0
      measurements--;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
