//-----------------------------------------------------------------------------
// F80x_CS0.c
//-----------------------------------------------------------------------------
// Copyright (C) 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program uses the CS0 peripheral with autoscan to determine when
// one of the two touch sense buttons on the C8051F800 target board
// have been pressed.  When a button is pressed, the CS0 greater than
// comparator interrupt gets serviced.  Inside this interrupt, firmware
// checks the CS0MUX register to determine which of the four buttons 
// has been pressed.  Firmware then updates a PCA SFR that adjusts the
// duty cycle of the PWM that is outputting on the LED's pin.  This
// adjustment brightens or dims the LED.
//
// How To Test:
//
// 1) Verify that a jumper has been placed on J3, connecting 'P1.0' to 'P1.0_LED'.
// 2) Run the program. Press the switches to change the brightness of the
//    LED.
//
//
// Target:         C8051F80x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0 - 03 SEP 2009 (JH)
//    -Initial Revision.
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F800_defs.h>            // SFR declarations


//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (LED,    SFR_P1, 0);              // '0' means ON, '1' means OFF

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK             24500000L


#define LED_ON           0
#define LED_OFF          1

#define SWITCH_THRESHOLD 0x7000


//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void);
void PORT_Init (void);
void CS0_Init (void);
void PCA0_Init (void);
INTERRUPT_PROTO(CS0_ComparatorInterruptServiceRoutine, INTERRUPT_CS0_GRT);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (disable watchdog timer)

   PORT_Init ();                       // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator
   CS0_Init();                         // Initialize capacitive touch sense
                                       // to be used as a wake-up source
   PCA0_Init();                        // Initialize PCA to control LED
   
   EA = 1;                             // Enable global interrupts

   
   while (1);                          // Spin forever
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
// This function initializes the system clock to use the internal high power
// oscillator.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN |= 0x80;                     // Enable the precision internal osc.
   CLKSEL = 0x00;                      // Select internal internal osc.
                                       // divided by 1 as the system clock
}

//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes one PCA channel to 8-bit PWM mode.
//
//-----------------------------------------------------------------------------
void PCA0_Init(void)
{
   PCA0CN    = 0x40;
   PCA0MD    = 0x00;
   PCA0CPM0  = 0x42;
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
// P1.0   digital   push-pull     LED
// P1.1   digital   open-drain    Switch
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0SKIP = 0xFF;                      // Skip all P0, route PCA to P1.0
   P1MDOUT |= 0x01;                    // P1.0 is push-pull
   P1MDIN &= ~0x60;                    // P1.5-P1.6 set to analog input for 
                                       // CS0
   XBR1    = 0x41;                     // Enable crossbar and enable
                                       // weak pull-ups, 1 PCA channel
}

//-----------------------------------------------------------------------------
// CS0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function enables capacitive touch sense peripheral CS0 and the
// CS0 comparator.  The function enables the auto-scan feature and configures
// the start of conversion source to be CS0BUSY.
//-----------------------------------------------------------------------------
void CS0_Init (void)
{
   CS0CF = 0x70;                       // Conversion enabled on CS0BUSY 
   									         // overflow
   CS0CN = 0x88;                       // Enable CS0, comparator 
   CS0CN |= 0x10;                      // Set CS0BUSY to begin conversions
   EIE2 |= 0x02;                       // Enable CS0 Greater than comp. interrupts
   CS0TH = SWITCH_THRESHOLD;           // Set comparator threshold value

   CS0SS = 0x0D;                       // Set channel 0 as autoscan 
                                       // starting channel
   CS0SE = 0x0E;                       // Set channel 1 as autoscan end channel
}


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
INTERRUPT(CS0_ComparatorInterruptServiceRoutine, INTERRUPT_CS0_GRT)
{
   CS0CN &= ~0x01;                     // Acknowledge interrupt

   switch (CS0MX)                      // Determine which button was pressed
   {
      case (0x0D):
         PCA0CPH0 = 0xF0;              // Set PCA duty cycle to change
         break;                        // LED's brightness
      case (0x0E):
         PCA0CPH0 = 0x40;
         break;
   }
   CS0CN |= 0x10;                      // Set CS0Busy to start another scan
}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
