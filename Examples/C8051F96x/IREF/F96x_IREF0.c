//-----------------------------------------------------------------------------
// F96x_IREF0.c
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
// P0.1 - Yellow LED
// P0.2 - Switch
// P0.7 - IREF0 output
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
// Target:         C8051F96x
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
#include <C8051F960_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Define Hardware
//-----------------------------------------------------------------------------
#define UDP_F960_MCU_MUX_LCD
//#define UDP_F960_MCU_EMIF
//-----------------------------------------------------------------------------
// Hardware Dependent definitions
//-----------------------------------------------------------------------------
#ifdef UDP_F960_MCU_MUX_LCD
SBIT (SW1, SFR_P0, 0);
SBIT (LED2,  SFR_P0, 1);
#endif

#ifdef UDP_F960_MCU_EMIF
SBIT (SW1, SFR_P3, 0);
SBIT (LED2,  SFR_P3, 1);
#endif

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             20000000    // Clock speed in Hz

#define LED_ON                0
#define LED_OFF               1

#define LOW_POWER             0
#define HIGH_CURRENT          1

#define CURRENT_MODE       LOW_POWER   // "LOW_POWER" or "HIGH_CURRENT"

#if(CURRENT_MODE == HIGH_CURRENT)      // Set the maximum output code depending
   #define MAX_VALUE  0x0C             // on the currently selected current
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

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   bit rising = 1;

   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   PORT_Init ();                       // Initialize crossbar and GPIO
   OSCILLATOR_Init();                  // Initialize the system clock
   IREF0_Init();                       // Initialize IREF0

   while(1)
   {
      SFRPAGE = LEGACY_PAGE;
      // If Switch is not pressed
      if(SW1)
      {
         //----------------------------------------
         // Set LED State
         //----------------------------------------

         // Turn off the Yellow LED
         LED2 = LED_OFF;

         //----------------------------------------
         // Control output voltage/current
         //----------------------------------------

         // If output voltage should be increasing
         if(rising)
         {
            // Increment the output current
            SFRPAGE = LEGACY_PAGE;
				IREF0CN++;
            // If we have reached the maximum value
            if((IREF0CN & 0x3F) >= MAX_VALUE)
            {
               // Change Direction
               rising = 0;
            }
         } else

         // Output voltage should be decreasing
         {
            // Decrement the output current
            SFRPAGE = LEGACY_PAGE;
            IREF0CN--;

            // If we have reached the minimum value
            if((IREF0CN & 0x3F) == MIN_VALUE)
            {
               // Change Direction
               rising = 1;
            }
         }

         //----------------------------------------
         // Insert Delay
         //----------------------------------------

         Wait_MS(50);                  // Wait 50 milliseconds

      } else

      // switch SW1 is pressed
      {

         //----------------------------------------
         // Set LED State
         //----------------------------------------

         // Turn on LED2
         LED2 = LED_ON;

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
// P0.7   analog    open-drain/High-Z    IREF0

//-----------------------------------------------------------------------------
void PORT_Init (void)
{

   // IREF0 Analog Output
   SFRPAGE = LEGACY_PAGE;
   P0MDIN  &= ~0x80;                   // P0.7 is analog
   P0MDOUT &= ~0x80;                   // P0.7 is open-drain
   P0      |=  0x80;                   // Set P0.7 latch to '1'
   P0SKIP  |=  0x80;                   // P0.7 skipped in Crossbar

   XBR2    = 0x40;                     // Enable crossbar and enable                                       // weak pull-ups
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
   SFRPAGE = LEGACY_PAGE;
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
   SFRPAGE = LEGACY_PAGE;

   #if(CURRENT_MODE == HIGH_CURRENT)

      IREF0CN = 0x40;                  // Configure IREF0 as a current source
                                       // in High Current mode

   #else

      IREF0CN = 0x00;                  // Configure IREF0 as a current source
                                       // in Low Power mode
   #endif
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

   SFRPAGE = LEGACY_PAGE;
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
// End Of File
//-----------------------------------------------------------------------------
