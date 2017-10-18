//-----------------------------------------------------------------------------
// F12x_Oscillator_RC.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This example demonstrates how to initialize for and switch to a
// Resistor-Capacitor configuration for the external oscillator.
// - Pinout:
//    P0.0 -> /SYSCLK
//
//    P1.6 -> LED
//
//    all other port pins unused
//
//    XTAL1 -> External Resistor-Capacitor Configuration
//
//
// How To Test:
//
// 1) Define the capacitor and pull-up resistor values using <Cap_Value> and
//      <Resistor_Value>.
// 2) Connect the RC oscillator to XTAL1 as shown in the Oscillators chapter
//      of the datasheet.
// 3) Compile and download code to a 'F12x device.
// 4) Run the code:
//      - the test will blink an LED
//      - the 'F12x will also output the SYSCLK to a port pin (P0.0) for
//         observation
//
//
// FID:            12X000031
// Target:         C8051F12x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -10 JULY 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F120.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

// Note: The Cap_Value and Resistor_Value numbers are formatted specifically
// to keep all numbers whole (not decimals) and within a representable range
// (32 bits).

#define Cap_Value 56                   // Value of the external capacitor, in
                                       // pF
                                       // The capacitor should be no greater
                                       // than 100 pF.

#define Resistor_Value 249000L         // Value of the external pull-up
                                       // resistor, in ohms

                                       // Note: Debugging cannot occur when the
                                       // SYSCLK is less than 32 kHz.  The
                                       // resistor-capacitor external
                                       // oscillator includes a divide by 2
                                       // stage.

sfr16 RCAP2 = 0xCA;                    // Timer2 reload value
sfr16 TMR2 = 0xCC;                     // Timer2 counter

sbit LED = P1^6;                       // LED='1' means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void ExtRCOsc_Init (void);
void Port_Init (void);
void Timer2_Init (void);

void Timer2_ISR (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, switches to the external
// resistor-capacitor oscillator, and loops forever, blinking the LED.
//
//-----------------------------------------------------------------------------
void main (void) 
{
   WDTCN = 0xDE;                       // Disable watchdog timer
   WDTCN = 0xAD;

   SFRPAGE = CONFIG_PAGE;

   SYSCLK_Init ();                     // Initialize system clock to 24.5MHz

   ExtRCOsc_Init ();                   // Initialize for and switch to the
                                       // external resistor-capacitor
                                       // oscillator

   Port_Init ();                       // Initialize crossbar and GPIO

   SFRPAGE = TMR2_PAGE;

   Timer2_Init ();                     // Init Timer2 to generate
                                       // interrupts for the LED.

   EA = 1;                             // Enable global interrupts

   SFRPAGE = LEGACY_PAGE;              // Sit in this SFRPAGE

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
// This routine initializes the system clock to use the internal 24.5 MHz
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   OSCICN = 0x83;                      // Configure internal oscillator for
                                       // its highest frequency (24.5 MHz)

   RSTSRC = 0x04;                      // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// ExtRCOsc_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes for and switches to an external resistor-capacitor
// for the External Oscillator.
//
//-----------------------------------------------------------------------------
void ExtRCOsc_Init (void)
{
   // Error checks on the input values
   // The capacitor should be no greater than 100 pF.
   #if (Cap_Value > 100)
      #error "Defined Capacitor Value outside allowable range!"
      #define XFCN 0
   #endif

   // Set the appropriate XFCN bits for the oscillator frequency
   //
   //   XFCN     RC (XOSCMD = 10x)
   //   000      f <= 25 kHz
   //   001      25 kHz < f <= 50 kHz
   //   010      50 kHz < f <= 100 kHz
   //   011      100 kHz < f <= 200 kHz
   //   100      200 kHz < f <= 400 kHz
   //   101      400 kHz < f <= 800 kHz
   //   110      800 kHz < f <= 1.6 MHz (1600 kHz)
   //   111      1.6 MHz (1600 kHz) < f <= 3.2 MHz (3200 kHz)
   //
   // from the datasheet: f (in MHz) = 1230 / (R (in kohms) * C (in pF))
   // f (in kHz) = (1230 * 10^3) / (R (in kohms) * C (in pF))
   // f (in kHz) = (1230 * 10^3 * 10^3) / (R (in ohms) * C (in pF))
   // f (in kHz) = 1230000000 / (R (in ohms) * C (in pF))
   //
   // Output_Freq = 1230000000 / (Resistor_Value * Cap_Value)
   #define Output_Freq 1230000000L/(Resistor_Value * Cap_Value)

   // Try to determine the closest XFCN setting to the Output_Freq value
   #if (Output_Freq <= 25L)
      #define XFCN 0
   #elif (Output_Freq <= 50L)
      #define XFCN 1
   #elif (Output_Freq <= 100L)
      #define XFCN 2
   #elif (Output_Freq <= 200L)
      #define XFCN 3
   #elif (Output_Freq <= 400L)
      #define XFCN 4
   #elif (Output_Freq <= 800L)
      #define XFCN 5
   #elif (Output_Freq <= 1600L)
      #define XFCN 6
   #elif (Output_Freq <= 3200L)
      #define XFCN 7
   #else
      #define XFCN 0
      #error "Resistor_Value and Cap_Value produce out-of-bounds frequency!"
   #endif

   OSCXCN = 0x40;                      // External Oscillator is an external
                                       // resistor-capacitor (divide by 2
                                       // stage)

   OSCXCN |= XFCN;

   // RC and C modes typically require no startup time.

   CLKSEL = 0x01;                      // Switch the system clock to the
                                       // external oscillator circuit.
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
//
// P1.6   digital   push-pull     LED
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0MDOUT |= 0x01;                    // Enable /SYSCLK as a push-pull output

   P1MDOUT |= 0x40;                    // Enable LED as a push-pull output

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
// Configure Timer2 to 16-bit auto-reload and generate an interrupt for an
// approximate 10 Hz blinking rate.
//
//-----------------------------------------------------------------------------
void Timer2_Init (void)
{
   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // auto-reload mode

   TMR2CF = 0x00;                      // Use SYSCLK/12 as timebase; count up

   // Init reload value;
   // Resistor-capacitor external oscillator with divide by 2 stage (in Hz),
   // Timer2 uses SYSCLK/12 for timebase, aim for a blink rate of roughly 10 Hz
   RCAP2 = -((Output_Freq*1000)/2/12/10);

   // Init the Timer2 value.
   TMR2 = -((Output_Freq*1000)/2/12/10);

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
   TF2 = 0;                            // Clear Timer2 interrupt flag

   LED = ~LED;                         // Change state of LED
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------