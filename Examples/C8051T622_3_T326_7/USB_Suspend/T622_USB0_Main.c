//-----------------------------------------------------------------------------
// T622_USB_Main.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This application will communicate with a PC across the USB interface.
// The device will appear to be a mouse, and will manipulate the cursor
// on screen.  Pressing SW2 (P1.0) will toggle whether or not the USB device is
// controlling the mouse cursor.  The device can be put in Suspend mode by
// putting the PC into Standby, and the Suspend mode current can be measured
// using J7 on the 'T62x mother board.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051T622/3 or 'T326/7
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   T622_USB0_Suspend
//
// Release 1.1
//    -All changes by ES
//    -4 OCT 2010
//    -Updated USB0_Init() to write 0x8F to CLKREC instead of 0x80
// Release 1.0
//    -Initial Revision (PD / TP)
//    -15 SEP 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T622_defs.h"
#include "T622_USB0_InterruptServiceRoutine.h"
#include "T622_USB0_Mouse.h"
#include "T622_USB0_Register.h"

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

SBIT (SW1, SFR_P0, 1);                 // Switches are '1' when not pressed
SBIT (SW2, SFR_P1, 0);
SBIT (LED1, SFR_P0, 6);
SBIT (LED2, SFR_P1, 2);

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
void Usb_Suspend (void);

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{
   System_Init ();
   USB0_Init ();

   EA = 1;
   while (1)
   {
      if (!SWITCH_2_STATE)             // Toggle whether data is sent to the
      {                                // host
         SendPacket (0);
      }
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

   // Switch one (SW1, P0.1) controls how quickly the mouse cursor moves.
   // Holding down switch one will increase the speed at which the mouse cusor
   // travels.
   if (!SW1)                           // Check for switch #1 pressed
   {
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

   // Switch two (SW2, P1.0) controls whether or not the device controls the
   // mouse cursor.  This is to make the mouse easier to control on the PC and
   // stop the device from interfering with the mouse cursor when it's not
   // desired.
   if (!SW2)                           // Check for switch #2 pressed
   {
      if (TOGGLE2 == 0)                // Toggle is used to debounce switch
      {                                // so that one press and release will
         SWITCH_2_STATE = ~SWITCH_2_STATE; // toggle the state of the switch sent
         TOGGLE2 = 1;                  // to the host
      }
   }
   else
   {
      TOGGLE2 = 0;
   }

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
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Usb_Suspend
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Enter suspend mode after suspend signalling is present on the bus.  This
// function is called from the USB ISR.
//
//-----------------------------------------------------------------------------
void Usb_Suspend (void)
{
   USB0XCN &= ~0x43;                   // Turn off USB tranciever
   VDM0CN = 0x00;                      // Turn off VDD Monitor
   REG01CN = 0x12;                     // Switch from the high-precision
                                       // regulator to the 5V regulator
   CLKSEL = 0x10;                      // Select the internal oscillator as the
                                       // USB and system clocks
   OSCLCN = 0x00;                      // Turn off the LFO
   CLKMUL = 0x00;                      // Turn off the Clock Multiplier -
                                       // this is no longer necessary but is
                                       // here for 'F34x compatibility

//    XBR1 |= 0x80;                      // Optional: disable weak pull-ups

   LED1 = 0;                           // Turn off the LEDs to save power
   LED2 = 0;

   PFE0CN = 0x00;                      // Disable the prefetch engine
   REG01CN = 0x13;                     // Enable the low power memory
                                       // controller

   // At least 12 NOP instructions are required after enabling the memory
   // controller before entering Suspend
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();
   _nop_ ();


   OSCICN |= 0x20;                     // Enter Suspend mode

   // Wait here until the device receives resume signalling from the USB bus

   PFE0CN = 0x20;                      // Re-enable the prefetch engine

   Sysclk_Init();                      // Re-enable the SYSCLK and USB clocks

   XBR1 &= ~0x80;                      // Re-enable weak pull-ups

   REG01CN = 0x00;                     // Switch back to normal operating mode
   REG01CN = 0x00;                     // Switch back to normal operating mode
   USB0XCN |= 0x40;                    // Re-enable the USB transceiver
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

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
