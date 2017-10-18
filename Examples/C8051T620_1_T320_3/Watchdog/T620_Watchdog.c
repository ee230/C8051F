//-----------------------------------------------------------------------------
// T620_Watchdog.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program helps the user to learn about operating the Watch Dog Timer.
// The WDT is used to generate resets if the times between writes to the WDT
// update register (PCA0CPH4) exceed a specified limit. The WDT can be disabled
// and enabled in the software as needed. When enabled the PCA Module 4 acts as
// the WDT. This program resets the MCU when P2.0 switch is pressed. Also upon
// reset the LED blinks approximately ten times faster when compared to before.
// The reset is caused due to a WDT overflow and can be confirmed by checking
// the value of the RSTSRC register where bit 3 is set to indicate a reset
// caused by WDT.
//
// How to Test:
//
// 1) Compile and download code to a 'T620 device on a 'T62x mother board.
// 2) Run the code:
//       - The test will blink the LED at a rate of 10Hz until the switch SW1
//         (P2.0) is pressed.
//       - Once the the switch is pressed and held for a long enough time,
//         it will cause a WDT reset.
//       - Upon reset, the code checks for a WDT reset and blinks the LED ten
//         times faster than before to indicate the reset occurred.
//
// Target:         C8051T620/1 or 'T320/1/2/3
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -25 JUN 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       12000000         // SYSCLK frequency in Hz

SBIT (LED, SFR_P2, 2);                // LED='1' means ON
SBIT (SW1, SFR_P2, 0);                // SW1='0' means switch pressed

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void PCA0_Init (void);
void Timer2_Init (int counts);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Oscillator_Init ();                 // Initialize system clock to 12/8 MHz
   PCA0_Init();                        // Intialize the PCA
   Port_Init();                        // Initialize crossbar and GPIO

   if ((RSTSRC & 0x02) == 0x00)        // First check the PORSF bit. if PORSF
   {                                   // is set, all other RSTSRC flags are
                                       // invalid

      // Check if the last reset was due to the Watch Dog Timer
      if (RSTSRC == 0x08)
      {
         Timer2_Init (SYSCLK / 12 / 100); // Enable Timer to make the LED blink
         EA = 1;                       // Enable global interrupts

         while (1);                    // Wait forever
      }
      else
      {
         Timer2_Init (SYSCLK / 12 / 10); // Init Timer2 to generate interrupts
                                         // at a 10Hz rate.
      }
   }

   // Calculate Watchdog Timer Timeout
   // Offset calculated in PCA0 clocks
   // Offset = ( 256 x PCA0CPL4 ) + 256 - PCA0L
   //        = ( 256 x 255(0xFF)) + 256 - 0
   // Time   = Offset * (12/SYSCLK)
   //        = 524 ms ( PCA uses SYSCLK/12 as its clock source)

   PCA0MD  &= ~0x40;                   // WDTE = 0 (clear watchdog timer
                                       // enable)
   PCA0L    = 0x00;                    // Set lower byte of PCA counter to 0
   PCA0H    = 0x00;                    // Set higher byte of PCA counter to 0
   PCA0CPL4 = 0xFF;                    // Write offset for the WDT
   PCA0MD  |= 0x40;                    // Enable the WDT
   EA = 1;                             // Enable global interrupts

   while (1)
   {
      while (!SW1);                    // Force the MCU to stay in this task as
                                       // long as SW1 is pressed. This task
                                       // must finish before the watchdog
                                       // timer timeout expires.

      PCA0CPH4 = 0x00;                 // Write a 'dummy' value to the PCA0CPH4
                                       // register to reset the watchdog timer
                                       // timeout. If a delay longer than the
                                       // watchdog timer timeout occurs between
                                       // successive writes to this register,
                                       // the device will be reset by the WDT.
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
// This routine initializes the system clock to use the internal 12 MHz / 8
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0x80;                      // Configure internal oscillator for
                                       // its lowest frequency
   RSTSRC = 0x04;                      // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the PCA to use the SYSCLK / 12 as its clock source.
// It also sets the offset value by writing to PCA0CPL4.
//
//-----------------------------------------------------------------------------
void PCA0_Init()
{
   PCA0CN =  0x40;                     // PCA counter enable
   PCA0MD &= ~0x40 ;                   // Watchdog timer disabled
   PCA0MD &=  0xF1;                    // Timebase selected - System clock / 12
   PCA0CPL4 =  0xFF;                   // Offset value
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the Crossbar and GPIO ports.
//
// P2.2   digital   push-pull     LED
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P2MDOUT |= 0x04;                    // Enable LED as a push-pull output

   XBR0 = 0x00;                        // No digital peripherals selected
   XBR1 = 0x40;                        // Enable crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1)  int counts - calculated Timer overflow rate
//                    range is positive range of integer: 0 to 32767
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
//-----------------------------------------------------------------------------
void Timer2_Init (int counts)
{
   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // use SYSCLK/12 as timebase
   CKCON &= ~0x60;                     // Timer2 clocked based on T2XCLK;

   TMR2RL = -counts;                   // Init reload values
   TMR2 = 0xffff;                      // Set to reload immediately
   ET2 = 1;                            // Enable Timer2 interrupts
   TR2 = 1;                            // Start Timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer2 overflows.
//
//-----------------------------------------------------------------------------
INTERRUPT(Timer2_ISR, INTERRUPT_TIMER2)
{
   TF2H = 0;                           // Clear Timer2 interrupt flag
   LED = !LED;                         // Change state of LED
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
