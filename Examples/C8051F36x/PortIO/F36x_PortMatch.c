//-----------------------------------------------------------------------------
// F36x_PortMatch.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This software shows the necessary configuration to use Port Match as an 
// interrupt source.  The code executes the initialization routines and then 
// spins in an infinite while() loop.  If the button on P1.4 (on the target 
// board) is pressed, then the Port Match Interrupt will cause the LED to 
// toggle.
//
// Pinout:
//
// Port Match:
// P1.4
// P2.4
//
// P3.0 - SW1 (Switch 1) ----> P1.4 
// P3.1 - SW2 (Switch 2) ----> P2.4
// P2.1 - LED1
// P2.3 - LED2
//
// How To Test:
//
// 1) Compile and download code to a 'F36x target board.
// 2) On the target board, connect P3.0 to P1.4 & P3.1 to P2.4.
// 3) Press the switches.  Every time a switch is pressed, the P3.2 or P3.3
//    LED should toggle.
//
// Target:         C8051F36x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (SM)
//    -13 JULY 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F360.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK    24500000             // Sleep 1 second

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void Interrupt_Init (void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

sbit SW1 = P3^0;                       // Push-button switch on board
sbit SW2 = P3^1;                       // Push-button switch on board
sbit LED1 = P3^2;                      // Green LED
sbit LED2 = P3^3;                      // Green LED

//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD  &= ~0x40;                   // Disable watchdog timer

   Oscillator_Init ();                 // Initialize 24.5 MHz oscillator
   Port_Init ();                       // Initialize IO ports
   Interrupt_Init ();                  // Initialize Port Match Interrupts
  
   EA = 1;                             // Set Global interrupt enable
   
   SFRPAGE = LEGACY_PAGE;

   P1MAT = 0x10;                       // Set up the port-match value on P2
   P1MASK = 0x10;                      // An event will occur if
                                       // (P1 & P1MASK) != (P1MAT & P1MASK)
                                       // The SW1 pin = 1 if the switch isn't
                                       // pressed
   P2MAT = 0x10;                       // Set up the port-match value on P2
   P2MASK = 0x10;
     
   while (1) {}                        // Wait for interrupt
}

//-----------------------------------------------------------------------------
// Port_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P3.0 - digital	open-drain 	SW1 (Switch 1)
// P3.1 - digital	open-drain 	SW2 (Switch 2)
// P3.2 - digital	push-pull 	LED1
// P3.3 - digital	push-pull	LED2
//
//-----------------------------------------------------------------------------
void Port_Init ()
{  
   unsigned char SFRPAGE_save = SFRPAGE;      // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   P1MDOUT &= ~0x10;                   // P1.4 is open drain  
   P3MDOUT |= 0x0C;                    // configure P3.2,P3.3 to push-pull

   XBR1 |= 0x40;                       // Enable crossbar

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 24.5 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE;    // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN |= 0x03;                     // Initialize internal oscillator to
                                       // highest frequency

   RSTSRC  = 0x04;                     // Enable missing clock detector
   
   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Interrupt_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function enables Port Match Interrupts.
//
//-----------------------------------------------------------------------------
void Interrupt_Init (void)
{
   EIE2 |= 0x02;                          
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORTMATCH_ISR
//-----------------------------------------------------------------------------
//
// Whenever an edge appears on P1.4, LED1 is toggled.
//
//-----------------------------------------------------------------------------
void PORTMATCH_ISR (void) interrupt 16
{
   // If the Port Match event occured on Port 1 than LED1 is toggled
   if((P1 & P1MASK) != (P1MAT & P1MASK))  {
      LED1 = !LED1; }
      
   // Waiting for switch 1 to be released
   while((P1 & P1MASK) != (P1MAT & P1MASK));
   
   // If the Port Match event occured on Port 0 than LED2 is toggled
   if((P2 & P2MASK) != (P2MAT & P2MASK))  {
      LED2 = !LED2;  }
   
   // Waiting for switch 2 to be released
   while((P2 & P2MASK) != (P2MAT & P2MASK));
}


//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
