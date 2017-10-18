//-----------------------------------------------------------------------------
// F06x_DACs_SineCosine.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program outputs sine and cosine waveforms using the hardware DACs
// on the C8051F06x microcontroller.  The waveforms are output on pins DAC0 
// and DAC1.
//
// The output of the DAC is updated upon a Timer4 interrupt.  The Timer4 ISR
// then calculates the output of the DACs for the next interrupt. 
//
// The frequency of the output waveforms is set by the #define <FREQUENCY>.
// To achieve the exact output frequency defined by <FREQUENCY>, an
// external crystal should be used.
//
//
// How To Test: 
//
// 1) Download the code to an C8051F060 Development Board
// 2) Ensure that pins 1 and 2 on the J22 header are shorted.  This ensures 
//    that the VREF used for the DAC is connected to decoupling capacitors.
// 3) Connect two oscillscope probes to pins DAC0 and DAC1.  These pins are 
//    available on the 96-pin header: DAC0 - C26;  DAC1 - B26.
// 3) Confirm that there are two waveforms with the output of DAC1 leading 
//    the output DAC0 by 90 degrees.  The frequency of both waveforms should
//    be close to frequency defined by <FREQUENCY>
//
// FID:            06X000019
// Target:         C8051F06x
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

#include <c8051f060.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F06x
//-----------------------------------------------------------------------------

sfr16 RCAP4    = 0xCA;                 // Timer4 capture/reload
sfr16 TMR4     = 0xCC;                 // Timer4
sfr16 DAC0     = 0xD2;                 // DAC0 data
sfr16 DAC1     = 0xD2;                 // DAC1 data

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       24500000          // Internal oscillator frequency in Hz

#define SAMPLE_RATE_DAC 100000L        // DAC sampling rate in Hz
#define PHASE_PRECISION 65536          // range of phase accumulator

#define FREQUENCY 1000                 // Frequency of output waveform in Hz
                                       
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
void TIMER4_Init(int counts);
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

   TIMER4_Init(SYSCLK/SAMPLE_RATE_DAC);// Initialize Timer4 to overflow 
                                       // <SAMPLE_RATE_DAC> times per
                                       // second

   EA = 1;                             // Enable global interrupts
   
   while(1);
} 

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TIMER4_ISR -- Wave Generator
//-----------------------------------------------------------------------------
//
// This ISR is called on Timer4 overflows.  Timer4 is set to auto-reload mode
// and is used to schedule the DAC output sample rate in this example.
// Note that the value that is written to DAC1 during this ISR call is
// actually transferred to DAC1 at the next Timer4 overflow.
//
void TIMER4_ISR (void) interrupt 16
{
   TF4 = 0;                            // Clear Timer4 overflow flag 
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
// This function initializes the system clock to use the internal oscillator
// at 24.5
//
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // set SFR page

   OSCICN |= 0x03;                     // Set internal oscillator to run
                                       // at its maximum frequency

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//-----------------------------------------------------------------------------
// DAC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure DAC0 to update on Timer4 overflows and enable the the VREF buffer.
//
//-----------------------------------------------------------------------------

void DAC0_Init(void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = DAC0_PAGE;
   
   DAC0CN = 0x94;                      // Enable DAC0 in left-justified mode
                                       // managed by Timer4 overflows
   SFRPAGE = ADC2_PAGE;

   REF2CN |= 0x03;                     // Enable the internal VREF (2.4v) and
                                       // the Bias Generator

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// DAC1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure DAC1 to update on Timer4 overflows and enable the the VREF buffer.
//
//-----------------------------------------------------------------------------

void DAC1_Init(void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = DAC1_PAGE;
   
   DAC1CN = 0x94;                      // Enable DAC1 in left-justified mode
                                       // managed by Timer4 overflows
   SFRPAGE = ADC2_PAGE;

   REF2CN |= 0x03;                     // Enable the internal VREF (2.4v) and
                                       // the Bias Generator

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//-----------------------------------------------------------------------------
// TIMER4_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1)  int counts - calculated Timer overflow rate
//                    range is positive range of integer: 0 to 32767
//
// Configure Timer4 to auto-reload at interval specified by <counts> using 
// SYSCLK as its time base.
//
//-----------------------------------------------------------------------------

void TIMER4_Init (int counts)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TMR4_PAGE;
   
   TMR4CN = 0x00;                      // Stop Timer4; Clear overflow flag (TF4);
                                       // Set to Auto-Reload Mode

   TMR4CF = 0x08;                      // Configure Timer4 to increment;
                                       // Timer4 counts SYSCLKs

   RCAP4 = -counts;                    // Set reload value
   TMR4 = RCAP4;                       // Initialzie Timer4 to reload value

   EIE2 |= 0x04;                       // Enable Timer4 interrupts
   TMR4CN |= 0x04;                     // Start Timer4

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// Set_DACs
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Calculates the update values for the two DACs using SINE_TABLE.  The DACs
// will actually be updated upon the next Timer4 interrupt.  Until that time,
// the DACs will hold their current value.
//
//-----------------------------------------------------------------------------

void Set_DACs(void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
   
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
   

   SFRPAGE = DAC0_PAGE;
   DAC0 = 0x8000 ^ SIN_temp;           // Write to DAC0

   SFRPAGE = DAC1_PAGE;
   DAC1 = 0x8000 ^ COS_temp;           // Write to DAC1
   
   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------