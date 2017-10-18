//=============================================================================
// Si102x_TxTone.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//
// This is a simple single file example for generating a TX Tone. This
// example provides run time support for setting the TX frequency. The default
// frequency is 915 MHz. But this can be changed by setting on function
// parameter.


//
// The RadioInit() function provides debug support to assist in debugging the
// radio. The radio SDN pin should be connected to P1.7 on the MCU. The IRQ pin
// should be connected to P1.6. Optional code may be included to manage the
// radio with SDN shorted to ground.
//
// To use, compile and download firmware to Si102x/3x. Make sure the SDN pin of
// of the radio is connected to P1.7 of the MCU. If the radio transmitter
// starts successfully, the LED on P0.1 will blink. Please also make sure
// the LED is connected to P0.1.
//
// Connect a spectrum analyzer to the TX output and measure the RF amplitude.
// Set the center frequency to 915 MHz, set the reference level to +20 dBm, and
// set the BW to 10 MHz per division. The spectrum analyzer settings will have
// to be adjusted after changing the spectrum parameters.
//
// Target:
//    Si102x/3x
//
// IDE:
//    Silicon Laboratories IDE
//
// Tool Chains:
//    Keil
//    SDCC
//    Raisonance
//
// Project Name:
//    Si102x/3x RF Code Examples
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1020_defs.h>

//-----------------------------------------------------------------------------
// Define SYSCLK using LP oscillator
//-----------------------------------------------------------------------------
#define SYSCLK_HZ       (10000000L)

//-----------------------------------------------------------------------------
// Defines used for delay and time out
//-----------------------------------------------------------------------------
#define ONE_MS          (SYSCLK_HZ/1000/48)
#define TWO_MS          (SYSCLK_HZ/500/48)
#define FIVE_MS         (SYSCLK_HZ/200/48)
#define TEN_MS          (SYSCLK_HZ/100/48)
#define TWENTY_MS       (SYSCLK_HZ/50/48)
#define TWENTY_FIVE_MS  (SYSCLK_HZ/40/48)

//-----------------------------------------------------------------------------
// Expected RADIO_VERSION code for radio VERSION register
// Production version should be 0x06
//-----------------------------------------------------------------------------
#define MIN_RADIO_VERSION   0x05

//-----------------------------------------------------------------------------
// RadioInit() error codes
//-----------------------------------------------------------------------------
#define RADIO_INIT_SUCCESS             0
#define RADIO_INIT_ERROR_NO_IRQ        1
#define RADIO_INIT_ERROR_SPI           2
#define RADIO_INIT_ERROR_VERSION       3
#define RADIO_INIT_ERROR_XTAL          4

//-----------------------------------------------------------------------------
// ET0 inverted and used as timeout flag
//-----------------------------------------------------------------------------
#define TIMEOUT_T0   (!ET0)

//-----------------------------------------------------------------------------
// Modulation control 2 mask for FSK and GFSK
//-----------------------------------------------------------------------------
#define FSK_MODULATION                   0x32
#define GFSK_MODULATION                  0x33

//-----------------------------------------------------------------------------
// Choose FSK or GFSK here.
//-----------------------------------------------------------------------------
#define MODULATION_SELECT                GFSK_MODULATION

//-----------------------------------------------------------------------------
// RF Switch enabled by default
//   Needed for +20 dBm parts only.
//   But does not cuase any problem with +13 dBm parts.
//-----------------------------------------------------------------------------
#define  ENABLE_RF_SWITCH

//-----------------------------------------------------------------------------
// bit definitions
//-----------------------------------------------------------------------------
SBIT(NSS1, SFR_P2, 3);
SBIT(IRQ, SFR_P1, 6);
SBIT(SDN, SFR_P1, 7);
SBIT(LED1, SFR_P0, 0);

//-----------------------------------------------------------------------------
// Radio Status located for POR debug
//-----------------------------------------------------------------------------
LOCATED_VARIABLE_NO_INIT(RadioStatus, U8, SEG_DATA, 0x43);

//-----------------------------------------------------------------------------
// Internal Function Prototypes
//-----------------------------------------------------------------------------
void  Port_Init (void);
void  SYSCLK_Init (void);
void  SPI1_Init (void);
void  delay (U16);
void  SetTimeOut (U16);
void  ClearTimeOut (void);
void  phyWrite (U8, U8);
U8    phyRead (U8);
U8    RadioInit(void);
U8    RadioInitSoftwareReset(void);
void  setTxPower(U8);
void  setTRxFrequency (U32);
U32   divideWithRounding (U32, U32);



//=============================================================================
// Functions
//=============================================================================
//-----------------------------------------------------------------------------
// main ()
//-----------------------------------------------------------------------------
void main (void)
{
   U8 status;

   PCA0MD   &= ~0x40;                  // disable  watchdog

   Port_Init ();
   SYSCLK_Init ();
   SPI1_Init ();

   RadioStatus = RadioInit();

   // Check status error code using debug trap.
   while(RadioStatus);

   // Change parameter to set tone frequency.
   // Set according to hardware design.
   setTRxFrequency (915000000L);

   // Change parameter to set tone power.
   // Actual power depends on device PA rating.
   setTxPower(7);                      // Full power







   // Set modulation control register for tone.



   phyWrite(EZRADIOPRO_MODULATION_MODE_CONTROL_2, 0x00);

#ifdef ENABLE_RF_SWITCH
   // RF Switch enabled by default
   // Needed for +20 dBm parts only.
   // set GPIO1 & GPIO2 to control the TRX switch
   phyWrite(EZRADIOPRO_GPIO1_CONFIGURATION, 0x12);
   phyWrite(EZRADIOPRO_GPIO2_CONFIGURATION, 0x15);
#endif

   // enable transmitter
   phyWrite(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,(EZRADIOPRO_TXON|EZRADIOPRO_XTON));

   delay(TWO_MS);                     // Wait 2 ms

   status = phyRead(EZRADIOPRO_DEVICE_STATUS);

   // Check status for transmit mode.
   if((status& 0x03)==0x02)
   {
      // Blink forever.
      while(1)
      {
         LED1 = !LED1;
         delay(TWENTY_MS);
         delay(TWENTY_MS);
         delay(TWENTY_MS);
         delay(TWENTY_MS);
         delay(TWENTY_MS);
      }
   }

   while(1);
}

//=============================================================================
//
// Init Functions
//
//=============================================================================
//-----------------------------------------------------------------------------
// Port_Init ()
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   U8 restoreSFRPAGE;   
   restoreSFRPAGE = SFRPAGE;   
   
   // Init UART
   SFRPAGE  = LEGACY_PAGE;
   XBR0    |= 0x01;                    // enable UART
   P0MDOUT |= 0x10;                    // TX push-pull
   SFRPAGE  = CONFIG_PAGE;
   P0DRV    = 0x10;                    // TX high-current mode

   // Init SPI
   SFRPAGE   = LEGACY_PAGE;
   XBR1    |= 0x40;                    // Enable SPI1 (3 wire mode)
   P2MDOUT |= 0x0D;                    // SCK1, MOSI1, & NSS1,push-pull
   SFRPAGE  = CONFIG_PAGE;
   P2DRV    = 0x0D;                    // MOSI1, SCK1, NSS1, high-drive mode

   // Initialize SDN
   SFRPAGE  = LEGACY_PAGE;
   P1MDOUT |= 0x80;                    // SDN P1.7 push-pull
   SFRPAGE  = CONFIG_PAGE;
   P1DRV   |= 0x80;                    // SDN P1.7 high-drive mode
   SFRPAGE  = LEGACY_PAGE;
   SDN      = 0;

   P2       = 0xFF;                    // P2 bug fix for SDCC and Raisonance

   XBR2    |= 0x40;                    // enable Crossbar

   IT01CF   = 0x06;                    // INT0 is assigned to P1.6
   IE0      = 0;
   IT0      = 0;
   EX0      = 1;
  
   SFRPAGE  = restoreSFRPAGE;
}

//-----------------------------------------------------------------------------
// SYSCLK_Init ()
//
// Return Value : None
// Parameters   : None
//
// The 10 MHz low power oscillator is used for this example.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   U8 restoreSFRPAGE;   
   restoreSFRPAGE = SFRPAGE;   

   SFRPAGE   = LEGACY_PAGE;

   CLKSEL    = 0x14;                   // use 10 MHz clock
   
   SFRPAGE  = restoreSFRPAGE;
}

//-----------------------------------------------------------------------------
// SPI1_Init ()
//-----------------------------------------------------------------------------
void SPI1_Init()
{
   U8 restoreSFRPAGE;   
   restoreSFRPAGE = SFRPAGE;   

   SFRPAGE   = SPI1_PAGE;

   SPI1CFG  |= 0x40;                   // master mode
   SPI1CN    = 0x00;                   // 3 wire master mode
   SPI1CKR   = 0x00;                   // Use SYSWCLK/2r
   SPI1CN   |= 0x01;                   // enable SPI
   NSS1      = 1;                      // set NSS high

   SFRPAGE  = restoreSFRPAGE;
}

//=============================================================================
//
// SPI Functions for PHY write and read primitives
//
//=============================================================================
//-----------------------------------------------------------------------------
// phyWrite()
//-----------------------------------------------------------------------------
void phyWrite (U8 reg, U8 value)
{
   U8 restoreSFRPAGE;
   restoreSFRPAGE = SFRPAGE;
   
   SFRPAGE = SPI1_PAGE;
   // Send SPI data using double buffered write
   NSS1 = 0;                           // drive NSS low
   SPIF1 = 0;                          // clear SPIF
   SPI1DAT = (reg | 0x80);             // write reg address
   while(!TXBMT1);                     // wait on TXBMT
   SPI1DAT = value;                    // write value
   while(!TXBMT1);                     // wait on TXBMT
   while((SPI1CFG & 0x80) == 0x80);    // wait on SPIBSY

   SPIF1 = 0;                          // leave SPIF cleared
   NSS1 = 1;                           // drive NSS high
   
   SFRPAGE = restoreSFRPAGE;
}

//-----------------------------------------------------------------------------
// phyRead()
//-----------------------------------------------------------------------------
U8 phyRead (U8 reg)
{
   U8 value;
   U8 restoreSFRPAGE;
   restoreSFRPAGE = SFRPAGE;
   
   SFRPAGE = SPI1_PAGE;
   // Send SPI data using double buffered write
   NSS1 = 0;                           // dsrive NSS low
   SPIF1 = 0;                          // cleat SPIF
   SPI1DAT = ( reg );                  // write reg address
   while(!TXBMT1);                     // wait on TXBMT
   SPI1DAT = 0x00;                     // write anything
   while(!TXBMT1);                     // wait on TXBMT
   while((SPI1CFG & 0x80) == 0x80);    // wait on SPIBSY
   value = SPI1DAT;                    // read value
   SPIF1 = 0;                          // leave SPIF cleared
   NSS1 = 1;                           // drive NSS low

   SFRPAGE = restoreSFRPAGE;
   
   return value;
}

//-----------------------------------------------------------------------------
// Function Name
//    RadioInit()
//
// Parameters   : none
// Return Value : status
//
// 0  RADIO_INIT_SUCCESS         - Radio in Idle mode with Xtal running
// 1  RADIO_INIT_ERROR_NO_IRQ    - IRQ pin not connected to P0.1
// 2  RADIO_INIT_ERROR_SPI       - SPI not communicating
// 3  RADIO_INIT_ERROR_VERSION   - Radio version unsupported
// 4  RADIO_INIT_ERROR_XTAL      - Radio XTAL not running, check XTAL
//
// After a pin reset or other reset the state of the Radio is unknown.
// The MCU will wait for full POR duration, then figure out if the
// Radio needs a software reset.
//
//-----------------------------------------------------------------------------
U8 RadioInit(void)
{
   U8 status;

   SDN = 0;

   delay(TWENTY_FIVE_MS);

   status = phyRead(EZRADIOPRO_DEVICE_VERSION); // check version
   if(status == 0xFF)
      return RADIO_INIT_ERROR_SPI;
   else if (status == 0x00)
      return  RADIO_INIT_ERROR_SPI;
   else if (status < MIN_RADIO_VERSION)
      return RADIO_INIT_ERROR_VERSION;

   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   if((status & EZRADIOPRO_IPOR)==0)
   {
      // radio needs a software reset
      return RadioInitSoftwareReset();
   }
   else if((status & EZRADIOPRO_ICHIPRDY)==0)
   {
      // disable POR interrupt
      phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, EZRADIOPRO_ENCHIPRDY);

      // wait on IRQ with 2 MS timeout
      SetTimeOut(TWO_MS);
      while(IRQ)
      {
         if(TIMEOUT_T0)
            return RADIO_INIT_ERROR_XTAL;
      }
      ClearTimeOut();
   }
   return RADIO_INIT_SUCCESS; // success
}

//-----------------------------------------------------------------------------
// Function Name
//    RadioInitSWReset()
//
// Parameters   : none
// Return Value : status
//
// 0  RADIO_INIT_SUCCESS         - Radio in Idle mode with Xtal running
// 1  RADIO_INIT_ERROR_NO_IRQ    - IRQ pin not connected to P0.1
// 2  RADIO_INIT_ERROR_SPI       - SPI not communicating
// 3  RADIO_INIT_ERROR_VERSION   - Radio version unsupported
// 4  RADIO_INIT_ERROR_XTAL      - Radio XTAL not running, check XTAL
//
// This function uses a software reset to reset the radio. This can be used
// to reset the radio when a radio POR has not occurred.
// A T0 interrupt timeout is used to minimize start-up time.
//-----------------------------------------------------------------------------
U8 RadioInitSoftwareReset(void)
{
   U8 status;

   // Clear interrupt enable and interrupt flag bits
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, 0);
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, 0);
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_1);
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   // SWReset
   phyWrite(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1, (EZRADIOPRO_SWRES|EZRADIOPRO_XTON));

   // wait on IRQ with 2 MS timeout
   SetTimeOut(TWO_MS);
   while(IRQ)
   {
      if(TIMEOUT_T0)
         return RADIO_INIT_ERROR_NO_IRQ;
   }
   ClearTimeOut();

   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   if((status & EZRADIOPRO_ICHIPRDY)==0)
   {
      // disable POR interrupt
      phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, EZRADIOPRO_ENCHIPRDY);

      // wait on IRQ with 2 MS timeout
      SetTimeOut(TWO_MS);
      while(IRQ)
      {
         if(TIMEOUT_T0)
            return RADIO_INIT_ERROR_XTAL;
      }
      ClearTimeOut();
   }

   return RADIO_INIT_SUCCESS; // success
}

//-----------------------------------------------------------------------------
// delay ()
//
// Parameters   : ticks
// Return Value : none
//
// This is simple delay using T0. This function will block until the delay has
// expired.
//-----------------------------------------------------------------------------
void delay (U16 ticks)
{
   UU16 reload;

   reload.U16 = -ticks;

   TR0 = 0;
   TF0 = 0;

   TMOD      = 0x01;    // T0 uses prescaler
   CKCON     = 0x02;    // divide by 48 prescaler

   TL0     = reload.U8[LSB];
   TH0     = reload.U8[MSB];

   TR0 = 1;
   while (!TF0);
   TR0 = 0;
   TF0 = 0;
}

//-----------------------------------------------------------------------------
// SetTimeOut ()
//
// Parameters   : ticks
// Return Value : none
//
// This function will set the Timeout value using T0 and enable the T0
// interrupt. The interrupt enable is used a timeout flag.
//
//-----------------------------------------------------------------------------
void SetTimeOut (U16 ticks)
{
   UU16 reload;

   reload.U16 = -ticks;

   TR0 = 0;
   TF0 = 0;

   TMOD      = 0x01;    // T0 uses prescaler
   CKCON     = 0x02;    // divide by 48 prescaler

   TL0     = reload.U8[LSB];
   TH0     = reload.U8[MSB];

   TR0 = 1;
   ET0 = 1;
}

//-----------------------------------------------------------------------------
// T0_ISR()
//
// Return Value : None
// Parameters   : none
//
//
// T0_ISR() is the Timer 0 interrupt service routine. This is used with
// the timeout. The interrupt service routine clears the interrupt enable
// to signal a timeout has occurred and prevent additional interrupts.
//-----------------------------------------------------------------------------
INTERRUPT(T0_ISR, INTERRUPT_TIMER0)
{
   ET0 = 0;
   TF0 = 0;
   TR0 = 0;
}

//-----------------------------------------------------------------------------
// ClearTimeOut()
//
// Return Value : None
// Parameters   : none
//
// ClearTimeOut() is used to cancel a TimeOut.
//-----------------------------------------------------------------------------
void ClearTimeOut (void)
{
   ET0 = 0;
   TF0 = 0;
   TR0 = 0;
}

//-----------------------------------------------------------------------------
// setTRxFrequency()
//
// Parameters   : frequency
// Return Value : None
//
// setTRxFrequency() will calculate the settings for the nominal carrier
// frequency registers and write to the radio registers.
//-----------------------------------------------------------------------------
void   setTRxFrequency (U32 frequency)
{
   U8 frequencyBandSelect;
   UU16 nominalCarrierFrequency;

   if (frequency > 480000000L )
   {
      frequency -= 480000000L;
      frequencyBandSelect  = frequency / 20000000L;
      frequency -= (U32)frequencyBandSelect * 20000000L;
      frequency  = divideWithRounding(frequency, 625);
      frequency <<= 1;
      frequencyBandSelect |= 0x20;
   }
   else
   {
      frequency -= 240000000L;
      frequencyBandSelect  = frequency / 10000000L;
      frequency -= (U32)frequencyBandSelect * 10000000L;
      frequency  = divideWithRounding(frequency, 625);
      frequency <<= 2;
   }

   frequencyBandSelect |= 0x40;        // set sbsel

   nominalCarrierFrequency.U16 = (U16)frequency;

   phyWrite(EZRADIOPRO_FREQUENCY_BAND_SELECT, frequencyBandSelect);
   phyWrite(EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_1, nominalCarrierFrequency.U8[MSB]);
   phyWrite(EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_0, nominalCarrierFrequency.U8[LSB]);

}

//-----------------------------------------------------------------------------
// divideWithRounding()
//
// Parameters   : value (Numerator), Divisor
// Return Value : Resultant
//
// This function will divide 32-bit unsigned numbers with rounding. Half of
// the divisor is added to the numerator before dividing.
//-----------------------------------------------------------------------------
U32   divideWithRounding (U32 value, U32 divisor)
{
   value += (divisor >> 1);
   value /= divisor;
   return value;
}

//-----------------------------------------------------------------------------
// setTxPower ()
//
// Parameters   : value (Numerator), Divisor
// Return Value : Resultant
//
// This function with set the TX power register. Bit 3 and 4 are set
// by default.
//-----------------------------------------------------------------------------
void setTxPower (U8 power)
{
   phyWrite(EZRADIOPRO_TX_POWER, 0x18|power); // set bit 3 and 4
}




































































//=============================================================================
