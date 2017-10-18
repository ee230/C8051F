//-----------------------------------------------------------------------------
// T630_External_Interrupts.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This software shows the necessary configuration to use External Interrupt 0
// (/INT0) as an interrupt source.  The code executes the initialization 
// routines and then spins in an infinite while() loop.  If the button on P0.7
// (on the target board) is pressed, then the edge-triggered /INT0 input on 
// P0.7 will cause an interrupt and toggle the LED.
//
// Pinout:
//
// P0.7 - /INT0 and SW1 (Switch 1)
// P1.3 - LED
//
// How To Test:
//
// 1) Compile and download code to a 'T630 target board.
// 2) Connect P0.7 to the switch using J9.
// 3) Connect P1.3 to the LED using J9.
// 3) Press the switch.  Every time the switch is pressed, the P1.3
//    LED should toggle.
//
// Target:         C8051T630
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (BD)
//    -23 JAN 2008
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051T630_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             245000000   // Clock speed in Hz

SBIT (LED, SFR_P1, 3);                 // LED==1 means ON
SBIT (SW1, SFR_P0, 7);                 // SW1==0 means switch depressed

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);           // Configure the system clock
void Port_Init (void);                 // Configure the Crossbar and GPIO
void Ext_Interrupt_Init (void);        // Configure External Interrupts (/INT0
                                       // and /INT1)
INTERRUPT_PROTO (INT0_ISR, INTERRUPT_INT0);
INTERRUPT_PROTO (INT1_ISR, INTERRUPT_INT1);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   Oscillator_Init();                  // Initialize the system clock
   Port_Init ();                       // Initialize crossbar and GPIO
   Ext_Interrupt_Init();               // Initialize External Interrupts

   EA = 1;

   while(1);                           // Infinite while loop waiting for
                                       // an interrupt from /INT0 or /INT1
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
// This routine initializes the system clock to use the precision internal
// oscillator as its clock source.
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x83;                      // Set internal oscillator to run
                                       // at its maximum frequency
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
// Pinout:
//
//
// P0.7 - digital   open-drain  /INT0 and SW1 (Switch 1)
// P1.3 - digital   push-pull   LED2
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   XBR1     = 0x40;                    // Enable crossbar and weak pullups

   P0SKIP   = 0x80;
   P1MDOUT  = 0x08;                    // LED is a push-pull output
}

//-----------------------------------------------------------------------------
// Ext_Interrupt_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures and enables /INT0 (External Interrupt)
// as negative edge-triggered.
//
//-----------------------------------------------------------------------------
void Ext_Interrupt_Init (void)
{
   TCON = 0x01;                        // /INT0  edge triggered

   IT01CF = 0x07;                      // /INT0 active low; /INT0 on P0.0;
                                   

   EX0 = 1;                            // Enable /INT0 interrupts
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// /INT0 ISR
//-----------------------------------------------------------------------------
//
// Whenever a negative edge appears on P0.7, LED1 is toggled.
// The interrupt pending flag is automatically cleared by vectoring to the ISR
//
//-----------------------------------------------------------------------------
INTERRUPT(INT0_ISR, INTERRUPT_INT0)
{
   LED = !LED;
}



//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
