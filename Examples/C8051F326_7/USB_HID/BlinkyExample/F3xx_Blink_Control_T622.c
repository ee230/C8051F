//-----------------------------------------------------------------------------
// F3xx_Blink_Control_T622.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Contains functions that control the LEDs.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   F3xx_BlinkyExample
//
// Release 1.3
//    -All changes by ES
//    -4 OCT 2010
//    -Updated USB0_Init() to write 0x8F to CLKREC instead of 0x80
//    -Updated USB0_Init() to write 0xAF to CLKREC instead of 0xA0
// Release 1.2
//    -11 SEP 2008 (TP)
//    -Updated for 'T622/3
//
// Release 1.1
//    -Added feature reports for dimming controls
//    -Added PCA dimmer functionality
//    -16 NOV 2006
//
// Release 1.0
//    -Initial Revision (PD)
//    -07 DEC 2005
//

//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------
#include "C8051F3xx.h"
#include "F3xx_Blink_Control.h"
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Register.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define SYSCLK             12000000    // SYSCLK frequency in Hz

// USB clock selections (SFR CLKSEL)
#define USB_4X_CLOCK       0x00        // Select 4x clock multiplier, for USB
#define USB_INT_OSC_DIV_2  0x10        // Full Speed
#define USB_EXT_OSC        0x20
#define USB_EXT_OSC_DIV_2  0x30
#define USB_EXT_OSC_DIV_3  0x40
#define USB_EXT_OSC_DIV_4  0x50

// System clock selections (SFR CLKSEL)
#define SYS_INT_OSC        0x00        // Select to use internal oscillator
#define SYS_EXT_OSC        0x01        // Select to use an external oscillator
#define SYS_4X_DIV_2       0x02

SBIT (SW1, SFR_P0, 1);
SBIT (LED1, SFR_P0, 6);                // LED='1' means ON
SBIT (LED2, SFR_P1, 2);

#define ON  1
#define OFF 0

//-----------------------------------------------------------------------------
// Global Variable Declarations
//-----------------------------------------------------------------------------

unsigned char BLINK_PATTERN[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned int xdata BLINK_RATE;
unsigned char xdata BLINK_SELECTOR;
unsigned char xdata BLINK_ENABLE;
unsigned char xdata BLINK_SELECTORUPDATE;
unsigned char xdata BLINK_LED1ACTIVE;
unsigned char xdata BLINK_LED2ACTIVE;
unsigned char xdata BLINK_DIMMER;
unsigned char xdata BLINK_DIMMER_SUCCESS;

//-----------------------------------------------------------------------------
// Local Function Prototypes
//-----------------------------------------------------------------------------

void Sysclk_Init(void);
void Port_Init(void);
void Usb_Init(void);
void Timer2_Init(void);
void Timer0_Init(void);
void PCA_Init(void);
void Delay(void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

// Last packet received from host
unsigned char xdata OUT_PACKET[9] = {0,0,0,0,0,0,0,0,0};

// Next packet to send to host
unsigned char xdata IN_PACKET[3]  = {0,0,0};

//-----------------------------------------------------------------------------
// Local Variables
//-----------------------------------------------------------------------------

bit TOGGLE1;

//-----------------------------------------------------------------------------
// Timer0_ISR
//-----------------------------------------------------------------------------
INTERRUPT(Timer0_ISR, INTERRUPT_TIMER0)
{
   static unsigned char xdata index = 0;
   static unsigned int xdata multiplier;
   static unsigned char xdata PatternSize;
   static unsigned char xdata Led1Active;
   static unsigned char xdata Led2Active;

   // If LED blinking has been enabled in the host application
   if(BLINK_ENABLE)
   {
      // Once the timer has been serviced BLINK_RATE times, update
      // the pattern
      if(multiplier++ >= BLINK_RATE)
      {
         multiplier = 0;

         // If index has reached the end of the pattern array,
         // reset the index and take measurements
         if((BLINK_PATTERN[index] == 0xFF) || (index >= 8)) {
            PatternSize = index;

            // Calculations for the percentage of the pattern
            // where the LED is on
            BLINK_LED1ACTIVE = (unsigned char)((unsigned int)Led1Active
                                 * 100 / PatternSize);
            BLINK_LED2ACTIVE = (unsigned char)((unsigned int)Led2Active
                                 * 100 / PatternSize);

            Led1Active = 0;
            Led2Active = 0;
            index = 0;
         }


         if(BLINK_PATTERN[index] != 0xFF)
         {
            // Turn LED on
            if(BLINK_PATTERN[index] & 0x01)
            {
               PCA0CPH0 = BLINK_DIMMER;
               Led1Active++;
            }
            // Turn LED off
            else PCA0CPH0 = 0xFF;

            // Turn LED on
            if(BLINK_PATTERN[index] & 0x02)
            {
               PCA0CPH1 = BLINK_DIMMER;
               Led2Active++;
            }
            // Turn LED off
            else PCA0CPH1 = 0xFF;
         }
         else
         {
            PCA0CPH0 = 0xFF;
            PCA0CPH1 = 0xFF;
         }
         index++;

      }
   }
   else // Blinking Not Enabled
   {
        PCA0CPH0 = 0xFF;
        PCA0CPH1 = 0xFF;
   }

   if (!(SW1))                         // Check for switch #1 pressed
   {
      if (TOGGLE1 == 0)                // Toggle is used to debounce switch
      {                                // so that one press and release will
                                       // send a single report to the host
         TOGGLE1 = 1;
         BLINK_SELECTOR = BLINK_SELECTOR + 255/5;
         BLINK_SELECTORUPDATE = 1;
      }
   }
   else TOGGLE1 = 0;                   // Reset toggle variable
}

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// System_Init (void)
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// This top-level initialization routine calls all support routine.
//
// ----------------------------------------------------------------------------
void System_Init(void)
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer

   Sysclk_Init();                      // Initialize oscillator
   Port_Init();                        // Initialize crossbar and GPIO
   Usb_Init();                         // Initialize USB0
   Timer2_Init();                      // Initialize timer2
   Timer0_Init();
   PCA_Init();
}

//-----------------------------------------------------------------------------
// Sysclk_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Initialize system clock to maximum frequency.
//
// ----------------------------------------------------------------------------
void Sysclk_Init(void)
{
#ifdef _USB_LOW_SPEED_

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKSEL  = SYS_INT_OSC;              // Select System clock
   CLKSEL |= USB_INT_OSC_DIV_2;        // Select USB clock
#else
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   // This clock multiplier code is no longer necessary, but it is retained
   // here for backwards compatibility with the 'F34x.

   CLKMUL  = 0x00;                     // Select internal oscillator as
                                       // input to clock multiplier

   CLKMUL |= 0x80;                     // Enable clock multiplier
   Delay();                            // Delay for clock multiplier to begin
   CLKMUL |= 0xC0;                     // Initialize the clock multiplier
   Delay();                            // Delay for clock multiplier to begin

   while(!(CLKMUL & 0x20));            // Wait for multiplier to lock
   CLKSEL  = SYS_INT_OSC;              // Select system clock
   CLKSEL |= USB_4X_CLOCK;             // Select USB clock
#endif  /* _USB_LOW_SPEED_ */
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Routine configure the Crossbar and GPIO ports.
//
// P0.1   digital input   SW1
//
// P0.6   digital output  LED1, PCA channel 0 (CEX0)
//
// P1.0   digital input   SW2
//
// P1.2   digital output  LED2, PCA channel 1 (CEX1)
//
//-----------------------------------------------------------------------------
void Port_Init(void)
{
   P0SKIP = 0xBF;                      // Route CEX0 to P0.6
   P1SKIP = 0x03;                      // Route CEX1 to P1.2

   P0MDOUT |= 0x40;                    // Make CEX0 and CEX1 push-pull outputs
   P1MDOUT |= 0x04;

   XBR0 = 0x00;
   XBR1 = 0x42;                        // Enable Crossbar and 2 PCA channels
}

//-----------------------------------------------------------------------------
// PCA_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Configures two PCA channels to act in 8-bit PWM mode.
//
//-----------------------------------------------------------------------------
void PCA_Init (void)
{
   PCA0CN = 0x40;                      // Enable PCA counter
   PCA0MD = 0x08;                      // PCA operation suspended when in idle
   PCA0CPM0 = 0x42;                    // Configure channel 0 to 8-bit PWM
   PCA0CPM1 = 0x42;                    // Configure channel 1 to 8-bit PWM
}

//-----------------------------------------------------------------------------
// USB_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//
// ----------------------------------------------------------------------------
void Usb_Init(void)
{
   POLL_WRITE_BYTE(POWER,  0x08);      // Force Asynchronous USB Reset
   POLL_WRITE_BYTE(IN1IE,  0x07);      // Enable Endpoint 0-2 in interrupts
   POLL_WRITE_BYTE(OUT1IE, 0x07);      // Enable Endpoint 0-2 out interrupts
   POLL_WRITE_BYTE(CMIE,   0x07);      // Enable Reset, Resume, and Suspend
                                       // interrupts
#ifdef _USB_LOW_SPEED_
   USB0XCN = 0xC0;                     // Enable transceiver; select low speed
   POLL_WRITE_BYTE(CLKREC, 0xAF);      // Enable clock recovery; single-step
                                       // mode disabled; low speed mode enabled
#else
   USB0XCN = 0xE0;                     // Enable transceiver; select full speed
   POLL_WRITE_BYTE(CLKREC, 0x8F);      // Enable clock recovery, single-step
                                       // mode disabled
#endif /* _USB_LOW_SPEED_ */

   EIE1 |= 0x02;                       // Enable USB0 Interrupts
   EA = 1;                             // Global Interrupt enable
                                       // Enable USB0 by clearing the USB
                                       // Inhibit bit
   POLL_WRITE_BYTE(POWER,  0x01);      // and enable suspend detection
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Timer 2 reload, used to check if switch pressed on overflow.
//
//-----------------------------------------------------------------------------
void Timer2_Init(void)
{
   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;

   CKCON &= ~0xF0;                     // Timer2 clocked based on T2XCLK;
   TMR2RL = 0;                         // Initialize reload value
   TMR2 = 0xffff;                      // Set to reload immediately

   TR2 = 1;                            // Start Timer2
}

//-----------------------------------------------------------------------------
// Timer0_Init()
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Configure and enable Timer 0.  Timer 0 controls the LEDs update rate and
// controls the LED blinking.
//
//-----------------------------------------------------------------------------
void Timer0_Init(void)
{
   TCON = 0x10;
   TMOD = 0x02;
   CKCON = 0x02;
   TH0 = -0xFA;
   ET0 = 1;                            // Enable Timer 0 as interrupt source
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Delay
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Used for a small pause, approximately 80 us in Full Speed,
// and 1 ms when clock is configured for Low Speed
//
// ----------------------------------------------------------------------------
void Delay(void)
{

   int x;
   for(x = 0;x < 500;x)
      x++;

}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
