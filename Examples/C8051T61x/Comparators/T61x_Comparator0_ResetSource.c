//-----------------------------------------------------------------------------
// T61x_Comparator0_ResetSource.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Silicon Laboratories MCUs can use different mechanisms to reset the device.
//
// This software shows the necessary configuration to use Comparator0 as
// a reset source.  The code executes the initialization routines and then
// spins in an infinite while() loop.  If the button on P0.7 (on the target
// board) is pressed, then the comparators postive input (CP0+) input will drop 
// below the comparator's negative input (CP0-).  When this happens,  
// Comparator0 will cause the device to be reset.  
//
// When the code is waiting for the button to be pressed, the LED is on.
//
// Upon every device reset, the code checks the RSTSRC register to determine
// if the last reset was due to a comparator reset.  If it was a 
// comparator reset, the code will blink the LED constantly. 
//
// How To Test:
//
// 1) Compile and download code to a 'T610 target board
// 2) On the target board, connect P1.4 to P0.7 and P1.1 to P1.2
// 3) Place jumpers on J9 (LED - P1.0 and SW - P0.7)
// 4) Run the code and verify that the LED is on
// 5) Press the P0.7 switch (this causes a comparator reset)
// 6) Verify that the LED is blinking
//
//
// FID:            31X000002
// Target:         C8051T61x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (SM)
//    -23 JULY 2007
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <c8051T610_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK 24500000                // Clock speed in Hz
#define COMPARATOR_SETTLE  10          // Settle time in us

SBIT (LED, SFR_P1, 0);                 // LED==1 means ON
SBIT (SWITCH, SFR_P0, 7);              // SW1==0 means switch depressed

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);           // Configure the system clock
void PORT_Init (void);                 // Configure the Crossbar and GPIO
void Comparator0_Init (void);          // Configure Comparator0
void ResetSRC_Init(void);              // Configure Reset Sources                       
void Timer0_Delay (unsigned int us);   // Used as a delay

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void) 
{
   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   OSCILLATOR_Init();                  // Initialize the system clock
   PORT_Init ();                       // Initialize crossbar and GPIO
   Comparator0_Init();                 // Initialize Comparator0

   // When checking the RSTSRC register for the source of the last reset,
   // you must first also check that the PORSF bit is also '0'.  If the PORSF
   // bit is set ('1'), then all other bits in the RSTSRC register are invalid.

   // If last reset was due to comparator reset
   if (((RSTSRC & 0x02) == 0x00) && ((RSTSRC & 0x20)==0x20))
   {
      while (1)
      {
         Timer0_Delay (30000);         // Delay
         LED = ~LED;                   // Flip LED state
      }
   }
   // Last reset was due to a power-on reset or something else
   else
   {
      ResetSRC_Init();                 // Init comparators as reset source
      LED = 1;                         // Turn LED on
      
      while(1){}                       // Infinite while loop waiting for 
	                                     // comparator reset
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the precision internal
// oscillator as its clock source.
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void)
{
   OSCICN |= 0x03;                     // SYSCLK is internal osc.
}                                      // running at 24.5 MHz

//-----------------------------------------------------------------------------
// PORT_Init
//
// Return Value : None
// Parameters   : None
//
//  This function configures the crossbar and GPIO ports.
//
// P0.7   digital   open-drain    Switch
// P1.4   digital   push-pull     Comparator + Input
// P1.1   digital   open-drain    Comparator - Input
// P1.0   digital   push-pull     LED
// P1.2   digital   push-pull     Shorted to P1.1

//-----------------------------------------------------------------------------

void PORT_Init (void)
{
   XBR0     = 0x00;                             
   XBR1     = 0x40;                    // Enable crossbar and weak pull-ups
   P1SKIP   = 0x12;                    // Skip Comparator Inputs on XBR
   P1MDIN  &= ~0x12;                   // Comparator 0 inputs (P1.4 and P1.1)
                                       // are analog inputs.
   P1MDOUT  = 0x05;                    // LED / P1.2 are push-pull output
   P1      |= 0x04;                    // Drive Vdd out of P1.2 to supply
                                       // the Comparator - Input
}

//-----------------------------------------------------------------------------
// Comparator0_Init
//
// Return Value : None
// Parameters   : None
//
//  This function configures the comparator to pins P1.0 and P1.1
//-----------------------------------------------------------------------------

void Comparator0_Init (void)
{
   CPT0CN = 0x80;                      // Comparator enabled
   CPT0MX = 0x01;                      // P1.1 = Negative Input  
                                       // P1.4 = Positive Input 
                                       
   Timer0_Delay (COMPARATOR_SETTLE);   // Allow CP0 output to settle for 10 us

   CPT0CN &= ~0x30;                    // Clear CP0 Flags
}

//-----------------------------------------------------------------------------
// ResetSRC_Init
//
// Return Value : None
// Parameters   : None
//
// This function configures the comparator as a reset source
//-----------------------------------------------------------------------------

void ResetSRC_Init (void)
{
   RSTSRC = 0x20;                      // Comparator 0 -> reset source
}                                               

//-----------------------------------------------------------------------------
// Timer0_Delay
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) unsigned int us - number of microseconds of delay
//                        range is full range of integer: 0 to 65335
//
// Configure Timer0 to delay <us> microseconds before returning.
//-----------------------------------------------------------------------------

void Timer0_Delay (unsigned int us)
{
   unsigned i;                         // Millisecond counter

   TCON  &= ~0x30;                     // STOP Timer0 and clear overflow flag
   TMOD  &= ~0x0f;                     // Configure Timer0 to 16-bit mode
   TMOD  |=  0x01;
   CKCON |=  0x04;                     // Timer0 counts SYSCLKs

   for (i = 0; i < us; i++)            // Count microseconds
   {          
      TCON &= ~0x10;                   // STOP Timer0
      TH0 = (-SYSCLK/1000000) >> 8;    // Set Timer0 to overflow in 1us
      TL0 = -SYSCLK/1000000;
      TCON |= 0x10;                    // START Timer0
      while (TF0 == 0);                // Wait for overflow
      TCON &= ~0x20;                   // Clear overflow indicator
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
