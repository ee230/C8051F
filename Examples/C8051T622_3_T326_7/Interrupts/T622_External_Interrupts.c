//-----------------------------------------------------------------------------
// T622_External_Interrupts.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This software shows the necessary configuration to use External Interrupt 0
// (/INT0) or External Interrupt 1 (/INT1) as an interrupt source.  The code
// executes the initialization routines and then spins in an infinite while()
// loop.  If the button on SW1 or SW2 (on the target board) is pressed, then the
// edge-triggered /INT0 and /INT1 input on P0.0 and P0.1  will cause an interrupt 
// and toggle the LED.
//
// Pinout:
//
// P0.0 - /INT0, SW2 (Switch 2)
// P0.1 - /INT1, SW1 (Switch 1)
//
// P0.6 - LED1
//
// P1.2 - LED2
//
// How To Test:
//
// 1) Compile and download code to a 'T622/3 device on a 'T62x target board.
// 2) On the mother board, connect the SW2 signal on J9 to P0.0 for /INT0 with
//    a wire. Connect P0.1 to SW1 on J9 using a jumper. Connect P1.2 to LED2 and 
//    P0.6 to LED1 using jumpers on J10.
// 3) Press the switches.  Every time a switch is pressed, the P0.6 or P1.2
//    LEDs should toggle.
//
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
//
//
// Release 1.1
//    -Modified testing instructions (ES)
//    -13 AUG 2010
// Release 1.0
//    -Initial Revision (TP)
//    -02 SEP 2008
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T622_defs.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             12000000    // Clock speed in Hz

SBIT (SW1, SFR_P0, 1);                 // Push-button switch on board
SBIT (SW2, SFR_P1, 0);                 // Push-button switch on board
SBIT (LED1, SFR_P0, 6);                // Green LED
SBIT (LED2, SFR_P1, 2);                // Green LED

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);           // Configure the system clock
void Port_Init (void);                 // Configure the Crossbar and GPIO
void Ext_Interrupt_Init (void);        // Configure External Interrupts (/INT0
                                       // and /INT1)

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
                                       // at 12 MHz.
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
// P0.0 - digital   open-drain  /INT0
// P0.1 - digital   open-drain  /INT1 and SW1 (Switch 1)
//
// P0.6 - digital   push-pull   LED1
//
// P1.0 - digital   open-drain  SW2 (Switch 2)
//
// P1.2 - digital   push-pull   LED2
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   XBR1     = 0x40;                    // Enable crossbar and weak pullups

   P0MDOUT  = 0x40;                    // LED1 is a push-pull output
   P1MDOUT  = 0x04;                    // LED2 is a push-pull output
}

//-----------------------------------------------------------------------------
// Ext_Interrupt_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures and enables /INT0 and /INT1 (External Interrupts)
// as negative edge-triggered.
//
//-----------------------------------------------------------------------------
void Ext_Interrupt_Init (void)
{
   TCON = 0x05;                        // /INT 0 and /INT 1 are edge triggered

   IT01CF = 0x10;                      // /INT0 active low; /INT0 on P0.0;
                                       // /INT1 active low; /INT1 on P0.1

   EX0 = 1;                            // Enable /INT0 interrupts
   EX1 = 1;                            // Enable /INT1 interrupts
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// /INT0 ISR
//-----------------------------------------------------------------------------
//
// Whenever a negative edge appears on P0.0, LED1 is toggled.
// The interrupt pending flag is automatically cleared by vectoring to the ISR
//
//-----------------------------------------------------------------------------
INTERRUPT (INT0_ISR, INTERRUPT_INT0)
{
   LED1 = !LED1;
}

//-----------------------------------------------------------------------------
// /INT1 ISR
//-----------------------------------------------------------------------------
//
// Whenever a negative edge appears on P0.1, LED2 is toggled.
// The interrupt pending flag is automatically cleared by vectoring to the ISR
//
//-----------------------------------------------------------------------------
INTERRUPT(INT1_ISR, INTERRUPT_INT1)
{
   LED2 = !LED2;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
