//-----------------------------------------------------------------------------
// F36x_LFO_Idle.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The program demonstrates the Low Frequency internal oscillator and power
// savings mode operation on the F360. The MCU will sleep in low power idle
// mode with only the low frequency oscillator and timer T0 running. When
// the MCU wakes up it will switch to a 24.5 MHz clock and turn on an LED
// and wait 100 ms. The 100 ms delay also uses T0 and the low frequency
// oscillator but does not use idle mode or interrupts.
//
// How To Test:
//
// 1) Download code to a 'F360 target board.
// 2) Make sure the P3.2 LED jumper is in place.
// 3) Run the code.
// 4) The P3.2 LED should blink once a second.
//
//
// FID:            36X000014
// Target:         C8051F36x
// Tool chain:     Keil C51 7.05 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (KAB / TP)
//    -13 OCT 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F360.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SLEEP_TIME 10000               // Sleep 1 second
#define BLINK_TIME  1000               // Blink 100 ms

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);

void lowPowerSleep (unsigned int);
void configMCU_Sleep(void);
void configMCU_Wake(void);
void delay (unsigned int);
void T0_ISR (void);                    // Interrupt key word not permitted.

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

sbit LED = P3^2;

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD  &= ~0x40;
   PCA0MD   = 0x00;

   Oscillator_Init ();
   Port_Init ();

   EA = 1;                             // Set global interrupt enable

   SFRPAGE = LEGACY_PAGE;

   while (1)
   {
      lowPowerSleep (SLEEP_TIME);
      LED = 1;
      delay (1000);
   }
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
// P3.2 push-pull output LED
//-----------------------------------------------------------------------------
void Port_Init ()
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   P3MDOUT  = 0x04;

   XBR1     |= 0x40;                   // Enable crossbar

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Oscillator_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the internal 24.5 MHz oscillator.
//-----------------------------------------------------------------------------
void Oscillator_Init ()
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN    =  0x83;                  // Enable 24.5 MHz int osc

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

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

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   XBR1    &= ~0xC0;                   // Disable crossbar, enabled weak pull ups

   // disable all peripherals for low power sleep
   SCON0   &= ~0x10;                   // Disable UART
// TCON    &= ~0x10;                   // Don't disable T0
   TCON    &= ~0x40;                   // Disable T1
   TMR2CN  &= ~0x04;                   // Disable T2
   TMR3CN  &= ~0x04;                   // Disable T3
   PCA0CN  &= ~0x40;                   // Disable PCA0
   SMB0CF  &= ~0x80;                   // Disable SMBUS
   SPI0CN  &= ~0x05;                   // Disable SPI
   CPT0CN  &= ~0x80;                   // Disable comparator
   REF0CN  &= ~0x07;                   // Disable voltage reference
   IDA0CN  &= ~0x80;                   // Disable iDAC
   VDM0CN  &= ~0x80;                   // Disable voltage monitor

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

// enable peripherals as needed
// SCON0   |= 0x10;                    // Enable UART
// TCON    |= 0x10;                    // Enable T0
// TCON    |= 0x40;                    // Enable T1
// TMR2CN  |= 0x04;                    // Enable T2
// TMR3CN  |= 0x04;                    // Enable T3
// PCA0CN  |= 0x40;                    // Enable PCA0
// SMB0CF  |= 0x80;                    // Enable SMBUS
// SPI0CN  |= 0x05;                    // Enable SPI
// CPT0CN  |= 0x80;                    // Enable comparator
// REF0CN  |= 0x07;                    // Enable voltage reference
// IDA0CN  |= 0x80;                    // Enable iDAC
// VDM0CN  |= 0x80;                    // Enable voltage monitor

   XBR1    |= 0x40;                    // Enable crossbar

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   TCON    &= ~0x30;                   // Stop T0 & clear flag
   TMOD    &= ~0x0F;                   // Clear T0 nibble of TMOD
   TMOD    |= 0x01;                    // T0 16 bit counter mode
   CKCON   |= 0x04;                    // T0 uses SYSCLK
   // set T0 using time
   time = -time;
   TL0      = (unsigned char)(time & 0x00FF);
   TH0      = (unsigned char)(time >> 8);
   //switch to low freq oscillator
   OSCLCN   = 0x80;                    // LFO is 10 kHz
   CLKSEL   = 0x02;                    // Switch to LFO
   TCON    |= 0x10;                    // Enable T0
   while(!TF0);                        // Wait for TOF
   TCON    &= ~0x30;                   // Stop T0 & clear flag
   OSCICN    =  0x83;                  // Enable 24.5 MHz int osc
   CLKSEL    = 0x00;                   // Use int osc

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
// T0ISR ()
//-----------------------------------------------------------------------------
void T0_ISR (void) interrupt 1
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN    =  0x83;                  // Enable 24.5 MHz int osc
   while ((OSCICN & 0x40) == 0);       // Wait for lock bit
   CLKSEL    = 0x00;                   // Use int osc

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE

   TCON   &= ~0x10;                    // Stop timer T0
   IE     &= ~0x02;                    // Disable T0 interrupt
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
