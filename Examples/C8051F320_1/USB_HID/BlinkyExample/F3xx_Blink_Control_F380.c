//-----------------------------------------------------------------------------
// F3xx_Blink_Control_F380.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
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
// FID             3XX000003
// Target:         C8051F32x/C8051F34x/C8051F38x
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_BlinkyExample
//
// Release 1.0
//    -Initial Revision (ES)
//    -18 JAN 2011
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
void Timer0_Init(void);
void PCA_Init(void);
void Delay(void);

void Blink_Update(void);


// ----------------------------------------------------------------------------
// Global Variables
// ----------------------------------------------------------------------------
// Last packet received from host
unsigned char xdata OUT_PACKET[9] = {0,0,0,0,0,0,0,0,0};

// Next packet to send to host
unsigned char xdata IN_PACKET[3]  = {0,0,0};




//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer0_ISR
//-----------------------------------------------------------------------------

void Timer0_ISR (void) interrupt 1
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

}

//-----------------------------------------------------------------------------
// Adc_ConvComplete_ISR
//-----------------------------------------------------------------------------
// Called after a conversion of the ADC has finished
// - Updates the appropriate variable for potentiometer
//
//
void Adc_ConvComplete_ISR(void) interrupt 10
{
   if(ADC0H != BLINK_SELECTOR)
   {
      BLINK_SELECTORUPDATE = 1;
      BLINK_SELECTOR = ADC0H;
   }
   AD0INT = 0;
}

// ----------------------------------------------------------------------------
// Initialization Routines
// ----------------------------------------------------------------------------

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
   Adc_Init();
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
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// - Configures ADC for single ended continuous conversion or Timer2
//
// ----------------------------------------------------------------------------
void Adc_Init(void)
{
   REF0CN  = 0x0E;                     // Enable voltage reference VREF
   AMX0P       = 0x04;                 // switch to potentiometer

   ADC0CF  = 0xFC;                     // SAR Period 0x1F, Left adjusted output

   ADC0CN  = 0xC2;                     // Continuous converion on timer 2
                                       // overflow with low power tracking
                                       // mode on

   EIE1   |= 0x08;                     // Enable conversion complete interrupt
}

// ----------------------------------------------------------------------------
// Port_Init
// ----------------------------------------------------------------------------
// Routine configure the Crossbar and GPIO ports.
//
void Port_Init(void)
{

   P0SKIP = 0xFF;                      // Route no peripherals to Port 0
   P1SKIP = 0xFF;                      // Route no peripherals to Port 1
   P2SKIP = 0x03;                      // Skip switches
   P2MDIN   = ~(0x20);                 // Port 2 pin 5 set as analog input
   P2MDOUT |= 0x0C;                    // Port 2 pins 2,3 set high impedance
   P2SKIP  |= 0x20;                    // Port 2 pin 5 skipped by crossbar
   XBR0     = 0x00;
   XBR1     = 0x42;                    // Enable Crossbar, enable 2 PCA chan.


}

// ----------------------------------------------------------------------------
// PCA_Init
// ----------------------------------------------------------------------------
// Configures two PCA channels to act in 8-bit PWM mode.
//
void PCA_Init (void)
{
   PCA0CN = 0x40;                      // Enable PCA counter
   PCA0MD = 0x08;                      // PCA operation suspended when in idle
   PCA0CPM0 = 0x42;                    // Configure channel 0 to 8-bit PWM
   PCA0CPM1 = 0x42;                    // Configure channel 1 to 8-bit PWM
}

//-----------------------------------------------------------------------------
// USB0_Init
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
   POLL_WRITE_BYTE(CLKREC, 0xA9);      // Enable clock recovery; single-step
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
// Timer_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// - Timer 2 reload, used to check if switch pressed on overflow and
// used for ADC continuous conversion
//
// ----------------------------------------------------------------------------
void Timer2_Init(void)
{
   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;

   CKCON  &= ~0xF0;                    // Timer2 clocked based on T2XCLK;
   TMR2RL  = 0;                        // Initialize reload value
   TMR2    = 0xffff;                   // Set to reload immediately

   TR2     = 1;                        // Start Timer2
}



//-----------------------------------------------------------------------------
// Timer0_Init()
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Configure and enable Timer 0.
//
//-----------------------------------------------------------------------------
void Timer0_Init()
{
   TCON      = 0x10;
   TMOD      = 0x02;
   CKCON     = 0x02;
   TH0       = -0xFA;
   ET0 = 1;                            // Enable Timer 0 as interrupt source
}


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

