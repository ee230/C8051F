//=============================================================================
// Si100x_TxSpectrum.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//
// This is a simple single file example for generating an FSK or GFSK spectrum.
// This example provides run time support for setting the TX frequency,
// deviation, and data rate. Any desired FSK or GFSK spectrum can be generated
// by changing the respective parameters.
//
// The RadioInit() function provides debug support to assist in debugging the
// radio. The Radio SDN pin should be connected to P2.6 or the MCU. The IRQ pin
// should be connected to P0.1. Optional code may be included to manage the radio
// with SDN shorted to ground.
//
// To use, compile and download firmware to Si1000. Make sure the SDN pin of
// of the radio is connected to P2.6 of the MCU. If the Radio transmitter
// starts successfully, the LED on P1.6 will blink. Please also make sure
// the LED is connected to P1.6.
//
// Connect a spectrum analyzer to the TX output and measure the RF amplitude.
// Set the center frequency to 915 MHz, set the reference level to +20 dB, and
// set the BW to 10 MHz per division. The spectrum analyzer settings will have
// to be adjusted after changing the spectrum parameters.
//
// Target:
//    si100x
//
// IDE:
//    Silicon Laboratories IDE   version 3.9
//
// Tool Chains:
//    Keil
//    SDCC
//    Raisonance
//
// Project Name:
//    Si1000 RF Code Examples
//
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1000_defs.h>
//-----------------------------------------------------------------------------
// Define si100x variant here
//
// SI1000_DAUGHTER_CARD - Si1000 and Si1001
// SI1002_DAUGHTER_CARD - Si1002 and Si1003
// SI1004_DAUGHTER_CARD - Si1004 and Si1005
//
//-----------------------------------------------------------------------------
#define SI1000_DAUGHTER_CARD
//#define SI1002_DAUGHTER_CARD
//#define SI1004_DAUGHTER_CARD
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
//   Needed for +20 daughtercards only.
//   But does not cuase any problem with +13 dB boards.
//-----------------------------------------------------------------------------
#ifdef   SI1000_DAUGHTER_CARD
#define  ENABLE_RF_SWITCH
#endif
//-----------------------------------------------------------------------------
// MCM internal bit definitions
//-----------------------------------------------------------------------------
SBIT(NSS1, SFR_P1, 4);
//-----------------------------------------------------------------------------
// SI1000 Daughter Card Connections
//-----------------------------------------------------------------------------
SBIT(IRQ, SFR_P0,1);
//-----------------------------------------------------------------------------
// SDN macros for Si100x variants Si1000/1/2/3/4/5
//-----------------------------------------------------------------------------
#ifdef   SI1000_DAUGHTER_CARD
SBIT(SDN, SFR_P2,6);                 // Si1000/1
#endif
#ifdef   SI1002_DAUGHTER_CARD
SBIT(SDN, SFR_P2,6);                 // Si1002/3
#endif
#ifdef   SI1004_DAUGHTER_CARD
SBIT(SDN, SFR_P1,6);                 // Si1004/5
#endif
//-----------------------------------------------------------------------------
// LED for target board
//-----------------------------------------------------------------------------
SBIT(LED, SFR_P1,5);
//-----------------------------------------------------------------------------
// Radio Status located for POR debug
//-----------------------------------------------------------------------------
LOCATED_VARIABLE_NO_INIT(RadioStatus, U8, SEG_DATA, 0x43);
//-----------------------------------------------------------------------------
// Internal Function Prototypes
//-----------------------------------------------------------------------------
void  PortInit (void);
void  SYSCLK_Init (void);
void  SPI1_Init (void);
void  SetTimeOut (U16);
void  ClearTimeOut (void);
void  delay (U16);
void  phyWrite (U8, U8);
U8    phyRead (U8);
U8    RadioInit(void);
U8    RadioInitSoftwareReset(void);
void  setTxPower(U8);
void  setTRxFrequency (U32);
U32   divideWithRounding (U32, U32);
void  setTxFrequencyDeviation (U32);
void  setTxDataRate (U32);
//=============================================================================
// Functions
//=============================================================================
//-----------------------------------------------------------------------------
// main ()
//-----------------------------------------------------------------------------
void main (void)
{
   U8 status;

   PCA0MD   &= ~0x40;                  // disable F930 watchdog

   // Configure DC/DC
   // Needed for Si1004/5 support only
   // Not needed, but harmless for other variants
#ifdef SI1004_DAUGHTER_CARD
   DC0CN = 0x43;                       // 20ns min, big SW, 2.1V
   DC0CF = 0x04;                       // Local osc, High-Peak Current
#endif

   PortInit ();
   SYSCLK_Init ();
   SPI1_Init ();

   RadioStatus = RadioInit();

   // Check status error code using debug trap.
   while(RadioStatus);

   // Change parameter to set tone frequency.
   // Set According to matching network on daughter card
#ifdef SI1000_DAUGHTER_CARD
   setTRxFrequency (915000000L);
#endif
#ifdef SI1002_DAUGHTER_CARD
   setTRxFrequency (868000000L);
#endif
#ifdef SI1004_DAUGHTER_CARD
   setTRxFrequency (868000000L);
#endif

   // Use with 474 MHz matching network
   // setTRxFrequency (470000000L);
   // Use with 474 MHz matching network
   // setTRxFrequency (434000000L);

   // Change parameter to set tone power.
   // Actual power depends on device PA rating.
   setTxPower(7);                      // Full power

   // Change parameter to set Tx Frequency Deviation.
   setTxFrequencyDeviation(20000L);

   // Change parameter to set Tx Data Rate.
   setTxDataRate (40000L);

   // Set modulation control register for FSK or GFSK spectrum.
   // Read/Modify/write used to preserve txscale
   status = phyRead(EZRADIOPRO_MODULATION_MODE_CONTROL_2);
   status |= MODULATION_SELECT;
   phyWrite(EZRADIOPRO_MODULATION_MODE_CONTROL_2, status);

#ifdef ENABLE_RF_SWITCH
   // RF Switch enabled by default
   // Needed for +20 daughtercards only.
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
         LED = !LED;
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
//
// PortInit()
//
// Return Value : None
// Parameters   : None
//
// This function initializes the Ports.
//
//-----------------------------------------------------------------------------
//
// P0.0  -  /IRQ input
//
// P1.0  -  SCK  (SPI1), Push-Pull,  Digital
// P1.1  -  MISO (SPI1), Open-Drain, Digital
// P1.2  -  MOSI (SPI1), Push-Pull,  Digital
// P1.3  -  skipped
// P1.4  -  NSS, Push-Pull,  Digital
//
// P2.6  -  SDN Push-Pull,  Digital  Si10001/2/3
// P1.6  -  SDN Push-Pull,  Digital  Si10004/5
//
//-----------------------------------------------------------------------------
void PortInit (void)
{
   // Initialize SPI
   XBR1    |= 0x40;                    // Enable SPI1 (3 wire mode)
   P1MDOUT |= 0x15;                    // MOSI, SCK, NSS, push-pull
   SFRPAGE  = CONFIG_PAGE;
   P1DRV   |= 0x15;                    // MOSI, SCK, NSS, high current mode
   SFRPAGE  = LEGACY_PAGE;

   // Initialize SDN
#ifdef   SI1000_DAUGHTER_CARD
   P2MDOUT |= 0x40;                    // SDN P2.6 push-pull
   SFRPAGE  = CONFIG_PAGE;
   P2DRV   |= 0x40;                    // SDN  P2.6 high-current mode
   SFRPAGE  = LEGACY_PAGE;
#endif
#ifdef   SI1002_DAUGHTER_CARD
   P2MDOUT |= 0x40;                    // SDN P2.6 push-pull
   SFRPAGE  = CONFIG_PAGE;
   P2DRV   |= 0x40;                    // SDN  P2.6 high-current mode
   SFRPAGE  = LEGACY_PAGE;
#endif
#ifdef   SI1004_DAUGHTER_CARD
   P1MDOUT |= 0x40;                    // SDN P1.6 push-pull
   SFRPAGE  = CONFIG_PAGE;
   P1DRV   |= 0x40;                    // SDN  P1.6 high-current mode
   SFRPAGE  = LEGACY_PAGE;
#endif
   SDN      = 0;                       // SDN low before enabling crossbar

   XBR2    |= 0x40;                    // enable Crossbar
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
   CLKSEL    = 0x14;                   // use 10 MHz clock
}
//-----------------------------------------------------------------------------
// SPI1_Init ()
//
// Return Value : None
// Parameters   : None
//
// SPI1 is configured for 3-wire master mode and runs at SYSWCLK/2.
//-----------------------------------------------------------------------------
void SPI1_Init()
{

   SPI1CFG   = 0x40;                   // master mode
   SPI1CN    = 0x00;                   // 3 wire master mode
   SPI1CKR   = 0x00;                   // Use SYSWCLK/2r
   SPI1CN   |= 0x01;                   // enable SPI
   NSS1 = 1;                           // set NSS high
}
//=============================================================================
//
// spi Functions for phy write and read primitives
//
//=============================================================================
//-----------------------------------------------------------------------------
// Function Name phyWrite()
//
// Return Value   : None
// Parameters :
//    U8 reg - register address from the si4432.h file.
//    U8 value - value to write to register
//
//-----------------------------------------------------------------------------
void phyWrite (U8 reg, U8 value)
{
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
}
//-----------------------------------------------------------------------------
// Function Name
//    phyRead()
//
// Parameters   : U8 reg - register address from the si4432.h file.
// Return Value : U8 value - value returned from the si4432 register
//
//-----------------------------------------------------------------------------
U8 phyRead (U8 reg)
{
   U8 value;

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
// to reset the radio when a radio POR has not occured.
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

   TMOD      = 0x01;    // T0 uses prescaller
   CKCON     = 0x02;    // divide by 48 prescaller

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

   TMOD      = 0x01;    // T0 uses prescaller
   CKCON     = 0x02;    // divide by 48 prescaller

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
// divideWithRounding
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
//-----------------------------------------------------------------------------
// setTxFrequencyDeviation
//
// Parameters   : deviation
// Return Value : None
//
// This function will calculate and set the values for the radio frequency
// deviation registers.
//-----------------------------------------------------------------------------
void setTxFrequencyDeviation (U32 deviation)
{
   U8 txFrequencyDeviation;
   U8 modulationControl2Mask;

   deviation  = divideWithRounding (deviation, 625);

   txFrequencyDeviation = (U8)deviation;

   if (deviation > 255)
      modulationControl2Mask = 0x04;
   else
      modulationControl2Mask = 0x00;

   phyWrite(EZRADIOPRO_FREQUENCY_DEVIATION, txFrequencyDeviation);
   phyWrite(EZRADIOPRO_MODULATION_MODE_CONTROL_2,
      (phyRead(EZRADIOPRO_MODULATION_MODE_CONTROL_2)|modulationControl2Mask));
}
//-----------------------------------------------------------------------------
// setTxDataRate
//
// Parameters   : dataRate
// Return Value : None
//
// This function will calculate and set the values for the TX data rate
// registers.
//-----------------------------------------------------------------------------
void setTxDataRate (U32 dataRate)
{
   UU16  txDataRate;
   U8    modulationControl1Mask;

  if (dataRate < 30000)
   {
      // dataRate = dataRate * 2^21 / 10^6  : exceeds 32-bits
      // dataRate = dataRate * 2^17 / 62500 : OK for 32 bit math
      dataRate <<= 17;
      dataRate = divideWithRounding (dataRate, 62500L);
      modulationControl1Mask = 0x20; //set txdtrtscale
   }
   else
   {
      // dataRate = dataRate * 2^16 / 10^6   : exceeds 32-bits
      // dataRate = dataRate * 2^15 / 500000 : OK for 32 bit math
     dataRate <<= 15;
     dataRate = divideWithRounding (dataRate, 500000L);
     modulationControl1Mask = 0x00; //don't set txdtrtscale
   }

   txDataRate.U16 = (U16)dataRate;

   // TX Modem Settings
   phyWrite(EZRADIOPRO_TX_DATA_RATE_1, txDataRate.U8[MSB]);
   phyWrite(EZRADIOPRO_TX_DATA_RATE_0, txDataRate.U8[LSB]);
   phyWrite(EZRADIOPRO_MODULATION_MODE_CONTROL_1,
      (phyRead(EZRADIOPRO_MODULATION_MODE_CONTROL_1)|modulationControl1Mask));

}
//=============================================================================
