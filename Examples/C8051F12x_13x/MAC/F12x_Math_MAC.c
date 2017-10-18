//-----------------------------------------------------------------------------
// F12x_Math_MAC.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program uses the built-in Keil MAC routines to perform basic math,
// using the UART to interface with the user.
//
// How To Test:
//
// 1) Download code to a 'F12x device that is connected to a UART transceiver
// 2) Connect serial cable from the transceiver to a PC
// 3) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 4) Run the code:
//      - the firmware will print a menu and prompt for a desired Math function
//      - the ASM code can be viewed by using the "#pragma SRC" directive
//
// Target:         C8051F12x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -19 JUN 2006
//
//-----------------------------------------------------------------------------

// Uncomment this to generate the assembly version to view the MAC code
// #pragma SRC (F12x_Math_MAC.asm)

// Use the built-in Keil C8051F12x MAC routines
// The evaluation version of the Keil compiler does not support these built-in
// routines.
#pragma mdu_f120

// signed int mul = ?C?IMUL_F120 routine
// unsigned int mul = ?C?IMUL_F120 routine
// signed long mul = ?C?LMUL_F120 routine
// unsigned long mul = ?C?LMUL_F120 routine
// signed long shift left = ?C?LSHL_F120 routine
// unsigned long shift left = ?C?LSHL_F120 routine
// signed long shift right = ?C?ULSHR_F120 routine
// unsigned long shift right = ?C?SLSHR_F120 routine

// Note: The arithmetic unit does not allow reentrant code and cannot be used
// in multiple threads or the main and interrupt routines at the same time.
// You may therefore need to disable the MDU unit with NOMOD_F120.

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F120.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F12x
//-----------------------------------------------------------------------------

sfr16 DP       = 0x82;                 // data pointer
sfr16 ADC0     = 0xbe;                 // ADC0 data
sfr16 ADC0GT   = 0xc4;                 // ADC0 greater than window
sfr16 ADC0LT   = 0xc6;                 // ADC0 less than window
sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 RCAP3    = 0xca;                 // Timer3 capture/reload
sfr16 RCAP4    = 0xca;                 // Timer4 capture/reload
sfr16 TMR2     = 0xcc;                 // Timer2
sfr16 TMR3     = 0xcc;                 // Timer3
sfr16 TMR4     = 0xcc;                 // Timer4
sfr16 DAC0     = 0xd2;                 // DAC0 data
sfr16 DAC1     = 0xd2;                 // DAC1 data
sfr16 PCA0CP5  = 0xe1;                 // PCA0 Module 5 capture
sfr16 PCA0CP2  = 0xe9;                 // PCA0 Module 2 capture
sfr16 PCA0CP3  = 0xeb;                 // PCA0 Module 3 capture
sfr16 PCA0CP4  = 0xed;                 // PCA0 Module 4 capture
sfr16 PCA0     = 0xf9;                 // PCA0 counter
sfr16 PCA0CP0  = 0xfb;                 // PCA0 Module 0 capture
sfr16 PCA0CP1  = 0xfd;                 // PCA0 Module 1 capture

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define BAUDRATE     57600             // Baud rate of UART in bps
#define IntOsc       24500000          // Internal oscillator frequency in Hz
#define SYSCLK       49000000          // Output of PLL derived from (IntOsc*2)

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void PORT_Init (void);
void UART1_Init (void);
void Timer3_Init (int counts);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   char key_press;

   // 16-bit Unsigned Multiply
   unsigned int u_multiplier, u_multiplicand, u_product;

   // 16-bit Signed Multiply
   int multiplier, multiplicand, product;

   // 32-bit Unsigned Multiply
   unsigned long ul_multiplier, ul_multiplicand, ul_product;

   // 32-bit Signed Multiply
   long l_multiplier, l_multiplicand, l_product;

   // Random Generator Operations
   unsigned long ulrand1, ulrand2;
   long lrand1, lrand2;
   unsigned char shifts;

   WDTCN = 0xde;                       // Disable watchdog timer
   WDTCN = 0xad;

   SYSCLK_Init ();                     // Initialize oscillator to 49 MHz
                                       // using the PLL

   UART1_Init ();                      // Initialize UART1

   PORT_Init ();                       // Initialize crossbar and GPIO


   while (1)                           // Serial port command decoder
   {

      SFRPAGE = UART1_PAGE;

      // Output the menu
      printf ("\n\nMAC Menu:\n\n");
      printf ("1. 16-bit Unsigned Multiply\n");
      printf ("2. 32-bit Unsigned Multiply\n");
      printf ("3. 16-bit Signed Multiply\n");
      printf ("4. 32-bit Signed Multiply\n");
      printf ("5. 32-bit Unsigned Multiply Random Generator\n");
      printf ("6. 32-bit Signed Multiply Random Generator\n");
      printf ("7. 32-bit Left Shift Random Generator\n");
      printf ("8. 32-bit Unsigned Right Shift Random Generator\n");
      printf ("9. 32-bit Signed Right Shift Random Generator\n\n");

      printf ("Enter a number:");

      // Wait for a user selection
      scanf  ("%c", &key_press);
      getchar();

      // Decode the user selection
      switch (key_press)
      {
         // 16-bit Unsigned Multiply
         case '1':
           {
              printf ("\nEnter Multiplicand:");
              scanf  ("%u",&u_multiplicand);
              getchar();

              printf ("\nEnter Multiplier:");
              scanf  ("%u",&u_multiplier);
              getchar();

              u_product = u_multiplicand * u_multiplier;
              printf ("\n16-bit Unsigned Multiply Product = ");
              printf ("%u\n\n", u_product);
           }
           break;

         // 32-bit Unsigned Multiply
         case '2':
           {
              printf ("\nEnter Multiplicand:");
              scanf  ("%lu",&ul_multiplicand);
              getchar();

              printf ("\nEnter Multiplier:");
              scanf  ("%lu",&ul_multiplier);
              getchar();

              ul_product = ul_multiplicand * ul_multiplier;
              printf ("\n32-bit Unsigned Multiply = ");
              printf ("%lu\n\n", ul_product);
           }
           break;

         // 16-bit Signed Multiply
         case '3':
           {
              printf ("\nEnter Multiplicand:");
              scanf  ("%u",&multiplicand);
              getchar();

              printf ("\nEnter Multiplier:");
              scanf  ("%u",&multiplier);
              getchar();

              product = multiplicand * multiplier;
              printf ("\n16-bit Signed Multiply: = ");
              printf ("%d\n", product);
           }
           break;

         // 32-bit Signed Multiply
         case '4':
           {
              printf ("\nEnter Multiplicand:");
              scanf  ("%ld",&l_multiplicand);
              getchar();

              printf ("\nEnter Multiplier:");
              scanf  ("%ld",&l_multiplier);
              getchar();

              l_product = l_multiplicand * l_multiplier;
              printf ("\n32-bit Signed Multiply = ");
              printf ("%ld\n\n", l_product);
           }
           break;

         // 32-bit Unsigned Multiply Random Generator
         case '5':
            {
               ulrand1 = 5543408L;     // Start with some number

               // Generate a "pseudo" random number
               ulrand1 = 1664525L * ulrand1 + 1013904223L;
               ulrand2 = 1664525L * ulrand2 + 1013904223L;
               ul_product = ulrand1 * ulrand2;

               printf ("\n32-bit Unsigned Random Number = ");
               printf ("%lu\n\n", ul_product);
            }
            break;

         // 32-bit Signed Multiply Random Generator
         case '6':
            {
               lrand1 = 5543408L;      // Start with some number

               // Generate a "pseudo" random number
               lrand1 = 1664525L * lrand1 + 1013904223L;
               lrand2 = 1664525L * lrand2 + 1013904223L;
               l_product = lrand1 * lrand2;

               printf ("\n32-bit Signed Random Number = ");
               printf ("%l\n\n", ul_product);
            }
            break;

         // 32-bit Left Shift Random Generator
         case '7':
            {
               ulrand1 = 5543408L;     // Start with some number

               // Generate a "pseudo" random number
               for(shifts = 0; shifts < 32; shifts++)
               {
                  ulrand1 = 1664525L * ulrand1 + 1013904223L;
                  ul_product = ulrand1 << shifts;

                  printf ("\n32-bit Left Shift Random Number: ");
                  printf ("%lx shifted left by %bu",ulrand1, shifts);
               }
            }
            break;

         // 32-bit Unsigned Right Shift Random Generator
         case '8':
            {
               ulrand1 = 5543408L;     // Start with some number

               // Generate a "pseudo" random number
               for(shifts = 0; shifts < 32; shifts++)
               {
                  ulrand1 = 1664525L * ulrand1 + 1013904223L;
                  ul_product = ulrand1 >> shifts;

                  printf ("\n32-bit Unsigned Right Shift Random Number: ");
                  printf ("%lx shifted right by %bu",ulrand1, shifts);
               }
            }
            break;

         // 32-bit Signed Right Shift Random Generator
         case '9':
            {
               lrand1 = 5543408L;      // Start with some number

               // Generate a "pseudo" random number
               for(shifts = 0; shifts < 32; shifts++)
               {
                  lrand1 = 1664525L * lrand1 + 1013904223L;
                  l_product = lrand1 >> shifts;

                  printf ("\n32-bit Signed Right Shift Random Number: ");
                  printf ("%lx shifted right by %bu",lrand1, shifts);
               }
            }
            break;

         default:                      // Indicate unknown command;
            printf ("\nUnknown command: '%c'\n", key_press);
           break;
      } // Menu Decode switch()
   } // loop forever

   while(1);
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal oscillator
// at 24.5 MHz multiplied by two using the PLL.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   int i;                              // software timer

   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // set SFR page

   OSCICN = 0x83;                      // set internal oscillator to run
                                       // at its maximum frequency

   CLKSEL = 0x00;                      // Select the internal osc. as
                                       // the SYSCLK source

   //Turn on the PLL and increase the system clock by a factor of M/N = 2
   SFRPAGE = CONFIG_PAGE;

   PLL0CN  = 0x00;                     // Set internal osc. as PLL source
   SFRPAGE = LEGACY_PAGE;
   FLSCL   = 0x10;                     // Set FLASH read time for 50MHz clk
                                       // or less
   SFRPAGE = CONFIG_PAGE;
   PLL0CN |= 0x01;                     // Enable Power to PLL
   PLL0DIV = 0x01;                     // Set Pre-divide value to N (N = 1)
   PLL0FLT = 0x01;                     // Set the PLL filter register for
                                       // a reference clock from 19 - 30 MHz
                                       // and an output clock from 45 - 80 MHz
   PLL0MUL = 0x02;                     // Multiply SYSCLK by M (M = 2)

   for (i=0; i < 256; i++) ;           // Wait at least 5us
   PLL0CN  |= 0x02;                    // Enable the PLL
   while(!(PLL0CN & 0x10));            // Wait until PLL frequency is locked
   CLKSEL  = 0x02;                     // Select PLL as SYSCLK source

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// UART1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART1 using Timer1, for <BAUDRATE> and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART1_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = UART1_PAGE;
   SCON1   = 0x10;                     // SCON1: mode 0, 8-bit UART, enable RX

   SFRPAGE = TIMER01_PAGE;
   TMOD   &= ~0xF0;
   TMOD   |=  0x20;                    // TMOD: timer 1, mode 2, 8-bit reload


   #if (SYSCLK/BAUDRATE/2/256 < 1)
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON |= 0x10;                   // T1M = 1; SCA1:0 = xx
   #elif (SYSCLK/BAUDRATE/2/256 < 4)
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x13;                  // Clear all T1 related bits
      CKCON |=  0x01;                  // T1M = 0; SCA1:0 = 01
   #elif (SYSCLK/BAUDRATE/2/256 < 12)
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x13;                  // T1M = 0; SCA1:0 = 00
   #elif (SYSCLK/BAUDRATE/2/256 < 48)
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x13;                  // Clear all T1 related bits
      CKCON |=  0x02;                  // T1M = 0; SCA1:0 = 10
   #else
      #error "Unable to generate specified baudrate!"
   #endif

   TL1 = TH1;                          // initialize Timer1
   TR1 = 1;                            // start Timer1

   SFRPAGE = UART1_PAGE;
   TI1 = 1;                            // Indicate TX1 ready

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page

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
// P0.0   digital   push-pull     UART1 TX
// P0.1   digital   open-drain    UART1 RX
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // set SFR page

   XBR0     = 0x00;
   XBR1     = 0x00;
   XBR2     = 0x44;                    // Enable crossbar and weak pull-up
                                       // Enable UART1

   P0MDOUT |= 0x01;                    // Set TX1 pin to push-pull

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

