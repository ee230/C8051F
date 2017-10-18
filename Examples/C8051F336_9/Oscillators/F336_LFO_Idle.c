//-----------------------------------------------------------------------------
// F336_LFO_Idle.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The program demonstrates the Low Frequency internal oscillator and power
// savings mode operation on the F336. The MCU will sleep in low power idle
// mode with only the low frequency oscillator and timer T0 running. When
// the MCU wakes up it will switch to a 24.5 MHz clock and turn on an LED
// and wait 100 ms. The 100 ms delay also uses T0 and the low frequency
// oscillator but does not use idle mode or interrupts.
//
// How To Test:
//
// 1) Download code to a 'F336 target board.
// 2) Make sure the P1.3 LED jumper is in place.
// 3) Run the code.
// 4) The P1.3_LED should blink once a second.
//
//
// Target:         C8051F336
// Tool chain:     Keil C51 7.05 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (SM)
//    -01 AUG 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F336_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SLEEP_TIME 10000               // Sleep 1 second
#define BLINK_TIME  1000               // Blink 100 ms

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void main (void);
void OSCILLATOR_Init (void);
void PORT_Init (void);
void lowPowerSleep (unsigned int);
void configMCU_Sleep(void);
void configMCU_Wake(void);
void delay (unsigned int);
INTERRUPT_PROTO(T0_ISR, INTERRUPT_TIMER0);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

SBIT (LED, SFR_P1, 3);                 // LED==1 means ON

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   PCA0MD  &= ~0x40;
   PCA0MD   = 0x00;

   OSCILLATOR_Init ();
   PORT_Init ();

   EA = 1;                             // Set global interrupt enable

   while (1)
   {
      lowPowerSleep (SLEEP_TIME);
      LED = 1;
      delay (1000);
   }
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P1.3   push-pull   output   LED
//-----------------------------------------------------------------------------

void Port_Init ()
{
// configure Port IO here

   P1MDOUT  = 0x08;
   XBR1     |= 0x40;                    // Enable crossbar
}

//-----------------------------------------------------------------------------
// OSC_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the internal 24.5 MHz oscillator.
//-----------------------------------------------------------------------------

void OSCILLATOR_Init ()
{
   OSCICN    =  0x83;                  // Enable 24.5 MHz int osc
}

//-----------------------------------------------------------------------------
// lowPowerSleep ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : time - sleep time (10000 = 1 second)
//
// This function will put the MCU into low power sleep mode using a T0
// interrupt to wake up the MCU. In sleep mode the MCU will be running off
// the internal low frequency oscillator running at 10 kHz with all of the
// peripherals disabled except T0.
//-----------------------------------------------------------------------------

void lowPowerSleep (unsigned int time)
{
   unsigned char restoreIE;
   unsigned char restoreEIE1;

   EA = 0;                             // Clear global interrupt enable

   // save interrupt state
   restoreIE = IE;                     // Preserve IE register
   restoreEIE1 = EIE1;                 // Preserve EIE1 register

   // disable all interrupts
   IE       = 0x00;                    // Disable interrupts
   EIE1     = 0x00;                    // Disable extended interrupts

   configMCU_Sleep ();                 // Configure for Sleep

   //init T0 to wake up MCU

   TCON    &= ~0x30;                   // Stop T0 & clear flag
   TMOD    &= ~0x0F;                   // Clear T0 nibble of TMOD
   TMOD    |= 0x01;                    // T0 16 bit counter mode
   CKCON   |= 0x04;                    // T0 uses SYSCLK

   // set T0 using time
   time = -time;
   TL0      = (unsigned char)(time & 0x00FF);
   TH0      = (unsigned char)(time >> 8);

   // Comment out this line to measure stead state idle current
   //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
   IE      |= 0x82;                    // Enable T0 & global interrupts
   //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   //switch to low freq oscillator
   OSCLCN   = 0x80;                    // LFO is 10 kHz
   CLKSEL   = 0x02;                    // Switch to LFO
   OSCICN   = 0x00;                    // Stop internal osc to save power

   TCON    |= 0x10;                    // Enable T0

   // go to IDLE mode
   PCON    |= 0x01;                    // Idle mode
   PCON     = PCON;

   //--wake--//

   configMCU_Wake ();

   // restore MCU pin configuration to portInit state & restore LEDs
   EIE1     = restoreEIE1;             // Restore ext interrupts
   IE       = restoreIE;               // Restore interrupts
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
// enabling weak pull-ups, and explicitly turning off all of the peripherals.
//
//-----------------------------------------------------------------------------

void configMCU_Sleep (void)
{
   XBR1    &= ~0xC0;                   // disable crossbar, enabled weak pull ups

   // disable all peripherals for low power sleep
   SCON0   &= ~0x10;                   // disable UART
// TCON    &= ~0x10;                   // don't disable T0
   TCON    &= ~0x40;                   // disable T1
   TMR2CN  &= ~0x04;                   // disable T2
   TMR3CN  &= ~0x04;                   // disable T3
   PCA0CN  &= ~0x40;                   // disable PCA0
   SMB0CF  &= ~0x80;                   // disable SMBUS
   SPI0CN  &= ~0x05;                   // disable SPI
   CPT0CN  &= ~0x80;                   // disable comparator
   REF0CN  &= ~0x07;                   // disable voltage reference
   IDA0CN  &= ~0x80;                   // disable iDAC
   VDM0CN  &= ~0x80;                   // disable voltage monitor
}

//-----------------------------------------------------------------------------
// configMCU_Wake ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will configure the MCU for wake mode operation. Un-comment
// the peripheral enables for the ones that you are using.
//-----------------------------------------------------------------------------

void configMCU_Wake (void)
{

// enable peripherals as needed
// SCON0   |= 0x10;                    // enable UART
// TCON    |= 0x10;                    // enable T0
// TCON    |= 0x40;                    // enable T1
// TMR2CN  |= 0x04;                    // enable T2
// TMR3CN  |= 0x04;                    // enable T3
// PCA0CN  |= 0x40;                    // enable PCA0
// SMB0CF  |= 0x80;                    // enable SMBUS
// SPI0CN  |= 0x05;                    // enable SPI
// CPT0CN  |= 0x80;                    // enable comparator
// REF0CN  |= 0x07;                    // enable voltage reference
// IDA0CN  |= 0x80;                    // enable iDAC
// VDM0CN  |= 0x80;                    // enable voltage monitor

   XBR1    |= 0x40;                    // enable crossbar
}

//-----------------------------------------------------------------------------
// delay ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : time - delay time (10000 = 1 second)
//
// This function will implement a delay using T0 and the LFO 10 kHz clock.
// A delay time of 10000 counts is equal to one second. This function modifies
// the system clock and exits with a 24.5 MHz clock.
//-----------------------------------------------------------------------------

void delay(unsigned int time)
{
   TCON    &= ~0x30;                   // stop T0 & clear flag
   TMOD    &= ~0x0F;                   // clear T0 nibble of TMOD
   TMOD    |= 0x01;                    // T0 16 bit counter mode
   CKCON   |= 0x04;                    // T0 uses SYSCLK
   // set T0 using time
   time = -time;
   TL0      = (unsigned char)(time & 0x00FF);
   TH0      = (unsigned char)(time >> 8);
   //switch to low freq oscillator
   OSCLCN   = 0x80;                    // LFO is 10 kHz
   CLKSEL   = 0x02;                    // Switch to LFO
   TCON    |= 0x10;                    // enable T0
   while(!TF0);                        // wait for TOF
   TCON    &= ~0x30;                   // stop T0 & clear flag
   OSCICN    =  0x83;                  // enable 24.5 MHz int osc
   CLKSEL    = 0x00;                   // use int osc
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// T0ISR ()
//-----------------------------------------------------------------------------

INTERRUPT(T0_ISR, INTERRUPT_TIMER0)
{
   OSCICN    =  0x83;                  // enable 24.5 MHz int osc
   while ((OSCICN & 0x40) == 0);       // wait for lock bit
   CLKSEL    = 0x00;                   // use int osc
   TCON   &= ~0x10;                    // stop timer T0
   IE     &= ~0x02;                    // disable T0 interrupt
}

//-----------------------------------------------------------------------------
// END F336_LFO_Idle.c
//-----------------------------------------------------------------------------
