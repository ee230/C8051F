//-----------------------------------------------------------------------------
// F0xx_Oscillator_Crystal.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This example demonstrates how to initialize for and switch to an external
// Crystal Oscillator.
// - Pinout:
//    P0.0 -> /SYSCLK
//
//    P1.6 -> LED
//
//    all other port pins unused
//
//    XTAL1 and XTAL2 -> External Crystal Oscillator pins
//
//
// How To Test:
//
// 1) Define the input external crystal frequency using <Crystal_Clock>
// 2) Compile and download code to a 'F0xx device.
// 3) Run the code:
//      - the test will blink an LED at a frequency based on the External
//         Crystal frequency
//      - the 'F0xx will also output the SYSCLK to a port pin (P0.0) for
//         observation
//
//
// FID:            00X000012
// Target:         C8051F0xx ('F005 TB)
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -1 JUNE 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F000.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define Crystal_Clock 22118400L        // External Crystal oscillator
                                       // frequency (Hz)
                                       // NOTE: This value will determine the
                                       // XFCN bits in ExtCrystalOsc_Init ()

// Timer2 using SYSCLK/12 as its time base
// Timer2 counts 65536 SYSCLKs per Timer2 interrupt
// LED target flash rate = 10 Hz
//
// If Crystal_Clock is too slow to divide into a number of counts,
// <count> will always remain 0.
#define LED_interrupt_count Crystal_Clock/12/65536/10


sfr16 RCAP2 = 0xCA;                    // Timer2 reload value
sfr16 TMR2 = 0xCC;                     // Timer2 counter

sbit LED = P1^6;                       // LED='1' means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void ExtCrystalOsc_Init (void);
void Port_Init (void);
void Timer2_Init (void);

void Timer2_ISR (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, switches to the external
// Crystal oscillator, and loops forever, blinking the LED.
//
//-----------------------------------------------------------------------------
void main (void) 
{
   WDTCN = 0xDE;                       // disable watchdog timer
   WDTCN = 0xAD;

   SYSCLK_Init ();                     // Initialize system clock to 16MHz

   ExtCrystalOsc_Init ();              // Initialize for and switch to the
                                       // external Crystal oscillator

   Port_Init ();                       // Initialize crossbar and GPIO

   Timer2_Init ();                     // Init Timer2 to generate
                                       // interrupts for the LED.

   EA = 1;                             // Enable global interrupts

   while (1) {                         // Spin forever
   }
}

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SYSCLK_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal 16 MHz
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------

void SYSCLK_Init (void)
{
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its highest frequency (16 MHz)

   OSCICN |= 0x80;                     // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// ExtCrystalOsc_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes for and switches to the External Crystal
// Oscillator.
//
//-----------------------------------------------------------------------------

void ExtCrystalOsc_Init (void)
{

   // Set the appropriate XFCN bits for the crystal frequency
   //
   //   XFCN     Crystal (XOSCMD = 11x)
   //   000      f <= 32 kHz
   //   001      32 kHz < f <= 84 kHz
   //   010      84 kHz < f <= 225 kHz
   //   011      225 kHz < f <= 590 kHz
   //   100      590 kHz < f <= 1.5 MHz
   //   101      1.5 MHz < f <= 4 MHz
   //   110      4 MHz < f <= 10 MHz
   //   111      10 MHz < f <= 30 MHz
   #if (Crystal_Clock <= 32000)
      #define XFCN 0
   #elif (Crystal_Clock <= 84000L)
      #define XFCN 1
   #elif (Crystal_Clock <= 225000L)
      #define XFCN 2
   #elif (Crystal_Clock <= 590000L)
      #define XFCN 3
   #elif (Crystal_Clock <= 1500000L)
      #define XFCN 4
   #elif (Crystal_Clock <= 4000000L)
      #define XFCN 5
   #elif (Crystal_Clock <= 10000000L)
      #define XFCN 6
   #elif (Crystal_Clock <= 30000000L)
      #define XFCN 7
   #else
      #error "Defined Crystal Frequency outside allowable range!"
      #define XFCN 0
   #endif

   unsigned int i;

   // Step 1. Enable the external oscillator.
   OSCXCN = 0x60;                      // External Oscillator is an external
                                       // crystal (no divide by 2 stage)

   OSCXCN |= XFCN;


   // Step 2. Wait at least 1 ms.
   for (i = 9000; i > 0; i--);         // at 16 MHz, 1 ms = 16000 SYSCLKs
                                       // DJNZ = 2 SYSCLKs


   // Step 3. Poll for XTLVLD => ‘1’.
   while ((OSCXCN & 0x80) != 0x80);


   // Step 4. Switch the system clock to the external oscillator.
   OSCICN |= 0x08;
}

//-----------------------------------------------------------------------------
// Port_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P0.0   digital   push-pull     /SYSCLK
// P1.6   digital   push-pull     LED
//
//-----------------------------------------------------------------------------

void PORT_Init (void)
{
   PRT0CF |= 0x01;                     // Enable /SYSCLK as a push-pull output

   PRT1CF |= 0x40;                     // Enable LED as a push-pull output

   XBR1 = 0x80;                        // Route /SYSCLK to a port pin
   XBR2 = 0x40;                        // Enable crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer2_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt after the
// maximum possible time (TMR2RL = 0x0000).
//
//-----------------------------------------------------------------------------

void Timer2_Init (void)
{
   T2CON = 0x00;                       // Stop Timer2; Clear TF2;
                                       // auto-reload mode

   CKCON &= ~0x20;                     // use SYSCLK/12 as timebase

   RCAP2 = 0x0000;                     // Init reload value
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

void Timer2_ISR (void) interrupt 5
{
   static unsigned int count = 0;

   TF2 = 0;                            // Clear Timer2 interrupt flag

   if (count == LED_interrupt_count)
   {
      LED = ~LED;                      // Change state of LED

      count = 0;
   }
   else
   {
      count++;
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------