//-----------------------------------------------------------------------------
// F0xx_DACs_SineCosine.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program outputs sine and cosine waveforms using the hardware DACs
// on the C8051F005 microcontroller.  The waveforms are output on pins DAC0 
// and DAC1.
//
// The output of the DAC is updated upon a Timer3 interrupt.
//
// The frequency of the output waveforms is set by the #define <FREQUENCY>.
// To achieve the exact output frequency defined by <FREQUENCY>, an
// external crystal should be used.
//
// How To Test: 
//
// 1) If desired, populate an external crystal on the C8051F005-TB.  Change
//    the #define USB_EXT_OSC and #define SYSCLK to reflect using the crystal.
//    Also verify that the crystal loading capacitors (C10 - 33pf, C11 - 33pf)
//    are appropriate for the crystal.
// 2) Download the code to the C8051F005 Development Board
// 3) Connect two oscillscope probes to pins DAC0 and DAC1.  These pins are 
//    available on the J2 header on pins 48 and 50 respectively.
// 4) Run the code and confirm that there are two waveforms with the output of 
//    DAC1 leading the output DAC0 by 90 degrees.  The frequency of both 
//    waveforms should be close to frequency defined by <FREQUENCY>.
//
// FID:            00X000015
// Target:         C8051F0xx
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.71
// Command Line:   Default
//
// Release 1.0
//    -Initial Revision (GP)
//    -11 JUL 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f000.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F02x
//-----------------------------------------------------------------------------

sfr16 TMR3RL   = 0x92;                 // Timer3 reload
sfr16 TMR3     = 0x94;                 // Timer3
sfr16 DAC0     = 0xd2;                 // DAC0 data
sfr16 DAC1     = 0xd5;                 // DAC1 data

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------


#define USE_EXT_OSC     0              // 0 = use internal oscillator
                                       // 1 = use external oscillator


#if(USE_EXT_OSC == 0)
 #define SYSCLK          16000000      // Internal oscillator frequency in Hz
#endif

#if(USE_EXT_OSC == 1)
 #define SYSCLK          22118400      // External oscillator frequency in Hz
#endif

#define SAMPLE_RATE_DAC 100000L        // DAC sampling rate in Hz
#define PHASE_PRECISION 65536          // range of phase accumulator

#define FREQUENCY       1000           // Frequency of output waveform in Hz
                                       
// <PHASE_ADD> is the change in phase between DAC samples; It is used in 
// the set_DACs routine.

unsigned int PHASE_ADD = FREQUENCY * PHASE_PRECISION / SAMPLE_RATE_DAC;

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
// Function Prototypes
//-----------------------------------------------------------------------------

void main(void);
void OSCILLATOR_Init(void);
void DAC0_Init (void);
void DAC1_Init (void);
void TIMER3_Init(int counts);
void Set_DACs(void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void) 
{
   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   OSCILLATOR_Init ();                 // Initialize oscillator
   DAC0_Init ();                       // Initialize DAC0
   DAC1_Init ();                       // Initialize DAC1   

   TIMER3_Init(SYSCLK/SAMPLE_RATE_DAC);// Initialize Timer3 to overflow 
                                       // <SAMPLE_RATE_DAC> times per
                                       // second

   EA = 1;                             // Enable global interrupts
   
   while(1);
} 

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TIMER3_ISR -- Wave Generator
//-----------------------------------------------------------------------------
//
// This ISR is called on Timer3 overflows.  Timer3 is set to auto-reload mode
// and is used to schedule the DAC output sample rate in this example.
//
void TIMER3_ISR (void) interrupt 14
{
   TMR3CN &= ~0x80;                    // Clear Timer3 overflow flag 

   Set_DACs();
}


//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to either the 16 Mhz or the 
// external crystal oscillator depending on #define USB_EXT_OSC
//
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void)
{

#if(USE_EXT_OSC == 0)

   OSCICN = 0x07;                      // Set internal oscillator to 16 Mhz

#endif

#if (USE_EXT_OSC == 1)

   int i;                              // Delay counter

   OSCXCN = 0x67;                      // Start external oscillator with
                                       // 22.1184 MHz crystal

   for (i = 0; i < 256; i++);          // XTLVLD blanking interval (>1ms)

   while (!(OSCXCN & 0x80));           // Wait for crystal osc. to settle

   OSCICN = 0x88;                      // Select external oscillator as SYSCLK
                                       // source and enable missing clock
                                       // detector
#endif

}


//-----------------------------------------------------------------------------
// DAC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Enable the DAC and the VREF buffer.
//
//-----------------------------------------------------------------------------

void DAC0_Init(void)
{
   DAC0CN = 0x87;                      // Enable DAC0 in left-justified mode

   REF0CN |= 0x03;                     // Enable the internal VREF (2.4v) and
                                       // the Bias Generator

}

//-----------------------------------------------------------------------------
// DAC1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Enable the DAC and the VREF buffer.
//
//-----------------------------------------------------------------------------


void DAC1_Init(void)
{
   DAC1CN = 0x87;                      // Enable DAC1 in left-justified mode

   REF0CN |= 0x03;                     // Enable the internal VREF (2.4v) and
                                       // the Bias Generator
}


//-----------------------------------------------------------------------------
// TIMER3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1)  int counts - calculated Timer overflow rate
//                    range is positive range of integer: 0 to 32767
//
// Configure Timer3 to auto-reload at interval specified by <counts> using 
// SYSCLK as its time base.
//
//-----------------------------------------------------------------------------

void TIMER3_Init (int counts)
{   
   TMR3CN  = 0;                        // STOP timer; set to auto-reload mode
   TMR3CN |= 0x02;                     // Timer3 counts SYSCLKs
   
   TMR3RL  = -counts;                  // Set reload value
   TMR3    = TMR3RLL;                  // Initialize Timer to reload value

   EIE2   |= 0x01;                     // Enable Timer3 interrupts
   TMR3CN |= 0x04;                     // Start Timer3

}

//-----------------------------------------------------------------------------
// Set_DACs
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Calculates the update values for the two DACs using SINE_TABLE.
//
//-----------------------------------------------------------------------------

void Set_DACs(void)
{
   static unsigned phase_acc = 0;      // Holds phase accumulator
      
   int SIN_temp, COS_temp;             // Temporary 16-bit variables 
   unsigned char index;                // Index into SINE table

   phase_acc += PHASE_ADD;             // Increment phase accumulator
   index = phase_acc >> 8;
   
   SIN_temp = SINE_TABLE[index];       // Read the table value
   
   index += 64;                        // 90 degree phase shift 
   COS_temp = SINE_TABLE[index];


   // Add a DC bias to change the the rails from a bipolar (-32768 to 32767)
   // to unipolar (0 to 65535)
   // Note: the XOR with 0x8000 translates the bipolar quantity into 
   // a unipolar quantity.

   DAC0 = SIN_temp ^ 0x8000;           // Write to DAC0
   
   DAC1 = COS_temp ^ 0x8000;           // Write to DAC1
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------