//-----------------------------------------------------------------------------
// T622_LFO_Idle.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The program demonstrates the Low Frequency internal oscillator and power
// savings mode operation on the 'T622/3 and 'T326/7. The MCU will sleep in low
// power idle mode with only the low frequency oscillator and timer T0 running.
// When the MCU wakes up it will switch to a 12 MHz clock and turn on an LED
// and wait 100 ms. The 100 ms delay also uses T0 and the low frequency
// oscillator but does not use idle mode or interrupts.
//
// How To Test:
//
// 1) Download code to a 'T622 on a 'T62x Development Board.
// 2) Make sure the P1.2 LED jumper is in place on J10.
// 3) Run the code.
// 4) The P1.2 LED should blink once a second.
//
//
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (TP)
//    -02 SEP 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T622_defs.h"            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SLEEP_TIME 10000               // Sleep 1 second
#define BLINK_TIME  1000               // Blink 100 ms

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

SBIT (LED, SFR_P1, 2);

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);

void lowPowerSleep (unsigned int);
void configMCU_Sleep (void);
void configMCU_Wake (void);
void delay (unsigned int);

//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD  &= ~0x40;                   // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init();
   Port_Init();

   EA = 1;                             // Set global interrupt enable

   while(1)
   {
      lowPowerSleep(SLEEP_TIME);
      LED = 1;
      delay(1000);
      LED = 0;
   }
}

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Port_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// Pinout:
//
// P1.2 push-pull output LED
//
//-----------------------------------------------------------------------------
void Port_Init ()
{
   // Configure Port IO here
   P1MDOUT = 0x04;                     // Enable P1.2 as a push-pull output
   XBR1 |= 0x40;                       // Enable crossbar
}

//-----------------------------------------------------------------------------
// Oscillator_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//-----------------------------------------------------------------------------
void Oscillator_Init ()
{
   OSCICN =  0x83;                     // Enable 12 MHz int osc
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// lowPowerSleep ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will put the MCU into low power sleep mode using a T0
// interrupt to wake up the MCU. In sleep mode the MCU will be running off
// the internal low frequency oscillator running at 10 kHz with all of the
// peripherals disabled except T0.
//
//-----------------------------------------------------------------------------
void lowPowerSleep (unsigned int time)
{
   unsigned char restoreIE;
   unsigned char restoreEIE1;

   EA = 0;                             // Clear global interrupt enable

   // Save interrupt state
   restoreIE = IE;                     // Preserve IE register
   restoreEIE1 = EIE1;                 // Preserve EIE1 register

   // Disable all interrupts
   IE = 0x00;                          // Disable interrupts
   EIE1 = 0x00;                        // Disable extended interrupts

   configMCU_Sleep ();                 // Configure for Sleep

   // Init T0 to wake up MCU

   TCON &= ~0x30;                      // Stop T0 & clear flag
   TMOD &= ~0x0F;                      // Clear T0 nibble of TMOD
   TMOD |= 0x01;                       // T0 16 bit counter mode
   CKCON |= 0x04;                      // T0 uses SYSCLK

   // Set T0 using time
   time = -time;
   TL0 = (unsigned char)(time & 0x00FF);
   TH0 = (unsigned char)(time >> 8);

   // Comment out this line to measure steady state idle current
   //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
   IE |= 0x82;                         // Enable T0 & global interrupts
   //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   // Switch to low freq oscillator
   OSCLCN = 0x80;                      // LFO is 10 kHz
   CLKSEL = 0x04;                      // Switch to LFO
   OSCICN = 0x00;                      // Stop internal osc to save power

   TCON |= 0x10;                       // Enable T0

   // Go to IDLE mode
   PCON |= 0x01;                       // Idle mode
   PCON = PCON;

   //--Wake--//

   configMCU_Wake ();

   // Restore MCU pin configuration to portInit state & restore LEDs
   EIE1 = restoreEIE1;                 // Restore ext interrupts
   IE = restoreIE;                     // Restore interrupts
   EA = 1;                             // Set global interrupt enable
}

//-----------------------------------------------------------------------------
// configMCU_Sleep ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the MCU for low power sleep disabling the crossbar,
// enabling weak pull-ups, and turning off MCU peripherals. The lowest possible
// sleep current can be obtained by turning off the Voltage monitor.
//
//-----------------------------------------------------------------------------
void configMCU_Sleep (void)
{
   XBR1 &= ~0xC0;                      // Disable crossbar, enabled weak pull ups

   // disable all peripherals for low power sleep

   SCON0 &= ~0x10;                     // Disable UART0
   TCON &= ~0x40;                      // Disable T1
   TMR2CN &= ~0x04;                    // Disable T2

   // Disable any additional peripherals here

   VDM0CN &= ~0x80;                    // Disable voltage monitor
}

//-----------------------------------------------------------------------------
// configMCU_Wake ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will configure the MCU for wake mode operation.
// Enable additional peripherals as needed.
//
//-----------------------------------------------------------------------------
void configMCU_Wake (void)
{
   VDM0CN |= 0x80;                     // Enable voltage monitor

   // Enable any additional peripherals here

   XBR1 |= 0x40;                       // Enable crossbar
}

//-----------------------------------------------------------------------------
// delay ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : time - (10000 = 1 second)
//
// This function will implement a delay using T0 and the LFO 10 kHz clock.
// A delay time of 10000 counts is equal to one second. This function modifies
// the system clock and exits with a 24.5 MHz clock.
//
//-----------------------------------------------------------------------------
void delay (unsigned int time)
{
   TCON &= ~0x30;                      // Stop T0 & clear flag
   TMOD &= ~0x0F;                      // Clear T0 nibble of TMOD
   TMOD |= 0x01;                       // T0 16 bit counter mode
   CKCON |= 0x04;                      // T0 uses SYSCLK

   // Set T0 using time
   time = -time;
   TL0 = (unsigned char)(time & 0x00FF);
   TH0 = (unsigned char)(time >> 8);

   // Switch to low freq oscillator
   OSCLCN = 0x80;                      // LFO is 10 kHz
   CLKSEL = 0x04;                      // Switch to LFO
   TCON |= 0x10;                       // Enable T0
   while (!TF0);                       // Wait for TOF
   TCON &= ~0x30;                      // Stop T0 & clear flag
   OSCICN =  0x83;                     // Enable 12 MHz int osc
   CLKSEL = 0x00;                      // Use int osc
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer0_ISR ()
//-----------------------------------------------------------------------------
INTERRUPT(Timer0_ISR, INTERRUPT_TIMER0)
{
   OSCICN = 0x83;                      // Enable 12 MHz int osc
   while ((OSCICN & 0x40) == 0);       // Wait for lock bit
   CLKSEL = 0x00;                      // Use int osc
   TCON &= ~0x10;                      // Stop timer T0
   IE &= ~0x02;                        // Disable T0 interrupt
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
