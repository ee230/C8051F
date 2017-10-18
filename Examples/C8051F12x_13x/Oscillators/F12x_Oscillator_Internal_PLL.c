//-----------------------------------------------------------------------------
// F12x_Oscillator_Internal_PLL.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// --------------------
//
// This example demonstrates how to setup the PLL using the internal Oscillator
// as the PLL input. The PLL can be used to provide a higher SYSCLK in the 
// range of 25 MHZ to 100 MHz. The user can change the PL divider and multiplier
// to any permissible value and the PLL configuration parameters are calculated
// using preprocessor macros
//
// Port Configuration:
// -------------------
// P0.0 Push-pull Output (SYSCLK/8)
//
// How To Test:
// -------------
//
// 1) Download code to a 'F12x device on a target board
// 
// 2) Connect a scope or frequency meter to P0.0
//
// 2) Run the code
// 
// 3) Measure SYSCLK/8 on P0.0
//
// Explanation of Compiler Errors
// -------------------------------
//
// The PLL configuration will generate errors if the PLL frequencies are set 
// out of range. Change the PLL divider or multiplier if this occurs.
//
// Additional Notes
//-----------------
//
// When operating at 100 MHz, the 'F12x requires adequate decoupling and a 
// power supply capable of providing 85mA. If the MCU resets when switching
// the CLKSEL register to 100 MHz, check the decoupling and power supply. 
// The voltage monitor will reset the MCU if the voltage drops too low.
//
// Setting the system clock faster than the Flash read time will put the 
// MCU in an inoperative state and the MCU must be reset by the IDE.
//
// FID:            12X000021
// Target:         C8051F12x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (KAB)
//    -18 MAY 2006
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F120.h>                 // SFR declarations

//-----------------------------------------------------------------------------
// User Modifiable PLL Constants
//-----------------------------------------------------------------------------

#define PLL_MUL      (4)               // Sets PLL multiplier N
#define PLL_DIV      (1)               // Sets PLL divider M

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define INTOSC       (24500000L)       // Internal oscillator frequency

//-----------------------------------------------------------------------------
// Macros used to calculate PLL and SYSCLK frequencies
//-----------------------------------------------------------------------------

#define PLL_DIV_CLK  (INTOSC/PLL_DIV)  // PLL divided clock input frequency
#define PLL_OUT_CLK  (PLL_DIV_CLK*PLL_MUL)
                                       // PLL output frequency
#define SYSCLK       PLL_OUT_CLK       // System clock derived from PLL

//-----------------------------------------------------------------------------
// Macros used to calculate Loop Filter Bits
//
// Reference
// C8051F121x-13x.pdf 
// SFR Definition 14.8. PLL0FLT: PLL Filter
//
// Note that PLL Loop filter bits give a suitable frequency range for each 
// setting. In some cases more than one setting may be acceptable for a 
// particular frequency. This macro will select the best fit setting for
// a particular frequency. 
//
//-----------------------------------------------------------------------------

#if (PLL_DIV_CLK)<(5000000L)
   #error "error: PLL divided clock frequency Too Low!"
#elif (PLL_DIV_CLK)<(8000000L)
   #define PLLFLT_LOOP 0x0F
#elif (PLL_DIV_CLK)<(12500000L)
   #define PLLFLT_LOOP 0x07
#elif (PLL_DIV_CLK)<(19500000L)
   #define PLLFLT_LOOP 0x03
#elif (PLL_DIV_CLK)<(30000001L)
   #define PLLFLT_LOOP 0x01
#else
   #error "error: PLL divided clock frequency Too High!"
#endif

//-----------------------------------------------------------------------------
// Macros used to calculate ICO  Bits
//
// Reference
// C8051F121x-13x.pdf 
// SFR Definition 14.8. PLL0FLT: PLL Filter
//
// Note that PLL ICO bits give a suitable frequency range for each setting. 
// In some cases more than one setting may be acceptable for a particular 
// frequency. This macro will select the best fit setting for a particular
// frequency. 
//
//-----------------------------------------------------------------------------

#if (PLL_OUT_CLK)<(25000000L)
   #error "error: PLL output frequency Too Low!"
#elif (PLL_OUT_CLK)<(42500000L)
   #define PLLFLT_ICO 0x30
#elif (PLL_OUT_CLK)<(52500000L)
   #define PLLFLT_ICO 0x20
#elif (PLL_OUT_CLK)<(72500000L)
   #define PLLFLT_ICO 0x10
#elif (PLL_OUT_CLK)<(100000001L)
   #define PLLFLT_ICO 0x00
#else
   #error "error: PLL output frequency Too High"
#endif

//-----------------------------------------------------------------------------
// Macros used to calculate  Flash Read Time  Bits
//
// Reference
// C8051F121x-13x.pdf 
// SFR Definition 15.2. FLSCL: Flash Memory Control
//-----------------------------------------------------------------------------

#if (SYSCLK)<(25000001L)
   #define FLSCL_FLRT 0x00
#elif (SYSCLK)<(50000001L)
   #define FLSCL_FLRT 0x10
#elif (SYSCLK)<(50000001L)
   #define FLSCL_FLRT 0x20
#elif (SYSCLK)<(100000001L)
   #define FLSCL_FLRT 0x30
#else
   #error "error: SYSCLK Too High"
#endif

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);           
void PORT_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   OSCILLATOR_Init ();                 // Initialize oscillator
   
   PORT_Init();                        // Initialize Ports and SYSCLK out
   
   while(1)                            // Loop forever
   {
   }
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
// This function initializes the PLL using the 24.5 MHZ internal oscillator.
// This function assumes the oscillator and PLL will be in the reset state
// and the MCU is running from the internal oscillator.
//
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void)
{
   unsigned char n;                    // n used for short delay counter

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x83;                      // Set internal oscillator to run
                                       // at its maximum frequency

   PLL0CN  = 0x00;                     // Set internal oscillator as PLL source

   SFRPAGE = LEGACY_PAGE;
   FLSCL   = FLSCL_FLRT;               // Set FLASH read time according to SYSCLK
   SFRPAGE = CONFIG_PAGE;

   PLL0CN |= 0x01;                     // Enable Power to PLL
   PLL0DIV = PLL_DIV;                  // Set PLL divider value using macro
   PLL0MUL = PLL_MUL;                  // Set PLL multiplier value using macro
   PLL0FLT = PLLFLT_ICO|PLLFLT_LOOP;   // Set the PLL filter loop and ICO bits
   for (n=0xFF;n!=0;n--);              // Wait at least 5us
   PLL0CN  |= 0x02;                    // Enable the PLL
   while(!(PLL0CN & 0x10));            // Wait until PLL frequency is locked
   CLKSEL  = 0x02;                     // Select PLL as SYSCLK source
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
// P0.0   digital   push-pull     SYSCLK/8
//-----------------------------------------------------------------------------

void PORT_Init (void)
{

   SFRPAGE   = CONFIG_PAGE;            // Set SFR page
    
   CLKSEL   |= 0x30;                   // Select SYSCLK/8 output

   XBR1     |= 0x80;                   // Enable SYSCLK output                
   XBR2     |= 0x40;                   // Enable crossbar
   P0MDOUT  |= 0x01;                   // Enable P0.0 push-pull output

}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------