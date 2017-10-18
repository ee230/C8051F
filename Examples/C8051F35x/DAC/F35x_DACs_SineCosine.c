//-----------------------------------------------------------------------------
// F35x_DACs_SineCosine.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program outputs sine and cosine waveforms using the hardware DACs
// on the C8051F350 microcontroller.  The waveforms are output on pins IDA0 
// and IDA1.
//
// The output of the DACs is updated upon a Timer3 interrupt.  The Timer3 ISR
// then calculates the output of the DACs for the next interrupt. 
//
// The frequency of the output waveforms is set by the #define <FREQUENCY>.
//
//
// How To Test: 
//
// 1) Download the code to an C8051F350 Development Board
// 2) Ensure that Jumpers J13 and J14 are shorted.  These jumpers connect
//    the external VREF to the device
// 3) Ensure that Jumpers J11 and J7 are shorted.  These jumpers connect 
//    resistors to pins IDA0 and IDA1 so that we can a view voltage
// 4) Connect two oscillscope probes to pins P1.6/IDA0 and P1.7/IDA1.  
//    These pins are available on both the J1 and J6 headers.
// 5) Confirm that there are two waveforms with the output of DAC1 leading 
//    the output DAC0 by 90 degrees.  The frequency of both waveforms should
//    be close to frequency defined by <FREQUENCY>
//
// FID:            35X000015
// Target:         C8051F35x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.71
// Command Line:   Default
//
// Release 1.0
//    -Initial Revision (GP)
//    -09 AUG 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f350.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F35x
//-----------------------------------------------------------------------------

sfr16 TMR3RL   = 0x92;                 // Timer3 reload value
sfr16 TMR3     = 0x94;                 // Timer3 counter

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK          24500000       // Internal oscillator frequency in Hz

#define SAMPLE_RATE_DAC 100000L        // DAC sampling rate in Hz
#define PHASE_PRECISION 65536          // range of phase accumulator

#define FREQUENCY       1000           // Frequency of output waveform in Hz
                                       
// <PHASE_ADD> is the change in phase between DAC samples; It is used in 
// the set_DACs routine.

unsigned int PHASE_ADD = FREQUENCY * PHASE_PRECISION / SAMPLE_RATE_DAC;

// a full cycle, 8-bit, 2's complement sine wave lookup table
char code SINE_TABLE[256] = {
   0x00, 0x03, 0x06, 0x09, 0x0c, 0x0f, 0x12, 0x15,
   0x18, 0x1c, 0x1f, 0x22, 0x25, 0x28, 0x2b, 0x2e,
   0x30, 0x33, 0x36, 0x39, 0x3c, 0x3f, 0x41, 0x44,
   0x47, 0x49, 0x4c, 0x4e, 0x51, 0x53, 0x55, 0x58,
   0x5a, 0x5c, 0x5e, 0x60, 0x62, 0x64, 0x66, 0x68,
   0x6a, 0x6c, 0x6d, 0x6f, 0x70, 0x72, 0x73, 0x75,
   0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7c,
   0x7d, 0x7e, 0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
   0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7e, 0x7e,
   0x7d, 0x7c, 0x7c, 0x7b, 0x7a, 0x79, 0x78, 0x77,
   0x76, 0x75, 0x73, 0x72, 0x70, 0x6f, 0x6d, 0x6c,
   0x6a, 0x68, 0x66, 0x64, 0x62, 0x60, 0x5e, 0x5c,
   0x5a, 0x58, 0x55, 0x53, 0x51, 0x4e, 0x4c, 0x49,
   0x47, 0x44, 0x41, 0x3f, 0x3c, 0x39, 0x36, 0x33,
   0x30, 0x2e, 0x2b, 0x28, 0x25, 0x22, 0x1f, 0x1c,
   0x18, 0x15, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03,
   0x00, 0xfd, 0xfa, 0xf7, 0xf4, 0xf1, 0xee, 0xeb,
   0xe8, 0xe4, 0xe1, 0xde, 0xdb, 0xd8, 0xd5, 0xd2,
   0xd0, 0xcd, 0xca, 0xc7, 0xc4, 0xc1, 0xbf, 0xbc,
   0xb9, 0xb7, 0xb4, 0xb2, 0xaf, 0xad, 0xab, 0xa8,
   0xa6, 0xa4, 0xa2, 0xa0, 0x9e, 0x9c, 0x9a, 0x98,
   0x96, 0x94, 0x93, 0x91, 0x90, 0x8e, 0x8d, 0x8b,
   0x8a, 0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x84,
   0x83, 0x82, 0x82, 0x81, 0x81, 0x81, 0x81, 0x81,
   0x80, 0x81, 0x81, 0x81, 0x81, 0x81, 0x82, 0x82,
   0x83, 0x84, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
   0x8a, 0x8b, 0x8d, 0x8e, 0x90, 0x91, 0x93, 0x94,
   0x96, 0x98, 0x9a, 0x9c, 0x9e, 0xa0, 0xa2, 0xa4,
   0xa6, 0xa8, 0xab, 0xad, 0xaf, 0xb2, 0xb4, 0xb7,
   0xb9, 0xbc, 0xbf, 0xc1, 0xc4, 0xc7, 0xca, 0xcd,
   0xd0, 0xd2, 0xd5, 0xd8, 0xdb, 0xde, 0xe1, 0xe4,
   0xe8, 0xeb, 0xee, 0xf1, 0xf4, 0xf7, 0xfa, 0xfd
};

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void main(void);
void OSCILLATOR_Init (void);
void PORT_Init (void);
void DAC0_Init (void);
void DAC1_Init (void);
void TIMER3_Init (int counts);
void Set_DACs (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void) 
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init ();
   DAC0_Init ();                       // Initialize DAC0
   DAC1_Init ();                       // Initialize DAC1   

   TIMER3_Init(SYSCLK/SAMPLE_RATE_DAC);// Initialize Timer4 to overflow 
                                       // <SAMPLE_RATE_DAC> times per
                                       // second

   EA = 1;                             // Enable global interrupts
   
   while(1) {}                         // Wait for interrupt
} 

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TIMER3_ISR
//-----------------------------------------------------------------------------
//
// This ISR is called on Timer3 overflows.  Timer3 is set to auto-reload mode
// and is used to schedule the DAC output sample rate in this example.
// Note that the values written to the DACs during this ISR call is
// actually transferred to the DACs at the next Timer3 overflow.
//
//-----------------------------------------------------------------------------
void TIMER3_ISR (void) interrupt 14
{
   TMR3CN &= ~0x80;                   // Clear Timer3 overflow flag 
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
// This routine initializes the IDAC pins and enables the crossbar.
//
// P1.6    analog    IDA0
// P1.7    analog    IDA1
//
//-----------------------------------------------------------------------------

void PORT_Init (void)
{
    P1MDIN    = 0x3F;                  // P1.6 and P1.7 are analog pins
    P1SKIP    = 0xC0;                  // Skip P1.6 and P1.7 on crossbar
    XBR1      = 0x40;                  // Enable crossbar
}

//-----------------------------------------------------------------------------
// DAC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure DAC0 to update on Timer3 overflows and disable the the VREF buffer
//
//-----------------------------------------------------------------------------

void DAC0_Init(void)
{
   REF0CN &= ~0x01;                    // Disable internal vref

   IDA0CN = 0xB3;                      // Enable IDA0 for 2.0 mA output
                                       // updated on Timer3 overflows
}

//-----------------------------------------------------------------------------
// DAC1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure DAC1 to update on Timer3 overflows and disable the the VREF buffer
//
//-----------------------------------------------------------------------------

void DAC1_Init(void)
{
   REF0CN &= ~0x01;                    // Disable internal vref

   IDA1CN = 0xB3;                      // Enable IDA1 for 2.0 mA output
                                       // updated on Timer3 overflows
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
   TMR3CN  = 0x00;                     // Resets Timer3, 
                                       // Sets to 16 bit mode
   CKCON  |= 0x40;                     // Use system clock
   TMR3RL  = -counts;                  // Initial reload value

   TMR3    = 0xffff;                   // Sets timer to reload automatically
   EIE1   |= 0x80;                     // Enable Timer3 interrupts
   TMR3CN  = 0x04;                     // Start Timer3
}

//-----------------------------------------------------------------------------
// Support Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Set_DACs
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Calculates the update values for the two DACs using SINE_TABLE.  The DACs
// will actually be updated upon the next Timer3 interrupt.  Until that time,
// the DACs will hold their current value.
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

   // Add a DC bias to change the the rails from a bipolar (-128 to 127)
   // to unipolar (0 to 255)
   // Note: the XOR with 0x80 translates the bipolar quantity into 
   // a unipolar quantity.

   IDA0 = SIN_temp ^ 0x80;             // Update DAC values
   IDA1 = COS_temp ^ 0x80;           
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------