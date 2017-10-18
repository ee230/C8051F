//-----------------------------------------------------------------------------
// F41x_DACs_SineCosine.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program outputs sine and cosine waveforms using the hardware DACs
// on the C8051F410 microcontroller.  The waveforms are output on pins
// P0.0/IDA0 and P0.1/IDA1.
//
// The output of the DACs is updated upon a Timer3 interrupt.  The Timer3 ISR
// then calculates the output of the DACs for the next interrupt.
//
// The frequency of the output waveforms is set by the #define <FREQUENCY>.
//
//
// How To Test:
//
// 1) Download the code to an C8051F410 Development Board
// 2) Check that the J6 and J16 shorting blocks are not installed.  This
//    ensures that the IDAC outputs are not connected to any other pins
// 3) Check that the J13 and J14 short blocks are installed. This connects
//    the DAC outputs to resistors so that the output current is converted
//    to a voltage
// 4) Connect two oscillscope probes to pins P0.0/IDA0 and P0.1/IDA1.
//    These pins are available on both the J2 and J11 headers.
// 5) Confirm that there are two waveforms with the output of DAC1 leading
//    the output DAC0 by 90 degrees.  The frequency of both waveforms should
//    be close to frequency defined by <FREQUENCY>
//
// FID:            41X000036
// Target:         C8051F41x
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0
//    -Initial Revision (GP)
//    -09 AUG 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F410_defs.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK          24500000       // Internal oscillator frequency in Hz

#define SAMPLE_RATE_DAC 100000L         // DAC sampling rate in Hz
#define PHASE_PRECISION 65536          // range of phase accumulator

#define FREQUENCY       1000           // Frequency of output waveform in Hz

// <PHASE_ADD> is the change in phase between DAC samples; It is used in
// the set_DACs routine.

unsigned int PHASE_ADD = FREQUENCY * PHASE_PRECISION / SAMPLE_RATE_DAC;

int code SINE_TABLE[256] =
{
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
   PORT_Init ();                       // Initialize crossbar and ports
   DAC0_Init ();                       // Initialize DAC0
   DAC1_Init ();                       // Initialize DAC1

   TIMER3_Init(SYSCLK/SAMPLE_RATE_DAC);// Initialize Timer3 to overflow at
                                       // <SAMPLE_RATE_DAC> times per second

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
// Note that the values written to the DACs during this ISR call are
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
   OSCICN  = 0x87;                     // Set clock to 24.5 MHz
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
// P0.0    analog    IDA0
// P0.1    analog    IDA1
//
//-----------------------------------------------------------------------------

void PORT_Init (void)
{
   P0MDIN    = 0xFC;                   // Configure P0.0 and P0.1 to analog
   P0SKIP    = 0x03;                   // Skip P0.0 and P0.1 on the crossbar
   XBR1      = 0x40;                   // Enable Crossbar
}

//-----------------------------------------------------------------------------
// DAC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure DAC0 to update on Timer3 overflows and enable VDD as VREF
//
//-----------------------------------------------------------------------------

void DAC0_Init(void)
{
   REF0CN = 0x0A;                      // Enable VDD as VREF

   IDA0CN = 0xB3;                      // Enable IDA0 for 2.0 mA output
                                       // left-justified
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
   REF0CN = 0x0A;                      // Enable VDD as VREF

   IDA1CN = 0xB3;                      // Enable IDA0 for 2.0 mA output
                                       // left-justified
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

   // Add a DC bias to change the the rails from a bipolar (-32768 to 32767)
   // to unipolar (0 to 65535)
   // Note: the XOR with 0x8000 translates the bipolar quantity into
   // a unipolar quantity.

   IDA0 = SIN_temp ^ 0x8000;             // Update DAC values
   IDA1 = COS_temp ^ 0x8000;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------