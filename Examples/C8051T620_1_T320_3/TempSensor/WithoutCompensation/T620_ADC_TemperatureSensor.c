//-----------------------------------------------------------------------------
// T620_ADC_TemperatureSensor.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program shows the use of the temperature sensor along with the ADC.
// Without calibration, the temperature accuracy is about +/- 10 degrees C.
//
// How To Test:
//
// 1) Add T620_ADC_TemperatureSensor.c to a project in the Silicon Labs IDE.
// 2) Compile, link and download the code to a 'T620 device on a 'T62x mother
//    board.
// 3) Run the code.
// 4) The designer can see the temperature by attaching a serial cable and
//    opening a terminal type program with the following setup:
//
//    <BAUDRATE> baud, no parity, 8 bits, 1 stop bit, no control
//
// 5) The format of the device output is as follows:
//
//    T = 24.3(C)
//
//    where T is the temperature in Celcius
//
// 6) The designer can see the temperature change by simply touching the
//    IC with any warm object including a finger (please be careful to
//    avoid ESD issues!)
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
//       4.11:  Vtemp = 3.48T(C) + 789mV
//
//       Moving T(C) to the left-hand side we have:
//
//          T(C) = (1 / 3.48)Vtemp - (789mV / 3.48)
//
//       In this instance the equivalent linear equation is given by:
//
//          T(C) = 0.287Vtemp - 226.7 (V in millivolts)
//
//       Converting from V to ADC codes gives us:
//
//          T(C) = (0.287*VDD*ADC) - 226.7;
//
//       Sampling 256 times:
//
//          T(C) = (0.287*VDD)*(ADC_sum/256) - 226.7;
//
//       Assuming a VDD = 3300 mV leads to:
//
//          T(C) = 3.699*ADC_sum - 226.7;
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
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"
#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SCALE             1000L        // Scale for temp calculations
#define SAMPLING_NUMBER     256        // Number of samples per calculation
#define OVER_ROUND           10        // Number of shifts (>>N) to reach the
                                       // correct number of bits of precision
#define SLOPE              3363        // Slope of the temp transfer function
#define OFFSET          226700L        // Offset for the temp transfer function
#define SYSCLK         12000000        // SYSCLK frequency in Hz
#define BAUDRATE           9600        // Baud rate of UART in bps

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

unsigned long ADC_SUM = 0;             // Accumulates the ADC samples
bit CONV_COMPLETE = 0;                 // ADC_SUM result ready flag

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
   long temp_scaled;                   // Stores scaled temperature value
   long temp_whole;                    // Stores unscaled whole number portion
                                       // of temperature for output
   int temp_frac;                      // Stores unscaled decimal portion of
                                       // temperature for output

   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)
   VDM0CN = 0x80;                      // Enable Vdd monitor

   Oscillator_Init ();                 // Initialize Oscillator
   ADC0_Init ();                       // Initialize ADC0
   Timer2_Init();                      // Init Timer2 to generate
   Port_Init ();                       // Initialize crossbar and GPIO
   UART0_Init ();                      // Initialize UART

   RSTSRC = 0x02;                      // Vdd monitor enabled
   EA = 1;                             // Enable all interrupts

   printf("\f");                       // Clear the screen before outputs

   while (1)                           // Spin forever
   {
      if(CONV_COMPLETE)
      {
         temp_scaled = ADC_SUM;        // Apply our derived equation to ADC
         temp_scaled *= SLOPE;

         // With a left-justified ADC, we have to shift the decimal place
         // of temp_scaled to the right so we can match the format of
         // OFFSET. Once the formats are matched, we can subtract OFFSET.
         temp_scaled = temp_scaled >> OVER_ROUND;
         temp_scaled -= OFFSET;

         // Calculate the temp's whole number portion by unscaling
         temp_whole = temp_scaled/SCALE;

         // temp_frac is the unscaled decimal portion of temperature
         temp_frac = (int)((temp_scaled - temp_whole*SCALE) / (SCALE/10));

         if(temp_frac < 0)
         {
            temp_frac *= -1;           // If negative, remove negative from
                                       // temp_frac portion for output.
         }

         printf (" T = %ld.%d(C) \r",temp_whole,temp_frac);

         CONV_COMPLETE = 0;
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
// at 12 MHz.  Also enables the Missing Clock Detector.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // 12 MHz.
   RSTSRC  = 0x04;                     // Enable missing clock detector
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
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
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
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } else if (SYSCLK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   } else if (SYSCLK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else {
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
   REF0CN = 0x8E;                      // Vref == Vdd, Temp. Sensor ON, Bias ON
   AMX0P = 0x1E;                       // Selects Temp. Sensor
   ADC0CF = ((SYSCLK/3000000)-1)<<3;   // Set SAR clock to 3MHz
   ADC0CF |= 0x04;                     // ADC0 is left justified
   ADC0CF |= 0x01;                     // Gain = 1
   //ADC0CF &= ~0x01;                     // Gain = 0.5

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
// This ISR averages 256 samples then copies the result to ADC_SUM.
//
//-----------------------------------------------------------------------------
INTERRUPT (ADC_ISR, INTERRUPT_ADC0_EOC)
{
   static unsigned long accumulator = 0; // Accumulator for averaging
   static unsigned int measurements = SAMPLING_NUMBER; // Measurement counter

   AD0INT = 0;                         // Clear ADC0 conv. complete flag

   // Checks if obtained the necessary number of samples
   if(measurements == 0)
   {

      ADC_SUM = accumulator;           // Copy total into ADC_SUM
      measurements = 256;              // Reset counter
      accumulator = 0;                 // Reset accumulator
      CONV_COMPLETE = 1;               // Set result ready flag
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
