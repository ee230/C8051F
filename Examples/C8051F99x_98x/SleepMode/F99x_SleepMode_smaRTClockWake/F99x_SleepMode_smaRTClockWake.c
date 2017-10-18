//-----------------------------------------------------------------------------
// F99x_SleepMode_smaRTClockWake.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program flashes the red LED on the C8051F996 target board about
// five times a second using the interrupt handler for Timer2 when the
// MCU is awake.
//
// The smaRTClock is configured to generate a wake-up event every 2 seconds.
// The device will toggle between Sleep/Suspend mode and Normal mode. In
// the low power state, the Yellow LED will be turned on.
//
// If a smaRTClock oscillator failure occurs, both LEDs will remain on
// and the device will be in a high power state.
//
// How To Test:
//
// 1) Download code to the 'F99x target board
// 2) Ensure that pins 1-2, 3-4, 5-6, 7-8 are shorted together on the
//    J8 header
// 3) Run the program. The Red LED will blink when the device is awake.
//
//
// Target:         C8051F99x-C8051F98x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    - Initial Revision (FB)
//    - 19 MAY 2010
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F990_defs.h>            // SFR declarations
#include <C8051F990_lib.h>             // library declarations

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (RED_LED,     SFR_P1, 5);         // Generates a 1ms trigger pulse
SBIT (YELLOW_LED,  SFR_P1, 3);         // Used to show analog acquisition time
SBIT (SW2,        SFR_P0, 2);          // SW2 == 0 means switch pressed
SBIT (SW3,        SFR_P0, 3);          // SW3 == 0 means switch pressed


//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
bit Blink_LED = 0;

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define LED_ON           0
#define LED_OFF          1

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
LOCATED_VARIABLE_NO_INIT (reserved, U8, SEG_XDATA, 0x0000);

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void);
void PORT_Init (void);
void Timer3_Init (int counts);

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

   YELLOW_LED = LED_OFF;               // Initilize the state of the signals
   RED_LED = LED_OFF;

   OSCILLATOR_Init ();                 // Initialize Oscillator

   Timer3_Init (RTCCLK / 2 / 10);      // Init Timer3 to generate interrupts
                                       // at a 10 Hz rate.

   RTC_Init ();                        // Initialize SmaRTClock
   RTC_WriteAlarm(WAKE_INTERVAL_TICKS);// Set the Alarm Value
   RTC0CN_SetBits(RTC0TR+RTC0AEN+ALRM);// Enable Counter, Alarm, and Auto-Reset

   LPM_Init();                         // Initialize Power Management
   LPM_Enable_Wakeup(RTC);

   EA = 1;


   RTC_Alarm = 1;                      // Set the RTC Alarm Flag on startup

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

         Blink_LED = !Blink_LED;       // Change blinking state

         if(Blink_LED)
         {
            // Adjust LED state for active mode
            YELLOW_LED = LED_OFF;
            RED_LED = LED_ON;

         }
         else
         {
            // Adjust LED state for sleep mode
            RED_LED = LED_OFF;
            YELLOW_LED = LED_ON;
         }

      }

      //-----------------------------------------------------------------------
      // Task #3 - Sleep for the remainder of the interval
      //-----------------------------------------------------------------------
      // If blinking, stay awake, otherwise sleep

      if(Blink_LED)
      {
         // Wait for next alarm or clock failure, then clear flags
         while((PMU0CF & RTCAWK) == 0);
         if(PMU0CF & RTCAWK) RTC_Alarm = 1;
         if(PMU0CF & RTCFWK) RTC_Failure = 1;
         PMU0CF = 0x20;

      }
      else
      {
         // Place the device in Sleep Mode
         P0 &= ~0x01;
         LPM(SLEEP);                      // Enter Sleep Until Next Alarm
         P0 |= 0x01;

      }

   }
}
//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer3 overflows.
//
INTERRUPT(Timer3_ISR, INTERRUPT_TIMER3)
{
   TMR3CN &= ~0x80;                       // clear Timer3 interrupt flag
   RED_LED = !RED_LED;                    // change state of the RED_LED

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
// P1.5   digital   push-pull     Red LED
// P1.3   digital   push-pull     Yellow LED
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0MDIN |= 0x0C;                     // P0.2, P0.3 are digital
   P1MDIN |= 0x28;                     // P1.5, P1.3 are digital

   P0MDOUT &= ~0x0C;                   // P0.2, P0.3 are open-drain
   P1MDOUT |= 0x28;                    // P1.5, P1.3 are push-pull

   P0     |= 0x0C;                     // Set P0.2, P0.3 latches to '1'

   XBR2    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups

   P1MDIN &= ~0xC0;                    // P1.6 and P1.7 (RTC Pins) are
                                       // analog
}

//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
void Timer3_Init (int counts)
{
   TMR3CN  = 0x03;                        // Stop Timer3; Clear TF3;
                                          // use SmaRTClock as timebase

   CKCON  &= ~0xC0;                       // Timer3 clocked based on T2XCLK;

   TMR3RL  = -counts;                     // Init reload values
   TMR3    = TMR3RL;                      // initalize timer to reload value
   EIE1   |= 0x80;                        // enable Timer3 interrupts
   TMR3CN |= 0x04;                        // start Timer3
}




