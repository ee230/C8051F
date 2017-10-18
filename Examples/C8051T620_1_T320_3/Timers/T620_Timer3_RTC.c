//-----------------------------------------------------------------------------
// T620_Timer3_RTC.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program presents an example of use of the Timer3 in "RTC" mode, where
// the timer is clocked by an external clock (32.768 kHz crystal).  The MCU
// is placed in SUSPEND mode and will wake whenever Timer3 overflows.  The
// program counts 0x1000 clocks (equal to 1 second at 32.768 kHz / 8) and
// increments variables to represent hours, minutes, and seconds.  The P2.2 LED
// toggles every second.
//
// This example also uses a UART interface to set and display time. UART
// reception will also wake the the device so a new time can be set.
//
//
// Pinout:
//
//    P0.2 -> XTAL1
//    P0.3 -> XTAL2
//
//    P0.4 -> UART0 TX
//    P0.5 -> UART0 RX / Port Match
//
//    P2.2 -> LED
//
//    all other port pins unused
//
// How To Test:
//
// Option1:
// 1) Open the T620_Timer3_RTC.c file in the Silicon Labs IDE.
// 2) Populate the Y1 crystal with a 32.768 kHz crystal, R2 with a 10 Mohm
//    resistor, and C5 and C6 appropriately (to match the crystal) on the 'T620
//    daughter card.
// 3) Remove the R4 and R3 resistors on the 'T620 daughter card.
// 4) Compile the project.
// 5) Download code to a 'T620 device on a 'T62x mother board.
// 6) Verify the LED pins of J10 are populated on the 'T62x MB.
// 7) Run the code.
// 8) Check that the LED is blinking every second.
//
// Option2:
// 1) Open the T620_Timer3_RTC.c file in the Silicon Labs IDE.
// 2) Populate the Y1 crystal with a 32.768 kHz crystal, R2 with a 10 Mohm
//    resistor, and C5 and C6 appropriately (to match the crystal) on the 'T620
//    daughter card.
// 3) Remove the R4 and R3 resistors on the 'T620 daughter card.
// 4) Ensure that jumpers are placed on J11 of the C8051T62x mother board
//    that connect the P0.4 pin to the RX_COMM signal and P0.5 to the TX_COMM
//    signal.
// 5) Specify the target baudrate in the constant <BAUDRATE>.
// 6) Connect a USB cable from the PC to the COMM connector (P4) on the mother
//    board (CP210x drivers must already be installed).
// 7) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE0>/<BAUDRATE1> and 8-N-1.
// 8) Compile the project.
// 9) Download code to a 'T620 device on a 'T62x mother board.
// 10) Set and display the time using HyperTerminal
//
//
// Target:         C8051T620/1 or 'T320/1/2/3
// Tool chain:     Keil / Raisonance
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (ES)
//    -13 APR 2010
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051T620_defs.h"            // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK               12000000  // SYSCLK in Hz (12 MHz internal
                                       // oscillator / 8)
                                       // the internal oscillator has a
                                       // tolerance of +/- 2%

#define BAUDRATE               115200  // Baud rate of UART in bps

#define CRYSTAL_FREQ            32768  // Crystal frequency in Hz
#define TIMER_1S       CRYSTAL_FREQ/8  // The Timer clock is EXTCLK / 8

// Note: The crystal frequency can't be greater than 524288 Hz (65536*8) since
// Timer3 is a 16-bit timer.

// Note: The greatest accuracy occurs with a crystal of a frequency like 32768
// Hz, which is a Timer count of 0x1000.  Other crystal values may be used
// instead, but the timer will be slightly inaccurate.

#if  (CRYSTAL_FREQ <= 32000)
   #define XFCN 0
#elif(CRYSTAL_FREQ <= 84000)
   #define XFCN 1
#elif(CRYSTAL_FREQ <= 225000)
   #define XFCN 2
#elif(CRYSTAL_FREQ <= 590000)
   #define XFCN 3
#else
   #error "The crystal frequency must be less than 524288 Hz (65536*8)."
#endif

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

SBIT (LED, SFR_P2, 2);                 // LED='1' means ON

unsigned char hours = 0;
unsigned char minutes = 0;
unsigned char seconds = 0;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Port_Init (void);                 // Port initialization routine
void Oscillator_Init (void);           // Initialize the external oscillator
void Timer3_Init (void);               // Timer3 initialization routine
void UART0_Init (void);                // UART0 initialization routine

void RTC_Set_Time (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   char input_character;

   PCA0MD &= ~0x40;                    // Clear watchdog timer enable

   Port_Init ();                       // Init Ports
   Oscillator_Init ();                 // Initialize external crystal
   Timer3_Init ();                     // Initialize the Timer3
   UART0_Init ();

   EA = 1;                             // Enable global interrupts

   TMR3CN |= 0x04;                     // Start Timer3

   printf ("\fPress 's' to set the RTC time.\n");

   while (1)
   {
      printf("\r%02d:%02d:%02d", (short)hours, (short)minutes, (short)seconds);

      while (TI0 != 1);                // Wait for the UART transmission to
                                       // finish for the last byte before
                                       // going to sleep

      // Check if the user entered 's' to set the time
      if (RI0 == 1)
      {
         input_character = getkey();
         if (input_character == 's')
         {
            RTC_Set_Time();
         }
         RI0 = 0;
      }

      OSCICN |= 0x20;                  // Sit in SUSPEND with the "RTC" running
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
// Pinout:
//
//    P0.2 -> XTAL1
//    P0.3 -> XTAL2
//
//    P0.4 -> UART0 TX
//    P0.5 -> UART0 RX / Port Match
//
//    P2.2 -> LED
//
//    all other port pins unused
//
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   P0MDIN &= ~0x0C;                    // XTAL1 and XTAL2 analog pins
   P0SKIP |= 0x0C;                     // Skip the XTAL1/2 pins

   P0MDOUT |= 0x10;                    // Set P0.4 (UART0 TX) to push-pull

   P2MDOUT |= 0x04;                    // Set P2.2 (LED) to push-pull
   P2SKIP |= 0x04;                     // Skip the LED pin

   XBR0 = 0x01;                        // Enable UART0
   XBR1 = 0x40;                        // Enable crossbar

   P0MAT = 0x20;                       // Wake the device when P0.5 is low
   P0MASK = 0x20;                      // Look at P0.5 (RX) for Port Match
}

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to 12 MHz and sets up the
// external crystal for Timer3.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   int i;                              // Delay counter

   OSCICN |= 0x03;                     // Set the Internal HFO to 12 MHz

   OSCXCN = (0x60 | XFCN);             // Start external oscillator with
                                       // the appropriate XFCN setting
                                       // based on crystal frequency


   for (i=0; i < 256; i++);            // Wait for crystal osc. to start

   #if(CRYSTAL_FREQ <= 100000)         // If tuning-fork crystal, add extra
                                       // delay before checking XTLVLD
   for (i=0; i < 256; i++);            // Add 1ms delay
   for (i=0; i < 256; i++);            // Add 1ms delay
   #endif

   while (!(OSCXCN & 0x80));           // Wait for crystal osc. to settle

   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // VDD Monitor reset
}

//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures Timer3 in RTC mode with a 16-bit reload timer,
// interrupt enabled, using the EXTCLK at crystal/8.  Timer3 will continue to
// clock even when the MCU is in SUSPEND.
//
//-----------------------------------------------------------------------------
void Timer3_Init(void)
{
   CKCON &= ~0xC0;                     // Timer3 uses EXTCLK / 8 as clock
   TMR3CN |= 0x01;                     // source

   TMR3RL = -TIMER_1S;                 // Reload value to be used in Timer3
   TMR3 = TMR3RL;                      // Init the Timer3 register

   EIE1 |= 0x80;                       // Timer3 interrupt enabled
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1 for <BAUDRATE> and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   if (SYSCLK/BAUDRATE/2/256 < 1)
   {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   }
   else if (SYSCLK/BAUDRATE/2/256 < 4)
   {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   }
   else if (SYSCLK/BAUDRATE/2/256 < 12)
   {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   }
   else
   {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // Init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
   TI0 = 1;                            // Indicate TX0 ready
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// RTC_Set_Time
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Set the RTC time using the global RTC variables.
//
//-----------------------------------------------------------------------------
void RTC_Set_Time (void)
{
   unsigned char temp;
   unsigned char temp_seconds;         // Temporary variables to check for a
   unsigned char temp_minutes;         // valid time entry
   unsigned char temp_hours;

   TMR3CN &= ~0x04;                    // Disable Timer3 while the time is
                                       // being updated

   printf("\nEnter the RTC time with the following format: HH:MM:SS\n");

   temp = getchar();                   // <hours> most-significant digit
   temp_hours = (temp - 0x30)*10;      // Convert from ASCII BCD to a value
   temp = getchar();                   // <hours> least-significant digit
   temp_hours += (temp - 0x30);        // Convert from ASCII and add

   temp = getchar();                   // Remove the semi-colon or space

   temp = getchar();                   // <minutes> most-significant digit
   temp_minutes = (temp - 0x30)*10;    // Convert from ASCII BCD to a value
   temp = getchar();                   // <minutes> least-significant digit
   temp_minutes += (temp - 0x30);      // Convert from ASCII and add

   temp = getchar();                   // Remove the semi-colon or space

   temp = getchar();                   // <seconds> most-significant digit
   temp_seconds = (temp - 0x30)*10;    // Convert from ASCII BCD to a value
   temp = getchar();                   // <seconds> least-significant digit
   temp_seconds += (temp - 0x30);      // Convert from ASCII and add

   // Print error statements, if necessary
   if (temp_hours >= 24)
   {
      printf("\nInvalid hours value\n");
   }
   else
   {
      if (temp_minutes >= 60)
      {
         printf("\nInvalid minutes value\n");
      }
      else
      {
         if (temp_seconds >= 60)
         {
            printf("\nInvalid seconds value\n");
         }
         // All entered variables are valid, so update the global values
         else
         {
            hours = temp_hours;
            minutes = temp_minutes;
            seconds = temp_seconds;

            printf("\nEntered Time: %02d:%02d:%02d\n", (short)hours,
                   (short)minutes, (short)seconds);
         }
      }
   }

   TMR3CN |= 0x04;                     // Restart Timer3
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer3_ISR
//-----------------------------------------------------------------------------
//
// Every time Timer3 overflows, update the RTC clock variables and toggle the
// LED.
//
//-----------------------------------------------------------------------------
INTERRUPT(Timer3_ISR, INTERRUPT_TIMER3)
{
   TMR3CN &= ~0x80;                    // Reset Interrupt

   LED = !LED;                         // Toggle the LED

   // Update the RTC time by one second
   seconds += 1;
   if (seconds == 60)
   {
      minutes +=1;
      seconds = 0;
      if (minutes == 60)
      {
         hours += 1;
         minutes = 0;
         if (hours == 24)
         {
            hours = 0;
         }
      }
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------