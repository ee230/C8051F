//-----------------------------------------------------------------------------
// Si101x_External_Interrupts.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This software shows the necessary configuration to use External Interrupt 0
// (/INT0) or External Interrupt 1 (/INT1) as an interrupt source.  The code
// executes the initialization routines and then spins in an infinite while()
// loop.  If the button on P0.2 (on the target board) is pressed, then the
// edge-triggered /INT0 input on P0.2 will cause an interrupt and toggle the
// yellow LED. If the button on P0.3 is pressed, then the edge-triggered
// /INT1 input on P0.3 will cause an interrupt and toggle the Red LED.
//
// Pinout:
//
// P0.2 - /INT0 (Switch 2)
// P0.3 - /INT1 (Swtich 3)
//
// P1.5 - Red LED
// P1.6 - Yellow LED
//
// How To Test:
//
// 1) Compile and download code to a target board.
// 1) Download code to a target board
// 2) Ensure that the Switch and LED pins on the J8 header are
//    properly shorted.
// 3) Press the switches.  Every time a switch is pressed, one of
//    the LEDs should toggle.
//
// Target:         Si101x
// Tool chain:     Generic
// Command Line:   None
//
//
//
// Release 1.0
//    -Initial Revision (FB)
//    - 18 MAY 2010
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <Si1010_defs.h>               // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (RED_LED,    SFR_P1, 5);          // '0' means ON, '1' means OFF
SBIT (YELLOW_LED, SFR_P1, 6);          // '0' means ON, '1' means OFF
SBIT (SW2,        SFR_P0, 2);          // SW2 == 0 means switch depressed
SBIT (SW3,        SFR_P0, 3);          // SW3 == 0 means switch depressed


//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             20000000    // Clock speed in Hz

#define LED_ON  0
#define LED_OFF 1


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
// This function initializes the system clock to use the internal low power
// oscillator at its maximum frequency.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   CLKSEL = 0x04;                     // Select low power oscillator with a 
                                      // clock divider value of 1 (20MHz)
}


//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
// Pinout:
//
// P0.2   digital   open-drain    Switch 2
// P0.3   digital   open-drain    Switch 3
// P1.5   digital   push-pull     Red LED
// P1.6   digital   push-pull     Orange LED
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDIN |= 0x0C;                     // P0.2, P0.3 are digital
   P1MDIN |= 0x60;                     // P1.5, P1.6 are digital

   P0MDOUT &= ~0x0C;                   // P0.2, P0.3 are open-drain
   P1MDOUT |= 0x60;                    // P1.5, P1.6 are push-pull

   P0     |= 0x0C;                     // Set P0.2, P0.3 latches to '1'

   XBR2    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
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

   IT01CF = 0x32;                      // /INT0 active low; /INT0 on P0.2;
                                       // /INT1 active low; /INT1 on P0.3

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
// Whenever a negative edge appears on P0.2, YELLOW_LED is toggled.
// The interrupt pending flag is automatically cleared by vectoring to the ISR
//
//-----------------------------------------------------------------------------
INTERRUPT(INT0_ISR, INTERRUPT_INT0)
{
   YELLOW_LED = !YELLOW_LED;
}

//-----------------------------------------------------------------------------
// /INT1 ISR
//-----------------------------------------------------------------------------
//
// Whenever a negative edge appears on P0.3, RED_LED is toggled.
// The interrupt pending flag is automatically cleared by vectoring to the ISR
//
//-----------------------------------------------------------------------------
INTERRUPT(INT1_ISR, INTERRUPT_INT1)
{
   RED_LED = !RED_LED;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
