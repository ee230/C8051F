//-----------------------------------------------------------------------------
// F326_LFO_Idle.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The program demonstrates the Low Frequency internal oscillator and power
// savings mode operation on the F326. The MCU will sleep in low power idle
// mode with only the low frequency oscillator and timer T0 running. When 
// the MCU wakes up it will switch to the internal 24.5 MHz clock and turn
// on an LED and wait BLINK_TIME/10 ms. The delay also uses T0 and the low 
// frequency oscillator but does not use idle mode or interrupts.
// 
// How To Test:
//
// 1) Download code to a 'F326 target board.
// 2) Make sure the P2.3 LED jumper is in place.
// 3) Run the code.
// 4) P2.3_LED should blink periodically.
//
// FID:            326000030
// Target:         C8051F326
// Tool chain:     Keil C51 7.05 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (KAB)
//    -07 AUG 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051F326.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SLEEP_TIME 10000               // Sleep 1 second
#define BLINK_TIME  2000               // Blink 100 ms

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void main (void);
void OSCILLATOR_Init (void);           
void PORT_Init (void);
void lowPowerSleep (unsigned int);
void configMCU_Sleep (void);
void configMCU_Wake (void);
void delay (unsigned int);
void T0_ISR (void);                    // Interrupt key word not permitted.

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

sbit LED = P2^3;

//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------

void main (void)
{
   OSCILLATOR_Init ();
   PORT_Init ();
   
   EA = 1;                             // Set global interrupt enable
   
   while (1)
   {
      lowPowerSleep (SLEEP_TIME);
      LED = 1;
      delay (BLINK_TIME);
      LED = 0;
   }
}

//-----------------------------------------------------------------------------
// PORT_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
// P1.3 push-pull output LED
//-----------------------------------------------------------------------------

void Port_Init (void)
{
// configure Port IO here
   P2MDOUT   = 0x08;                    // enable P2.3 output
   GPIOCN    = 0x00;                    // enable weak-pull up, disable inputs
}

//-----------------------------------------------------------------------------
// OSCILLATOR_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the internal oscillator to 24.5 MHz.
//
//-----------------------------------------------------------------------------

void OSCILLATOR_Init(void)
{
   OSCICN    =  0x83;                  // enable 24.5 MHz int osc
}

//-----------------------------------------------------------------------------
// lowPowerSleep ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : time - 10,000 = 1 second, so max sleep time is ~6.5 seconds
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
   
   EA = 0;                             // clear global interrupt enable
   
   // save interrupt state
   restoreIE = IE;                     // preserve IE register
   restoreEIE1 = EIE1;                 // preserve EIE1 register
   
   // disable all interrupts  
   IE       = 0x00;                    // disable interrupts
   EIE1     = 0x00;                    // disable extended interrupts
       
   configMCU_Sleep ();                 // configure for Sleep
   
   //init T0 to wake up MCU 

   TCON    &= ~0x30;                   // stop T0 & clear flag
   TMOD    &= ~0x0F;                   // clear T0 nibble of TMOD
   TMOD    |= 0x01;                    // T0 16 bit counter mode
   CKCON   |= 0x04;                    // T0 uses SYSCLK
   
   // set T0 using time
   time = -time;
   TL0      = (unsigned char)(time & 0x00FF);
   TH0      = (unsigned char)(time >> 8);  
   
   // Comment out this line to measure stead state idle current
   //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
   IE      |= 0x82;                    // enable T0 & global interrupts
   //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   //switch to low freq oscillator
   OSCLCN   = 0x80;                    // LFO is 10 kHz
   CLKSEL   = 0x03;                    // Switch to LFO
   OSCICN   = 0x00;                    // Stop internal osc to save power                          

   TCON    |= 0x10;                    // enable T0
   
   // go to IDLE mode
   PCON    |= 0x01;                    // Idle mode
   PCON     = PCON;
   
   //--wake--//

   configMCU_Wake ();
   
   // restore MCU pin configuration to portInit state & restore LEDs
   EIE1     = restoreEIE1;             // restore ext interrupts
   IE       = restoreIE;               // restore interrupts
   EA = 1;                             // set global interrupt enable
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
   GPIOCN    = 0x00;                    // enable weak pull-up, disable inputs

   // disable all peripherals for low power sleep
   
   SCON0   &= ~0x10;                   // disable UART0
   TCON    &= ~0x40;                   // disable T1

   // disable any additional peripherals here

   VDM0CN  &= ~0x80;                   // disable voltage monitor
                                       
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
//-----------------------------------------------------------------------------

void configMCU_Wake (void)
{

   VDM0CN  |= 0x80;                    // enable voltage monitor

   // enable any additional peripherals here
   
   GPIOCN    = 0x10;                    // enable inputs

}

//-----------------------------------------------------------------------------
// delay ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : time - (10,000 = 1 second)
//
// This function will implement a delay using T0 and the LFO 10 kHz clock.
// A delay time of 10000 counts is equal to one second. This function modifies
// the system clock and exits with a 24.5 MHz clock.
//-----------------------------------------------------------------------------

void delay (unsigned int time)

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
   CLKSEL   = 0x03;                    // Switch to LFO
   TCON    |= 0x10;                    // enable T0
   while (!TF0);                       // wait for TOF
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

void T0_ISR (void) interrupt 1
{   
   OSCICN    =  0x83;                  // enable 24.5 MHz int osc
   CLKSEL    = 0x00;                   // use int osc
   TCON   &= ~0x10;                    // stop timer T0   
   IE     &= ~0x02;                    // disable T0 interrupt 
}

//-----------------------------------------------------------------------------
// END F326_LFO_Sleep.c
//-----------------------------------------------------------------------------
