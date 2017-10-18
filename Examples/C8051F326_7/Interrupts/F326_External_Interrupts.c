//-----------------------------------------------------------------------------
// F326_External_Interrupts.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This software shows the necessary configuration to use External Interrupt 0
// (/INT0) as an interrupt source.  The code executes the init routines 
// and then spins in an infinite while() loop.  If the button P2.0 on the 
// target board is pressed, it will pull P0.0 low causing an edge-triggered 
// /INT0 interrupt.  The interrupt Interrupt Service Routine toggles an LED.
//
// Pinout:
//
// P0.0 - /INT0
//
// P2.0 - SW1 (Switch 1)
// P2.2 - LED1
//
// How To Test:
//
// 1) Compile and download code to a 'F326 target board.
// 2) On the target board, connect P2.0_SW on J3 to P0.0
// 3) Press the P2.0 switch to toggle the P2.2 LED
//
// Target:         C8051F326
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (GP)
//    -10 JULY 2007
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include <C8051F326.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             12000000    // Clock speed in Hz

sbit SW1 = P2^0;                       // Push-button switch on board
sbit LED1 = P2^2;                      // Green LED

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);           // Configure the system clock
void Port_Init (void);                 // Configure the Crossbar and GPIO
void Ext_Interrupt_Init (void);        // Configure /INT0

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   Oscillator_Init();                  // Initialize the system clock
   Port_Init ();                       // Initialize crossbar and GPIO
   Ext_Interrupt_Init();               // Initialize External Interrupts

   EA = 1;

   while(1);                           // Infinite while loop waiting for
                                       // an interrupt from /INT0
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
// This function configures the pins.
//
// Pinout:
//
// P0.0 - digital	open-drain 	/INT0
//
// P2.0 - digital	open-drain 	SW1 (Switch 1)
// P2.2 - digital	push-pull 	LED1
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   GPIOCN   = 0x40;                    // Enable input path and weak pullups

   P2MDOUT  = 0x04;                    // LED1 is a push-pull outputs
}

//-----------------------------------------------------------------------------
// Ext_Interrupt_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures and enables the /INT0 interrupt
//
// The configuration of /INT0 depends on the setting of the TMOD.3
//
// -----------------------------------------------------
// |                    |  TMOD.3 = 0  |  TMOD.3 = 1   |
// -----------------------------------------------------
// |  /INT0 Pinout      |  P0.0        |  P0.2         |
// |  Edge Sensitivity  |  Rising Edge |  Falling Edge |
// |  Level Senstivity  |  Active High |  Active Low   |
// -----------------------------------------------------
//
// See Section 6.3.2 of the 'F326 Datasheet for more details
//
// In this example TMOD.3 = 0, so /INT0 is assigned to P0.0, and is
// Rising Edge Sensitive.
//
//-----------------------------------------------------------------------------
void Ext_Interrupt_Init (void)
{
   EX0 = 1;                            // Enable /INT0 interrupt

   TMOD &= ~0x08;                      // Clear TMOD.3

   TCON |= 0x01;                       // /INT0 is edge sensitive

   // The /INT0 pending flag is set when the /INT0 configuration is changed,
   // so clear it now to prevent vectoring to the ISR immediately when
   // global interrupts are enabled
   IE0 = 0;                              

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
void INT0_ISR (void) interrupt 0
{
   LED1 = !LED1;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
