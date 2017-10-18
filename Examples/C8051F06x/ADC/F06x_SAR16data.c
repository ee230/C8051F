//------------------------------------------------------------------------------------
// F06x_SAR16Data.c
//------------------------------------------------------------------------------------
// Copyright (C) 2005 Silicon Laboratories, Inc.
//
//
// Data Acquisition example for 'F06x
//
// Target: C8051F060TB Target Board
//
// Description: This example illustrates the use of ADC1 and the DMA to acquire and store
// data.  It is intended for use with the C8051F060TB target board in the development kit.
//
// This code measures a signal at the AIN0 input with the 16-bit SAR ADC0.
// The data is sampled at a rate of 100kHz.  ADC output data is transferred to
// XDATA memory space using the DMA.
//
// With the EMIF configured for off-chip XRAM access (on the upper ports),
// this code moves the ADC data to the C8051F060TB target boards's SRAM device.
// Once data acquisition is complete, the code then prompts the user to press the P3.7
// button on the target board when ready to receive the data via the UART0 serial port.
// The TB features an RS-232 transceiver and connector, so the data can be transferred
// to a PC via its serial port.
//
// The code is set to acquire up to 32768 samples (for 64kbytes of data).  The SRAM device
// can accommodate up to 128kbytes, but this requires banking (A16 signal on the SRAM).
//
// Target Board Configuration:  This example uses RS-232 communications, typically with
// a PC.  A serial cable should be connected to the J5 RS-232 DB-9 connector on the
// target board and to a serial port on a PC.  (A serial port will also be needed for
// PC communications with the EC-2 serial adapter for programming and debug).
// Data can be input from the PC's serial port using commonly available Hyperterminal.
// The following target board jumpers must be configured (see user's guide for schematic):
//
//   J2: installed (voltage supply)
//   J6 and J9: installed (RS-232 combs)
//   J17 and J28: installed (external voltage reference for SIR)
//   J11: installed (SRAM chip select)
//   J14: Installed to connect to (SRAM) A16 to GND, and NOT P3.7 which will be used
//        for push-button
//   J1: Installed to connect P3.7 to push-button.
//   J20: Installed to enable +/-5V switching power supply. Connect pin 1 (Van, +5V) to
//        pin 2 (/SHIN).
//
// See the User's Guide for additional information and jumper connection information.
//
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------
#include <c8051f060.h>                    // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F06x
//-----------------------------------------------------------------------------

sfr16 RCAP3    = 0xCA;                 // Timer3 reload value
sfr16 TMR3     = 0xCC;                 // Timer3 counter

sfr16 ADC0     = 0xBE;                 // ADC0 Data
sfr16 ADC1     = 0xBE;                 // ADC1 Data

sfr16 DMA0DS   = 0xDB;                 // DMA0 XRAM Address Pointer
sfr16 DMA0CT   = 0xF9;                 // DMA0 Repeat Counter Limit
sfr16 DMA0DA   = 0xD9;                 // DMA0 Address Beginning
sfr16 DMA0CS   = 0xFB;                 // DMA0 Repeat Counter


//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------

#define SYSCLK 22118400                   // SYSCLK frequency in Hz
#define BAUDRATE 115200                   // Baud Rate for UART0

// DMA INSTRUCTIONS
#define DMA0_END_OF_OP     0x00           // End-of-Operation
#define DMA0_END_OF_OP_C   0x80           // End-of-Operation + Continue
#define DMA0_GET_ADC0      0x10           // Retrieve ADC0 Data
#define DMA0_GET_ADC1      0x20           // Retrieve ADC1 Data
#define DMA0_GET_ADC01     0x30           // Retrieve ADC0 and ADC1 Data
#define DMA0_GET_DIFF      0x40           // Retrieve Differential Data
#define DMA0_GET_DIFF1     0x60           // Retrieve Differential and ADC1 Data


#define NUM_SAMPLES        32768          // Number of ADC sample to acquire (each sample 2 bytes)
#define XRAM_START_ADD     0x0000         // DMA0 XRAM Start address of ADC data log
#define SAMP_RATE          100000         // ADC sample rate in Hz


sbit LED = P0^7;                          // LED: '1' = ON; '0' = OFF
sbit BUTTON = P3^7;                       // pushbutton on the target board
sbit RAM_CS = P4^5;                       // chip select bit is P4^4

//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------
void SYSCLK_Init (void);
void UART0_Init (void);
void PORT_Init (void);
void ADC0_Init (void);
void ADC1_Init (void);
void DMA0_Init (void);
void Timer3_Init (int counts);
void EMIF_Init (void);
void SendData(void);

//-------------------------
// Global Variables
//-------------------------
unsigned char Conv_Complete = 0;
unsigned int xdata * data read_ptr;

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main (void)
{
   WDTCN = 0xde;                          // disable watchdog timer
   WDTCN = 0xad;

   SYSCLK_Init ();                        // initialize SYSCLK

   PORT_Init ();

   EMIF_Init ();                          // Storing ADC samples in SRAM on the
                                          // target board.
   SFRPAGE = CONFIG_PAGE;

   RAM_CS = 0;                            // assert SRAM chip select

   UART0_Init ();                         // initialize UART0

   Timer3_Init (SYSCLK/SAMP_RATE);        // Init Timer3 for 100ksps sample rate

   ADC0_Init ();                          // configure ADC0 and ADC1 for differential
                                          // measurement.

   DMA0_Init ();                          // Configure DMA to move NUM_SAMP samples.


   SFRPAGE = UART0_PAGE;
   printf ("Data Acquisition in progress...\n");

   SFRPAGE = DMA0_PAGE;                   // Switch to DMA0 Page

   while (!(DMA0CN & 0x40));              // Wait for DMA to obtain and move ADC samples
                                          // by polling DMA0INT bit.

   SFRPAGE = LEGACY_PAGE;
   printf ("Data Acquisition complete.\nPress P3.7 button when ready to receive file.\n");

   while (BUTTON != 0);                   // Wait for user to press P3.7 on the TB.

   SendData();                            // Send data via the UART0.

   while(1);                              // Done.


}

//------------------------------------------------------------------------------------
// PORT_Init
//------------------------------------------------------------------------------------
//
// Configure the Crossbar and GPIO ports
//
void PORT_Init (void)
{
char old_SFRPAGE = SFRPAGE;

   SFRPAGE = CONFIG_PAGE;              // Switch to configuration page


   XBR0    = 0x04;                     // Enable UART0 on crossbar
   XBR1    = 0x00;
   XBR2    = 0x40;                     // Enable crossbar and weak pull-ups
   P0MDOUT |= 0xFF;                    // enable Port0 outputs as push-pull

   SFRPAGE = old_SFRPAGE;              // restore SFRPAGE
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Configure the UART0 using Timer1, for <baudrate> and 8-N-1.
//
void UART0_Init (void)
{
char old_SFRPAGE = SFRPAGE;

   SFRPAGE = UART0_PAGE;               // Switch to UART0 page

   SCON0   = 0x50;                     // SCON: mode 1, 8-bit UART, enable RX
   SSTA0 = 0x10;                       // Timer 1 generates UART0 baud rate and
                                       // UART0 baud rate divide by two disabled

   SFRPAGE = TIMER01_PAGE;             // Switch to Timer 0/1 page

   TMOD   = 0x20;                      // TMOD: timer 1, mode 2, 8-bit reload
   TH1    = -(SYSCLK/BAUDRATE/16);     // set Timer1 reload value for baudrate
   TR1    = 1;                         // start Timer1
   CKCON |= 0x10;                      // Timer1 uses SYSCLK as time base
   PCON  |= 0x80;                      // SMOD = 1

   SFRPAGE = UART0_PAGE;               // Switch to UART0 page

   TI0    = 1;                         // Indicate TX ready

   SFRPAGE = old_SFRPAGE;              // restore SFRPAGE

}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   char old_SFRPAGE = SFRPAGE;
   int i;                                 

   SFRPAGE = CONFIG_PAGE;              // Switch to Configuration Page

   OSCXCN = 0x67;                      // start external oscillator with
                                       // 22.1184MHz crystal on TB

   for (i=0; i <5000; i++) ;           // XTLVLD blanking interval (>1ms)

   while (!(OSCXCN & 0x80)) ;          // Wait for crystal osc. to settle

   RSTSRC = 0x04;                      // enable missing clock detector reset

   CLKSEL = 0x01;                      // change to external crystal

   OSCICN = 0x00;                      // disable internal oscillator



   SFRPAGE = old_SFRPAGE;              // restore SFRPAGE
}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   char old_SFRPAGE = SFRPAGE;
   int i;

   SFRPAGE = ADC0_PAGE;                // Switch to ADC0 Page

   ADC0CN = 0x44;                      // ADC Disabled, Timer3 start-of-conversion
                                       // track 16 SAR clocks before data conversion
                                       // upon Timer3 OV.  DMA will enable ADC as needed
                                       //

   REF0CN = 0x03;                      // turn on bias generator and internal reference.

   for(i=0;i<10000;i++);               // Wait for Vref to settle (large cap used on target board)

   AMX0SL = 0x00;                      // Single-ended mode

   ADC0CF = (SYSCLK/25000000) << 4;    // Select SAR clock frequency =~ 25MHz

   SFRPAGE = old_SFRPAGE;              // restore SFRPAGE
}



//-----------------------------------------------------------------------------
// DMA0_Init
//-----------------------------------------------------------------------------
void DMA0_Init (void)
{
   char old_SFRPAGE = SFRPAGE;

   SFRPAGE = DMA0_PAGE;                // Switch to DMA0 Page

   DMA0CN = 0x00;                      // Disable DMA interface

   DMA0DA = XRAM_START_ADD;            // Starting Point for XRAM addressing

   DMA0CT = NUM_SAMPLES;               // Get NUM_SAMPLES samples

   DMA0IPT = 0x00;                     // Start writing at location 0

   // Push instructions onto stack in order they will be executed
   DMA0IDT = DMA0_GET_ADC0;            // DMA to move ADC0 data.
   DMA0IDT = DMA0_END_OF_OP;

   DMA0BND = 0x00;                     // Begin instruction executions at address 0
   DMA0CN = 0xA0;                      // Mode 1 Operations, Begin Executing DMA Ops
                                       // (which will start ADC0)

   SFRPAGE = old_SFRPAGE;              // restore SFRPAGE
}

//------------------------------------------------------------------------------------
// Timer3_Init
//------------------------------------------------------------------------------------
//
// Configure Timer3 to auto-reload and generate ADC sample rate
// specified by <counts> using SYSCLK as its time base.
//
void Timer3_Init (int counts)
{
   char old_SFRPAGE = SFRPAGE;

   SFRPAGE = TMR3_PAGE;                // Switch to Timer 3 page

   TMR3CN = 0x00;                      // Stop Timer3; Clear TF3;
   TMR3CF = 0x08;                      // use SYSCLK as timebase
   RCAP3   = -counts;                  // Init reload values
   TMR3    = 0xffff;                   // set to reload immediately
   TR3 = 1;                            // start Timer3

   SFRPAGE = old_SFRPAGE;              // restore SFRPAGE
}

//-----------------------------------------------------------------------------
// EMIF_Init
//-----------------------------------------------------------------------------
//
// Configure the external memory interface to use upper port pins in
// non-multiplexed mode to a mixed on-chip/off-chip configuration without
// Bank Select.
//
void EMIF_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = EMI0_PAGE;                // Save SFR_PAGE
   EMI0CF = 0x3C;                      // upper ports; non-muxed mode;
                                       // split mode w/o bank select
   EMI0TC = 0x45;                      // timing (7-cycle MOVX)

   SFRPAGE = CONFIG_PAGE;
   P4MDOUT |= 0xFF;                    // all EMIF pins configured as
   P5MDOUT |= 0xFF;                    // push-pull
   P6MDOUT |= 0xFF;
   P7MDOUT |= 0xFF;

   SFRPAGE = SFRPAGE_SAVE;             // restore SFR_PAGE
}

//-----------------------------------------------------------------------------
// SendData
//-----------------------------------------------------------------------------
//
//Send data out UART0 
//
void SendData(void)
{
   unsigned int i;
   char old_SFRPAGE = SFRPAGE;
   SFRPAGE = UART0_PAGE;               // Switch to UART0 page

   read_ptr = XRAM_START_ADD;          // Set pointer to beginning of data

   for (i=0;i<NUM_SAMPLES;i++)
   {
   printf ("%u\n",*read_ptr);          // Send data as unsigned integers
   read_ptr++;
   }

   SFRPAGE = old_SFRPAGE;

}//End




