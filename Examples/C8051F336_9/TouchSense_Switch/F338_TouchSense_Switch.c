//-----------------------------------------------------------------------------
// F338_TouchSense_Switch.c
//-----------------------------------------------------------------------------
// Copyright (C) 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program lights up the green LED on the C8051F338 target board when
// the capacitive touch sense (contactless) switch (SW3) is touched. 
//
// A relaxation oscillator is implemented using an on-chip analog comparator 
// and external resistors R15 through R19. The high-to-low transitions of the 
// relaxation oscillator are counted by Timer0. The relaxation oscillator 
// frequency depends on the capacitance of the touch sense trace capacitor. 
// The count of Timer0 is periodically checked on every Timer2 overflow. 
// Depending on the count, it can be determined whether SW3 is being touched
// or not. 
//
//
// How To Test:
//
// Setup:
// 1) Download code to the 'F338 target board
// 2) Ensure that pins 1 and 2 are shorted together on the J3 header
// 3) Ensure that pins 1-2, 3-4, 5-6, and 7-8 are shorted on the J11 header
//
// One Time Calibration (stored in non-volatile flash):
// 1) The P1.3 LED will be lit. Place a finger on the touch switch SW3, and 
//    hold it there. Do not remove this finger till step 3.
// 2) Press and hold switch SW2 (P0.7). Release the switch after holding 
//    down for more than a second. 
// 3) The P1.3 LED will continue to be lit. Remove finger from SW3. The LED 
//    should switch off.
//
// Usage:
// 1) Touch SW3. The LED (P1.3) should light up in response.
//
// Target:         C8051F336, 'F337, 'F338, 'F339
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (PKC)
//    -29 AUG 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // Compiler-specific macros
#include <C8051F336_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             24500000UL  // SYSCLK frequency in Hz

#define T2_OVERFLOW_RATE   20          // Timer2 overflow rate in ms

#define SW3_SENSITIVITY    3000        // Higher number means more sensitive
                                       // This value is added to the cal value

SBIT (LED, SFR_P1, 3);                 // LED==1 means ON
SBIT (SW2, SFR_P0, 7);                 // SW1==0 means switch depressed

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Init_Device (void);
void PCA_Init (void);
void OSCILLATOR_Init (void);
void PORT_Init (void);
void COMPARATOR0_Init (void);
void TIMER0_Init (void);
void TIMER2_Init (void);
void INTERRUPTS_Init (void);

void Calibrate_SW3 (void);
void FLASH_ByteWrite (U16 addr, U8 byte);

INTERRUPT_PROTO (TIMER2_ISR, INTERRUPT_TIMER2);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
// Timer0 count of SW3 relaxation oscillator transitions
UU16 SW3_T0_Count;                     

// Touch Switch: SW3==0 means finger sensed; SW3==1 means finger not sensed.
U8 SW3_Status;                         

// This 16-bit variable stores the calibration value to non-volatile Flash 
// memory. It is initialized to 0xFFFF becaused erased Flash memory locations
// are set to 1. To recalibrate, re-download code to MCU.
SEGMENT_VARIABLE (SW3_Cal_Value, U16, code) = 0xFFFF;

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   Init_Device ();

   SW3_Status = 1;                     // Initialize switch to not sensed

   EA = 1;                             // Enable global interrupts
   
   if (SW3_Cal_Value == 0xFFFF) 
      Calibrate_SW3 ();

   while (1) {}                        // Spin forever
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Init_Device
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function calls all peripheral initialization funtions.
//-----------------------------------------------------------------------------
void Init_Device (void)
{
   PCA_Init ();
   OSCILLATOR_Init ();
   PORT_Init ();
   COMPARATOR0_Init ();
   TIMER0_Init ();
   TIMER2_Init ();
   INTERRUPTS_Init ();
}

//-----------------------------------------------------------------------------
// PCA_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the PCA; disables the watchdog timer.
//-----------------------------------------------------------------------------
void PCA_Init (void)
{
   PCA0MD    &= ~0x40;                // WDTE = 0 (disable watchdog timer)
   PCA0MD    = 0x00;
}

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the Internal Oscillator.
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   OSCICN    = 0x83;                   // Sets SYSCLK to 24.5 MHz
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
// P0.0  -  Skipped (VREF),   Open-Drain, Analog
// P0.1  -  Skipped (IDAC),   Open-Drain, Analog
// P0.2  -  Skipped (XTAL1),  Open-Drain, Analog
// P0.3  -  Skipped (XTAL2),  Open-Drain, Analog
// P0.4  -  TX0 (UART0),      Push-Pull,  Digital
// P0.5  -  RX0 (UART0),      Open-Drain, Digital
// P0.6  -  Skipped (CNVSTR), Open-Drain, Digital
// P0.7  -  Skipped (S2),     Open-Drain, Digital

// P1.0  -  CP0A (Cp0A),      Push-Pull,  Digital
// P1.1  -  Skipped (RTS),    Open-Drain, Digital
// P1.2  -  Skipped (CTS),    Open-Drain, Digital
// P1.3  -  Skipped (LED),    Push-Pull,  Digital
// P1.4  -  Skipped (AIN),    Open-Drain, Analog
// P1.5  -  Skipped (AIN),    Open-Drain, Analog
// P1.6  -  Skipped (POT),    Open-Drain, Analog
// P1.7  -  Skipped,          Open-Drain, Digital

// P2.0  -  Skipped (CP0+),   Open-Drain, Analog
// P2.1  -  Skipped (CP0-),   Open-Drain, Analog 
// P2.2  -  Skipped,          Open-Drain, Digital
// P2.3  -  T0 (Timer0),      Open-Drain, Digital
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0MDIN    = 0xF0;                   // P0.0 thru P0.3 Analog Inputs
   P1MDIN    = 0x8F;                   // P1.4 thru P1.6 Analog Inputs
   P2MDIN    = 0x0C;                   // P2.0 thru P2.1 Analog Inputs
   P0MDOUT   = 0x10;                   // P0.4 Push-Pull Output
   P1MDOUT   = 0x09;                   // P1.0, P1.3 Push-Pull Outputs
   P0SKIP    = 0xCF;                   // P0.0-P0.3, P0.6-P0.7 - Crossbar skip
   P1SKIP    = 0xFE;                   // P1.1-P1.7  - Crossbar skip
   P2SKIP    = 0x07;                   // P2.0-P2.2 -  Crossbar skip
   XBR0      = 0x21;                   // Comparator0 Async o/p, UART0 enabled
   XBR1      = 0x50;                   // T0 routed to port pin; Enable crossbar
}

//-----------------------------------------------------------------------------
// COMPARATOR0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures comparator0.
//-----------------------------------------------------------------------------
void COMPARATOR0_Init (void)
{
   unsigned int i = 0;
   CPT0CN    = 0x80;
   for (i = 0; i < 35; i++);           // Wait 10us for initialization
   CPT0CN    &= ~0x30;                 // Clear rising and falling edge flags
   CPT0MX    = 0x88;                   // CP0+ = P2.0;  CP0- = P2.1
}

//-----------------------------------------------------------------------------
// TIMER0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures timer0.
//-----------------------------------------------------------------------------
void TIMER0_Init (void)
{
   TMOD     &= ~0x0F;                  // Clear Timer0 bits in the TMOD SFR
                                       // Sets Mode0, and no INT0 gating

   TMOD     |= 0x04;                   // Timer0 incremented by high-to-low 
                                       // transitions on ext. input T0 (P2.3)

   TH0       = 0x00;
   TL0       = 0x00;

   TR0       = 1;                      // Start Timer0
}

//-----------------------------------------------------------------------------
// TIMER2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures timer2 in 16-bit auto-reload mode, clocked by
// SYSCLK/12; set to overflow at T2_OVERFLOW_RATE.
//-----------------------------------------------------------------------------
void TIMER2_Init (void)
{
   CKCON    &= ~0x30;                  // Timer2 clock is defined in TMR2CN
   TMR2CN    = 0x00;                   // Timer2 is clocked by SYSCLK/12
                                       // Operates in 16-bit auto-reload mode

   // Set to overflow at T2_OVERFLOW_RATE
   // (Negation sign in front because the timer counts "0xFFFF-TMR2RL" before
   //  an overflow)
   TMR2RL    = -(SYSCLK / 12 * T2_OVERFLOW_RATE / 1000);

   TMR2      = 0xFFFF;                 // Set to overflow immediately

   TR2       = 1;                      // Start Timer2
}

//-----------------------------------------------------------------------------
// INTERRUPTS_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures interrupts; Enables Timer2 interrupt.
//-----------------------------------------------------------------------------
void INTERRUPTS_Init (void)
{
   ET2 = 1;
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TIMER2_ISR
//-----------------------------------------------------------------------------
//
// This ISR is triggered whenever Timer2 overflows. It checks the switch status
// by comparing the current count of Timer0 against an established threshold.
//-----------------------------------------------------------------------------
INTERRUPT(TIMER2_ISR, INTERRUPT_TIMER2)
{
   TF2H = 0;                           // Clear Timer2 high-byte overflow flag

   TR0 = 0;                            // Stop Timer0
   
   // Save Timer0 count (SW3 relaxation oscillator transition count)
   SW3_T0_Count.U8[LSB] = TL0;         
   SW3_T0_Count.U8[MSB] = TH0;
   
   TL0 = 0;                            // Reset Timer0 count
   TH0 = 0;
   
   TR0 = 1;                            // Restart Timer0

   // If the count is greater than the cal value, it implies lower capacitance,
   // which means the switch is not being touched.
   // If the count is lower or equal to the cal value, it implies higher
   // capacitance that is added due to a finger on the switch.

   if (SW3_T0_Count.U16 > SW3_Cal_Value)
   {
      LED = 0;                         // Switch LED OFF
      SW3_Status = 1;                  // Update status of global variable
   }
   else
   {
      LED = 1;                         // Switch LED ON
      SW3_Status = 0;                  // Update status of global variable
   }
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Calibrate_SW3
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine will store the touch value in non-volatile Flash memory as
// the calibration threshold for the switch SW3.
//
// 1. Place a finger on the touch switch SW3, and hold it there.
// 2. Press switch SW2 (P0.7) for >1 sec and release using another finger.
// 3. Remove finger from SW3.
//-----------------------------------------------------------------------------
void Calibrate_SW3 (void)
{
   U8 EA_Save;                    
        
   while (SW2);                        // Wait till SW2 is pressed
   
   EA_Save = IE;                       // Preserve EA
   EA = 0;                             // Disable interrupts

   // Add additional sensitivity value to calibration value
   SW3_T0_Count.U16 += SW3_SENSITIVITY;
   
   FLASH_ByteWrite ((U16)(&SW3_Cal_Value)+MSB, SW3_T0_Count.U8[MSB]);
   FLASH_ByteWrite ((U16)(&SW3_Cal_Value)+LSB, SW3_T0_Count.U8[LSB]);

   if ((EA_Save & 0x80) != 0)          // Restore EA
      EA = 1;
}

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters:
//   1) U16 addr - Flash address to write
//         Range - 0x0000 to 0x3DFF (15.5k user accessible flash)
//   2) U8 byte  - Byte to write
//         Range - 0x00 to 0xFF
//
// This routine writes "byte" at flash address "addr"; Adapted from AN201.
//
//-----------------------------------------------------------------------------
void FLASH_ByteWrite (U16 addr, U8 byte)
{
   U8 EA_Save = IE;                    // Preserve EA
   SEGMENT_VARIABLE_SEGMENT_POINTER (pwrite, U8, xdata, data); 
   //unsigned char xdata * data pwrite;// FLASH write pointer

   EA = 0;                             // Disable interrupts

   VDM0CN = 0x80;                      // Enable VDD monitor


   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   pwrite = (char xdata *) addr;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x01;                      // PSWE = 1


   VDM0CN = 0x80;                      // Enable VDD monitor


   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   *pwrite = byte;                     // Write the byte

   PSCTL &= ~0x01;                     // PSWE = 0

   if ((EA_Save & 0x80) != 0)          // Restore EA
      EA = 1;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------