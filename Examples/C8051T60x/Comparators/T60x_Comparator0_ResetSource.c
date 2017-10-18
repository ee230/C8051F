//-----------------------------------------------------------------------------
// T60x_Comparator0_ResetSource.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Silicon Laboratories MCUs can use different mechanisms to reset the device.
//
// This software shows the necessary configuration to use Comparator0 as
// a reset source.  The code executes the initialization routines and then
// spins in an infinite while() loop.  If the button on P0.3 (on the target
// board) is pressed, then the comparators postive input (CP0+) input will drop
// below the comparator's negative input (CP0-).  When this happens,
// Comparator0 will cause the device to be reset.
//
// IMPORTANT NOTE: The negative input voltage (CP0-) for the comparator is
//    established on P0.1 using potentiometer R13. Changing R13 will adjust
//    the threshold where the comparator trips.
//
// When the code is waiting for the button to be pressed, the LED is on.
//
// Upon every device reset, the code checks the RSTSRC register to determine
// if the last reset was due to a comparator reset.  If it was a
// comparator reset, the code will blink the LED.
//
// How To Test:
//
// 1) Compile and download code to a T60x device
// 2) Declare the chip number you're using in CHIPNUM
// 3) On the target board, connect the pot to P0.1 via J7.
// 4) Connect the switch to P0.4 via J3 connection (P0.3 SW to P0.4).
// 5) Connect the LED on J3.
// 6) Adjust R13 to a middle location.
// 7) Run the code and verify that the LED is on
// 8) Press the P0.3 switch (this causes a comparator reset)
// 9) Verify that the LED is blinking
//
// Target:         C8051T60x
// Tool chain:     Keil C51 8.0 / Keil EVAL C51
// Command Line:   None
//
// Release 1.2 / 03 NOV 2008 (BD)
//    - Simplified hardware connections.
//
// Release 1.1 / 29 JUL 2008 (ADT)
//    - Updated to use compiler_defs.h
//    - Includes support for T606
//
// Release 1.0
//    -Initial Revision (GP)
//    -17 MAY 2006
//
//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051T600_defs.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define CHIPNUM   600                  // Define the chip number being used
                                       // Options: 600, 601, 602, 603,
                                       //          604, 605, 606

#define SYSCLK 24500000                // Clock speed in Hz
#define COMPARATOR_SETTLE  10          // Settle time in us

#if (CHIPNUM == 606)
 SBIT (LED, SFR_P0, 2);                // Green Led
#else
 SBIT (LED, SFR_P0, 6);
#endif

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);           // Configure the system clock
void PORT_Init (void);                 // Configure the Crossbar and GPIO
void Comparator0_Init (void);          // Configure Comparator0
void ResetSRC_Init(void);              // Configure Reset Sources
void Timer0_Delay (U16 us);            // Used as a delay

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   REG0CN |= 0x01;                     // OTP Power Controller Enable

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

       while(1);                        // Infinite while loop waiting for
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
// P0.1   analog                  Comparator - Input (connected to Pot)
//
// P0.4   analog                  Comparator + Input (connected to switch)
//
// For the T600:
// P0.6   digital   push-pull     LED
// For the T606:
// P0.2   digital   push-pull     LED
//-----------------------------------------------------------------------------

void PORT_Init (void)
{
   XBR2     = 0x40;                    // Enable crossbar and weak pull-ups
   P0MDIN  &= ~0x12;                   // Comparator 0 inputs (P0.1 and P0.4)
                                       // are analog inputs.
#if (CHIPNUM == 606)
   P0MDOUT  = 0x04;                    // LED is push-pull output
#else
   P0MDOUT  = 0x40;
#endif
}

//-----------------------------------------------------------------------------
// Comparator0_Init
//
// Return Value : None
// Parameters   : None
//
//  This function configures the comparator to pins P0.0 and P0.1
//-----------------------------------------------------------------------------

void Comparator0_Init (void)
{
   CPT0CN = 0x80;                      // Comparator enabled
   CPT0MX = 0x02;                      // P0.1 = Negative Input
                                       // P0.4 = Positive Input

   Timer0_Delay (COMPARATOR_SETTLE);   // Allow CP0 output to settle for 10 us
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

void Timer0_Delay (U16 us)
{
   U16 i;                              // Millisecond counter

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
