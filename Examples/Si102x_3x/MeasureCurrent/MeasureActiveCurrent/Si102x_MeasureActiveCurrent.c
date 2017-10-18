//-----------------------------------------------------------------------------
// Si102x_MeasureActiveCurrent.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This example code demonstrates the reduction in active mode current
// when powering the MCU off the DCDC.
//
// To Use with the UPMP-F960-MLCD development board:
//
// *  Cut the Trace on the bottom of the PCB between the two pins on the VBAT
//    CURRENT header J15.
//
// *  Cut the Trace on the bottom of the PCB between the two pins of header
//    J20.
//
// *  Connect a current meter between the two pins on header J15.
//
// *  Set the VBAT Selector SW5 to the desired power source.
//
// *  Set VIO Select (SW7) and VIORF(SW12) Select to VBAT.
//
// *  Install a shorting block on J16 between P0.3 and LED/SW4 to enable SW4.
//
// *  Build and download the code.
//
// *  Run.
//
// *  Press any of the four buttons to select the desired power
//    configuration.
//
// *  Disconnect the USB debug connector for an accurate measurement.
//
// The four power-management configurations are listed below:
//
// *  SW1 - Precision Oscillator - 24.5 MHz, DCDC disabled
//
// *  SW2 - Precision Oscillator - 24.5 MHz, DCDC enabled
//
// *  SW3 - Low-Power Oscillator - 20 MHz, DCDC disabled
//
// *  SW4 - Low-Power Oscillator - 20 MHz, DCDC enabled
//
// Notes:
//
// The MCU is executing a typical instruction loop located at a mod 4
// boundary for repeatable active current measurements.
//
// The VDD monitors are disabled.
//
// The default Reset condition is the same as SW1.
//
// The code uses a Port Match Interrupt to change modes. Pressing any
// button will cause a port match interrupt. The port is then read to
// determine which button was pressed. The ISR calls functions to switch
// the oscillator and DCDC configuration.
//
//
// Target:         C8051F960 Muxed LCD Board
// Tool chain:     Keil Only
// Command Line:   None
//
// Release 1.0
//    - Initial Revision (MRF)
//    - 28 OCTOBER 2011
//
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1020_defs.h>               // SFR declarations

//-----------------------------------------------------------------------------
// Defined code loop for measurement below.
//-----------------------------------------------------------------------------
#define USE_TYPICAL_LOOP
//#define USE_LOW_CURRENT_LOOP
//#define USE_HIGH_CURRENT_LOOP
//#define USE_LONG_NOP_LOOP

//-----------------------------------------------------------------------------
// asm file Function Prototypes
//-----------------------------------------------------------------------------
void TypicalActiveLoop(void);
void LowCurrentActiveLoop(void);
void HighCurrentActiveLoop(void);
void LongNOPActiveLoop(void);

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void InitPrecisionBuckOff (void);
void InitPrecisionBuckOn (void);
void InitLowPowerBuckOff (void);
void InitLowPowerBuckOn (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   // Disable watchdog timer
   SFRPAGE = LEGACY_PAGE;
   PCA0MD = 0x00;
   VDM0CN = 0x00;                      // disable both VDD monitors

   SFRPAGE  =  DPPE_PAGE;
   DC0MD = 0x48;                       // disable DCDC and force bypass

   SFRPAGE = LEGACY_PAGE;
   OSCICN |= 0x80;
   CLKSEL    = 0x00;

   P0MASK = 0x0F;                      // Mask out all pins except P0.0-P0.3
   P0MAT = 0x0F;                       // P0.0-0.3 normally high

   EIE2 |= 0x02;                       // Enable Port Match interrupt
   EA = 1;

#ifdef USE_TYPICAL_LOOP
   TypicalActiveLoop();
#elif defined USE_LOW_CURRENT_LOOP
   LowCurrentActiveLoop();
#elif defined USE_HIGH_CURRENT_LOOP
   HighCurrentActiveLoop();
#elif defined USE_LONG_NOP_LOOP
   LongNOPActiveLoop();
#else
   TypicalActiveLoop();
#endif
}

//-----------------------------------------------------------------------------
// PortMatch_ISR()
//
// This interrupt service routine is called on any button press. This function
// calls different functions depending on which button was pressed.
//
// Note that this code is specific to the F960 MCU card with Muliplexed LCD.
// The F960 MCU card with EMIF is not supported.
//
//-----------------------------------------------------------------------------
INTERRUPT(PortMatch_ISR, INTERRUPT_PORT_MATCH)
{
   U8 button;

   SFRPAGE = LEGACY_PAGE;

   button = ~P0 & 0x0F;

   switch (button)
   {
      case 0x01:
         InitPrecisionBuckOff();
         break;

      case 0x02:
         InitPrecisionBuckOn();
         break;

      case 0x04:
         InitLowPowerBuckOff();
         break;

      case 0x08:
         InitLowPowerBuckOn();
         break;

      default:
         break;
   }
}

//-----------------------------------------------------------------------------
// InitPrecisionBuckOff()
//
// Called on SW1 press.
//-----------------------------------------------------------------------------
void InitPrecisionBuckOff (void)
{
   // If VBAT switch enabled, disable it and disable DCDC.
   SFRPAGE = LEGACY_PAGE;
   if((PCON & 0x04) == 0x04)
   {
      PCON &= ~0x04;                   // Disable VBAT switch
      SFRPAGE  =  DPPE_PAGE;
      DC0MD = 0x48;                    // Disable DCDC and enable force bypass
      LCD0MSCN &= ~0x20;               // Disable DCBIAS in LCD0
   }

   // If Precision osc not enabled, enable it.
   SFRPAGE = LEGACY_PAGE;
   if((OSCICN & 0x80) == 0x00)
   {
      OSCICN |= 0x80;                  // Enable precision oscillator
      CLKSEL  = 0x00;                  // Switch SYSCLK to precision oscillator
   }
}

//-----------------------------------------------------------------------------
// InitPrecisionBuckOn()
//
// Called on SW2 press.
//-----------------------------------------------------------------------------
void InitPrecisionBuckOn (void)
{
   SFRPAGE = LEGACY_PAGE;

   // If precision osc not enabled, enable it.
   SFRPAGE = LEGACY_PAGE;
   if((OSCICN & 0x80) == 0x00)
   {
      OSCICN |= 0x80;                  // Enable precision oscillator
      CLKSEL  = 0x00;                  // Switch SYSCLK to precision oscillator
   }

   // If VBAT switch is disabled, enable DCDC and VBAT switch.
   SFRPAGE = LEGACY_PAGE;
   if((PCON & 0x04) == 0x00)
   {
      SFRPAGE  =  LCD0_PAGE;
      LCD0MSCN |= 0x20;                // Enable DCDC bias
      LCD0PWR &= ~0x08;                // Clear LCD mode bit when usign DCDC

      SFRPAGE  =  DPPE_PAGE;
      DC0CN = 0x03;                    // 40 ns pulse
      DC0CF = 0x0B;                    // 1.9 V max switch
      DC0MD = 0x043;                   // enable DCDC

      // wait on DCDC above low threshold.
      while((DC0RDY & 0x40)!=0x40);

      PCON |= 0x04;                    // enable VBAT switch
     
      // wait on DCDC below high threshold.
      while((DC0RDY & 0x80)!=0x00);
 
   }
}

//-----------------------------------------------------------------------------
// InitLowPowerBuckOff()
//
// Called on SW3 press.
//-----------------------------------------------------------------------------
void InitLowPowerBuckOff (void)
{
   SFRPAGE = LEGACY_PAGE;

   // If VBAT switch enabled, disable it and disable DCDC.
   if((PCON & 0x04) == 0x04)
   {
      PCON &= ~0x04;                   // Disable VBAT switch
      SFRPAGE  =  DPPE_PAGE;
      DC0MD = 0x48;                    // Disable DCDC and enable force bypass
      LCD0MSCN &= ~0x20;               // Disable DCBIAS in LCD0
   }

   // If Precision osc enabled, switch to LP osc.
   SFRPAGE = LEGACY_PAGE;
   if((OSCICN & 0x80) == 0x80)
   {
      CLKSEL  = 0x04;                  // Switch SYSCLK to low-power oscillator
      OSCICN &= ~0x80;                 // Disable Precision oscillator
      REG0CN  = 0x00;                  // Disable oscillator Bias
   }
}

//-----------------------------------------------------------------------------
// InitPrecisionBuckOn()
//
// Called on SW4 press.
//-----------------------------------------------------------------------------
void InitLowPowerBuckOn (void)
{
   SFRPAGE = LEGACY_PAGE;

   // If Precision osc enabled, switch to LP osc.
   SFRPAGE = LEGACY_PAGE;
   if((OSCICN & 0x80) == 0x80)
   {
      CLKSEL  = 0x04;                  // Switch SYSCLK to low-power oscillator
      OSCICN &= ~0x80;                 // Disable Precision oscillator
      REG0CN  = 0x00;                  // Disable oscillator Bias
   }

   // If VBAT switch is disabled, enable DCDC and VBAT switch.
   SFRPAGE = LEGACY_PAGE;
   if((PCON & 0x04) == 0x00)
   {
      SFRPAGE  =  LCD0_PAGE;
      LCD0MSCN |= 0x20;                // enable DCDC bias
      LCD0PWR &= ~0x08;                // force bandgap on continuously

      SFRPAGE  =  DPPE_PAGE;
      DC0CN = 0x03;                    // 40 ns pulse
      DC0CF = 0x0B;                    // 1.9 V max switch
      DC0MD = 0x043;                   // enable DCDC

      // wait on DCDC above low threshold.
      while((DC0RDY & 0x40)!=0x40);

      PCON |= 0x04;                    // enable VBAT switch
     
      // wait on DCDC below high threshold.
      while((DC0RDY & 0x80)!=0x00);
   }
}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------