//-----------------------------------------------------------------------------
// main.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program is a Template for  firmware that uses Sleep or Suspend Mode
//
// Target:         Si101x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    - 18 MAY 10 (FB)
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <Si1010_defs.h>
#include <Si1010_lib.h>
                       
//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------


SBIT (TRIGGER,     SFR_P1, 5);         // Generates a 1ms trigger pulse
SBIT (DEBUG_PIN,   SFR_P1, 6);         // Used to show analog acquisition time

SBIT (POT_ENABLE,  SFR_P1, 4);         // '0' means potentiometer enabled
SBIT (SW2,         SFR_P0, 2);         // SW2 == 0 means switch pressed
SBIT (SW3,         SFR_P0, 3);         // SW3 == 0 means switch pressed



//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------


// Constants for the analog measurement
#define VREF      (1650L * 2L)         // Internal 1.65V Reference with a
                                       // 2x multiplier due to the 0.5 gain
                                       // at the front end of the ADC

#define THRESHOLD ((2500L*1024L)/VREF) // 2.5 Volt Threshold converted to
                                       // a 10-bit ADC code used to determine
                                       // if the analog voltage is greater
                                       // or less than 2.5V

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void);
void PORT_Init (void);
void ADC0_Init (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   while(!SW2);                        // Debug Trap -- Prevents the system
                                       // from entering sleep mode after
                                       // a reset if Switch 2 is pressed.


   PORT_Init ();                       // Initialize Port I/O

   TRIGGER = 0;                        // Initilize the state of the signals
   DEBUG_PIN = 0;                      

   OSCILLATOR_Init ();                 // Initialize Oscillator
   ADC0_Init ();                       // Initialize ADC0

 
   RTC_Init ();                        // Initialize SmaRTClock
   RTC_WriteAlarm(WAKE_INTERVAL_TICKS);// Set the Alarm Value
   RTC0CN_SetBits(RTC0TR+RTC0AEN+ALRM);// Enable Counter, Alarm, and Auto-Reset

   LPM_Init();                         // Initialize Power Management
   LPM_Enable_Wakeup(RTC);
                                       
   
   //----------------------------------
   // Main Application Loop
   //----------------------------------
   while (1)
   {
      
      //-----------------------------------------------------------------------
      // Task #1 - Handle RTC Failure
      //-----------------------------------------------------------------------
      if(RTC_Failure)
      {
         RTC_Failure = 0;              // Reset RTC Failure Flag to indicate
                                       // that we have detected an RTC failure
                                       // and are handling the event

         // Do something...RTC Has Stopped Oscillating
         while(1);                     // <Insert Handler Code Here>
      }


      //-----------------------------------------------------------------------
      // Task #2 - Handle RTC Alarm
      //-----------------------------------------------------------------------
      if(RTC_Alarm)
      {

         RTC_Alarm = 0;                // Reset RTC Alarm Flag to indicate 
                                       // that we have detected an alarm
                                       // and are handling the alarm event

         //-----------------------------------------------------------------------
         // Generate Oscilloscope Trigger Pulse
         //-----------------------------------------------------------------------
         //
         // This code generates a 1 ms pulse using the SmaRTClock. The RTC alarm
         // is temporarily "borrowed" for 1ms and restored at the end of the
         // 1ms interval.

         TRIGGER = 1;                    // Set TRIGGER indicator HIGH
         
         RTC_SleepTicks(ONE_MS_TICKS);   // Sleep for 1 ms
         
         TRIGGER = 0;                    // Set TRIGGER indicator low

         //-----------------------------------------------------------------------
         // Measure Analog Value
         //-----------------------------------------------------------------------
         //
         // Take ADC conversion.
         //
         // Enable the Potentiometer
         POT_ENABLE = 0;                     // Enable the Potentiometer (active-low)

         DEBUG_PIN = 1;                      // Turn on the debug signal

         // Initiate a Conversion
         AD0INT = 0;                         // clear ADC0 conv. complete flag
         AD0WINT = 0;                        // clear window detector flag
         AD0BUSY = 1;                        // initiate conversion

         // Wait for conversion to complete
         while(!AD0INT);

         DEBUG_PIN = 0;                      // Turn off the debug pin

         POT_ENABLE = 1;                     // Disable the Potentiometer (active-low)
         

         // Check if window detector has detected a voltage less than 2.5 V, then 
         // the AD0WINT flag will be logic 1. If the voltage is higher than 2.5V,
         // then the AD0WINT flag will be a logic 0.  
         if(AD0WINT == 0)
         {
            // Do something if the potentiometer voltage is > 2.5V
         }

      }

      //-----------------------------------------------------------------------
      // Task #3 - Sleep for the remainder of the interval
      //-----------------------------------------------------------------------

      // Place the device in Sleep Mode
      LPM(SLEEP);                      // Enter Sleep Until Next Alarm
      
      
   }
}


//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal precision
// oscillator.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{


   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor enabled.

   CLKSEL = 0x04;                      // Select low power internal osc.
                                       // divided by 1 as the system clock

   FLSCL |= 0x40;                      // Set the BYPASS bit for operation > 10MHz

}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
// P0.2   digital   open-drain    Switch 2
// P0.3   digital   open-drain    Switch 3
//
// P0.6   analog    open-drain    Potentiometer / Analog Input
//
// P1.5   digital   push-pull     TRIGGER indicator 
// P1.6   digital   push-pull     Debug Pin

//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0MDIN |= 0x0C;                     // P0.2, P0.3 are digital
   P0MDIN &= ~0x40;                    // P0.6 is analog
   P1MDIN |= 0x60;                     // P1.5, P1.6 are digital

   P0MDOUT &= ~0x0C;                   // P0.2, P0.3 are open-drain
   P1MDOUT |= 0x60;                    // P1.5, P1.6 are push-pull

   P0     |= 0x0C;                     // Set P0.2, P0.3 latches to '1'

   XBR2    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configures ADC0 for Burst Mode
//
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   ADC0CN = 0x40;                      // ADC0 disabled, Burst Mode enabled,
                                       // conversion triggered on writing '1'
                                       // to AD0BUSY

   ADC0MX = 0x06;                      // Select Potentiometer as the ADC input

   REF0CN = 0x1C;                      // Select internal high speed voltage
                                       // reference and enable the temperature
                                       // sensor

   ADC0CF = ((SYSCLK/8300000))<<3;     // Set SAR clock to 8.3MHz

   SFRPAGE = 0x0F;
   ADC0PWR = 0x04;
   SFRPAGE = 0x00;

   ADC0CF |= 0x00;                     // Select Gain of 0.5

   ADC0AC = 0x00;                      // Right-justify results, shifted right
                                       // by 0 bits. Accumulate 1 sample for
                                       // an output word of 10-bits.

   ADC0GT = 0xFFFF;                    // Setup Window detector for 2.5V
   ADC0LT = THRESHOLD;


}




              