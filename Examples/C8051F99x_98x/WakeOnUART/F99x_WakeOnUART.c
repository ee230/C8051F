//-----------------------------------------------------------------------------
// F99x_WakeOnUART.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program implements Low-Power Wake-on-UART. The MCU will be in sleep mode
// until a high-to-low transition is detected on the UART RX pin. It will then
// wake up and receive the character. The received character is echoed back.
//
// How To Test:
//
// Setup:
// 1) Customize the baud rate and rebuild.
// 2) Download code to a C8051F99xTB Target Board.
// 3) Run HyperTerminal on the PC.
// 4) Set up the baud rate of HyperTerminal to match the baud rate of
//    the MCU.
// 5) Type characters into the terminal and click the "Send Data" button. The
//    MCU will echo them back. When the MCU is awake, the Red LED (P1.5) will
//    be lit.
//
//
// Target:         C8051F99x-C8051F98x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    - Initial Revision (FB)
//    - 19 MAY 2010
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <C8051F990_defs.h>            // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (RED_LED,      SFR_P1, 5);        // '0' means ON, '1' means OFF
SBIT (YELLOW_LED,   SFR_P1, 3);        // '0' means ON, '1' means OFF
SBIT (SW2,          SFR_P0, 2);        // SW2 == 0 means switch pressed

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK          24500000L      // SYSCLK frequency in Hz
#define RTCCLK          32768          // smaRTClock frequency in Hz

#define BAUDRATE        57600L         // Baud rate of UART in bps

#define TIMEOUT_MS         1           // Wait time in ms


#define WAKE_LED_ACTIVE    1           // 0 = Red LED will be always off
                                       // 1 = Red LED will be on if MCU is awake

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
LOCATED_VARIABLE_NO_INIT (reserved, U8, SEG_XDATA, 0x0000);


//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void PORT_IO_Init (void);
void SYSCLK_Init (void);
void TIMER2_Init (void);
void UART0_Init (void);

//-----------------------------------------------------------------------------
// Generic UART definitions to support both Keil and SDCC
//-----------------------------------------------------------------------------

#ifdef SDCC
#include <sdcc_stdio.h>
#endif

char *GETS (char *, unsigned int);

#ifdef __C51__
#define GETS gets
#endif

#ifdef SDCC
#include <sdcc_stdio.c>
#endif

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void)
{
   U8 rx_char;
   U16 temp;

   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   SYSCLK_Init ();
   PORT_IO_Init ();
   TIMER2_Init ();
   UART0_Init ();

   for (temp = 0; temp < 0xffff; temp++);    // Delay before sending to ToolStick

   printf ("\f\f");
   printf ("\nHello! This program demonstrates low-power Wake-on-UART\n");
   printf ("MCU will enter sleep mode now, and will wake up on receiving a UART RX transition\n");

   EIE2 |= 0x02;                       // Enable Port Match interrupt
   EA = 1;                             // Enable global interrupts

   while (1)
   {
      if (TF2H == 1 && TI0 == 1)       // If timeout has occured AND last character was transmitted:
      {
         RED_LED = 1;                  // Turn off LED

         PMU0CF |= 0x20;               // Clear flags
         PMU0CF = 0x82;                // Enter Sleep mode with Port Match Wake Up

         #if WAKE_LED_ACTIVE == 1
            RED_LED = 0;               // Turn On LED
         #endif

         TR2 = 0;                      // Set timeout
         TF2H = 0;
         TMR2 = -(TIMEOUT_MS*SYSCLK/12/1000);
         TR2 = 1;
      }

      while (RI0 == 0 && TF2H == 0); // Wait till byte fully received or timeout

      if (RI0 == 1)
      {
         RI0 = 0;
         rx_char = SBUF0;              // Read received character
         printf ("%c", SBUF0);         // Echo received character
      }
   }
}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal precision
// oscillator at its maximum frequency and enables the missing clock
// detector.
//
//-----------------------------------------------------------------------------

void SYSCLK_Init (void)
{

   REG0CN |= 0x10;                     // Enable the precision osc. bias
   OSCICN |= 0x80;                     // Enable the precision internal osc.
   while((OSCICN & 0x40) == 0);        // Wait till IFRDY is set

   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor enabled.

   FLSCL  = 0x40;                      // Configre Flash read timing for
                                       // SYSCLK >10 MHz.

   CLKSEL = 0x00;                      // Select precision internal osc.
                                       // divided by 1 as the system clock

}

//-----------------------------------------------------------------------------
// Port_IO_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will initialize the Port I/O pins.
//
//-----------------------------------------------------------------------------
void PORT_IO_Init (void)
{
   // P0.0  -  Unassigned,  Open-Drain, Digital
   // P0.1  -  Unassigned,  Open-Drain, Digital
   // P0.2  -  Skipped,     Open-Drain, Digital (Push Button Switch, S2)
   // P0.3  -  Unassigned,  Open-Drain, Digital
   // P0.4  -  TX0 (UART0), Pull-Pull,  Digital
   // P0.5  -  RX0 (UART0), Open-Drain, Digital
   // P0.6  -  Unassigned,  Open-Drain, Digital
   // P0.7  -  Unassigned,  Open-Drain, Digital

   // P1.0  -  Unassigned,  Open-Drain, Digital
   // P1.1  -  Unassigned,  Open-Drain, Digital
   // P1.2  -
   // P1.3  -  Skipped,     Open-Drain, Digital (Yellow LED)
   // P1.4  -  Unassigned,  Open-Drain, Digital
   // P1.5  -  Skipped,     Open-Drain, Digital (Red LED)
   // P1.6  -  Unassigned,  Open-Drain, Digital
   // P1.7  -  Unassigned,  Open-Drain, Digital

   // P2.0  -  Unassigned,  Open-Drain, Digital
   // P2.1  -  Unassigned,  Open-Drain, Digital
   // P2.2  -  Unassigned,  Open-Drain, Digital
   // P2.3  -  Unassigned,  Open-Drain, Digital
   // P2.4  -  Unassigned,  Open-Drain, Digital
   // P2.5  -  Unassigned,  Open-Drain, Digital
   // P2.6  -  Unassigned,  Open-Drain, Digital
   // P2.7  -  Skipped,     Open-Drain, Digital

   P0MDOUT   = 0x10;
   P0SKIP    = 0x04;
   P1SKIP    = 0x28;
   P1MDOUT   = 0x28;

   P0MASK    = 0x24;                   // Port match interrupt when TX0=0 or SW2=0

   XBR0      = 0x01;
   XBR2      = 0x40;
}

//-----------------------------------------------------------------------------
// TIMER2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes Timer2 as 16-bit auto-reload timer clocked by
// SYSCLK/12.
//
//-----------------------------------------------------------------------------

void TIMER2_Init (void)
{
   TMR2CN = 0x00;
   TMR2RL = -(TIMEOUT_MS*SYSCLK/12/1000);
   TMR2 = 0xFFFF;                      // Set to overflow immediately
   TR2 = 1;                            // Start timer
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1, for <BAUDRATE> and 8-N-1.
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   #if (SYSCLK/BAUDRATE/2/256 < 1)
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   #elif (SYSCLK/BAUDRATE/2/256 < 4)
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   #elif (SYSCLK/BAUDRATE/2/256 < 12)
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   #else
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   #endif

   TL1 = TH1;                          // Init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
   TI0 = 1;                            // Indicate TX0 ready
}

//-----------------------------------------------------------------------------
// PORTMATCH_ISR
//-----------------------------------------------------------------------------
//
// This ISR code is executed on a port match event.
// It resets Timer2, which is used as a timeout to go back into sleep mode.
//
//-----------------------------------------------------------------------------
INTERRUPT (PORTMATCH_ISR, INTERRUPT_PORT_MATCH)
{
   TR2 = 0;
   TF2H = 0;
   TMR2 = -(TIMEOUT_MS*SYSCLK/12/1000);
   TR2 = 1;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------