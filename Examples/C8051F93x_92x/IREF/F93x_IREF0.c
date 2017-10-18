//-----------------------------------------------------------------------------
// F93x_IREF0.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
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
// 1) Compile and download code to a 'F93x target board.
// 2) Ensure that C19 and R14 are populated and that a shorting
//    block is installed on J7.
// 3) Ensure that a shorting block is installed on J8[5-6].
// 4) Place voltmeter or oscilloscope on P0.7/IREF0.
// 5) To hold the voltage/current at its current value, press the
//    P0.2 Switch.
//
// Target:         C8051F93x
// Tool chain:     Generic
// Command Line:   None
//
//
//
// Release 1.2 
//    - Updated to be compatible with Raisonance Toolchain (FB)
//    - 14 APR 2010
//
// Release 1.1 
//    - Compiled and tested for C8051F930-TB (JH)
//    - 06 JULY 2009
//
// Release 1.0
//    - Initial Revision (FB)
//    - 15 AUG 2007
//

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F930_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (RED_LED,    SFR_P1, 5);          // '0' means ON, '1' means OFF
SBIT (YELLOW_LED, SFR_P1, 6);          // '0' means ON, '1' means OFF
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

#define CURRENT_MODE       LOW_CURRENT // "LOW_POWER" or "HIGH_CURRENT"

#if(CURRENT_MODE == HIGH_CURRENT)      // Set the maximum output code depending
   #define MAX_VALUE  0x0C             // on the currently selected current
#else                                  // mode
   #define MAX_VALUE  0x3F
#endif 

#define MIN_VALUE     0x00             // Set the minimum output code

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
         }

         //----------------------------------------
         // Insert Delay
         //----------------------------------------

         Wait_MS(50);                  // Wait 50 milliseconds
      
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
   P1MDIN  |= 0x60;                    // P1.5, P1.6 are digital
   P1MDOUT |= 0x60;                    // P1.5, P1.6 are push-pull
   P1      |= 0x60;                    // Set P1.5, P1.6 latches to '1'
   P1SKIP  |= 0x60;                    // P1.5, P1.6 skipped in Crossbar


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
// End Of File
//-----------------------------------------------------------------------------
