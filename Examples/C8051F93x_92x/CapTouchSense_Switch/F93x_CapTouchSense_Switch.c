//-----------------------------------------------------------------------------
// F93x_CapTouchSense_Switch.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program lights up the yellow LED on the C8051F93x target board when
// the capacitive touch sense (contactless) switch (P2.0) is touched. It 
// also lights up the red LED when the (contactless) switch (P2.1) is touched. 
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
// 1) Download code to the 'F930 target board
// 2) Ensure that pins 1-2, 3-4, 5-6, and 7-8 are shorted on the J8 header
//
// One Time Calibration (stored in non-volatile flash):
// 1) Both LEDs will be lit. Place a finger on either Touch Sense Pad (P2.0
//    or P2.1), and  hold it there. Do not remove this finger till step 3.
// 2) Press and hold switch SW2 (P0.2) for if your finger is on Touch Sense 
//    Pad (P2.0). Otherwise press and hold SW3 (P0.3) if your finger is on 
//    Touch Sense Pad (P2.1).  Release the switch after 1 second.
// 3) The yellow LED will continue to be lit if your finger was touching the 
//    P2.0 pad. The red LED will continue to be lit if your finger was touching
//    the P2.1 pad. Remove finger from the Touch Sense Pad. The LEDs should 
//    switch off.
//
// Usage:
// 1) Touch SW20. The Yellow LED (P1.6) should light up in response.
// 2) Touch SW21. The Red LED (P1.5) should light up in response.
//
// Target:         C8051F930, C8051F920
// Tool chain:     Generic
// Command Line:   None
//
//
// Release 1.2 
//    - Updated to be compatible with Raisonance Toolchain (FB)
//    - 14 APR 2010
//
// Release 1.1 
//    - Compiled and tested for C8051F930-TB (JH)
//    - 06 JULY 2009
//
// Release 1.0
//    - Initial Revision (FB)
//    - 05 OCT 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F930_defs.h>            // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------

SBIT (RED_LED,    SFR_P1, 5);          // '0' means ON, '1' means OFF
SBIT (YELLOW_LED, SFR_P1, 6);          // '0' means ON, '1' means OFF
SBIT (SW2,        SFR_P0, 2);          // SW2 == 0 means switch pressed
SBIT (SW3,        SFR_P0, 3);          // SW3 == 0 means switch pressed

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK      24500000           // SYSCLK frequency in Hz
#define BAUDRATE      230400           // Baud rate of UART in bps

#define LED_ON              0          // Macros to turn LED on and off
#define LED_OFF             1

#define SW21_SENSITIVITY   250         // Sensitivity value used for both
                                       // Touch Sense switches. Larger values
                                       // make sthe switches more sensitive

#define CAL_ADDRESS         0          // Address in the scratchpad to store
                                       // the calibrated switch value

#define SCRATCHPAD          1          // Passed to the Flash access routines
                                       // to indicate that the calibration
                                       // constant is located in the scratchpad
               

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
// Timer2 count of SW20 relaxation oscillator transitions
UU16 SW20_Timer_Count;                     
UU16 SW21_Timer_Count;

// Touch Switch: SW20==0 means finger sensed; SW20==1 means finger not sensed.
U8 SW20_Status;     
U8 SW21_Status; 

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void PORT_Init (void);
void TouchSense_Init (void);

void Wait_MS(unsigned int ms);
void TouchSense_Update(void);
void Calibrate (void);
U16 Get_Calibration(void);

// FLASH read/write/erase routines
void FLASH_ByteWrite (U16 addr, U8 byte, U8 SFLE);
U8   FLASH_ByteRead  (U16 addr, U8 SFLE);
void FLASH_PageErase (U16 addr, U8 SFLE);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void)
{

   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   PORT_Init();                        // Initialize Port I/O
   SYSCLK_Init ();                     // Initialize Oscillator
   
   TouchSense_Init();                  // Initialize Comparator0 and 
                                       // Timer2 for use with TouchSense
   
   if( Get_Calibration() == 0xFFFF)
   {
      Calibrate ();
   }   


   while(1)
   {
      Wait_MS(25);                     // Polling Interval 
      
      if(!SW2 || !SW3)                 // If the P0.2 Switch is Pressed
      {
         Calibrate();                  // Calibrate the Touch Pads
      }

      TouchSense_Update();             // Update switch variables

      YELLOW_LED = SW20_Status;        // Set LED states based on the 
      RED_LED = SW21_Status;           // switch variables
   }

}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TouchSense_Update
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
//
//-----------------------------------------------------------------------------
void TouchSense_Update (void)
{  
   
   UU16 timer_count_A, timer_count_B;

   // Prepare Timer2 for the first TouchSense reading
   TMR2CN &= ~0x80;                    // Clear overflow flag
   while(!(TMR2CN & 0x80));            // Wait for overflow
   timer_count_A.U16 = TMR2RL;         // Record value
   
   // Prepare Timer2 for the second TouchSense reading
   TMR2CN &= ~0x80;                    // Clear overflow flag
   while(!(TMR2CN & 0x80));            // Wait for overflow
   timer_count_B.U16 = TMR2RL;         // Record value
   
   // Calculate the oscillation period
   SW20_Timer_Count.U16 = timer_count_B.U16 - timer_count_A.U16;
   
   // Change the CP0 Multiplexer to select switch on P2.1
   CPT0MX = 0x8C;                      // Positive Mux: TouchSense Compare
                                       // Negative Mux: P2.1 - TouchSense Switch

   // Prepare Timer2 for the first TouchSense reading
   TMR2CN &= ~0x80;                    // Clear overflow flag
   while(!(TMR2CN & 0x80));            // Wait for overflow
   timer_count_A.U16 = TMR2RL;         // Record value
   
   // Prepare Timer2 for the second TouchSense reading
   TMR2CN &= ~0x80;                    // Clear overflow flag
   while(!(TMR2CN & 0x80));            // Wait for overflow
   timer_count_B.U16 = TMR2RL;         // Record value
   
   // Calculate the oscillation period
   SW21_Timer_Count.U16 = timer_count_B.U16 - timer_count_A.U16;
   
   // Change the CP0 Multiplexer to select switch on P2.0
   CPT0MX = 0xC8;                      // Positive Mux: P2.0 - TouchSense Switch
                                       // Negative Mux: TouchSense Compare

   // Update the status variable for SW20
   if(SW20_Timer_Count.U16 > (Get_Calibration() - SW21_SENSITIVITY))
   {
      SW20_Status = 0;
   
   } else
   {
      SW20_Status = 1;
   }
   // Update the status variable for SW21   
   if(SW21_Timer_Count.U16 > (Get_Calibration() - SW21_SENSITIVITY))
   {
      SW21_Status = 0;
   
   } else
   {
      SW21_Status = 1;
   }
}


//-----------------------------------------------------------------------------
// Wait_MS
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters:
//   1) unsigned int ms - number of milliseconds of delay
//                        range is full range of integer: 0 to 65335
//
// This routine inserts a delay of <ms> milliseconds.
//-----------------------------------------------------------------------------
void Wait_MS(unsigned int ms)
{
   char i;

   TR0 = 0;                            // Stop Timer0
   
   TMOD &= ~0x0F;                      // Timer0 in 8-bit mode
   TMOD |= 0x02;
   
   CKCON &= ~0x03;                     // Timer0 uses a 1:48 prescaler
   CKCON |= 0x02;                   
    
   
   TH0 = -SYSCLK/48/10000;             // Set Timer0 Reload Value to 
                                       // overflow at a rate of 10kHz
   
   TL0 = TH0;                          // Init Timer0 low byte to the
                                       // reload value
   
   TF0 = 0;                            // Clear Timer0 Interrupt Flag
   ET0 = 0;                            // Timer0 interrupt disabled
   TR0 = 1;                            // Timer0 on

   while(ms--)
   {
      for(i = 0; i < 10; i++)
      {
         TF0 = 0;
         while(!TF0);
      }
   }

   TF0 = 0;

}


//-----------------------------------------------------------------------------
// Calibrate
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine will store the touch value in non-volatile Flash memory as
// the calibration threshold for the switch SW21.
//
// 1. Place a finger on the touch switch SW21, and hold it there.
// 2. Press switch SW2 (P0.7) for >1 sec and release using another finger.
// 3. Remove finger from SW21.
//-----------------------------------------------------------------------------
void Calibrate (void)
{
   U8  EA_Save; 
   U8  switch_number;
   
   YELLOW_LED = LED_ON;         
   RED_LED = LED_ON;         
        
   while (SW2 && SW3);                 // Wait till any switch is pressed
   
   if(!SW2)                            // Decode which switch was pressed
   {
      switch_number = 0;
   } else
   {
      switch_number = 1;
   }
      
   while (!SW2 || !SW3);               // Wait till switches released   

   EA_Save = IE;                       // Preserve EA
   EA = 0;                             // Disable interrupts
   
   TouchSense_Update();                // Update switch variables
   
   // Erase the scratchpad
   FLASH_PageErase (CAL_ADDRESS, SCRATCHPAD);   

   
   if(switch_number == 0)
   {
      // Write the expected switch value to the scratchpad
      FLASH_ByteWrite ((U16)(CAL_ADDRESS+MSB), SW20_Timer_Count.U8[MSB], SCRATCHPAD);
      FLASH_ByteWrite ((U16)(CAL_ADDRESS+LSB), SW20_Timer_Count.U8[LSB], SCRATCHPAD);
   
   } else
   {

      // Write the expected switch value to the scratchpad
      FLASH_ByteWrite ((U16)(CAL_ADDRESS+MSB), SW21_Timer_Count.U8[MSB], SCRATCHPAD);
      FLASH_ByteWrite ((U16)(CAL_ADDRESS+LSB), SW21_Timer_Count.U8[LSB], SCRATCHPAD);
   }
   
   if ((EA_Save & 0x80) != 0)          // Restore EA
   {
      EA = 1;
   }
}

//-----------------------------------------------------------------------------
// Get_Calibration
//-----------------------------------------------------------------------------
//
// Return Value : 
//   1) unsigned int   -  calibration value
//                        range is full range of integer: 0 to 65335
// Parameters: None
//
// This routine reads the calibration value from Flash
//-----------------------------------------------------------------------------
U16 Get_Calibration(void)
{
   UU16 cal_value;   

   cal_value.U8[MSB] = FLASH_ByteRead ((U16)(CAL_ADDRESS+MSB),  SCRATCHPAD);
   cal_value.U8[LSB] = FLASH_ByteRead ((U16)(CAL_ADDRESS+LSB),  SCRATCHPAD);
   
   return cal_value.U16;

}


//-----------------------------------------------------------------------------
// Flash Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of the byte to write to
//                    valid range is from 0x0000 to 0xFBFF for 64K devices
//                    valid range is from 0x0000 to 0x7FFF for 32K devices
//   2) char byte - byte to write to Flash.
//
// This routine writes <byte> to the linear FLASH address <addr>.
//-----------------------------------------------------------------------------
void FLASH_ByteWrite (U16 addr, U8 byte, U8 SFLE)
{
   U8 EA_Save = IE;                    // Preserve EA
   SEGMENT_VARIABLE_SEGMENT_POINTER (pwrite, U8, xdata, data); 
   //unsigned char xdata * data pwrite;// FLASH write pointer

   EA = 0;                             // Disable interrupts

   VDM0CN = 0x80;                      // Enable VDD monitor


   RSTSRC = 0x06;                      // Enable VDD monitor as a reset source
                                       // Leave missing clock detector enabled

   pwrite = (char xdata *) addr;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x01;                      // PSWE = 1
   
   if(SFLE)
   {
      PSCTL |= 0x04;                   // Access Scratchpad
   }

   VDM0CN = 0x80;                      // Enable VDD monitor


   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   *pwrite = byte;                     // Write the byte

   PSCTL &= ~0x05;                     // SFLE = 0; PSWE = 0

   if ((EA_Save & 0x80) != 0)          // Restore EA
   {
      EA = 1;
   }

}

//-----------------------------------------------------------------------------
// FLASH_ByteRead
//-----------------------------------------------------------------------------
//
// Return Value :
//      unsigned char - byte read from Flash
// Parameters   :
//   1) FLADDR addr - address of the byte to read to
//                    valid range is from 0x0000 to 0xFBFF for 64K devices
//                    valid range is from 0x0000 to 0x7FFF for 32K devices
//
// This routine reads a <byte> from the linear FLASH address <addr>.
//-----------------------------------------------------------------------------
U8 FLASH_ByteRead (U16 addr, U8 SFLE)
{
   U8 EA_Save = IE;                    // Preserve EA
   
   SEGMENT_VARIABLE_SEGMENT_POINTER (pread, U8, code, data); 
   //char code * data pread;             // FLASH read pointer

   unsigned char byte;

   EA = 0;                             // Disable interrupts

   pread = (char code *) addr;

   if(SFLE)
   {
      PSCTL |= 0x04;                   // Access Scratchpad
   }

   byte = *pread;                      // Read the byte

   PSCTL &= ~0x04;                     // SFLE = 0

   if ((EA_Save & 0x80) != 0)          // Restore EA
   {
      EA = 1;
   }

   return byte;

}

//-----------------------------------------------------------------------------
// FLASH_PageErase
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of any byte in the page to erase
//                    valid range is from 0x0000 to 0xFBFF for 64K devices
//                    valid range is from 0x0000 to 0x7FFF for 32K devices
//
// This routine erases the FLASH page containing the linear FLASH address
// <addr>.  Note that the page of Flash containing the Lock Byte cannot be
// erased if the Lock Byte is set.
//
//-----------------------------------------------------------------------------
void FLASH_PageErase (U16 addr, U8 SFLE)
{
   U8 EA_Save = IE;                    // Preserve EA
   
   SEGMENT_VARIABLE_SEGMENT_POINTER (pwrite, U8, xdata, data); 
   //unsigned char xdata * data pwrite;// FLASH write pointer

   EA = 0;                             // Disable interrupts

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x06;                      // Enable VDD monitor as a reset source
                                       // Leave missing clock detector enabled

   pwrite = (char xdata *) addr;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1
   
   if(SFLE)
   {
      PSCTL |= 0x04;                   // Access Scratchpad
   }

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
   *pwrite = 0;                        // Initiate page erase

   PSCTL &= ~0x07;                     // SFLE = 0; PSWE = 0; PSEE = 0

   if ((EA_Save & 0x80) != 0)          // Restore EA
   {
      EA = 1;
   }
}



//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TouchSense_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Comparator 0 and Timer 2 for use with Touchsense.
//-----------------------------------------------------------------------------
void TouchSense_Init (void)
{
   // Initialize Comparator0
   CPT0CN = 0x8F;                      // Enable Comparator0; clear flags
                                       // select maximum hysterisis
   CPT0MD = 0x0F;                      // Comparator interrupts disabled, 
                                       // lowest power mode

   CPT0MX = 0xC8;                      // Positive Mux: P2.0 - TouchSense Switch
                                       // Negative Mux: TouchSense Compare
   
   // Initialize Timer2
   CKCON |= 0x10;                      // Timer2 counts system clocks
   TMR2CN = 0x16;                      // Capture mode enabled, capture 
                                       // trigger is Comparator0. 
                                       // Start timer (TR2 = 1).

   SW20_Status = 1;                    // Set switch status to finger not 
   SW21_Status = 1;                    // detected.

}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P0.2   digital   open-drain    SW2
// P0.3   digital   open-drain    SW3
// P0.4   digital   push-pull     UART TX
// P0.5   digital   open-drain    UART RX
//
// P1.5   digital   push-pull     Red LED
// P1.6   digital   push-pull     Yellow LED
//
// P2.0   analog    open-drain    Touch Sense Switch
// P2.1   analog    open-drain    Touch Sense Switch
//

//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   
   // Configure TouchSense switches
   P2MDIN &= ~0x03;                    // P2.0, P2.1 are analog
   P2MDOUT &= ~0x03;                   // P2.0, P2.1 are open-drain
   P2      |=  0x03;                   // P2.0, P2.1 latches -> '1'
   
   // Configure Hardware Switches
   P0MDIN |= 0x0C;                     // P0.2, P0.3 are digital
   P0MDOUT &= ~0x0C;                   // P0.2, P0.3 are open-drain
   P0     |= 0x0C;                     // Set P0.2, P0.3 latches to '1'

   // Configure LEDs
   P1MDIN |= 0x60;                     // P1.5, P1.6 are digital
   P1MDOUT |= 0x60;                    // P1.5, P1.6 are push-pull

   // Configure Crossbar
   XBR2    = 0x40;                     // Enable crossbar and weak pull-ups
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
   
   OSCICN |= 0x80;                     // Enable the precision internal osc.
   
   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor enabled.

   CLKSEL = 0x00;                      // Select precision internal osc. 
                                       // divided by 1 as the system clock
   
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------