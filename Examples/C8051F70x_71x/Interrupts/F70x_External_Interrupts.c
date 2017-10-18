//-----------------------------------------------------------------------------
// F70x_External_Interrupts.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This software shows the necessary configuration to use External Interrupts 0
// and 1 as interrupt sources.  The code executes the initialization 
// routines and then spins in an infinite while() loop.  
// If the button on P1.1 (on the target board) is pressed, then the
// edge-triggered /INT0 input on P0.0 and P0.1 will cause an interrupt and toggle
// the LED.
//
// Pinout:
//
// P0.0 - /INT0
// P1.0 - LED
// P1.1 - SW (Switch)

// How To Test:
//
// 1) Compile and download code to a 'F70x target board.
// 2) On the target board, connect P1.1_SW signal on J12 to P0.0
//    for /INT0 or P0.1 for /INT1.
// 3) Press the switch.  Every time a switch is pressed, the P1.0
//    LED should toggle.
//
// Target:         C8051F70x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (PD)
//    -07 JUL 2008
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include <compiler_defs.h>             // compiler declarations
#include <C8051F700_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             24500000    // Clock speed in Hz

SBIT (LED,    SFR_P1, 0);              // '0' means ON, '1' means OFF
SBIT (SW,     SFR_P1, 1);              // '0' means ON, '1' means OFF

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);           // Configure the system clock
void Port_Init (void);                 // Configure the Crossbar and GPIO
void Ext_Interrupt_Init (void);        // Configure External Interrupts (/INT0
                                       // and /INT1)
INTERRUPT_PROTO(Ext_Interrup0t_ISR, INTERRUPT_INT0);
INTERRUPT_PROTO(Ext_Interrupt1_ISR, INTERRUPT_INT1);
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   WDTCN = 0xDE;                       // Disable Watchdog timer
   WDTCN = 0xAD;

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
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal
// oscillator.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   // Save the current SFRPAGE
   U8 SFRPAGE_save = SFRPAGE;

   SFRPAGE = CONFIG_PAGE;
   
   OSCICN |= 0x80;                     // Enable the precision internal osc.
      
   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor enabled.

   CLKSEL = 0x00;                      // Select precision internal osc. 
                                       // divided by 1 as the system clock

   SFRPAGE = SFRPAGE_save;  
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
// P0.1 - digital   open-drain  /INT1
//
// P1.0 - digital   open-drain  SW
// P1.1 - digital   push-pull   LED
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   // Save the current SFRPAGE
   U8 SFRPAGE_save = SFRPAGE;

   SFRPAGE = CONFIG_PAGE;

   XBR1     = 0x40;                    // Enable crossbar and weak pullups

   P1MDOUT  = 0x01;                    // LED1 and LED2 are push-pull outputs

   SFRPAGE = SFRPAGE_save;
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
   // Save the current SFRPAGE
   U8 SFRPAGE_save = SFRPAGE;

   SFRPAGE = CONFIG_PAGE;

   TCON = 0x05;                        // /INT 0, /INT1 are edge triggered

   IT01CF = 0x10;                      // /INT0 active low; /INT0 on P0.0;
                                       // /INT1 active low; /INT1 on P0.1

   EX0 = 1;                            // Enable /INT0 interrupts
   EX1 = 1;
   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// /Ext_Interrupt0_ISR
//-----------------------------------------------------------------------------
//
// Whenever a negative edge appears on P0.0, LED is toggled.
// The interrupt pending flag is automatically cleared by vectoring to the ISR
//
//-----------------------------------------------------------------------------
INTERRUPT(Ext_Interrupt0_ISR, INTERRUPT_INT0)
{
   LED = !LED;
}

//-----------------------------------------------------------------------------
// /Ext_Interrupt1_ISR
//-----------------------------------------------------------------------------
//
// Whenever a negative edge appears on P0.1, LED is toggled.
// The interrupt pending flag is automatically cleared by vectoring to the ISR
//
//-----------------------------------------------------------------------------
INTERRUPT(Ext_Interrupt1_ISR, INTERRUPT_INT1)
{
   LED = !LED;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
