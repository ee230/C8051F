//-----------------------------------------------------------------------------
// F41x_RTC_Suspend.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// The program demonstrates the SmaRTClock oscillator and power
// savings mode operation on the F410. The MCU will sleep in low power suspend
// mode with only the RTC running. When the MCU wakes up it will switch
// to a 24.5 MHz internal oscillator & turn on an LED. It will then switch
// to the 32kHz RTC clock and wait 10 ms. The 10 ms delay also uses T0
// and the RTC clock oscillator but does not use idle mode or interrupts.
//
// How To Test:
//
// 1) Download code to a 'F410 target board.
// 2) Make sure J5 has a jumper connecting P2.3 to D5.
// 3) Run the code.
//    D5 should blink once a secoond.
//
// Target:         C8051F41x
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
//
// Release 1.0
//    -Initial Revision (KAB)
//    -07 AUG 2006
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F410_defs.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define RTC0CN    0x06                 // RTC address of RTC0CN register
#define RTC0XCN   0x07                 // RTC address of RTC0XCN register

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void main (void);
void OSC_Init (void);
void PORT_Init (void);
void RTC_Init (void);
unsigned char RTC_Read (unsigned char);
void RTC_Write (unsigned char, unsigned char);
void RTC_SetBits (unsigned char, unsigned char);
void RTC_ClearBits (unsigned char, unsigned char);
void RTC_ClearCapture (void);
void RTC_ClearAlarm (void);
void RTC_BumpAlarm (unsigned char code *p);
void configSleep (void);
void configWake (void);
void delay (unsigned int);
void RTC_ISR (void);                   // interrupt key word not permitted.

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

sbit LED = P2^3;                       // port pin used for LED

// RTC_Interval - little endian data added to alarm register = 1 second
unsigned char code RTC_Interval[6]= {0x00,0x00,0x01,0x00,0x00,0x00};

//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------

void main (void)
{
   PCA0MD  &= ~0x40;                   // disable watchdog timer

   OSC_Init ();                        // initialize 24.5 MHz oscillator
   PORT_Init ();                       // initialize IO ports
   RTC_Init ();                        // initiralize RTC

   EA = 1;                             // set global interrupt enable

   while (1)
   {
      configSleep ();                  // configure MCU for low power sleep
      OSCICN |= 0x20;                  // set suspend mode bit
      configWake ();                   // configure MCU for wake mode
      LED = 1;                         // turn on LED
      delay (164);                     // wait 50 ms
      LED = 0;                         // turn off LED
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

void Port_Init ()
{
// configure Port IO here
   P2MDOUT   = 0x08;                   // enable P2.3 output
   XBR1     |= 0x40;                   // enable crossbar
}

//-----------------------------------------------------------------------------
// OSC_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the internal oscilloator.
// Note that the OSCICN setting for 24.5MHz is different than F12x and F3xx
//
//-----------------------------------------------------------------------------

void OSC_Init ()
{
   OSCICN    =  0x87;                  // enable 24.5 MHz int osc
}

//-----------------------------------------------------------------------------
// RTC_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will initialize the RTC. First it unlocks the RTC interface,
// enables the RTC, clears ALRMn and CAPTUREn bits. Then it sets up the RTC
// to operate using a 32.768khZ crystal. Lastly it enables the alarm and
// interrupt. This function uses the RTC primitive functions to access
// the internal RTC registers.
//
//-----------------------------------------------------------------------------

void RTC_Init (void)
{
   unsigned char i;

   RTC0KEY = 0xA5;                     // unlock the RTC interface
   RTC0KEY = 0xF1;

   RTC_Write (RTC0CN, 0x80);           // enable RTC

   RTC_ClearCapture ();                // clear CAPTUREn registers
   RTC_ClearAlarm ();                  // clear ALARMn registers

   RTC_SetBits (RTC0XCN, 0x40);        // set crystal mode

   OSCICN    =  0x80;                  // switch to 192 kHz int osc
   for (i=0xFF;i!=0;i--);              // wait > 1 ms
   OSCICN    =  0x87;                  // switch back to 24.5 MHz int osc

   // wait for clock valid
   while ((RTC_Read (RTC0XCN) & 0x10)== 0x00);

   RTC_SetBits (RTC0CN, 0x40);         // enable missing clock detector
   RTC_SetBits (RTC0XCN, 0x80);        // enable ACG to save current

   RTC_SetBits (RTC0CN, 0x02);         // transfer capture to clock
   RTC_SetBits (RTC0CN, 0x10);         // enable RTC run
   RTC_SetBits (RTC0CN, 0x08);         // enable RTC alarm

   EIE1 |= 0x02;                       // enable RTC interrupt
}

//-----------------------------------------------------------------------------
// configSleep ()
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

void configSleep (void)
{
   XBR1    &= ~0xC0;                   // disable crossbar, enabled weak pull ups

   // disable all peripherals for low power sleep

   SCON0   &= ~0x10;                   // disable UART0
   TCON    &= ~0x10;                   // disable T0
   TCON    &= ~0x40;                   // disable T1
   TMR2CN  &= ~0x04;                   // disable T2

   // disable any additional peripherals here

   VDM0CN  &= ~0x80;                   // disable voltage monitor

}

//-----------------------------------------------------------------------------
// configWake
//
// Return Value : None
// Parameters   : None
//
// This function will configure the MCU for wake mode operation.
// Enable additional peripherals as needed.
//-----------------------------------------------------------------------------

void configWake (void)
{
   VDM0CN  |= 0x80;                    // enable voltage monitor
   TCON    |= 0x10;                    // enable T0

   // enable any additional peripherals here

   XBR1    |= 0x40;                    // enable crossbar
}

//-----------------------------------------------------------------------------
// delay ()
//
// Return Value : None
// Parameters   : None
//
// This function will implement a delay using T0 and the RTC 32.768 kHz clock.
// A delay time of 32768 counts is equal to one second. This function modifies
// the system clock and exits with a 24.5 MHz clcok.
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
   CLKSEL   = 0x03;                    // Switch to RTC clock
   TCON    |= 0x10;                    // enable T0
   while (!TF0);                       // wait for TOF
   TCON    &= ~0x30;                   // stop T0 & clear flag
   OSCICN    =  0x87;                  // enable 24.5 MHz int osc
   CLKSEL    = 0x00;                   // use int osc
}

//=============================================================================
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// T0ISR ()
//-----------------------------------------------------------------------------

void RTC_ISR (void) interrupt 8        // RTC
{
   RTC_ClearBits (RTC0CN, 0x0C);       // clear RTCAEN and ALARM bits
   RTC_BumpAlarm (RTC_Interval);       // Add RTC interval to ALARMn
   RTC_SetBits (RTC0CN, 0x08);         // enable RTC alarm
}

//=============================================================================
// RTC Primitive Fuctions - used for read, write, and bit twiddling
//-----------------------------------------------------------------------------
// RTC_Read ()
//-----------------------------------------------------------------------------
//
// Return Value : RTC0DAT
// Parameters   : 1) unsigned char reg - address of RTC register to read
//
// This function will read one byte from the specified RTC register.
// Using a register number greater that 0x0F is not permited,
//
//-----------------------------------------------------------------------------

#pragma DISABLE
unsigned char RTC_Read (unsigned char reg)
{
   reg &= 0x0F;                        // mask low nibble
   RTC0ADR  = reg;                     // pick register
   RTC0ADR |= 0x80;                    // set BUSY bit to read
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   return RTC0DAT;                     // return value
}

//-----------------------------------------------------------------------------
// RTC_Write ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : 1) unsigned char reg - address of RTC register to write
//                2) unsigned char value - the value to write to <reg>
//
// This function will Write one byte from the specified RTC register.
//
//-----------------------------------------------------------------------------

#pragma DISABLE
void RTC_Write (unsigned char reg, unsigned char value)
{
   reg &= 0x0F;                        // mask low nibble
   RTC0ADR  = reg;                     // pick register
   RTC0DAT = value;                    // write data
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
}

//-----------------------------------------------------------------------------
// RTC_SetBits ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : 1) unsigned char reg - address of RTC register to set
//                2) unsigned char value - bitmask for the register
//
// This function will set bits in the specified RTC register using the
// supplied bit mask. This function can only be used with an RTC register.
// Using a register number greater that 0x0F is not permited,
//
//-----------------------------------------------------------------------------

#pragma DISABLE
void RTC_SetBits (unsigned char reg, unsigned char value)
{
   reg &= 0x0F;                        // mask low nibble
   RTC0ADR  = reg;                     // pick register
   RTC0ADR |= 0x80;                    // set BUSY bit to read
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   value  |= RTC0DAT;                  // read RTC0DAT and OR to set bits
   RTC0ADR  = reg;                     // overwrite autoincrement value
   RTC0DAT = value;                    // write to RTC0DAT
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
}

//-----------------------------------------------------------------------------
// RTC_ClearBits ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : 1) unsigned char reg - address of RTC register to clear
//                2) unsigned char value - bitmask for the register
//
// This function will clear bits in the specified RTC register using the
// supplied bit mask, ones will be cleared.
//
// Note that writing to the ALARMn or CAPTUREn registers will autoincrement
// RTC0ADDR. So the RTC0ADDR register is set twice.
//
//-----------------------------------------------------------------------------

#pragma DISABLE
void RTC_ClearBits (unsigned char reg, unsigned char value)
{
   reg &= 0x0F;                        // mask low nibble
   value =~value;                      // complement mask
   RTC0ADR  = reg;                     // pick register
   RTC0ADR |= 0x80;                    // set BUSY bit to read
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   value  &= RTC0DAT;                  // read RTC0DAT and AND to clear bits
   RTC0ADR  = reg;                     // overwrite autoincrement value
   RTC0DAT = value;                    // write to RTC0DAT
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
}

//-----------------------------------------------------------------------------
// RTC_ClearCapture ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : none
//
// This function will clear all six CAPTURE registers of the RTC.
//
//-----------------------------------------------------------------------------

#pragma DISABLE
void RTC_ClearCapture (void)
{
   unsigned char n;

   for (n=0;n<6;n++)                   // n = 0-5 for CAPTURE0-5
   {
      RTC0ADR = n;                     // select CAPTUREn register
      RTC0DAT = 0x00;                  // clear data
      while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit
   }
}

//-----------------------------------------------------------------------------
// RTC_ClearAlarm ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : none
//
// This function will clear all six ALARM registers of the RTC.
//
//-----------------------------------------------------------------------------

#pragma DISABLE
void RTC_ClearAlarm (void)
{
   unsigned char n;

   for (n=8;n<14;n++)                  // n = 8-13 for ALARM0-5
   {
      RTC0ADR = n;                     // select ALARMn
      RTC0DAT = 0x00;                  // clear data
      while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit
   }
}

//-----------------------------------------------------------------------------
// RTC_BumpAlarm ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : pointer to 6 byte value stored in code flash memory
//
// This function will add the 48-bit value located by the supplied pointer
// to the 48-bit value in the ALARM registers. The new sum is calculated
// byte-by-byte and store back into the ALARM registers. This is more efficient
// that reading all six bytes, adding, and writing back.
//
// Note Autoread is not used. However the RTC0ADDR register is
// auto-incremented. So the RTC0ADR must be set a second time.
//
//-----------------------------------------------------------------------------

#pragma DISABLE
void RTC_BumpAlarm (unsigned char code *p)
{
   unsigned char n;
   unsigned int sum;

   sum = 0;

   for (n=8;n<14;n++)                  // n = 8-13 for ALARM0-5
   {
      RTC0ADR = n;                     // select ALARMn
      RTC0ADR |= 0x80;                 // set BUSY bit to initiate read
      while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit
      sum += RTC0DAT;                  // add to sum
      sum += *p++;                     // add constant value
      RTC0ADR = n;                     // select ALARMn again
      RTC0DAT = (unsigned char)(sum);  // write sum LSB
      while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit
      sum = sum>>8 ;                   // save remainder
   }
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
