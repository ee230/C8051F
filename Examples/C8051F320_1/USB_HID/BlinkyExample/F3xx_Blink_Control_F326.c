//-----------------------------------------------------------------------------
// F3xx_Blink_Control_F326.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
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
// FID             3XX000002
// Target:         C8051F32x/C8051F340
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_BlinkyExample
//
// Release 1.3
//    -All changes by BW
//    -1 SEP 2010
//    -Updated USB0_Init() to write 0x89 to CLKREC instead of 0x80
// Release 1.2 (ES)
//    -Added support for Raisonance
//    -No change to this file
//    -02 APR 2010
// Release 1.1
//    -Added feature reports for dimming controls
//    -Added PCA dimmer functionality
//    -16 NOV 2006
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

sbit Led1 = P2^2;                      // LED='1' means ON
sbit Led2 = P2^3;

#define ON  1
#define OFF 0

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F32x
//-----------------------------------------------------------------------------
sfr16 TMR2RL   = 0xca;                   // Timer2 reload value
sfr16 TMR2     = 0xcc;                   // Timer2 counter



// ----------------------------------------------------------------------------
// Global Variable Declarations
// ----------------------------------------------------------------------------
unsigned char BLINK_PATTERN[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned int xdata BLINK_RATE;
unsigned char xdata BLINK_SELECTOR;
unsigned char xdata BLINK_ENABLE;
unsigned char xdata BLINK_SELECTORUPDATE;
unsigned char xdata BLINK_LED1ACTIVE;
unsigned char xdata BLINK_LED2ACTIVE;
unsigned char xdata BLINK_DIMMER;
unsigned char xdata BLINK_DIMMER_SUCCESS;

// ----------------------------------------------------------------------------
// Local Function Prototypes
// ----------------------------------------------------------------------------
void Timer0_Init(void);
void Sysclk_Init(void);
void Port_Init(void);
void Usb_Init(void);
void Timer2_Init(void);
void Adc_Init(void);
void Timer_Init(void);
void Delay(void);

void Blink_Update(void);


// ----------------------------------------------------------------------------
// Global Variables
// ----------------------------------------------------------------------------
// Last packet received from host
unsigned char xdata OUT_PACKET[9] = {0,0,0,0,0,0,0,0,0};

// Next packet to send to host
unsigned char xdata IN_PACKET[3]  = {0,0,0};


// ----------------------------------------------------------------------------
// Local Variables
// ----------------------------------------------------------------------------
bit TOGGLE1;

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer1_ISR
//-----------------------------------------------------------------------------
// This interrupt controls LED blinking.
//-----------------------------------------------------------------------------
void Timer1_ISR (void) interrupt 3
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
               Led1 = ON;
               Led1Active++;
            }
            // Turn LED off
            else Led1 = OFF;

            // Turn LED on
            if(BLINK_PATTERN[index] & 0x02)
            {
               Led2 = ON;
               Led2Active++;
            }
            // Turn LED off
            else Led2 = OFF;
         }
         else
         {
            Led1 = OFF;
            Led2 = OFF;
         }
         index++;

      }
   }
   else // Blinking Not Enabled
   {
      Led1 = OFF;
      Led2 = OFF;
   }

   if (!(P2 & 0x01))                   // Check for switch #1 pressed
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

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// System_Init(void)
//-----------------------------------------------------------------------------
// This routine calls subroutines that initialize peripherals on the
// microncontroller.
//-----------------------------------------------------------------------------
void System_Init(void)
{
   Sysclk_Init();
   Port_Init();
   Timer_Init();
}

//-----------------------------------------------------------------------------
// Usb_Init
//-----------------------------------------------------------------------------
// USB Initialization performs the following:
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//
//-----------------------------------------------------------------------------
void Usb_Init(void)
{

   POLL_WRITE_BYTE(POWER,  0x08);      // Force Asynchronous USB Reset
   POLL_WRITE_BYTE(IN1IE,  0x03);      // Enable Endpoint 0-1 in interrupts
   POLL_WRITE_BYTE(OUT1IE, 0x02);      // Enable Endpoint 0-1 out interrupts
   POLL_WRITE_BYTE(CMIE,   0x07);      // Enable Reset, Resume, and Suspend
                                       // interrupts
   USB0XCN = 0xE0;                     // Enable transceiver; select full speed
   POLL_WRITE_BYTE(CLKREC, 0x89);      // Enable clock recovery, single-step
                                       // mode disabled

   EIE1 |= 0x02;                       // Enable USB0 Interrupts
   EA = 1;                             // Global Interrupt enable
                                       // Enable USB0 by clearing the USB
   POLL_WRITE_BYTE(POWER,  0x01);      // Inhibit Bit and enable suspend
                                       // detection
}

//-----------------------------------------------------------------------------
// Sysclk_Init
//-----------------------------------------------------------------------------
// This routine intitalizes the system clock and USB clock.
//
//-----------------------------------------------------------------------------
void Sysclk_Init(void)
{
   OSCICN |= 0x83;
   CLKMUL  = 0x00;
   CLKMUL |= 0x80;

   Delay();

   CLKMUL |= 0xC0;                     // Initialize the clock multiplier

   while(!(CLKMUL & 0x20));            // Wait for multiplier to lock

   CLKSEL = 0x02;                      // Use Clock Multiplier/2 as
                                       // system clock
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
// Port Initialization
// - Configure the Crossbar and GPIO ports.
//
//-----------------------------------------------------------------------------
void Port_Init(void)
{

   P2MDOUT |= 0x0C;                    // Enable LEDs as a push-pull outputs

   GPIOCN = 0x40;                      // This is the reset value
}



//-----------------------------------------------------------------------------
// Timer_Init
//-----------------------------------------------------------------------------
// Timer initialization
// - Timer 1 reload, used to check if switch pressed on overflow and
// used for ADC continuous conversion
//
//-----------------------------------------------------------------------------
void Timer_Init(void)
{

   TCON      = 0x40;
   TMOD      = 0x20;
   CKCON     = 0x02;

   TH1       = 0x00;                   // Set reload value
   IE        = 0x08;                   // Enable timer interrupt

}

//-----------------------------------------------------------------------------
// Delay
//-----------------------------------------------------------------------------
// Used for a small pause, approximately 80 us in Full Speed,
// and 1 ms when clock is configured for Low Speed
//
//-----------------------------------------------------------------------------
void Delay(void)
{

   int x;
   for(x = 0;x < 500;x)
      x++;

}


//-----------------------------------------------------------------------------
// Timer1_ISR
//-----------------------------------------------------------------------------

