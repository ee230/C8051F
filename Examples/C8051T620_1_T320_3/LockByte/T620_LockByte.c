//-----------------------------------------------------------------------------
// T620_LockByte.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The program blinks an LED and sets the value of the Lock Byte.  The 'T620/1
// Security Lock Byte write and read locks the EPROM memory.  If any of the
// write lock bits (7-4) or read lock bits (3-0) are cleared, the entire EPROM
// memory will be write or read locked.
//
// The LockByte.c code is be located using the Keil command-line linker flags.
// Please see the project settings under Project->Tool Chain Integration...
// ->Linker tab for more information.
//
// This program is part of the "T620_LockByte" project.
//
// How To Test:
//
// 1) Open the "T620_LockByte.wsp" project in the Silicon Labs IDE.
// 2) Compile and download the code to a 'T620 on a 'T62x Development Board.
// 3) Make sure the P2.2 LED jumper is in place on J10.
// 4) Run the code.
// 5) Verify the code is running; P2.3 LED should blink once a second.
// 6) Stop the code in the IDE.
// 7) Open the View->Debug Windows->Code Memory window in the IDE.
// 8) Verify the LockByte settings are in effect:
//       - If the EPROM is read-locked, the IDE should be unable to display the
//         code at address 0x0000.
//       - If the EPROM is write-locked, the IDE should be unable to write to
//         the code memory.
//
// Note: This code is intended to be used with Keil using the Keil command-line
// flags to locate the LockByte.
//
// Target:         C8051T620/1 or C8051T320/1/2/3
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -27 JUN 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       12000000 / 8      // SYSCLK frequency in Hz

#define BLINK_RATE   10                // Timer2 Interrupts per second

SBIT (LED1, SFR_P2, 2);                // LED='1' means ON

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void Timer2_Init (int counts);

INTERRUPT_PROTO(Timer2_ISR, INTERRUPT_TIMER2);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init ();                 // Initialize system clock
   Port_Init ();                       // Initialize crossbar and GPIO

   Timer2_Init (SYSCLK / 12 / BLINK_RATE); // Init Timer2 to generate
                                           // interrupts at a 10Hz rate.

   EA = 1;                             // Enable global interrupts

   while (1) {}                        // Spin forever
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
// This function initializes the system clock to (12 Mhz / 8)
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x80;                      // Configure internal oscillator for
                                       // its lowest frequency
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P2.2   digital   push-pull     LED1
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   XBR0 = 0x00;                        // No digital peripherals selected
   XBR1 = 0x40;                        // Enable crossbar and weak pull-ups
   P2MDOUT |= 0x04;                    // Enable LED as a push-pull output
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1)  int counts - calculated Timer overflow rate
//                    range is postive range of integer: 0 to 32767
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
//-----------------------------------------------------------------------------
void Timer2_Init (int counts)
{
   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // Use SYSCLK/12 as timebase
   CKCON &= ~0x60;                     // Timer2 clocked based on T2XCLK;

   TMR2RL = -counts;                   // Init reload values
   TMR2 = 0xffff;                      // Set to reload immediately
   ET2 = 1;                            // Enable Timer2 interrupts
   TR2 = 1;                            // Start Timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer2 overflows.
//
//-----------------------------------------------------------------------------
INTERRUPT(Timer2_ISR, INTERRUPT_TIMER2)
{
   TF2H = 0;                           // Clear Timer2 interrupt flag
   LED1 = !LED1;                       // Change state of LED
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
