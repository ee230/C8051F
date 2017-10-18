//-----------------------------------------------------------------------------
// F12x_Oscillator_Capacitor.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This example demonstrates how to initialize for and switch to a Capacitor
// configuration for the external oscillator.
// - Pinout:
//    P0.0 -> /SYSCLK
//
//    P1.6 -> LED
//
//    all other port pins unused
//
//    XTAL1 and XTAL2 -> External Capacitor Configuration
//
//
// How To Test:
//
// 1) Define the VDD voltage, capacitor value, and desired frequency using
//      <VDD_Value>, <Cap_Value>, and <Desired_Freq>.
// 2) Compile and download code to a 'F12x device.
// 3) Run the code:
//      - the test will blink an LED
//      - the 'F12x will also output the SYSCLK to a port pin (P0.0) for
//         observation
//
//
// FID:            12X000029
// Target:         C8051F12x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -9 JUNE 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F120.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

// Note: The VDD_Value, Cap_Value, Desired_Freq, and KFactor numbers are
// formatted specifically to keep all numbers whole (not decimals) and within
// a representable range (32 bits).

#define VDD_Value 3300                 // Value of VDD for the 'F12x in mV
                                       // VDD should be no higher than 3600 mV

#define Cap_Value 56                   // Value of the external capacitor, in
                                       // pF
                                       // The capacitor should be no greater
                                       // than 100 pF.

#define Desired_Freq 100000            // roughly the desired frequency, in Hz
                                       // Note: Debugging cannot occur when the
                                       // SYSCLK is less than 32 kHz.  The
                                       // capacitor external oscillator
                                       // includes a divide by 2 stage.

sfr16 RCAP2 = 0xCA;                    // Timer2 reload value
sfr16 TMR2 = 0xCC;                     // Timer2 counter

sbit LED = P1^6;                       // LED='1' means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void ExtCapOsc_Init (void);
void Port_Init (void);
void Timer2_Init (void);

void Timer2_ISR (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, switches to the external
// capacitor oscillator, and loops forever, blinking the LED.
//
//-----------------------------------------------------------------------------
void main (void) 
{
   WDTCN = 0xDE;                       // disable watchdog timer
   WDTCN = 0xAD;

   SFRPAGE = CONFIG_PAGE;

   SYSCLK_Init ();                     // Initialize system clock to 24.5MHz

   ExtCapOsc_Init ();                  // Initialize for and switch to the
                                       // external capacitor oscillator

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
// ExtCapOsc_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes for and switches to an external capacitor for the
// External Oscillator.
//
//-----------------------------------------------------------------------------
void ExtCapOsc_Init (void)
{
   // Error checks on the input values
   // The capacitor should be no greater than 100 pF.
   #if (Cap_Value > 100)
      #error "Defined Capacitor Value outside allowable range!"
      #define XFCN 0
   // The VDD value should not be above 3.6 V, according to Table 3.1 in the
   // datasheet.
   #elif (VDD_Value > 3600)
      #error "Defined VDD Value outside allowable range!"
      #define XFCN 0
   #endif

   // Set the appropriate XFCN bits for the oscillator frequency
   //
   //   XFCN     C (XOSCMD = 10x)
   //   000      K Factor = 0.87 => 870
   //   001      K Factor = 2.6 => 2600
   //   010      K Factor = 7.7 => 7700
   //   011      K Factor = 22 => 22000
   //   100      K Factor = 65 => 65000
   //   101      K Factor = 180 => 180000
   //   110      K Factor = 664 => 664000
   //   111      K Factor = 1590 => 1590000

   // from the datasheet: f (in MHz) = KF / (C x VDD (in V))
   // f (in MHz) x C x VDD (in V) = KF
   // f (in Hz) x C x VDD (in V) = KF x 1000000
   // f (in Hz) x C x VDD (in mV) = KF x 1000000 x 1000
   // f (in Hz) x C x VDD (in mV) = KF x 1000000000
   //
   // Desired_Freq x Cap_Value x VDD_Value = KFactor x 1000000000
   // (Desired_Freq x Cap_Value x VDD_Value) / 1000000 = KFactor x 1000
   // (Desired_Freq)/1000 x (Cap_Value x VDD_Value)/1000 = KFactor x 1000
   #define KFactor (Desired_Freq/1000)*((Cap_Value*VDD_Value)/1000)

   // Try to determine the closest XFCN setting to the KFactor value
   //
   // For example, if the KFactor is 2000, the XFCN value should be 2.6,
   // but if the KFactor is 1600, the XFCN value should be 0.87
   #if (KFactor <= 1735L) // (870 + 2600) / 2
      #define XFCN 0
   #elif (KFactor <= 5150L) // (2600 + 7700) / 2
      #define XFCN 1
   #elif (KFactor <= 14850L) // (7700 + 22000) / 2
      #define XFCN 2
   #elif (KFactor <= 43500L) // (22000 + 65000) / 2
      #define XFCN 3
   #elif (KFactor <= 122500L) // (65000 + 180000) / 2
      #define XFCN 4
   #elif (KFactor <= 422000L) // (180000 + 664000) / 2
      #define XFCN 5
   #elif (KFactor <= 1127000L) // (664000 + 1590000) / 2
      #define XFCN 6
   #else // > (664000 + 1590000) / 2
      #define XFCN 7
   #endif

   OSCXCN = 0x40;                      // External Oscillator is an external
                                       // capacitor (divide by 2 stage)

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

   TMR2CF = 0x00;                      // use SYSCLK/12 as timebase; count up

   RCAP2 = -(Desired_Freq/2/12/10);    // Init reload value;
                                       // Capacitor external oscillator with
                                       // divide by 2 stage, Timer2 uses
                                       // SYSCLK/12 for timebase, aim for a
                                       // blink rate of roughly 10 Hz
   TMR2 = -(Desired_Freq/2/12/10);     // Init the Timer2 value.

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