//-----------------------------------------------------------------------------
// F380_USB0_Mouse.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Source file for routines that mouse data.
//
//
// How To Test:    See Readme.txt
//
//
// FID:
// Target:         C8051F340
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_MouseExample
//
// Release 1.0
//    -Initial Revision (ES)
//    -18 JAN 2011
//


#include "c8051f3xx.h"
#include "F3xx_USB0_Mouse.h"
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

//-----------------------------------------------------------------------------
// Local Definitions
//-----------------------------------------------------------------------------

#define Sw1 0x01                       // These are the port2 bits for Sw1
#define Sw2 0x02                       // and Sw2 on the development board

//-----------------------------------------------------------------------------
// Global Variable Declarations
//-----------------------------------------------------------------------------
signed char MOUSE_VECTOR;
unsigned char MOUSE_AXIS;
unsigned char MOUSE_BUTTON;

unsigned char IN_PACKET[4];

//-----------------------------------------------------------------------------
// Local Variable Declarations
//-----------------------------------------------------------------------------

unsigned char SWITCH_1_STATE = 0;      // Indicate status of switch
unsigned char SWITCH_2_STATE = 0;      // starting at 0 == off

unsigned char TOGGLE1 = 0;             // Variable to make sure each button
unsigned char TOGGLE2 = 0;             // press and release toggles switch


//-----------------------------------------------------------------------------
// Local Function Prototypes
//-----------------------------------------------------------------------------

void Sysclk_Init (void);
void Port_Init (void);
void USB0_Init (void);
void Timer_Init (void);
void ADC0_Init (void);
void Delay (void);

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// Called when Timer 2 overflows, check to see if switch is pressed,
// then watch for release.
//
//-----------------------------------------------------------------------------

void Timer2_ISR (void) interrupt 5
{
   if (!(P2 & Sw1))                    // Check for switch #1 pressed
   {
      if (TOGGLE1 == 0)                // Toggle is used to debounce switch
      {                                // so that one press and release will
                                       // toggle the state of the switch sent
         SWITCH_1_STATE = ~SWITCH_1_STATE;
         TOGGLE1 = 1;                  // to the host
      }
   }
   else TOGGLE1 = 0;                   // Reset toggle variable

   SWITCH_2_STATE = (P2 & Sw2);

   TF2H = 0;                           // Clear Timer2 interrupt flag
}

//-----------------------------------------------------------------------------
// Adc_ConvComplete_ISR
//-----------------------------------------------------------------------------
// Called after a conversion of the ADC has finished
// - Updates the appropriate variable for either potentiometer or temperature
//   sensor
// - Switches the Adc multiplexor value to switch between the potentiometer and
//   temp sensor
//
//-----------------------------------------------------------------------------
void Adc_ConvComplete_ISR (void) interrupt 10
{

   AD0INT = 0;                         // acknowledge interrupt

   MOUSE_VECTOR = ADC0H - 128;         // save potentiometer value, and add
                                       // offset

   MOUSE_VECTOR = MOUSE_VECTOR / 16;   // reduce magnitude of value

   // Switch 1 functions to toggle between movement along the
   // x axis and movement along the y axis
   if (SWITCH_1_STATE)
   {
      MOUSE_AXIS = Y_Axis;
   }
   else
   {
      MOUSE_AXIS = X_Axis;
   }

   // Switch 2 operates as the left mouse button
   if(SWITCH_2_STATE)
   {
      // button not pressed
      MOUSE_BUTTON = 0;
   }
   else
   {
      // button pressed
      MOUSE_BUTTON = 0x01;
   }

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
void System_Init (void)
{
   PCA0MD &= ~0x40;                    // Disable Watchdog timer
   Sysclk_Init ();                     // initialize system clock
   Port_Init ();                       // configure cross bar
   Timer_Init ();                      // configure timer
   ADC0_Init ();
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

void USB0_Init (void)
{

   POLL_WRITE_BYTE (POWER, 0x08);      // Force Asynchronous USB Reset
   POLL_WRITE_BYTE (IN1IE, 0x07);      // Enable Endpoint 0-1 in interrupts
   POLL_WRITE_BYTE (OUT1IE,0x07);      // Enable Endpoint 0-1 out interrupts
   POLL_WRITE_BYTE (CMIE, 0x07);       // Enable Reset, Resume, and Suspend
                                       // interrupts
   USB0XCN = 0xE0;                     // Enable transceiver; select full speed
   POLL_WRITE_BYTE (CLKREC,0x8F);      // Enable clock recovery, single-step
                                       // mode disabled

   EIE1 |= 0x02;                       // Enable USB0 Interrupts

                                       // Enable USB0 by clearing the USB
   POLL_WRITE_BYTE (POWER, 0x01);      // Inhibit Bit and enable suspend
                                       // detection

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
void Sysclk_Init (void)
{
#ifdef _USB_LOW_SPEED_

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKSEL  = SYS_EXT_OSC;              // Select System clock
   CLKSEL |= USB_INT_OSC_DIV_2;        // Select USB clock
#else
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKMUL  = 0x00;                     // Select internal oscillator as
                                       // input to clock multiplier

   CLKMUL |= 0x80;                     // Enable clock multiplier
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
// - Configure the Crossbar and GPIO ports.
//
// ----------------------------------------------------------------------------
void Port_Init(void)
{
   P2MDIN   = ~(0x20);                 // Port 2 pin 5 set as analog input
   P0MDOUT |= 0x0F;                    // Port 0 pins 0-3 set high impedence
   P1MDOUT |= 0x0F;                    // Port 1 pins 0-3 set high impedence
   P2MDOUT |= 0x0C;                    // Port 2 pins 0,1 set high empedence
   P2SKIP   = 0x20;                    // Port 2 pin 5 skipped by crossbar
   XBR0     = 0x00;
   XBR1     = 0x40;                    // Enable Crossbar
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
void Timer_Init (void)
{
   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;

   CKCON  &= ~0xF0;                    // Timer2 clocked based on T2XCLK;
   TMR2RL  = 0;                        // Initialize reload value
   TMR2    = 0xffff;                   // Set to reload immediately

   ET2     = 1;                        // Enable Timer2 interrupts
   TR2     = 1;                        // Start Timer2
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
void ADC0_Init (void)
{
   REF0CN = 0x0E;                      // Enable voltage reference VREF
   AMX0P = 0x04;                       // switch to potentiometer
   ADC0CF = 0xFC;                      // place ADC0 in left-adjusted mode
   AMX0N = 0x1F;                       // Single ended mode(negative input=gnd)

   ADC0CF = 0xFC;                      // SAR Period 0x1F, Left adjusted output

   ADC0CN = 0xC2;                      // Continuous converion on timer 2
                                       // overflow with low power tracking
                                       // mode on

   EIE1 |= 0x08;                       // Enable conversion complete interrupt
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
void Delay (void)
{
   int x;
   for (x = 0; x < 500; x)
      x++;
}