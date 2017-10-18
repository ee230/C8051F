//-----------------------------------------------------------------------------
// F33x_IDA0_SineWave.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program outputs a sine wave using IDA0.  IDA0's output is
// scheduled to update at a rate determined by the constant
// <SAMPLE_RATE_DAC>, managed and timed by Timer3.
//
// Implements a 256-entry full-cycle sine table of 16-bit precision.
//
// The output frequency is determined by a 16-bit phase adder.
// At each DAC update cycle, the phase adder value is added to a running
// phase accumulator, <phase_accumulator>, the upper bits of which are used 
// to access the sine lookup table.
//
// How To Test:
//
// 1) Run the code on a C8051F330 Target board
// 2) Check that the J6 jumper is shorted
// 3) Use an oscilloscope to watch the IDAC output on the J1 or P2 header
//    and verify that there is a sine wave with a peak amplitude of 500 mV
//
//
// FID:            33X000016
// Target:         C8051F330 Target Board
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (GP)
//    -22 MAY 2006
//
    
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f330.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F33x
//-----------------------------------------------------------------------------

sfr16 TMR3RL   = 0x92;                 // Timer3 reload value
sfr16 TMR3     = 0x94;                 // Timer3 counter
sfr16 IDA0     = 0x96;                 // IDA0 data

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void main (void);
void OSCILLATOR_Init (void);
void PORT_Init (void);
void IDA0_Init (void);

void Timer3_Init (int counts);
void Timer3_ISR (void);

//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------

typedef union lng {                    // Access a long variable as two
      long Long;                       // 16-bit integer values
      int Int[2];
   } lng;

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK          24500000       // SYSCLK frequency in Hz

#define SAMPLE_RATE_DAC 100000L        // DAC sampling rate in Hz

#define PHASE_PRECISION 65536          // Range of phase accumulator

#define FREQUENCY       1000L          // Frequency of output in Hz

#define PHASE_ADD       (FREQUENCY * PHASE_PRECISION / SAMPLE_RATE_DAC)

#define AMPLITUDE       100 * 655      // 655 is the scaling factor
                                       // see the Timer3 ISR

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

// A full cycle, 16-bit, 2's complement sine wave lookup table
int code SINE_TABLE[256] = {

   0x0000, 0x0324, 0x0647, 0x096a, 0x0c8b, 0x0fab, 0x12c8, 0x15e2, 
   0x18f8, 0x1c0b, 0x1f19, 0x2223, 0x2528, 0x2826, 0x2b1f, 0x2e11,
   0x30fb, 0x33de, 0x36ba, 0x398c, 0x3c56, 0x3f17, 0x41ce, 0x447a, 
   0x471c, 0x49b4, 0x4c3f, 0x4ebf, 0x5133, 0x539b, 0x55f5, 0x5842,
   0x5a82, 0x5cb4, 0x5ed7, 0x60ec, 0x62f2, 0x64e8, 0x66cf, 0x68a6, 
   0x6a6d, 0x6c24, 0x6dca, 0x6f5f, 0x70e2, 0x7255, 0x73b5, 0x7504,
   0x7641, 0x776c, 0x7884, 0x798a, 0x7a7d, 0x7b5d, 0x7c29, 0x7ce3, 
   0x7d8a, 0x7e1d, 0x7e9d, 0x7f09, 0x7f62, 0x7fa7, 0x7fd8, 0x7ff6,
   0x7fff, 0x7ff6, 0x7fd8, 0x7fa7, 0x7f62, 0x7f09, 0x7e9d, 0x7e1d, 
   0x7d8a, 0x7ce3, 0x7c29, 0x7b5d, 0x7a7d, 0x798a, 0x7884, 0x776c,
   0x7641, 0x7504, 0x73b5, 0x7255, 0x70e2, 0x6f5f, 0x6dca, 0x6c24, 
   0x6a6d, 0x68a6, 0x66cf, 0x64e8, 0x62f2, 0x60ec, 0x5ed7, 0x5cb4,
   0x5a82, 0x5842, 0x55f5, 0x539b, 0x5133, 0x4ebf, 0x4c3f, 0x49b4, 
   0x471c, 0x447a, 0x41ce, 0x3f17, 0x3c56, 0x398c, 0x36ba, 0x33de,
   0x30fb, 0x2e11, 0x2b1f, 0x2826, 0x2528, 0x2223, 0x1f19, 0x1c0b, 
   0x18f8, 0x15e2, 0x12c8, 0x0fab, 0x0c8b, 0x096a, 0x0647, 0x0324,
   0x0000, 0xfcdc, 0xf9b9, 0xf696, 0xf375, 0xf055, 0xed38, 0xea1e, 
   0xe708, 0xe3f5, 0xe0e7, 0xdddd, 0xdad8, 0xd7da, 0xd4e1, 0xd1ef,
   0xcf05, 0xcc22, 0xc946, 0xc674, 0xc3aa, 0xc0e9, 0xbe32, 0xbb86, 
   0xb8e4, 0xb64c, 0xb3c1, 0xb141, 0xaecd, 0xac65, 0xaa0b, 0xa7be,
   0xa57e, 0xa34c, 0xa129, 0x9f14, 0x9d0e, 0x9b18, 0x9931, 0x975a, 
   0x9593, 0x93dc, 0x9236, 0x90a1, 0x8f1e, 0x8dab, 0x8c4b, 0x8afc,
   0x89bf, 0x8894, 0x877c, 0x8676, 0x8583, 0x84a3, 0x83d7, 0x831d, 
   0x8276, 0x81e3, 0x8163, 0x80f7, 0x809e, 0x8059, 0x8028, 0x800a,
   0x8000, 0x800a, 0x8028, 0x8059, 0x809e, 0x80f7, 0x8163, 0x81e3, 
   0x8276, 0x831d, 0x83d7, 0x84a3, 0x8583, 0x8676, 0x877c, 0x8894,
   0x89bf, 0x8afc, 0x8c4b, 0x8dab, 0x8f1e, 0x90a1, 0x9236, 0x93dc, 
   0x9593, 0x975a, 0x9931, 0x9b18, 0x9d0e, 0x9f14, 0xa129, 0xa34c,
   0xa57e, 0xa7be, 0xaa0b, 0xac65, 0xaecd, 0xb141, 0xb3c1, 0xb64c, 
   0xb8e4, 0xbb86, 0xbe32, 0xc0e9, 0xc3aa, 0xc674, 0xc946, 0xcc22,
   0xcf05, 0xd1ef, 0xd4e1, 0xd7da, 0xdad8, 0xdddd, 0xe0e7, 0xe3f5, 
   0xe708, 0xea1e, 0xed38, 0xf055, 0xf375, 0xf696, 0xf9b9, 0xfcdc,
};

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void) 
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   OSCILLATOR_Init ();                 // Initialize Oscillator
   PORT_Init ();                       // Initialize Ports
   IDA0_Init ();                       // Initialize IDAC

   Timer3_Init(SYSCLK/SAMPLE_RATE_DAC);// Using Timer3 as update scheduler	
      
   EA = 1;                             // Enable global interrupts
      
   while(1){} 
} // end main

//-----------------------------------------------------------------------------
// Init Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal 24.5MHz
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN  = 0x83;                     // Set clock to 24.5 MHz
   RSTSRC  = 0x04;                     // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// P0.1     (skipped in crossbar)      IDA0 output pin
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0SKIP   = 0x02;                    // Skip P0.1 which is used for IDA0
   P0MDIN  &= ~0x02;                   // Configure P0.1 as an analog pin

   XBR0     = 0x00;                   
   XBR1     = 0x40;                    // Enable crossbar and weak pullups 
}

//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1)  int counts - calculated Timer overflow rate
//                    range is positive range of integer: 0 to 32767
//
// Configure Timer3 to auto-reload at interval specified by <counts>
// using SYSCLK as its time base.  Interrupts are enabled.
//-----------------------------------------------------------------------------

void Timer3_Init(int counts)
{
   TMR3CN  = 0x00;                     // Resets Timer 3, sets to 16 bit mode
   CKCON  |= 0x40;                     // Use system clock
   TMR3RL  = -counts;                  // Initial reload value

   TMR3    = 0xffff;                   // Sets timer to reload automatically
   EIE1   |= 0x80;                     // Enable Timer 3 interrupts
   TMR3CN  = 0x04;                     // Start Timer 3
}

//-----------------------------------------------------------------------------
// IDA0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Enable VREF and IDAC0
//-----------------------------------------------------------------------------
void IDA0_Init (void)
{
   REF0CN = 0x03;                      // Enable internal VREF generator

   IDA0CN = 0xB2;                      // Enable IDAC0 for 2.0 mA full-scale
                                       // output; updated on Timer3 overflows
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer3_ISR
//-----------------------------------------------------------------------------
//
// This ISR is called on Timer3 overflows.  Timer3 is set to auto-reload mode
// and is used to schedule the DAC output sample rate in this example.
// Note that the value that is written to the IDAC during this ISR call is
// actually transferred to the IDAC at the next Timer3 overflow.
//
//-----------------------------------------------------------------------------
void Timer3_ISR (void) interrupt 14 using 3
{
  
   static unsigned phase_acc = 0;      // Holds phase accumulator
   
   int temp1;                          // The temporary value that passes 
                                       // through 3 stages before being written 
                                       // to the IDAC

   int code *table_ptr;                // Pointer to the lookup table
   
   lng temporary_long;                 // Holds the result of a 16-bit multiply

   TMR3CN &= ~0x80;                    // Clear Timer3 overflow flag
	
   table_ptr = SINE_TABLE;         
   
   phase_acc += PHASE_ADD;             // Increment phase accumulator 
                                   
  
   // Set the value of <temp1> to the next output of DAC1 at full-scale
   // amplitude; the  rails are +32767, -32768 
   
   // Read the table value
   temp1 = *(table_ptr + (phase_acc >> 8)); 
                    
   // Adjust the Gain
   temporary_long.Long = (long) ((long)temp1 * (long)AMPLITUDE); 
   
   temp1 = temporary_long.Int[0];      // Same as temporary_long >> 16

   // Add a DC bias to make the rails 0 to 65535
   // Note: the XOR with 0x8000 translates the bipolar quantity into 
   // a unipolar quantity.

   IDA0 = 0x8000 ^ temp1;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------