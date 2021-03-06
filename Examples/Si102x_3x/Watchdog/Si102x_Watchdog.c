//-----------------------------------------------------------------------------
// Si102x_Watchdog.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program helps the user to learn about operating the Watch Dog Timer.
// The WDT is used to generate resets if the times between writes to the WDT
// update register (PCA0CPH5) exceed a specified limit. The WDT can be disabled
// and enabled in the software as needed. When enabled the PCA Module 2 acts as
// the WDT. This program resets the MCU when P0.2 switch is pressed. Also upon
// reset the LED blinks approximately five times faster when compared to before.
// The reset is caused due to a WDT overflow and can be confirmed by checking
// the value of the RSTSRC register where bit 3 is set to indicate a reset
// caused by WDT.
//
// How to Test:
// 1) Compile and download code to a device.
// 2) Run the code:
//        - The test will blink LED2 at a rate of 2 Hz until switch SW1
//          is pressed.
//        - Once the the switch is pressed and held for a long enough time,
//          it will prevent the WDT from being touched and the WDT will
//          cause a reset.
//        - Upon reset the code checks for a WDT reset and blinks the LED five
//          times faster than before to indicate the same.
//
// Target:         Si102x/3x
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    - Initial Revision (MRF)
//    - 29 SEPTEMBER 2011
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <Si1020_defs.h>               // SFR declarations
//-----------------------------------------------------------------------------
// Define Hardware
//-----------------------------------------------------------------------------
#define UDP_F960_MCU_MUX_LCD
//#define UDP_F960_MCU_EMIF
//-----------------------------------------------------------------------------
// Hardware Dependent definitions
//-----------------------------------------------------------------------------
#ifdef UDP_F960_MCU_MUX_LCD
SBIT (SW1, SFR_P0, 0);
SBIT (LED2, SFR_P0, 1);
#endif

#ifdef UDP_F960_MCU_EMIF
SBIT (SW1, SFR_P3, 0);
SBIT (LED2, SFR_P3, 1);
#endif
//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       (20000000L / 8)      // SYSCLK frequency in Hz

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
LOCATED_VARIABLE_NO_INIT (reserved, U8, SEG_XDATA, 0x0000);

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void PCA_Init (void);
void Timer2_Init (int counts);
INTERRUPT_PROTO (TIMER2_ISR, INTERRUPT_TIMER2);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
//
// The MAIN routine performs all the intialization, and then loops until the
// switch is pressed. When SW2 (P0.7) is pressed the code checks the RSTSRC
// register to make sure if the last reset is because of WDT.
//-----------------------------------------------------------------------------
void main(void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   OSCILLATOR_Init();                  // Initialize system clock to 24.5/8 MHz
   PCA_Init();                         // Intialize the PCA
   PORT_Init();                        // Initialize crossbar and GPIO

   if((RSTSRC & 0x02) == 0x00)         // First check the PORSF bit. if PORSF
   {                                   // is set, all other RSTSRC flags are
                                       // invalid
      // Check if the last reset was due to the Watch Dog Timer
      if(RSTSRC == 0x08)
      {
         Timer2_Init(SYSCLK / 12 / 20);// Make LED blink at 20Hz
         EA = 1;                       // Enable global interrupts

         while(1);                     // wait forever
      }
      else
      {
         // Init Timer2 to generate interrupts at a 4 Hz rate.
         Timer2_Init(SYSCLK / 12 / 4);
      }

   }
   // Calculate Watchdog Timer Timeout
   // Offset calculated in PCA clocks
   // Offset = ( 256 x PCA0CPL5 ) + 256 - PCA0L
   //        = ( 256 x 255(0xFF)) + 256 - 0
   // Time   = Offset * (12/SYSCLK)
   //        = 255 ms ( PCA uses SYSCLK/12 as its clock source)

   PCA0MD  &= ~0x40;                   // WDTE = 0 (clear watchdog timer
                                       // enable)
   PCA0L    = 0x00;                    // Set lower byte of PCA counter to 0
   PCA0H    = 0x00;                    // Set higher byte of PCA counter to 0
   PCA0CPL5 = 0xFF;                    // Write offset for the WDT
   PCA0MD  |= 0x40;                    // enable the WDT
   EA = 1;                             // enable global interrupts

   //--------------------------------------------------------------------------
   // Main Application Loop/Task Scheduler
   //--------------------------------------------------------------------------

   while(1)
   {
       //----------------------------------------------------------------------
       // Task #1 - Check Port I/O
       //----------------------------------------------------------------------
       while(!SW1);                    // Force the MCU to stay in this task as
                                       // long as SW1 is pressed. This task must
                                       // finish before the watchdog timer
                                       // timeout expires.

       //----------------------------------------------------------------------
       // Task #2 - Reset Watchdog Timer
       PCA0CPH5 = 0x05;                // Write a 'dummy' value to the PCA0CPH2
                                       // register to reset the watchdog timer
                                       // timeout. If a delay longer than the
                                       // watchdog timer delay occurs between
                                       // successive writes to this register,
                                       // the device will be reset by the watch
                                       // dog timer.
    }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
// Return Value : None
// Parameters   : None
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init()
{
   SFRPAGE = LEGACY_PAGE;

   CLKSEL = 0x34;                      // Select Low-Power OSC/8 as SYSCLK
}


//-----------------------------------------------------------------------------
// PCA_Init
//-----------------------------------------------------------------------------
// Return Value : None
// Parameters   : None
//
// This routine initializes the PCA to use the SYSCLK / 12
// as its clock source.  It also sets the offset value by writing to PCA0CPL5.
//-----------------------------------------------------------------------------
void PCA_Init()
{
   SFRPAGE = LEGACY_PAGE;    
   PCA0CN     =  0x40;                 // PCA counter enable
   PCA0MD    &= ~0x40 ;                // Watchdog timer disabled-clearing bit 6
   PCA0MD    &=  0xF1;                 // timebase selected - System clock / 12
   PCA0CPL2   =  0xFF;                 // Offset value
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
// Return Value : None
// Parameters   : None
//
// This function configures the Crossbar and GPIO ports.
//-----------------------------------------------------------------------------
void PORT_Init()
{
   SFRPAGE = LEGACY_PAGE;    

   XBR0     = 0x00;                    // no digital peripherals selected
   XBR2     = 0x40;                    // Enable crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
// Return Value : None
// Parameters   :
//   1)  int counts - calculated Timer overflow rate
//                    range is positive range of integer: 0 to 32767
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//-----------------------------------------------------------------------------
void Timer2_Init(int counts)
{
   SFRPAGE = LEGACY_PAGE;    
 
   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;
                                       // use SYSCLK/12 as timebase
   CKCON  &= ~0x30;                    // Timer2 clocked based on T2XCLK;
   TMR2RL  = -counts;                  // Init reload values
   TMR2    = 0xffff;                   // set to reload immediately
   ET2     = 1;                        // enable Timer2 interrupts
   TR2     = 1;                        // start Timer2
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer2 overflows.
//-----------------------------------------------------------------------------
INTERRUPT(TIMER2_ISR, INTERRUPT_TIMER2)
{
   SFRPAGE = LEGACY_PAGE;    

   TF2H = 0;                           // clear Timer2 interrupt flag
   LED2 = !LED2;                       // change state of LED
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
