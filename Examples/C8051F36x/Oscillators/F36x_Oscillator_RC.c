//-----------------------------------------------------------------------------
// F36x_Oscillator_RC.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the oscillator for use with an
// external RC Oscillator.
//
//
// How To Test:
//
// 1) Define the capacitor and pull-up resistor values using <Cap_Value> and
//      <Resistor_Value>.
// 2) Connect the RC oscillator to XTAL2 as shown in the Oscillators chapter
//      of the datasheet.
// 3) Compile and download code to a 'F36x device.
// 4) Run the code:
//      - the test will blink an LED
//      - the 'F36x will also output the SYSCLK to a port pin (P0.0) for
//         observation
//
//
// FID:            36X000019
// Target:         C8051F36x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (FB / TP)
//    -13 OCT 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F360.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// External Oscillator RC Frequency Selection
//-----------------------------------------------------------------------------
//
// Note: The Cap_Value and Resistor_Value numbers are formatted specifically
// to keep all numbers whole (not decimals) and within a representable range
// (32 bits).
//

#define Cap_Value 33                   // Value of the external capacitor, in
                                       // pF
                                       // The capacitor should be no greater
                                       // than 100 pF.

#define Resistor_Value 330000L         // Value of the external pull-up
                                       // resistor, in ohms

                                       // Note: Debugging cannot occur when the
                                       // SYSCLK is less than 32 kHz.

//-----------------------------------------------------------------------------
// 16-bit Registers for the 'F36x
//-----------------------------------------------------------------------------

sfr16 TMR2RL = 0xCA;
sfr16 TMR2 = 0xCC;

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

sbit LED = P3^2;                       // LED='1' means on

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void ExtRCOsc_Init (void);
void Port_Init (void);
void Timer2_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   SYSCLK_Init();                      // Initialize SYSCLK
   Port_Init();                        // Initialize Port I/O
   ExtRCOsc_Init ();                   // Initialize External Oscillator
   Timer2_Init();

   EA = 1;

   SFRPAGE = LEGACY_PAGE;

   while (1) {};                       // Loop forever

}                                      // End of main()

//-----------------------------------------------------------------------------
// Initialization Subroutines
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
void SYSCLK_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its highest frequency (24.5 MHz)

   RSTSRC = 0x04;                      // Enable missing clock detector

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
// The oscillator pins should be configured as analog inputs when using the
// external oscillator in crystal mode.
//
// P0.0   digital    push-pull     /SYSCLK
// P0.6   analog     don't care    XTAL2
//
// P3.2   digital    push-pull     LED
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   // Oscillator Pins
   P0MDIN &= ~0x40;                    // P0.6 is analog
   P0SKIP |= 0x40;                     // P0.6 skipped in the Crossbar

   // LED pin
   P3MDOUT |= 0x04;                    // P3.2 is push-pull

   // Buffered System Clock Output
   P0MDOUT |= 0x01;                    // P0.0 is push-pull

   // Crossbar Initialization
   XBR0    = 0x08;                     // Route /SYSCLK to first available pin
   XBR1    = 0x40;                     // Enable Crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// ExtRCOsc_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the external oscillator
// in RC mode.
//
//-----------------------------------------------------------------------------
void ExtRCOsc_Init (void)
{
   int i;                              // Delay counter
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

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
                                       // resistor-capacitor

   OSCXCN |= XFCN;

   // RC and C modes typically require no startup time.

   for (i=0; i < 256; i++);            // Wait for RC osc. to start

   CLKSEL = 0x01;                      // Select external oscillator as system
                                       // clock source

   OSCICN = 0x00;                      // Disable the internal oscillator.

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
void Timer2_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // 16-bit auto-reload mode

   CKCON &= ~0x20;                     // Use SYSCLK/12 as timebase

   // Init reload value;
   // Timer2 uses SYSCLK/12 for timebase, aim for a blink rate of roughly 10 Hz
   TMR2RL = -((Output_Freq*1000)/12/10);

   // Init the Timer2 value.
   TMR2 = -((Output_Freq*1000)/12/10);

   ET2 = 1;                            // Enable Timer2 interrupts

   TR2 = 1;                            // Start Timer2

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
void Timer2_ISR (void) interrupt 5
{
   TF2H = 0;                           // Clear Timer2 interrupt flag

   LED = ~LED;                         // Change state of LED
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------