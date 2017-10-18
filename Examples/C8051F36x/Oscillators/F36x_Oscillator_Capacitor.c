//-----------------------------------------------------------------------------
// F36x_Oscillator_Capacitor.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the oscillator for use with an
// external capacitor.
//
// Pinout:
//
//    P0.0 - /SYSCLK
//
//    P0.6 - XTAL2
//
//    P3.2 -> LED
//
//    all other port pins unused
//
// How To Test:
//
// 1) Define the VDD voltage, capacitor value, and desired frequency using
//      <VDD_Value>, <Cap_Value>, and <Desired_Freq>.
// 2) Compile and download code to a 'F0xx device.
// 3) Run the code:
//      - the test will blink an LED
//      - the 'F0xx will also output the SYSCLK to a port pin (P0.0) for
//         observation
//
// FID:            36X000015
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

#include <C8051F360.h>                // SFR declarations

//-----------------------------------------------------------------------------
// External Oscillator RC Frequency Selection
//-----------------------------------------------------------------------------
//
// Please initialize the following constant to the target frequency
//
#define VDD_Value 3300                 // Value of VDD for the 'F36x in mV
                                       // VDD should be no higher than 3600 mV

#define Cap_Value 33                   // Value of the external capacitor, in
                                       // pF
                                       // The capacitor should be no greater
                                       // than 100 pF.

#define Desired_Freq 100000            // roughly the desired frequency, in Hz
                                       // Note: Debugging cannot occur when the
                                       // SYSCLK is less than 32 kHz.

//-----------------------------------------------------------------------------
// 16-bit Register Definitions for the 'F36x
//-----------------------------------------------------------------------------

sfr16 TMR2RL = 0xCA;                   // Timer2 Reload Register
sfr16 TMR2 = 0xCC;                     // Timer2 Register

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

sbit LED = P3^2;                       // LED='1' means on

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void ExtCapOsc_Init (void);
void Port_Init (void);
void Timer2_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   Port_Init();                        // Initialize Port I/O
   SYSCLK_Init ();                     // Initialize Oscillator
   ExtCapOsc_Init ();                  // Initialize External Capacitor
                                       // Oscillator
   Timer2_Init ();                     // Init Timer2 to generate interrupts
                                       // for the LED.

   EA = 1;

   SFRPAGE = LEGACY_PAGE;

   while (1) {};                       // Loop forever
}                                      // End of main()

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
// The oscillator pins should be configured as analog inputs when using the
// external oscillator in capacitor mode.
//
// P0.0   digital    push-pull     /SYSCLK
// P0.6   analog     don't care    XTAL2
//
// P3.2   digital    push-pull     LED
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   // Oscillator Pins
   P0MDIN &= ~0x40;                    // P0.6 is analog
   P0SKIP |= 0x40;                     // P0.6 skipped in the Crossbar

   // LED Pins
   P3MDOUT |= 0x04;                    // P3.2 is push-pull

   // Buffered System Clock Output
   P0MDOUT |= 0x01;                    // P0.0 is push-pull

   // Crossbar Initialization
   XBR0    = 0x08;                     // Route /SYSCLK to first available pin
   XBR1    = 0x40;                     // Enable Crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// SYSCLK_Init
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
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN |= 0x03;                     // Initialize internal oscillator to
                                       // highest frequency

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
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
   unsigned char i;
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   // Error checks on the input values
   // The capacitor should be no greater than 100 pF.
   #if (Cap_Value > 100)
      #error "Defined Capacitor Value outside allowable range!"
      #define XFCN 0
   // The VDD value should not be above 3.6 V, according to the Table in
   // Chapter 3 of the datasheet.
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
   #elif (KFactor <= 245000L) // (65000 + 180000) / 2
      #define XFCN 4
   #elif (KFactor <= 844000L) // (180000 + 664000) / 2
      #define XFCN 5
   #elif (KFactor <= 1127000L) // (664000 + 1590000) / 2
      #define XFCN 6
   #else // > (664000 + 1590000) / 2
      #define XFCN 7
   #endif

   OSCXCN = 0x50;                      // External Oscillator is an external
                                       // capacitor

   OSCXCN |= XFCN;

   // RC and C modes typically require no startup time.

   for (i = 0; i < 255; i++);          // Wait for C osc. to start

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
//-----------------------------------------------------------------------------
void Timer2_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;
                                       // 16-bit auto-reload mode

   CKCON &= ~0x20;                     // use SYSCLK/12 as timebase

   TMR2RL = -(Desired_Freq/12/10);     // Init reload value;
                                       // Timer2 uses SYSCLK/12 for timebase,
                                       // aim for a blink rate of roughly
                                       // 10 Hz
   TMR2 = -(Desired_Freq/12/10);       // Init the Timer2 value.

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
//-----------------------------------------------------------------------------
void Timer2_ISR (void) interrupt 5
{
   TF2H = 0;                           // Clear Timer2 interrupt flag

   LED = ~LED;                         // Change state of LED
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------