//-----------------------------------------------------------------------------
// Si101x_smaRTClock_Date_Tracker.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program will keep track of the date and time and output both to the
// toolstick terminal. The initial date and time can be set by changing
// YEAR_INIT, DAY_INIT, HOUR_INIT, MINUTE_INIT under the global constants
// section. Each time P0.2 is pressed, the current date and time will be output
// to the toolstick terminal. If a smaRTClock error occurs, it will be output
// to the terminal.
//
// The program works by setting an RTC alarm to trigger at the end of each day,
// or after the 24th hour. Each alarm trigger increments a global variable
// which stores the the day number. Each time SW2 is pressed, the day (ranging
// from 1-366) is decoded into a month number (ranging from 1-12) and a day of
// the month number (ranging from 1-31). The RTC counter value is captured and
// and decoded into hours, minutes, seconds, and hundreths of a second. To
// handle leap years, the section of the code that decodes the day of the year
// to a month and day of the month is broken up into two sections: leap year
// and non leap year. The first RTC alarm is set to trigger at the end of the
// day based on the initial time settings in the global constants section.
// After the first alarm, a software flag is set (the variable 'second_alarm')
// and the RTC alarm is reconfigured to trigger after every 24 hours.
//
// To switch between SLEEP and SUSPEND mode, change the definition of
// POWER_MODE in the global constants section of the code to either SLEEP
// or SUSPEND
//
// Note: At the begining of Main(), there is a trap while loop. If the device
//       is stuck in SLEEP mode and you are unable to connect to the device,
//       unplug the daughter card from the toolstick. Then, while holding down
//       SW2, plug the daughter card back into the toolstick and then try to
//       connect to the device. After you are connected to the device, you can
//       release SW2.
//
// How To Test:
//
// 1) Download code to the device
// 2) Disconnect from the device in the IDE
// 3) Open terminal
// 4) Each time SW2 (P0.2) is pressed, the current time will be output to
//    the terminal screen
//
//
// Target:         Si101x
// Tool chain:     Generic
// Command Line:   None
//
//
// Release 1.0
//    - Initial Revision (FB)
//    - 22 SEP 2008
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>             // Compiler declarations
#include <Si1010_defs.h>               // SFR declarations
#include <stdio.h>                     // For printf()

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (SW2, SFR_P0, 2);                 // SW2 == 0 means switch pressed

//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------
// Made output_month, output_days, hours, minutes, seconds, hundreths all INTs
// because printf will only print INTs in decimal form

typedef struct DATE_INFO {
   U16 years;
   U16 output_month;
   U16 days;
   U16 output_days;
   U16 hours;
   U16 minutes;
   U16 seconds;
   U16 hundreths;
} DATE_INFO;

//-----------------------------------------------------------------------------
// Global Constants and Variables
//-----------------------------------------------------------------------------

#define SYSCLK         24500000        // SYSCLK frequency in Hz

#define BAUDRATE         115200        // Baud rate of UART in bps

#define RTCCLK            32768        // SmaRTClock frequency in Hz

#define RTC_DAY      2831155200        // Number of RTC counts in a day
#define RTC_HOUR      117964800        // Number of RTC counts in an hour
#define RTC_MINUTE      1966080        // Number of RTC counts in a minute
#define RTC_SECOND        32768        // Number of RTC counts in a second
#define RTC_HUNDRETH        328        // Number of RTC counts in a hundreth
                                       // of a second (rounded up)

#define SUSPEND            0x40        // Value to write to PMU0CF to place
                                       // the device in Suspend mode

#define SLEEP              0x80        // Value to write to PMU0CF to place
                                       // the device in Sleep Mode

#define POWER_MODE      SUSPEND        // Select between Suspend and Sleep
                                       // mode. When debugging, if the
                                       // MCU is stopped/halted while
                                       // in Sleep Mode, the connection
                                       // to the IDE will be lost. The
                                       // IDE connection will not be lost
                                       // if the MCU is stopped/halted
                                       // while in suspend mode.

#define SWITCH_PRESSED        0        // Macros to determine switch state
#define SWITCH_NOT_PRESSED    1

#define FIVE_MS    5*(SYSCLK/12/1000)  // 5 ms delay constant
#define US_DELAY   (SYSCLK/12/1000)/10 // 500 us delay constant

// Used in debounce section of code
#define DEBOUNCE_INTERVAL  10          // Debounce interval in milliseconds
#define T3_DEBOUNCE_TICKS  DEBOUNCE_INTERVAL*(SYSCLK/12/1000)

// Initial settings for time and date
// Current date setting would be September 22, 2008
#define YEAR_INIT             8        // Output will be 2000+YEAR_INIT
#define DAY_INIT            266        // Valid range of DAY_INIT is 1-366
#define HOUR_INIT            10        // Valid range of HOUR_INIT is 0-23
#define MINUTE_INIT          15        // Valid range of MINUTE_INIT is 0-59
#define SECOND_INIT          55        // Valid range of SECOND_INIT is 0-59

// Convert initial time settings to RTC alarm counts. Used in RTC_Init()
// (note: '\' is in following line to allow code to continue on next line)
#define TIME_INIT    ((RTC_HOUR*HOUR_INIT)+(RTC_MINUTE*MINUTE_INIT)+   \
                     (RTC_SECOND*SECOND_INIT))

DATE_INFO event1;                      // This will keep the runnnig date
                                       // information

// Software flags to indicate smaRTClock events are pending
bit Alarm_Pending = 0;
bit smaRTClock_Fail = 0;

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

U8 RTC_Read (U8 reg);
void RTC_Write (U8 reg, U8 value);
U8 Leap_Year (U16 year);
void Compute_Month (U16* month, U16* days, U16 total, U16 year);
void SYSCLK_Init (void);
void PORT_Init (void);
void RTC_Init (void);
void UART0_Init (void);
INTERRUPT_PROTO (RTC0ALARM_ISR, INTERRUPT_RTC0ALARM);
INTERRUPT_PROTO (RTC0_OSC_FAIL_ISR, INTERRUPT_RTC0_OSC_FAIL);

#if defined SDCC
   void putchar (char output);
#endif

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void)
{

   static bit wakeup_pending;          // This is checked before putting the
                                       // device to sleep to ensure a wake-up
                                       // wasn't triggered during a clear of
                                       // PMU0CF

   volatile bit second_alarm;          // Software flag used to signify that
                                       // the first alarm has already occured.
   U8 wakeup_source;

   UU32 RTC_Capture;                   // Will hold contents of RTC timer when
                                       // button is pushed

   UU32 alarm_interval_second;         // Will hold new alarm interval after
                                       // second_alarm bit set

   PCA0MD &= ~0x40;                    // Disable watchdog timer

   // Trap in case device gets stuck in SLEEP
   if (!SW2)
   {
      while (1);
   }

   PORT_Init ();                       // Initialize Port I/O
   SYSCLK_Init ();                     // Initialize Oscillator
   UART0_Init ();                      // Initialize UART
   RTC_Init ();                        // Initialize RTC

   printf("\nsmaRTClock Date Tracker\n");

   // Initialize DATE_INFO struct
   event1.seconds = SECOND_INIT;
   event1.minutes = MINUTE_INIT;
   event1.hours   = HOUR_INIT;
   event1.days    = DAY_INIT;
   event1.years   = (2000+YEAR_INIT);

   EA = 1;                             // Enable global interrupts
   second_alarm = 0;                   // Initialize software flag

   // Update output_month and output_days for output
   Compute_Month(&event1.output_month, &event1.output_days, event1.days,
                  event1.years);

   // Output initial time
   printf("Current Time: %d-%d-%d %d:%d:%d", event1.output_month,
            event1.output_days, event1.years, event1.hours, event1.minutes,
            event1.seconds);

   printf("\nPress P0.2 to Output Time\n");

   //----------------------------------
   // Main Application Loop
   //----------------------------------
   while (1)
   {
         wakeup_pending = 0;           // Clear software flag

         // Clear the PMU wake-up flags
         // Interrupts for all enabled sources that are not persistent
         // should be enabled when clearing the wake-up source flags.
         // This is a precautionary step in case these events occur
         // during the same clock cycle that clears the PMU0CF flags.
         EIE1 |= 0x02;                 // Enable the alarm interrupt
         EIE2 |= 0x04;                 // Enable the clock fail interrupt
         PMU0CF = 0x20;
         EIE1 &= ~0x02;                // Disable the alarm interrupt
         EIE2 &= ~0x04;                // Disable the clock fail interrupt

         // Check for non-persistent wake-up sources
         if ((Alarm_Pending) || (smaRTClock_Fail))
         {
            wakeup_pending = 1;        // Set software flag
         }

         // Enable wake-up sources and place the device in Sleep Mode
         // Wake-Up sources enabled are:
         // A. smaRTClock Alarm
         // B. smaRTClock Oscillator Fail
         // C. Port Match (wake-up on switch press)
         // D. Pulse on Reset Pin (always enabled)
         if ((!wakeup_pending) | (SW2))
         {
          RSTSRC = 0x04;               // Disable VDDMON, leave missing clock
                                       // detector enabled
          // Put device to sleep
          PMU0CF = 0x0E | POWER_MODE;  // To change between SUSPEND or SLEEP
         }                             // modes, change the POWER_MODE
                                       // definition in the global constants

         //--------------------------------------------------------------------
         // ~~~~~~~~~~~~~~~~~~~~ Device Sleeping ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         //--------------------------------------------------------------------

         while (!(VDM0CN & 0x20));     // Wait for VDDOK to be set to 1

         RSTSRC = 0x06;                // Enable VDDMON, missing clock detector

         // Read the wake-up source flags
         wakeup_source = PMU0CF & 0x1F;

         // Clear the wake-up source flags
         PMU0CF = 0x20;

         // Check for a reset pin wake-up (wait about 5 ms)
         if (wakeup_source & 0x10)
         {
               TMR3 = 0;               // Initialize T3 to zero
               TMR3CN |= 0x04;         // Start timer (TR3 = 1)
               while (TMR3 < FIVE_MS);
               TMR3CN &= ~0x04;        // Stop timer (TR3 = 0)
         }

         // Check for smaRTClock Oscillator failure
         if ((wakeup_source & 0x08) || smaRTClock_Fail)
         {
            EA = 0;
            smaRTClock_Fail = 0;       // Clear the software flag
            printf("\nsmaRTClock Failure\n");
            while (1);

         }

         // Check for smaRTClock alarm
         if ((wakeup_source & 0x04) || Alarm_Pending)
         {
            Alarm_Pending = 0;         // Clear the software flag

            if ((event1.days == 365) & (Leap_Year(event1.years)))
            {
                  event1.days++;       // Check for leap year, increment days
            }
            else if (event1.days >= 365)
            {
                  event1.days = 1;     // If non-leap year, reset days,
                  event1.years ++;     // increment years
            }
            else
            {
                  event1.days ++;
            }

            // 5 ms wait
            TMR3 = 0;                  // Initialize T3 to zero
            TMR3CN |= 0x04;            // Start timer (TR3 = 1)
            while (TMR3 < FIVE_MS);
            TMR3CN &= ~0x04;           // Stop timer (TR3 = 0)

            if (!second_alarm)
            {
               second_alarm = 1;       // Set software flag, only want to come
                                       // into this section of code once

               alarm_interval_second.U32 = (RTC_DAY); // Set alarm to trigger
                                                      // every 24 hours
               // Copy the alarm interval to the alarm registers
               RTC_Write (ALARM0, alarm_interval_second.U8[b0]);     // LSB
               RTC_Write (ALARM1, alarm_interval_second.U8[b1]);
               RTC_Write (ALARM2, alarm_interval_second.U8[b2]);
               RTC_Write (ALARM3, alarm_interval_second.U8[b3]);     // MSB
            }
         }

         // Check for Port Match event
         if (wakeup_source & 0x02)
         {
            if (SW2 == SWITCH_PRESSED)
            {
               // Debounce the Switch (wait > 5 ms)
               TMR3 = 0;               // Initialize T3 to zero
               TMR3CN |= 0x04;         // Start timer (TR3 = 1)
               while (TMR3 < T3_DEBOUNCE_TICKS);
               TMR3CN &= ~0x04;        // Stop timer (TR3 = 0)

               // Check if the switch is still pressed
               if (SW2 == SWITCH_PRESSED)
               {
                  // Compute month and days for output
                    Compute_Month(&event1.output_month, &event1.output_days,
                           event1.days, event1.years);

                  RTC_Write (RTC0CN, 0xDD);        // Initiate smaRTClock timer
                                                   // capture
                  // Wait for smaRTClock timer capture valid
                  while ((RTC_Read (RTC0CN) & 0x01)== 0x01);

                  // Copy the RTC capture timer to the RTC capture variable
                  RTC_Capture.U8[b0] = RTC_Read (CAPTURE0);    // LSB
                  RTC_Capture.U8[b1] = RTC_Read (CAPTURE1);
                  RTC_Capture.U8[b2] = RTC_Read (CAPTURE2);
                  RTC_Capture.U8[b3] = RTC_Read (CAPTURE3);    // MSB

                  if (!second_alarm){
                     // Adjust captured time with initial time settings if
                     // frst day has not passed
                     (RTC_Capture.U32) = (RTC_Capture.U32)+TIME_INIT;
                  }

                  event1.hours = (RTC_Capture.U32)/RTC_HOUR;
                  (RTC_Capture.U32) = (RTC_Capture.U32)%RTC_HOUR;
                  event1.minutes = (RTC_Capture.U32)/RTC_MINUTE;
                  (RTC_Capture.U32) = (RTC_Capture.U32)%RTC_MINUTE;
                  event1.seconds = (RTC_Capture.U32)/RTC_SECOND;
                  (RTC_Capture.U32) = (RTC_Capture.U32)%RTC_SECOND;
                  event1.hundreths = (RTC_Capture.U32)/RTC_HUNDRETH;

                  // Output date
                  printf("\nCurrent Time: %d-%d-%d ", event1.output_month,
                     event1.output_days, event1.years);

                  printf("%d:%d:%d.%d\n",event1.hours, event1.minutes,
                     event1.seconds, event1.hundreths);

                  // If device is configured to enter SLEEP mode, delay for
                  // a short time (~500 us) to let the UART finish
                  // finish transmitting before looping and going
                  // back into SLEEP mode
                  #if POWER_MODE == SLEEP
                     // 500 us wait
                     TMR3 = 0;                  // Initialize T3 to zero
                     TMR3CN |= 0x04;            // Start timer (TR3 = 1)
                     while (TMR3 < US_DELAY);
                     TMR3CN &= ~0x04;           // Stop timer (TR3 = 0)
                  #endif
               }
               else
               {
                  // Switch is bouncing, take no action
               }
            }
         }
   }
}

//-----------------------------------------------------------------------------
// Support Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// RTC_Read
//-----------------------------------------------------------------------------
//
// Return Value : RTC0DAT
// Parameters   : 1) U8 reg - address of RTC register to read
//
// This function will read one byte from the specified RTC register.
// Using a register number greater that 0x0F is not permited.
//
//-----------------------------------------------------------------------------

U8 RTC_Read (U8 reg)
{
   reg &= 0x0F;                        // Mask low nibble
   RTC0ADR  = reg;                     // Pick register
   RTC0ADR |= 0x80;                    // Set BUSY bit to read
   while ((RTC0ADR & 0x80) == 0x80);   // Poll on the BUSY bit
   return RTC0DAT;                     // Return value
}

//-----------------------------------------------------------------------------
// RTC_Write
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : 1) U8 reg - address of RTC register to write
//                2) U8 value - the value to write to <reg>
//
// This function will Write one byte to the specified RTC register.
// Using a register number greater that 0x0F is not permited.
//
//-----------------------------------------------------------------------------

void RTC_Write (U8 reg, U8 value)
{
   reg &= 0x0F;                        // Mask low nibble
   RTC0ADR  = reg;                     // Pick register
   RTC0DAT = value;                    // Write data
   while ((RTC0ADR & 0x80) == 0x80);   // Poll on the BUSY bit
}

//-----------------------------------------------------------------------------
// Leap_Year
//-----------------------------------------------------------------------------
//
// Return Value : '1' if a leap year, '0' if not a leap year
// Parameters   : U16 year - the current year in the counter
//
// This function will return a whether a year is a leap year or not
//
//-----------------------------------------------------------------------------

U8 Leap_Year (U16 year)
{
   if ((((year%4)==0) && ((year%100)!=0)) || ((year%400)==0))
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

//-----------------------------------------------------------------------------
// Compute_Month
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : 1) U16* month - pointer to month number variable (1-12)
//                2) U16* days - pointer to number of days  variable (1-31)
//                3) U16 total - total number of days to compute
//                   month and days from
//                4) U16 total - current year (in 20xx format)
//
// This function will calculate the month number and day number based on the
// total number of days in a given year that have passed. This function is
// split into a leap year section and non-leap year section.
//
//-----------------------------------------------------------------------------

void Compute_Month (U16* month, U16* days, U16 total, U16 year)
{
   if (total < 32)                     // First month does not depend on
   {                                   // leap year
      (*month) = 1;                    // January
      (*days) = total;
   }
   else if (Leap_Year(year))
   {
      // Calculate month based on leap year
      if (total < 61)
      {
         (*month) = 2;                 // February
         (*days) = total-31;
      }
      else if (total < 92)
      {
         (*month) = 3;                 // March
         (*days) = total-60;
      }
      else if (total < 122)
      {
         (*month) = 4;                 // April
         (*days) = total - 91;
      }
      else if (total < 153)
      {
         (*month) = 5;                 // May
         (*days) = total - 121;
      }
      else if (total < 183)
      {
         (*month) = 6;                 // June
         (*days) = total-152;
      }
      else if (total < 214)
      {
         (*month) = 7;                 // July
         (*days) = total - 182;
      }
      else if (total < 245)
      {
         (*month) = 8;                 // August
         (*days) = total - 213;
      }
      else if (total < 275)
      {
         (*month) = 9;                 // September
         (*days) = total - 244;
      }
      else if (total < 306)
      {
         (*month) = 10;                // October
         (*days) = total-274;
      }
      else if (total < 336)
      {
         (*month) = 11;                // November
         (*days) = total - 305;
      }
      else
      {
         (*month) = 12;                // December
         (*days) = total - 335;
      }

   }
   else
   {
      // Calculate month based on non leap year
      if (total < 60)
      {
         (*month) = 2;                 // February
         (*days) = total-31;
      }
      else if (total < 91)
      {
         (*month) = 3;                 // March
         (*days) = total-59;
      }
      else if (total < 121)
      {
         (*month) = 4;                 // April
         (*days) = total - 90;
      }
      else if (total < 152)
      {
         (*month) = 5;                 // May
         (*days) = total - 120;
      }
      else if (total < 182)
      {
         (*month) = 6;                 // June
         (*days) = total-151;
      }
      else if (total < 213)
      {
         (*month) = 7;                 // July
         (*days) = total - 181;
      }
      else if (total < 244)
      {
         (*month) = 8;                 // August
         (*days) = total - 212;
      }
      else if (total < 274)
      {
         (*month) = 9;                 // September
         (*days) = total - 243;
      }
      else if (total < 305)
      {
         (*month) = 10;                // October
         (*days) = total-273;
      }
      else if (total < 335)
      {
         (*month) = 11;                // November
         (*days) = total - 304;
      }
      else
      {
         (*month) = 12;                // December
         (*days) = total - 334;
      }
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal precision
// oscillator at its maximum frequency and enables the missing clock
// detector.
//
//-----------------------------------------------------------------------------

void SYSCLK_Init (void)
{

   OSCICN |= 0x80;                     // Enable the precision internal osc.

   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor enabled.

   CLKSEL = 0x00;                      // Select precision internal osc.
                                       // divided by 1 as the system clock
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
// P0.4   digital   push-pull     UART TX
// P0.5   digital   open-drain    UART RX
//
//-----------------------------------------------------------------------------

void PORT_Init (void)
{
   P0MDIN  |= 0x04;                    // P0.2 is an digital input

   P0MDOUT &= ~0x04;                   // P0.2 is open-drain
   P0MDOUT |= 0x10;                    // Enable UTX as push-pull output

   P0      |= 0x04;                    // Set P0.2 latch to '1'

   XBR0     = 0x01;                    // Enable UART on P0.4(TX) and P0.5(RX)
   XBR2     = 0x40;                    // Enable crossbar and enable
                                       // weak pull-ups

   // Setup the Port Match Wake-up Source to wake up on the falling
   // edge of P0.2 (when the switch is pressed)
   P0MASK   = 0x04;                    // Mask out all pins except for P0.2
   P0MAT    = 0x04;                    // P0.2 should be HIGH while the device
                                       // is in the low power mode
}

//-----------------------------------------------------------------------------
// RTC_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will initialize the smaRTClock.
//
//-----------------------------------------------------------------------------

void RTC_Init (void)
{
   U16 i;                              // Counter used in for loop

   UU32 alarm_interval_first;          // Stores first alarm value

   U8 CLKSEL_SAVE = CLKSEL;


   RTC0KEY = 0xA5;                     // Unlock the smaRTClock interface
   RTC0KEY = 0xF1;


   RTC_Write (RTC0XCN, 0x60);          // Configure the smaRTClock
                                       // oscillator for crystal mode
                                       // Bias Doubling Enabled
                                       // AGC Disabled


   RTC_Write (RTC0XCF, 0x83);          // Enable Automatic Load Capacitance
                                       // stepping and set the desired
                                       // load capacitance to 4.5pF plus
                                       // the stray PCB capacitance

   RTC_Write (RTC0CN, 0x80);           // Enable smaRTClock oscillator


   //----------------------------------
   // Wait for smaRTClock to start
   //----------------------------------

   CLKSEL    =  0x74;                  // Switch to 156 kHz low power
                                       // internal oscillator
   // Wait > 20 ms

   for (i=0x150;i!=0;i--);

   // Wait for smaRTClock valid
   while ((RTC_Read (RTC0XCN) & 0x10)== 0x00);


   // Wait for Load Capacitance Ready
   while ((RTC_Read (RTC0XCF) & 0x40)== 0x00);

   //----------------------------------
   // smaRTClock has been started
   //----------------------------------

   RTC_Write (RTC0XCN, 0xC0);          // Enable Automatic Gain Control
                                       // and disable bias doubling

   RTC_Write (RTC0CN, 0xC0);           // Enable missing smaRTClock detector
                                       // and leave smaRTClock oscillator
                                       // enabled.
   // Wait > 2 ms
   for (i=0x40;i!=0;i--);

   PMU0CF = 0x20;                      // Clear PMU wake-up source flags

   CLKSEL = CLKSEL_SAVE;               // Restore system clock
   while (!(CLKSEL & 0x80));           // Poll CLKRDY


   //----------------------------------
   // Set the smaRTClock Alarm
   //----------------------------------

   // Calculate the alarm interval in smaRTClock ticks
   alarm_interval_first.U32 = (RTC_DAY-TIME_INIT);

   // Copy the alarm interval to the alarm registers
   RTC_Write (ALARM0, alarm_interval_first.U8[b0]);   // LSB
   RTC_Write (ALARM1, alarm_interval_first.U8[b1]);
   RTC_Write (ALARM2, alarm_interval_first.U8[b2]);
   RTC_Write (ALARM3, alarm_interval_first.U8[b3]);   // MSB

   // Enable the smaRTClock timer and alarm with auto-reset
   RTC_Write (RTC0CN, 0xDC);
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1, for <BAUDRATE> and 8-N-1.
//
//-----------------------------------------------------------------------------

void UART0_Init (void)
{
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   #if (SYSCLK/BAUDRATE/2/256 < 1)
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   #elif (SYSCLK/BAUDRATE/2/256 < 4)
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   #elif (SYSCLK/BAUDRATE/2/256 < 12)
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   #else
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   #endif

   TL1 = TH1;                          // Init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
   TI0 = 1;                            // Indicate TX0 ready
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// RTC0ALARM_ISR
//-----------------------------------------------------------------------------
//
// This ISR sets a software flag to indicate that the ALRM flag was set to 1.
//
//-----------------------------------------------------------------------------

INTERRUPT (RTC0ALARM_ISR, INTERRUPT_RTC0ALARM)
{
   EIE1 &= ~0x02;                      // Disable the Alarm interrupt

   Alarm_Pending = 1;                  // Set the software flag
}

//-----------------------------------------------------------------------------
// RTC0_OSC_FAIL_ISR
//-----------------------------------------------------------------------------
//
// This ISR sets a software flag to indicate that the ALRM flag was set to 1.
//
//-----------------------------------------------------------------------------

INTERRUPT (RTC0_OSC_FAIL_ISR, INTERRUPT_RTC0_OSC_FAIL)
{
   EIE2 &= ~0x04;                      // Disable the Clock Fail interrupt

   smaRTClock_Fail = 1;                // Set the software flag
}

#if defined SDCC

// SDCC does not include a definition for putchar(), which is used in printf()
// and so it is defined here.  The prototype does not need to be explicitly
// defined because it is provided in stdio.h

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : character to send to UART
//
// This function outputs a character to the UART.
//
//-----------------------------------------------------------------------------

void putchar (char output)
{
   if (output == '\n')
   {
      while (!TI0);
      TI0 = 0;
      SBUF0 = 0x0D;
   }
   while (!TI0);
   TI0 = 0;
   SBUF0 = output;
}

#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------