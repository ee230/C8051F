//-----------------------------------------------------------------------------
// F99x_IREF0.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This software shows how to enable and configure the current reference
// to source current in Low Power Mode, where each step is 1uA and the
// maximum current is 63uA. When R14 is populated with a 10K Ohm resistor,
// the voltage on P0.7/IREF0 should vary between 0V and 0.63V. To keep the
// output voltage/current constant, press the P0.2 switch.
//
// The compile option <CURRENT_MODE> may be set to "HIGH_CURRENT" to place
// IREF0 in High Current mode, where each step is 8uA and the maximum
// current is 504uA. In this mode, the voltage should vary between 0V and 0.95V
// when a 10K Ohm resistor is used at the IREF0 output.
//
// Pinout:
//
// P0.2 - Switch
// P0.7 - IREF0 output
// P1.5 - Red LED
// P1.6 - Yellow LED
//
// How To Test:
//
// 1) Compile and download code to a target board.
// 2) Ensure that C19 and R14 are populated and that a shorting
//    block is installed on J7.
// 3) Ensure that a shorting block is installed on J8[5-6].
// 4) Place voltmeter or oscilloscope on P0.7/IREF0.
// 5) To hold the voltage/current at its current value, press the
//    P0.2 Switch.
//
// Target:         C8051F99x-C8051F98x
// Tool chain:     Generic
// Command Line:   None
//
//
// Release 1.0
//    - Initial Revision (FB)
//    - 19 MAY 2010
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F990_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (RED_LED,    SFR_P1, 5);          // '0' means ON, '1' means OFF
SBIT (YELLOW_LED, SFR_P1, 3);          // '0' means ON, '1' means OFF
SBIT (SW2,        SFR_P0, 2);          // SW2 == 0 means switch pressed
SBIT (SW3,        SFR_P0, 3);          // SW3 == 0 means switch pressed


//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             20000000    // Clock speed in Hz

#define LED_ON                0
#define LED_OFF               1

#define LOW_POWER             0
#define HIGH_CURRENT          1

#define CURRENT_MODE       HIGH_CURRENT  // "LOW_POWER" or "HIGH_CURRENT"

#if(CURRENT_MODE == HIGH_CURRENT)      // Set the maximum output code depending
   #define MAX_VALUE  0x0F             // on the currently selected current
#else                                  // mode
   #define MAX_VALUE  0x3F
#endif

#define MIN_VALUE     0x00             // Set the minimum output code


//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
LOCATED_VARIABLE_NO_INIT (reserved, U8, SEG_XDATA, 0x0000);

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);           // Configure the system clock
void PORT_Init (void);                 // Configure the Crossbar and GPIO
void IREF0_Init (void);                // Configure IREF0
void Wait_MS(unsigned int ms);         // Inserts programmable delay
void PCA0_Init (void);


//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   bit rising = 1;

   U8 pwm;

   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   PORT_Init ();                       // Initialize crossbar and GPIO
   OSCILLATOR_Init();                  // Initialize the system clock
   IREF0_Init();                       // Initialize IREF0
   PCA0_Init();                        // Initialize PCA0

   while(1)
   {
      // If P0.2 Switch is not pressed
      if(SW2)
      {
         //----------------------------------------
         // Set LED State
         //----------------------------------------

         // Turn off the Yellow LED
         YELLOW_LED = LED_OFF;

         //----------------------------------------
         // Control output voltage/current
         //----------------------------------------

         // If output voltage should be increasing
         if(rising)
         {
            // Increment the output current
            IREF0CN++;

            for(pwm = 128; pwm > 0; pwm--)
            {
               PCA0CPH0 = pwm;
               Wait_MS(1);
            }

            // If we have reached the maximum value
            if((IREF0CN & 0x3F) == MAX_VALUE)
            {
               // Change Direction
               rising = 0;
            }
         } else

         // Output voltage should be decreasing
         {
            // Decrement the output current
            IREF0CN--;

            // If we have reached the minimum value
            if((IREF0CN & 0x3F) == MIN_VALUE)
            {
               // Change Direction
               rising = 1;
            }

            Wait_MS(100);
         }

         //----------------------------------------
         // Insert Delay
         //----------------------------------------

         //Wait_MS(50);                  // Wait 50 milliseconds

      } else

      // P0.2 switch is pressed
      {

         //----------------------------------------
         // Set LED State
         //----------------------------------------

         // Turn on the Yellow LED
         YELLOW_LED = LED_ON;

         //----------------------------------------
         // Control output voltage/current
         //----------------------------------------

         // Keep the current value as-is.

         //----------------------------------------
         // Insert Delay
         //----------------------------------------

         // There is no need to insert a delay.


      }


   } // end while(1)

}

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
// Pinout:
//
// P0.2   digital   open-drain/High-Z    Switch 2
// P0.3   digital   open-drain/High-Z    Switch 3
// P0.7   analog    open-drain/High-Z    IREF0

// P1.5   digital   push-pull            Red LED
// P1.6   digital   push-pull            Yellow LED
//-----------------------------------------------------------------------------
void PORT_Init (void)
{

   // IREF0 Analog Output
   P0MDIN  &= ~0x80;                   // P0.7 is analog
   P0MDOUT &= ~0x80;                   // P0.7 is open-drain
   P0      |=  0x80;                   // Set P0.7 latch to '1'
   P0SKIP  |=  0x80;                   // P0.7 skipped in Crossbar

   // Switches
   P0MDIN  |=  0x0C;                   // P0.2, P0.3 are digital
   P0MDOUT &= ~0x0C;                   // P0.2, P0.3 are open-drain
   P0      |=  0x0C;                   // Set P0.2, P0.3 latches to '1'
   P0SKIP  |=  0x0C;                   // P0.2, P0.3 skipped in Crossbar

   // LEDs
   P1MDIN  |= 0x28;                    // P1.5, P1.3 are digital
   P1MDOUT |= 0x28;                    // P1.5, P1.3 are push-pull
   P1      |= 0x28;                    // Set P1.5, P1.3 latches to '1'
   P1SKIP  |= 0x28;                    // P1.5, P1.3 skipped in Crossbar


   XBR2    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal low power
// oscillator at its maximum frequency.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   CLKSEL = 0x04;                     // Select low power oscillator with a
                                      // clock divider value of 1 (20MHz)
}



//-----------------------------------------------------------------------------
// IREF0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the current reference to source current in
// Low Power Mode, where each step is 1uA and the maximum current is 63uA.
//
//-----------------------------------------------------------------------------
void IREF0_Init (void)
{

   #if(CURRENT_MODE == HIGH_CURRENT)

      IREF0CN = 0x40;                  // Configure IREF0 as a current source
                                       // in High Current mode

   #else

      IREF0CN = 0x00;                  // Configure IREF0 as a current source
                                       // in Low Power mode
   #endif

   IREF0CF = 0x80;                     // Use CEX0 for modulating the two
                                       // least significant current sources
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Wait_MS
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters:
//   1) unsigned int ms - number of milliseconds of delay
//                        range is full range of integer: 0 to 65335
//
// This routine inserts a delay of <ms> milliseconds.
//-----------------------------------------------------------------------------
void Wait_MS(unsigned int ms)
{
   char i;

   TR0 = 0;                            // Stop Timer0

   TMOD &= ~0x0F;                      // Timer0 in 8-bit mode
   TMOD |= 0x02;

   CKCON &= ~0x03;                     // Timer0 uses a 1:48 prescaler
   CKCON |= 0x02;


   TH0 = -SYSCLK/48/10000;             // Set Timer0 Reload Value to
                                       // overflow at a rate of 10kHz

   TL0 = TH0;                          // Init Timer0 low byte to the
                                       // reload value

   TF0 = 0;                            // Clear Timer0 Interrupt Flag
   ET0 = 0;                            // Timer0 interrupt disabled
   TR0 = 1;                            // Timer0 on

   while(ms--)
   {
      for(i = 0; i < 10; i++)
      {
         TF0 = 0;
         while(!TF0);
      }
   }

   TF0 = 0;

}


//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the PCA time base, and sets up 8-bit PWM output
// mode for Module 0 (CEX0 pin).
//
// The frequency of the PWM signal generated at the CEX0 pin is equal to the
// PCA main timebase frequency divided by 256.
//
// The PCA time base in this example is configured to use SYSCLK, and SYSCLK
// is set up to use the internal oscillator running at 24.5 MHz.  Therefore,
// the frequency of the PWM signal will be 24.5 MHz / 256 = 95.7 kHz.
// Using different PCA clock sources or a different processor clock will
// result in a different frequency for the PWM signal.
//
//    -------------------------------------------------------------------------
//    How "8-Bit PWM Mode" Works:
//
//       The PCA's 8-bit PWM Mode works by setting an output pin low every
//    time the main PCA counter low byte (PCA0L) overflows, and then setting
//    the pin high whenever a specific match condition is met.
//
//    Upon a PCA0L overflow (PCA0L incrementing from 0xFF to 0x00), two things
//    happen:
//
//    1) The CEXn pin will be set low.
//    2) The contents of the PCA0CPHn register for the module are copied into
//       the PCA0CPLn register for the module.
//
//    When the PCA0L register increments and matches the PCA0CPLn register for
//    the selected module, the CEXn pin will be set high, except when the
//    ECOMn bit in PCA0CPMn is cleared to '0'.  By varying the value of the
//    PCA0CPHn register, the duty cycle of the waveform can also be varied.
//
//    When ECOMn = '1', the duty cycle of the PWM waveform is:
//
//       8-bit PWM Duty Cycle = (256 - PCA0CPLn) / 256
//
//    To set the duty cycle to 100%, a value of 0x00 should be loaded into the
//    PCA0CPHn register for the module being used (with ECOMn set to '1').
//    When the value of PCA0CPLn is equal to 0x00, the pin will never be
//    set low.
//
//    To set the duty cycle to 0%, the ECOMn bit in the PCA0CPMn register
//    should be cleared to 0.  This prevents the PCA0CPLn match from occuring,
//    which results in the pin never being set high.
//    -------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void PCA0_Init (void)
{
   // Configure PCA time base; overflow interrupt disabled
   PCA0CN = 0x00;                      // Stop counter; clear all flags
   PCA0MD = 0x08;                      // Use SYSCLK as time base

   PCA0CPM0 = 0x42;                    // Module 0 = 8-bit PWM mode

   // Configure initial PWM duty cycle = 50%
   PCA0CPH0 = 256 - (256 * 0.5);

   // Start PCA counter
   CR = 1;
}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
