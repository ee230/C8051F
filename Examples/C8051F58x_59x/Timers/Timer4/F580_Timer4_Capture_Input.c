//-----------------------------------------------------------------------------
// F580_Timer4_Capture_Input.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program uses Timer 4 Capture mode to record the number of cycles
// between two button presses.
//
// A high-to-low transition on the TnEX pin will causes the 16 bit
// value in the associated timer (TnH, TnL) to be loaded into the
// capture registers (TMRnCAPH, TMRnCAPL). Whenever a capture occurs, the
// EXFn (TMRnCN.6) will become a logic high, and an interrupt will occur
// if interrupts are enabled. This bit is not cleared when vectoring to
// the ISR and must be cleared by software.
//
// How To Test:
// 1) Download code to a 'F580 device with a jumper on J19.
// 2) Run the program
// 3) Press the switch on P1.4 twice. If the LED (P1.3) lights up, you stop
//    the program and look at <BUTTON_PRESS_DELAY>. That variable will tell you
//    the number of cycles between button presses. If the LED does not light
//    up then it means that Timer 4 overflowed and will not give the correct
//    value. The slowest delay between button presses is:
//          (65535 / (24000000 / 128 / 12)) = 4.19s
//
// Target:         C8051F580 / C8051F580
// Tool chain:     Keil C51 8.00 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0 / 29 JUL 2008 (ADT)
//    -Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <c8051f580_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       24000000 / 128    // Internal oscillator frequency in Hz

U16 BUTTON_PRESS_DELAY = 0;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void Timer4_Init (void);

SBIT (SW, SFR_P1, 4);                  // 1 = not pressed, 0 = pressed
SBIT (LED, SFR_P1, 3);                 // 1 = on, 0 = off

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   SFRPAGE = ACTIVE_PAGE;              // Set SFR Page for PCA0MD

   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   PORT_Init ();                       // Initialize crossbar and GPIO
   OSCILLATOR_Init ();                 // Initialize oscillator
   Timer4_Init ();                     // Initialize Timer 4

   LED = 0;                            // Initially off

   EA = 1;

   while (1);
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
// This function initializes the system clock to use the internal oscillator
// at 187.500 kHz
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   char SFRPAGE_save = SFRPAGE;        // Save Current SFR page
   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x80;                      // Set internal oscillator to run
                                       // at max / 128.

   CLKSEL = 0x00;                      // Select the internal osc. as
                                       // the SYSCLK source

   SFRPAGE = SFRPAGE_save;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P1.3   digital   push-pull       LED
// P1.4   digital   open-drain      SW (T4EX)
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;          // Save Current SFR page
   SFRPAGE = CONFIG_PAGE;              // Set SFR Page

   P1MDOUT = 0x08;                     // P1.3 is push-pull
                                       // P1.4 is open-drain

   P0SKIP = 0xFF;                      // Skip to P1.4
   P1SKIP = 0x0F;
   XBR3    = 0x20;                     // T4EX routed to P1.4
   XBR2    = 0x40;                     // Enable crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;             // Restore SFR Page
}

//-----------------------------------------------------------------------------
// Timer4_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Initialize Timer 4 for capture mode. Whenever there is a high to low
// transition on T4EX, the current value of TMR4 will be captured in the
// TMR4CAP register.
//-----------------------------------------------------------------------------
void Timer4_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;          // Save Current SFR page
   SFRPAGE = ACTIVE2_PAGE;             // Set SFR Page

   TMR4CN = 0x09;                      // T4EX enabled
                                       // Timer4 is in Capture mode

   TMR4CF = 0x00;                      // Timer4 uses SYSCLK / 12
                                       // Increment only

   EIE2 = 0x40;                        // Enable Timer 4 interrupts

   TMR4CN |= 0x04;                     // Timer 4 run enabled

   SFRPAGE = SFRPAGE_save;             // Restore SFR Page
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TIMER4_ISR
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This is the ISR for Timer 4. Overflows and high-to-low transitions on T4EX
// will trigger interrupts.
//
// This interrupt will keep track of button presses. Whenever the button is
// pressed once, it will record the captured value. It will then transition
// to wait for the second press. overflow_count will keep track of
// how many interrupts occur between button presses. If > 1, then the values
// are invalid. If there are 2 valid presses, it will record the number of
// cycles inbetween them in the global variable, BUTTON_PRESS_DELAY.
//
//-----------------------------------------------------------------------------
INTERRUPT (TIMER4_ISR, INTERRUPT_TIMER4)
{
   static U8 button_state = 0;
   static UU16 timer4_a, timer4_b;
   static U8 t4_control;
   static U8 overflow_count;

   t4_control = TMR4CN;
   if (button_state == 1)              // Waiting for second press
   {
      if ((t4_control & 0x80) != 0)    // If overflow caused interrupt
      {
         overflow_count++;             // Number of overflows while waiting
                                       // for the second press
      }
   }

   if ((t4_control & 0x40) != 0)       // Change on T4EX caused the interrupt
   {
      if (button_state == 0)           // First Press
      {
         button_state = 1;             // Move to next state
         timer4_a.U8[MSB] = TMR4CAPH;  // Record value
         timer4_a.U8[LSB] = TMR4CAPL;
      }
      else if (button_state == 1)      // Second Press
      {
         timer4_b.U8[MSB] = TMR4CAPH;  // Record value
         timer4_b.U8[LSB] = TMR4CAPL;
         if (overflow_count == 0)      // No overflow
         {// Second value is valid
            button_state = 2;          // Done state
            BUTTON_PRESS_DELAY = timer4_b.U16 - timer4_a.U16;
            LED = 1;
         }
         else if ((overflow_count == 1) && (timer4_b.U16 < timer4_a.U16))
         {// One overflow but still valid
            button_state = 2;          // Done state
            BUTTON_PRESS_DELAY = timer4_b.U16 - timer4_a.U16;
            LED = 1;
         }
         else
         {
            button_state = 0;          // Reset State
            overflow_count = 0;
         }
      }
   }

   TMR4CN &= ~0xC0;                    // Clear Timer 4 overflow/underflow flag
                                       // Clear T4EX flag
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------