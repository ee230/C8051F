//-----------------------------------------------------------------------------
// T61x_UART_INT.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the C8051T610 to write to and read 
// from the UART interface. The program reads a word using the UART interrupts 
// and outputs that word to the screen, with all characters in uppercase
//
// How To Test:
//
// 1) Ensure that jumpers are placed on J10 of the C8051T610 target board
//    that connect the P0.4 pin to the TX signal, and P0.5 to the RX signal.
// 2) Ensure that a USB Cable connects the PC to the COMM USB Port (P4)
//    on the target board.
// 3) Specify the target baudrate in the constant <BAUDRATE>.
// 4) Open Hyperterminal, or a similar program, and connect to the target
//    board's USB Port (Select no flow control). 
// 5) Download and execute code on an 'T61x target board.
// 6) Type upto UART_BUFFERSIZE characters into hyperterminal followed by 
//    a carraige return to see those characters echoed back onto the 
//    hyperterminal by the 'T610.
//   
//
// FID:            32X000088
// Target:         C8051T61x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (SM)
//    -16 AUG 2007
//
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <c8051T610_defs.h>            // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK      24500000           // SYSCLK frequency in Hz
#define BAUDRATE        9600           // Baud rate of UART in bps


//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void UART0_Init (void);
void PORT_Init (void);
void Timer2_Init (int);

INTERRUPT_PROTO (UART0_ISR, INTERRUPT_UART0);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

#define UART_BUFFERSIZE 64
U8 UART_Buffer[UART_BUFFERSIZE];
U8 UART_Buffer_Size = 0; 
U8 UART_Input_First = 0;
U8 UART_Output_First = 0;
U8 TX_Ready =1;
static U8 Byte;
   
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void) 
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer 
                                       // enable)
   PORT_Init();                        // Initialize Port I/O
   SYSCLK_Init ();                     // Initialize Oscillator
   UART0_Init();
   
   EA = 1;

   while(1)
   {
	   if((TX_Ready == 1) && (UART_Buffer_Size != 0) && (Byte == 13))  
	   // If the complete word has been entered via the hyperterminal followed by
	   // carriage return
	  { 
	     TX_Ready = 0;                                     // Set the flag to zero
        TI0 = 1;                                          // Set transmit flag to 1
	  }         
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P0.4   digital   push-pull    UART TX
// P0.5   digital   open-drain   UART RX
// 
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0MDOUT |= 0x10;                    // Enable UTX as push-pull output
   XBR0    = 0x01;                     // Enable UART on P0.4(TX) and P0.5(RX)                     
   XBR1    = 0x40;                     // Enable crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal oscillator
// at its maximum frequency.
// Also enables the Missing Clock Detector.
//-----------------------------------------------------------------------------

void SYSCLK_Init (void)
{
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency
   RSTSRC  = 0x04;                     // Enable missing clock detector
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
   if (SYSCLK/BAUDRATE/2/256 < 1) {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } else if (SYSCLK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01                  
      CKCON |=  0x01;
   } else if (SYSCLK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;                       
   TR1 = 1;                            // START Timer1
   TX_Ready = 1;                       // Flag showing that UART can transmit
   IP |= 0x10;                         // Make UART high priority
   ES0 = 1;                            // Enable UART0 interrupts
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// UART0_Interrupt
//-----------------------------------------------------------------------------
//
// This routine is invoked whenever a character is entered or displayed on the
// Hyperterminal.
//
//-----------------------------------------------------------------------------

INTERRUPT(UART0_ISR, INTERRUPT_UART0)
{
  if (RI0 == 1)
   {
      if( UART_Buffer_Size == 0)  UART_Input_First = 0;   
	   // Check if a new word is being entered
	  
	   RI0 = 0;                                             
      Byte = SBUF0;                                      
      // Read a character from Hyperterminal 
 
      if (UART_Buffer_Size < UART_BUFFERSIZE)
	   {
         UART_Buffer[UART_Input_First] = Byte;               
	      // Store the character in an array 
      
	  	   UART_Buffer_Size++;             // Update array's size
	  
	      UART_Input_First++;             // Update counter
	   }
   }

   if (TI0 == 1)                                         
   // Check if transmit flag is set
   {
      TI0 = 0;
      if (UART_Buffer_Size != 1)         // If buffer not empty
      {
        
         if ( UART_Buffer_Size == UART_Input_First )      
	  	   // Check if a new word is being output 
		   {
		      UART_Output_First = 0;
         }

         Byte = UART_Buffer[UART_Output_First];           
  		   // Store a character in the variable byte  
         SBUF0 = Byte - 32;                              
		   // Convert the character to uppercase and transmit to Hyperterminal

         UART_Output_First++;            // Update counter

         UART_Buffer_Size--;             // Decrease array size
         		 		                                         
	   }
      else
      {  
	     UART_Buffer_Size = 0;            // Set the array size to 0 
	     TX_Ready = 1;                                   
		  // TX_Ready flag is set to 1 indicating that transmission is not complete   
      }
   }

}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------