//-----------------------------------------------------------------------------
// T622_USB0_Mouse.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
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
// Target:         C8051T622/3, 'T326/7
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   F3xx_MouseExample
//
// Release 1.3
//    -All changes by ES
//    -4 OCT 2010
//    -Updated USB0_Init() to write 0x8F to CLKREC instead of 0x80
// Release 1.2 (ES)
//    -Added support for Raisonance
//    -No change to this file
//    -02 APR 2010
// Release 1.0
//    -Initial Revision (TP)
//    -11 SEP 2008
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

#define Sw1 0x02                       // These are the port0 and port1 bits
#define Sw2 0x01                       // for Sw1 and Sw2 on the development
                                       // board

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
INTERRUPT(Timer2_ISR, INTERRUPT_TIMER2)
{
   static unsigned char Mouse_Count = 0;
   static unsigned char Mouse_MovementMultiplier = 0;
   static signed char Mouse_Direction = 1;
   static unsigned char Mouse_StateMachine = 0;

   if (!(P0 & Sw1))                    // Check for switch #1 pressed
   {
      // if switch 1 is pressed, increase the magnitude of the mouse
      // movement vector
      if(Mouse_MovementMultiplier <= 255)
      {
         Mouse_MovementMultiplier += 1;
      }

      if (TOGGLE1 == 0)                // Toggle is used to debounce switch
      {                                // so that one press and release will
         SWITCH_1_STATE = ~SWITCH_1_STATE; // toggle the state of the switch sent
         TOGGLE1 = 1;                  // to the host
      }
   }
   else
   {
      TOGGLE1 = 0;                     // Reset toggle variable

      // if switch 1 is not pressed, reduce the magnitude of the mouse
      // movement vector
      if(Mouse_MovementMultiplier > 0)
      {
         Mouse_MovementMultiplier -= 1;
      }
   }


   SWITCH_2_STATE = (P1 & Sw2);

   // Mouse_Count sets the number of timer interrupts serviced before
   // the mouse changes direction
   if(Mouse_Count++ == 50)
   {
      // the mouse state machine cycles through states 0-3
      if(++Mouse_StateMachine == 4) Mouse_StateMachine = 0;

      // each state sets the mouse to turn 90 degrees clockwise
      switch (Mouse_StateMachine)
      {
      case(0):
         // positive X direction
         Mouse_Direction = 1;
         MOUSE_AXIS = X_Axis;
      break;
      case(1):
         // negative Y direction
         Mouse_Direction = -1;
         MOUSE_AXIS = Y_Axis;
      break;
      case(2):
         // negative X direction
         Mouse_Direction = -1;
         MOUSE_AXIS = X_Axis;
      break;
      case(3):
         // positive Y direction
         Mouse_Direction = 1;
         MOUSE_AXIS = Y_Axis;
      break;
      }

      Mouse_Count = 0;
   }

  // Sets Mouse Vector
   MOUSE_VECTOR = (1 * ((Mouse_MovementMultiplier >> 5)+1)) * Mouse_Direction;

   TF2H = 0;                             // Clear Timer2 interrupt flag
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
   Sysclk_Init ();                     // Initialize system clock
   Port_Init ();                       // Configure cross bar
   Timer_Init ();                      // Configure timer
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

   // This Clock Multiplier code is no longer necessary, but it's kept here
   // for backwards compatibility with the 'F34x.

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
// Pinout:
//
// P0.1   digital open-drain   SW1
//
// P0.6   digital push-pull    LED1
//
// P1.0   digital open-drain   SW2
//
// P1.2   digital push-pull    LED2
//
// ----------------------------------------------------------------------------
void Port_Init(void)
{
   P0MDOUT |= 0x40;                    // P0.6 (LED1) set to push-pull
   P1MDOUT |= 0x04;                    // P1.2 (LED2) set to push-pull

   XBR0 = 0x00;
   XBR1 = 0x40;                        // Enable Crossbar
}

//-----------------------------------------------------------------------------
// Timer_Init
//-----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// Timer 2 reload used to check if switch pressed on overflow.
//
// ----------------------------------------------------------------------------
void Timer_Init (void)
{
   TMR2CN = 0x00;                      // Stop Timer2; Clear TF2;

   CKCON &= ~0xF0;                     // Timer2 clocked based on T2XCLK;
   TMR2RL = 0;                         // Initialize reload value
   TMR2 = 0xffff;                      // Set to reload immediately

   ET2 = 1;                            // Enable Timer2 interrupts
   TR2 = 1;                            // Start Timer2
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

// ----------------------------------------------------------------------------
// End of File
// ----------------------------------------------------------------------------
