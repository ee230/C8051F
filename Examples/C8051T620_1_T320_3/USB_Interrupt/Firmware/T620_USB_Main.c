//-----------------------------------------------------------------------------
// T620_USB_Main.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This application note covers the implementation of a simple USB application
// using the interrupt transfer type. This includes support for device
// enumeration, control and interrupt transactions, and definitions of
// descriptor data. The purpose of this software is to give a simple working
// example of an interrupt transfer application; it does not include
// support for multiple configurations or other transfer types.
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051T620/1 or C8051T320/1/2/3
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   T620_USB_Interrupt
//
//
// Release 1.2
//    -All changes by ES
//    -4 OCT 2010
//    -Updated USB0_Init() to write 0x8F to CLKREC instead of 0x80
//    -Updated USB0_Init() to write 0xAF to CLKREC instead of 0xA0
// Release 1.1
//    -All changes by TP
//    -14 AUG 2008
//    -Updated for 'T620/1
//
// Release 1.0
//    -Initial Revision (GP)
//    -22 NOV 2005
//    -Ported from 'F320_USB_Bulk
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"
#include "T620_USB_Register.h"
#include "T620_USB_Main.h"
#include "T620_USB_Descriptor.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

SBIT (LED1, SFR_P2, 2);                // LED='1' means ON
SBIT (LED2, SFR_P2, 3);

#define Sw1 0x01                       // These are the port2 bits for Sw1
#define Sw2 0x02                       // and Sw2 on the development board
U8 Switch1State = 0;                   // Indicate status of switch
U8 Switch2State = 0;                   // starting at 0 == off

U8 Toggle1 = 0;                        // Variable to make sure each button
U8 Toggle2 = 0;                        // press and release toggles switch

U8 Potentiometer = 0x00;               // Last read potentiometer value
U8 Temperature = 0x00;                 // Last read temperature sensor value

idata U8 Out_Packet[64];               // Last packet received from host
idata U8 In_Packet[64];                // Next packet to sent to host

code const U8 TEMP_ADD = 112;          // This constant is added to Temperature


//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   Oscillator_Init();                  // Initialize oscillator
   Port_Init();                        // Initialize crossbar and GPIO
   USB0_Init();                        // Initialize USB0
   Timer2_Init();                      // Initialize Timer2
   ADC0_Init();                        // Initialize ADC0

   while (1)
   {
      // It is possible that the contents of the following packets can change
      // while being updated.  This doesn't cause a problem in the sample
      // application because the bytes are all independent.  If data is NOT
      // independent, packet update routines should be moved to an interrupt
      // service routine, or interrupts should be disabled during data updates.

      if (Out_Packet[0] == 1) LED1 = 1; // Update status of LED #1
      else LED1 = 0;
      if (Out_Packet[1] == 1) LED2 = 1; // Update status of LED #2
      else LED2 = 0;
      P1 = (Out_Packet[2] & 0x0F);     // Set Port 1 pins

      In_Packet[0] = Switch1State;     // Send status of switch 1
      In_Packet[1] = Switch2State;     // and switch 2 to host
      In_Packet[2] = (P0 & 0x0F);      // Port 0 [0-3]
      In_Packet[3] = Potentiometer;    // Potentiometer value
      In_Packet[4] = Temperature;      // Temperature sensor value
   }
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
// Initialize the system clock and USB clock
//
//-----------------------------------------------------------------------------
void Oscillator_Init(void)
{
// Select USB Low Speed
#ifdef _USB_LOW_SPEED_

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // 12 MHz

   CLKSEL  = SYS_INT_OSC;              // Select System clock
   CLKSEL |= USB_INT_OSC_DIV_2;        // Select USB clock
// Select USB Full Speed
#else
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // 12 MHz

   // This code is no longer necessary for the 'T620, but it is kept here
   // for backwards compatibility with the 'F34x.

   CLKMUL  = 0x00;                     // Select internal oscillator as
                                       // input to clock multiplier

   CLKMUL |= 0x80;                     // Enable clock multiplier
   Delay();                            // Delay for clock multiplier to begin
   CLKMUL |= 0xC0;                     // Initialize the clock multiplier
   Delay();                            // Delay for clock multiplier to begin

   while(!(CLKMUL & 0x20));            // Wait for multiplier to lock
   CLKSEL  = SYS_INT_OSC;              // Select system clock
   CLKSEL |= USB_4X_CLOCK;             // Select USB clock
#endif
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
// P0.7   analog                  VREF
//
// P2.0   digital   open-drain    SW1
// P2.1   digital   open-drain    SW2
// P2.2   digital   push-pull     LED
// P2.3   digital   push-pull     LED
//
// P2.5   analog                  Potentiometer
//
//-----------------------------------------------------------------------------
void Port_Init(void)
{
   P2MDIN &= ~0x20;                    // P2.5 set as analog input
   P2SKIP = 0x20;                      // P2.5 skipped by crossbar

   P0MDIN &= ~0x80;                    // P0.7 set to analog mode
   P0SKIP = 0x80;                      // P0.7 skipped by crossbar

   P0MDOUT |= 0x0F;                    // P0 pins 0-3 set to push-pull
   P1MDOUT |= 0x0F;                    // P1 pins 0-3 set to push-pull
   P2MDOUT |= 0x0C;                    // P2.2/3 set to push-pull

   XBR0 = 0x00;
   XBR1 = 0x40;                        // Enable Crossbar
}

//-----------------------------------------------------------------------------
// USB0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//-----------------------------------------------------------------------------
void USB0_Init(void)
{
   U8 Count;

   // Set initial values of In_Packet and Out_Packet to zero
   // Initialize here as opposed to above main() to prevent WDT reset
   for (Count = 0; Count < 64; Count++)
   {
      Out_Packet[Count] = 0;
      In_Packet[Count] = 0;
   }

   POLL_WRITE_BYTE(POWER,  0x08);      // Force Asynchronous USB Reset
   POLL_WRITE_BYTE(IN1IE,  0x07);      // Enable Endpoint 0-2 in interrupts
   POLL_WRITE_BYTE(OUT1IE, 0x07);      // Enable Endpoint 0-2 out interrupts
   POLL_WRITE_BYTE(CMIE,   0x07);      // Enable Reset,Resume,Suspend interrupts

#ifdef _USB_LOW_SPEED_
   USB0XCN = 0xC0;                     // Enable transceiver; select low speed
   POLL_WRITE_BYTE(CLKREC, 0xAF);      // Enable clock recovery; single-step mode
                                       // disabled; low speed mode enabled
#else
   USB0XCN = 0xE0;                     // Enable transceiver; select full speed
   POLL_WRITE_BYTE(CLKREC, 0x8F);      // Enable clock recovery, single-step mode
                                       // disabled
#endif // _USB_LOW_SPEED_

   EIE1 |= 0x02;                       // Enable USB0 Interrupts
   EA = 1;                             // Global Interrupt enable
                                       // Enable USB0 by clearing the USB
                                       // Inhibit bit
   POLL_WRITE_BYTE(POWER,  0x01);      // and enable suspend detection
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer 2 reload, used to check if switch pressed on overflow and used for
// ADC continuous conversion
//
//-----------------------------------------------------------------------------
void Timer2_Init(void)
{
   TMR2CN = 0x00;                     // Stop Timer2; Clear TF2;

   CKCON &= ~0xF0;                    // Timer2 clocked based on T2XCLK;
   TMR2RL = 0;                        // Initialize reload value
   TMR2 = 0xFFFF;                     // Set to reload immediately

   ET2 = 1;                           // Enable Timer2 interrupts
   TR2 = 1;                           // Start Timer2
}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configures ADC for single ended continuous conversion or Timer2.
//
//-----------------------------------------------------------------------------
void ADC0_Init(void)
{
   REF0CN = 0x0F;                      // Enable voltage reference VREF (2.4 V)
                                       // Enable temperature sensor
   AMX0P = 0x1E;                       // Positive input starts as temp sensor

   ADC0CF = 0xF8;                      // SAR Period 0x1F, Right adjusted
   ADC0CF |= 0x01;                     // Gain = 1

   ADC0CN = 0xC2;                      // Continuous converion on Timer 2
                                       // overflow; low power tracking mode on

   EIE1 |= 0x08;                       // Enable conversion complete interrupt
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Delay
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Used for a small pause, approximately 80 us in Full Speed, and 1 ms when
// clock is configured for Low Speed.
//
//-----------------------------------------------------------------------------
void Delay(void)
{
   int x;
   for(x = 0;x < 500;x)
   {
      x++;
   }
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// Called when Timer2 overflows, checks to see if switch is pressed, then
// waits for release.
//
// Timer2 overflows also trigger an ADC conversion.
//
//-----------------------------------------------------------------------------
INTERRUPT(Timer2_ISR, INTERRUPT_TIMER2)
{
   if (!(P2 & Sw1))                    // Check for switch #1 pressed
   {
      if (Toggle1 == 0)                // Toggle is used to debounce switch
      {                                // so that one press and release will
         Switch1State = ~Switch1State; // toggle the state of the switch sent
         Toggle1 = 1;                  // to the host
      }
   }
   else Toggle1 = 0;                   // Reset toggle variable

   if (!(P2 & Sw2))                    // Same as above, but Switch2
   {
      if (Toggle2 == 0)
      {
         Switch2State = ~Switch2State;
         Toggle2 = 1;
      }
   }
   else Toggle2 = 0;

   TF2H = 0;                           // Clear Timer2 interrupt flag
}

//-----------------------------------------------------------------------------
// ADC0_ConvComplete_ISR
//-----------------------------------------------------------------------------
//
// The ADC ISR is called after a conversion of the ADC has finished.  This ISR
// updates the appropriate variable for potentiometer or temperature sensor and
// switches the ADC multiplexor value to switch between the potentiometer
// and temp sensor.
//
//-----------------------------------------------------------------------------
INTERRUPT(ADC0_ConvComplete_ISR, INTERRUPT_ADC0_EOC)
{
   if (AMX0P == 0x1E)                  // This switches the AMUX between
   {                                   // the temperature sensor and the
      Temperature = ADC0L;             // potentiometer pin after conversion
      Temperature += TEMP_ADD;         // Add offset to Temperature
      AMX0P = 0x0D;                    // Switch to potentiometer
      ADC0CF |= 0x04;                  // Place ADC0 in left-adjusted mode
   }
   else
   {
      Potentiometer = ADC0H;
      AMX0P = 0x1E;                    // Switch to temperature sensor
      ADC0CF &= ~0x04;                 // Place ADC0 in right-adjusted mode
   }

   AD0INT = 0;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------