//-----------------------------------------------------------------------------
// T630_Comparator_Suspend.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program presents an example of use of the comparator to wake up the
// internal oscillator from suspend C8051T630. It uses the 'T630MB+DB as the 
// HW platform.
//
// If the P0.7 pin is grounded, it will cause a Comparator output low event, 
// and will wake up the MCU from suspend. On wake up, the MCU will toggle the 
// LED and go back into suspend.
//
// Pinout:
//
//    P1.3 -> LED
//
//    P0.7 -> Switch SW1        -------| connect P0.7 to P0.6 using 
//    P0.6 -> Comparator0 CP0+  -------| a jumper on J2[7-8]
//
//    P1.6 -> Potentiometer     -------| connect P1.6 to P1.7 using  
//    P1.7 -> Comparator0 CP0-  -------| a jumper on J3[7-8]
//
//    all other port pins unused
//
// How To Test:
//
// 1) Open the T630_Comparator_Suspend.c file in the Silicon Labs IDE.
// 2) Compile and download the code.
// 3) Connect the jumpers J9[1-2] and J9[3-4] on the 'T630 MB.
//    Also connect the jumper J12[1-2].
// 4) Connect the pins P0.7 and P0.6 using a jumper on J2[7-8].
// 5) Connect the pins P1.7 and P1.6 using a jumper on J3[7-8].
// 6) Run the code.
// 7) Press the switch labeled P0.7. This should toggle the LED state.
// 8) Release the switch to let the MCU go back into suspend.
//
//
// Target:         C8051T630
// Tool chain:     KEIL C51 7.20 / KEIL EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (PKC)
//    -19 NOV 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051T630_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

SBIT (LED, SFR_P1, 3);                 // LED==1 means ON
SBIT (SW1, SFR_P0, 7);                 // SW1==0 means pressed

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);           // Oscillator initialization routine
void Port_Init (void);                 // Port initialization routine
void Comparator0_Init (void);          // Comparator initialization routine

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   unsigned int i;

   PCA0MD &= ~0x40;                    // Clear watchdog timer enable

   Port_Init ();                       // Init Ports
   Comparator0_Init ();

   while (1)                           // Loop forever
   {
      OSCICN |= 0x20;                  // Suspend Internal H-F Oscillator.
                                       // Comparator Zero-Output event will wake 
                                       // it up.
                                       
      // 1) If the MCU did not stay in suspend till the Comparator Zero-Output 
      //    event, the LED will blink without having to press the switch.
      
      // 2) If the MCU stayed in suspend indefinitely, the LED will stay solid
      //    even if the button was pressed.
      
      // 3) If the MCU left suspend mode correctly on the Comparator Zero-Output 
      //    event, then the LED will toggle state every time the switch is 
      //    pressed and released.
      
      // Press switch P0.7 to toggle LED.
      LED = ~LED;                      // Toggle the LED

      for (i = 1; i != 0; i++);        // Wait for some period of time
      
      while ( (CPT0CN & 0x40) == 0);   // Wait till switch is released
                                       // and comparator output is 1
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P0.6  analog   (CP0+)
// P0.7  digital  open-drain Switch (SW1)
// P1.3  digital  push-pull  LED
// P1.6  analog   Potentiometer
// P1.7  analog   (CP0-) 
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDOUT &= ~0x80;                   // Set SW1 to open-drain
   P0MDIN &= ~0x40;                    // Set P0.6 to analog input

   P1MDIN &= ~0xC0;                    // Set P1.7, P1.6 to analog inputs
   P1MDOUT = 0x08;                     // Set LED to push-pull
  
   XBR1 = 0x40;                        // Enable crossbar
}

//-----------------------------------------------------------------------------
// Comparator0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the comparator.
//
// P0.6  digital  open-drain (CP0+)
// P1.6  analog   Potentiometer
// P1.7  analog   (CP0-)
//
//-----------------------------------------------------------------------------
void Comparator0_Init (void)
{
   unsigned char j;

   // Note: The external connections are needed because we want to use the
   // switch as the trigger, which has a normally-high behavior, while the 
   // comparator output needs to go low when the switch is pressed to awake
   // the device from suspend.
   
   // CP0- is the potentiometer on P1.6 connected externally to P0.7
   // IMPORTANT: Set the potentiometer to be somewhere in its mid range.
   // The potentiometer output should be less than 3.3V, but more than 0V.

   // CP0+ is the switch SW1 connected externally to P0.6
   // When switch is not pressed, the CP0OUT should be 1
   // When switch is pressed, the CP0OUT should be 0

   CPT0MX = 0x73;                      // CP0- = P1.7; CP0+ = P0.6
   CPT0CN |= 0x80;                     // Enable comparator
   for (j = 0; j < 100; j++);          // Wait for a short time
   CPT0CN &= ~0x30;                    // Clear CP0RIF and CP0FIF flags
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------