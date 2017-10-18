//-----------------------------------------------------------------------------
// F70x_CS0.c
//-----------------------------------------------------------------------------
// Copyright (C) 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program uses the CS0 peripheral with autoscan to determine when
// one of the four touch sense buttons on the C8051F700 target board
// have been pressed.  When a button is pressed, the CS0 greater than
// comparator interrupt gets serviced.  Inside this interrupt, firmware
// checks the CS0MUX register to determine which of the four buttons 
// has been pressed.  Firmware then updates a PCA SFR that adjusts th
// duty cycle of the PWM that is outputting on the LED's pin.  This
// adjustment brightens or dims the LED.
//
// How To Test:
//
// 1) Verify that a jumper has been placed on J8, connecting 'P1.0' to 'DS3'.
// 2) Run the program. Press the switches to change the brightness of the
//    LED.
//
//
// Target:         C8051F70x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (PD)
//    -14 JUL 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F700_defs.h>            // SFR declarations


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

   WDTCN = 0xDE;                       // Disable watchdog timer
   WDTCN = 0xAD;

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
   U8 SFRPAGE_save = SFRPAGE;          // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;

   OSCICN |= 0x80;                     // Enable the precision internal osc.
   CLKSEL = 0x00;                      // Select internal internal osc.
                                       // divided by 1 as the system clock

   SFRPAGE = SFRPAGE_save;

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
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;
   PCA0CN    = 0x40;
   PCA0MD    = 0x00;
   PCA0CPM0  = 0x42;
   SFRPAGE = SFRPAGE_save;
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
   U8 SFRPAGE_save = SFRPAGE;          // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;

   P0SKIP = 0xFF;                      // Skip all P0, route PCA to P1.0
   P1MDOUT |= 0x01;                    // P1.0 is push-pull
   P2MDIN &= ~0x0F;                    // P2.0-P2.3 set to analog input for 
                                       // CS0
   XBR1    = 0x41;                     // Enable crossbar and enable
                                       // weak pull-ups, 1 PCA channel
   SFRPAGE = SFRPAGE_save;
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
   U8 SFRPAGE_save = SFRPAGE;          // Save the current SFRPAGE


   SFRPAGE = LEGACY_PAGE;

   CS0CF = 0x70;                       // Conversion enabled on CS0BUSY 
   									         // overflow
   CS0CN = 0x88;                       // Enable CS0, comparator 
   CS0CN |= 0x10;                      // Set CS0BUSY to begin conversions
   EIE2 |= 0x02;                       // Enable CS0 Greater than comp. interrupts
   CS0TH = SWITCH_THRESHOLD;           // Set comparator threshold value

   SFRPAGE = CONFIG_PAGE;
   CS0SS = 0x00;                       // Set channel 0 as autoscan 
                                       // starting channel
   CS0SE = 0x03;                       // Set channel 1 as autoscan end channel

   SFRPAGE = SFRPAGE_save;
}


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
INTERRUPT(CS0_ComparatorInterruptServiceRoutine, INTERRUPT_CS0_GRT)
{
   static S16 counter;
   CS0CN &= ~0x01;                     // Acknowledge interrupt

   switch (CS0MX)                      // Determine which button was pressed
   {
      case (0):
         PCA0CPH0 = 0xF0;              // Set PCA duty cycle to change
         break;                        // LED's brightness
      case (1):
         PCA0CPH0 = 0xB0;
         break;
      case (2):
         PCA0CPH0 = 0x80;
         break;
      case (3):
         PCA0CPH0 = 0x40;
         break;
   };
   CS0CN |= 0x10;                      // Set CS0Busy to start another scan
}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
