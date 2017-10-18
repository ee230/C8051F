//=============================================================================
// Si100x_RxRSSI.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This is a simple single file example for measuring the RSSI of a received
// FSK or GFSK signal.
//
// This example provides run time support for setting the TRx frequency and
// all the RX modem parameters. The measured RSSI will depend somewhat on the
// modem settings.
//
// The RadioInit() function provides debug support to assist in debugging the
// radio. The Radio SDN pin should be connected to P1.7 or the MCU. The IRQ pin
// should be connected to P0.6. Optional code may be included to manage the radio
// with SDN shorted to ground.
//
// To use, compile and download firmware to the Si102x/3x. Make sure the SDN
// pin of the radio is connected to P1.7 of the MCU. If the Radio transmitter
// starts successfully, the LED on P0.0 will blink. Please also make sure
// the LED is connected to P0.0.
//
// Connect one end of a USB cable to the VCP USB connection on the
// UPMP-F960-MLCD development board. Connect the other end to the computer.
// Open Hyperterminal, select the appropriate COM port and set the
// communication parameters to 115200-N-1 & no flow control.
//
// Use a second Si102x/3x board with the TxSpectrum code to generate a
// compatible RF spectrum. Connect antennas to both boards. Observe the RSSI
// measurement while moving the boards further apart.
//
// If desired the frequency and data rate parameters may be changed to match
// a particular application.
//
// While every effort has been made to support various data rates, this code
// is not guaranteed to provide the best possible RF settings for any arbitrary
// frequency. To ensure the best possible RF performance, please double-check
// the run time calculated RF settings with the excel spreadsheet values for
// your particular applications requirements. Note that it is the user's
// responsiblity to chose appropriates value for rxBandwidth, rxDataRate,
// and rxDeviation.
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
#include <stdio.h>

//-----------------------------------------------------------------------------
// Define SYSCLK using LP oscillator
//-----------------------------------------------------------------------------
#define SYSCLK_HZ       (24500000L)

//-----------------------------------------------------------------------------
// Defines used for delay and time out
//-----------------------------------------------------------------------------
#define ONE_MS          (SYSCLK_HZ/1000/48)
#define TWO_MS          (SYSCLK_HZ/500/48)
#define FIVE_MS         (SYSCLK_HZ/200/48)
#define TEN_MS          (SYSCLK_HZ/100/48)
#define TWENTY_MS       (SYSCLK_HZ/50/48)
#define TWENTY_FIVE_MS  (SYSCLK_HZ/40/48)
#define ONE_HUNDRED_MS  (SYSCLK_HZ/10/48)
#define TWO_HUNDRED_MS  (SYSCLK_HZ/5/48)

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
// Baud rate macros
//-----------------------------------------------------------------------------
#define BAUD               (115200L)
#if (SYSCLK_HZ/BAUD/2/1)<256
   #define T1_RELOAD -((SYSCLK_HZ/BAUD/1+1)/2)
#else
   #error "error: baud rate too low!"
#endif

//-----------------------------------------------------------------------------
// RSSI Samples
//-----------------------------------------------------------------------------
#define RSSI_SAMPLES   10

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
//
// Look up table for IF Filter Settings assuming m < 2
//
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (TableRxBandwidth[54], U16, SEG_CODE) =
{
   26,   // 2.6 kHz
   28,   // 2.8 kHz
   31,   // 3.1 kHz
   32,   // 3.2 kHz
   37,   // 3.7 kHz
   49,   // 4.9 kHz
   54,   // 5.4 kHz
   59,   // 5.9 kHz
   61,   // 6.1 kHz
   72,   // 7.2 kHz
   95,   // 9.5 kHz
   106,  // 10.6 kHz
   115,  // 11.5 kHz
   121,  // 12.1 kHz
   142,  // 14.2 kHz
   162,  // 16.2 kHz
   175,  // 17.5 kHz
   194,  // 19.4 kHz
   214,  // 21.4 kHz
   239,  // 23.9 kHz
   257,  // 25.7 kHz
   282,  // 28.2 kHz
   322,  // 32.2 kHz
   347,  // 34.7 kHz
   386,  // 38.6 kHz
   427,  // 42.7 kHz
   477,  // 47.7 kHz
   512,  // 51.2 kHz
   562,  // 56.2 kHz
   641,  // 64.1 kHz
   692,  // 69.2 kHz
   752,  // 75.2 kHz
   832,  // 83.2 kHz
   900,  // 90 kHz
   953,  // 95.3 kHz
   1121, // 112.1 kHz
   1279, // 127.9 kHz
   1379, // 137.9 kHz
   1428, // 142.8 kHz
   1678, // 167.8 kHz
   1811, // 181.1 kHz
   1915, // 191.5 kHz
   2084, // 208.4 kHz
   2251, // 225.1 kHz
   2488, // 248.8 kHz
   2693, // 269.3 kHz
   2849, // 284.9 kHz
   3355, // 335.5 kHz
   3618, // 361.8 kHz
   4202, // 420.2 kHz
   4684, // 468.4 kHz
   5188, // 518.8 kHz
   5770, // 577 kHz
   6207 // 620.7 kHz
};

const SEGMENT_VARIABLE (TableIF_FilterSetting[54], U8, SEG_CODE) =
{
   0x51, // 2.6 kHz, dwn3_byp = 0, ndec_exp = 5 , filset = 1
   0x52, // 2.8 kHz, dwn3_byp = 0, ndec_exp = 5 , filset = 2
   0x53, // 3.1 kHz, dwn3_byp = 0, ndec_exp = 5 , filset = 3
   0x54, // 3.2 kHz, dwn3_byp = 0, ndec_exp = 5 , filset = 4
   0x55, // 3.7 kHz, dwn3_byp = 0, ndec_exp = 5 , filset = 5
   0x41, // 4.9 kHz, dwn3_byp = 0, ndec_exp = 4 , filset = 1
   0x42, // 5.4 kHz, dwn3_byp = 0, ndec_exp = 4 , filset = 2
   0x43, // 5.9 kHz, dwn3_byp = 0, ndec_exp = 4 , filset = 3
   0x44, // 6.1 kHz, dwn3_byp = 0, ndec_exp = 4 , filset = 4
   0x45, // 7.2 kHz, dwn3_byp = 0, ndec_exp = 4 , filset = 5
   0x31, // 9.5 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 1
   0x32, // 10.6 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 2
   0x33, // 11.5 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 3
   0x34, // 12.1 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 4
   0x35, // 14.2 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 5
   0x36, // 16.2 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 6
   0x3A, // 17.5 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 10
   0x3B, // 19.4 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 11
   0x3C, // 21.4 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 12
   0x3D, // 23.9 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 13
   0x3E, // 25.7 kHz, dwn3_byp = 0, ndec_exp = 3 , filset = 14
   0x25, // 28.2 kHz, dwn3_byp = 0, ndec_exp = 2 , filset = 5
   0x26, // 32.2 kHz, dwn3_byp = 0, ndec_exp = 2 , filset = 6
   0x27, // 34.7 kHz, dwn3_byp = 0, ndec_exp = 2 , filset = 7
   0x2B, // 38.6 kHz, dwn3_byp = 0, ndec_exp = 2 , filset = 11
   0x2C, // 42.7 kHz, dwn3_byp = 0, ndec_exp = 2 , filset = 12
   0x2D, // 47.7 kHz, dwn3_byp = 0, ndec_exp = 2 , filset = 13
   0x2E, // 51.2 kHz, dwn3_byp = 0, ndec_exp = 2 , filset = 14
   0x15, // 56.2 kHz, dwn3_byp = 0, ndec_exp = 1 , filset = 5
   0x16, // 64.1 kHz, dwn3_byp = 0, ndec_exp = 1 , filset = 6
   0x17, // 69.2 kHz, dwn3_byp = 0, ndec_exp = 1 , filset = 7
   0x01, // 75.2 kHz, dwn3_byp = 0, ndec_exp = 0 , filset = 1
   0x02, // 83.2 kHz, dwn3_byp = 0, ndec_exp = 0 , filset = 2
   0x03, // 90 kHz, dwn3_byp = 0, ndec_exp = 0 , filset = 3
   0x04, // 95.3 kHz, dwn3_byp = 0, ndec_exp = 0 , filset = 4
   0x05, // 112.1 kHz, dwn3_byp = 0, ndec_exp = 0 , filset = 5
   0x06, // 127.9 kHz, dwn3_byp = 0, ndec_exp = 0 , filset = 6
   0x07, // 137.9 kHz, dwn3_byp = 0, ndec_exp = 0 , filset = 7
   0x94, // 142.8 kHz, dwn3_byp = 1, ndec_exp = 1 , filset = 4
   0x95, // 167.8 kHz, dwn3_byp = 1, ndec_exp = 1 , filset = 5
   0x99, // 181.1 kHz, dwn3_byp = 1, ndec_exp = 1 , filset = 9
   0x96, // 191.5 kHz, dwn3_byp = 1, ndec_exp = 1 , filset = 6
   0x9A, // 208.4 kHz, dwn3_byp = 1, ndec_exp = 1 , filset = 10
   0x81, // 225.1 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 1
   0x82, // 248.8 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 2
   0x83, // 269.3 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 3
   0x84, // 284.9 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 4
   0x88, // 335.5 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 8
   0x89, // 361.8 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 9
   0x8A, // 420.2 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 10
   0x8B, // 468.4 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 11
   0x8C, // 518.8 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 12
   0x8D, // 577 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 13
   0x8E, // 620.7 kHz, dwn3_byp = 1, ndec_exp = 0 , filset = 14
};
///----------------------------------------------------------------------------
// Internal Function Prototypes
//-----------------------------------------------------------------------------
void  Port_Init (void);
void  SYSCLK_Init (void);
void  SPI1_Init (void);
void  UART0_Init(void);
void  phyWrite (U8, U8);
U8    phyRead (U8);
U8    RadioInit(void);
U8    RadioInitSoftwareReset(void);
void  SetTimeOut (U16);
void  ClearTimeOut (void);
void  delay (U16);
void  setTRxFrequency (U32);
U8    lookUpFilterIndex (U32);
U32   calcAFC_PullInRange(U32);
void  setAFC_Limit (U32);
U16   calcRxoverSamplingRatio (U8, U32);
void  setRxoverSamplingRatio (U16);
U32   calcClockRecoveryOffset (U8, U32);
void  setClockRecoveryOffset (U32);
U16   calcClockRecoveryTimingLoopGain (U32, U16, U32);
void  setClockRecoveryTimingLoopGain (U16);
U8    sampleRSSI (void);
U32   divideWithRounding (U32, U32);
S16   calcInputPower (U8);

//=============================================================================
// Functions
//=============================================================================
//-----------------------------------------------------------------------------
// main ()
//-----------------------------------------------------------------------------
void main (void)
{
   U8 status;
   U8 rssi;
   S16 inputPower;

   // user variables
   U32 volatile trxFrequency;
   U32 volatile rxDataRate;
   U32 volatile rxDeviation;
   U32 volatile rxBandwidth;

   // used with look-up table
   U8 volatile filterSetting;
   U8 volatile filterIndex;

   // calculated values
   U16 volatile rxOverSamplingRatio;
   U32 volatile rxClockRecoveryOffset;
   U16 volatile rxLoopGain;
   U32 volatile rxAFC_PullInRange;

   PCA0MD  &= ~0x40;                  // disable watchdog

   Port_Init ();
   SYSCLK_Init ();
   SPI1_Init ();
   UART0_Init();

   RadioStatus = RadioInit();

   // Check status error code using debug trap.
   while(RadioStatus);

   // Set Frequency according to hardware design
   trxFrequency = 915000000L;

   // Change User Settings to match TxSpectrum
   // Default user Settings
   rxDataRate = 40000L;
   rxDeviation = 20000L;
   rxBandwidth = 80000L;

   // SetTRxFrequency same as transmitter.
   setTRxFrequency (trxFrequency);

   // Use RX bandwidth to look-up filter index.
   filterIndex = lookUpFilterIndex (rxBandwidth);
   // use index to set filter setting register
   filterSetting = TableIF_FilterSetting[filterIndex];
   phyWrite(EZRADIOPRO_IF_FILTER_BANDWIDTH, filterSetting);

   // also use index to update Rxbandwidth and set AFC limit
   rxBandwidth = (TableRxBandwidth[filterIndex]) * (100L);
   rxAFC_PullInRange = calcAFC_PullInRange(rxBandwidth);
   setAFC_Limit(rxAFC_PullInRange);

   // Use Filter setting and RxDataRate to set RxoverSamplingRatio.
   rxOverSamplingRatio = calcRxoverSamplingRatio (filterSetting, rxDataRate);
   setRxoverSamplingRatio(rxOverSamplingRatio);

   // Use Filter setting and RxDataRate to set ClockRecoveryOffset.
   rxClockRecoveryOffset = calcClockRecoveryOffset (filterSetting, rxDataRate);
   setClockRecoveryOffset (rxClockRecoveryOffset);

   // Use RxDataRate, RxoverSamplingRatio, and RxDeviation to set Loop Gain.
   rxLoopGain = calcClockRecoveryTimingLoopGain (rxDataRate, rxOverSamplingRatio, rxDeviation);
   setClockRecoveryTimingLoopGain(rxLoopGain);

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

   // Enable Reciever
   phyWrite(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,(EZRADIOPRO_RXON|EZRADIOPRO_XTON));

   delay(TWO_MS);                     // Wait 2 ms

   status = phyRead(EZRADIOPRO_DEVICE_STATUS);

   // Check status for transmit mode.
   if((status& 0x03)==0x01)
   {
      // Blink forever.
      while(1)
      {
         rssi = sampleRSSI();

         inputPower = calcInputPower (rssi);

         printf("RSSI = %i    \r", inputPower);

         delay(TWO_HUNDRED_MS);

         LED1 = !LED1;
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
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   U8 restoreSFRPAGE;   
   restoreSFRPAGE = SFRPAGE;   

   SFRPAGE   = LEGACY_PAGE;

   OSCICN |= 0x80;                     // Enable the precision internal osc.

   CLKSEL = 0x00;                      // use 24,5 MHz clock
   FLSCL = 0x40; // Bypass one-shot

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

//-----------------------------------------------------------------------------
// UART0_Init ()
//-----------------------------------------------------------------------------
void   UART0_Init (void)
{
   U8 restoreSFRPAGE;   
   restoreSFRPAGE = SFRPAGE;  
   
   SCON0  = 0x10;                      // SCON0: 8-bit variable bit rate, no receiver
   CKCON |= 0x08;                      // T1 uses SYSCLK
   TH1    = T1_RELOAD;                 // reload value calculated from BAUD
   TL1    = T1_RELOAD;                 // also load into TL1
   TMOD  &= ~0xF0;                     // clear T1 bits in TMOD
   TMOD  |=  0x20;                     // set TMOD for 8 bit reload
   TR1    = 1;                         // START Timer1
   TI0    = 1;                         // Transciever ready
   
   SFRPAGE  = restoreSFRPAGE;
}

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : character to send to UART
//
// This function outputs a character to the UART.
// Used for SDCC build only.
//-----------------------------------------------------------------------------
#ifdef SDCC
void putchar (char c)
{
   while (!TI0);
   TI0 = 0;
   SBUF0 = c;
}
#endif

//=============================================================================
//
// SPI Functions for phy write and read primitives
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

   TMOD  &= ~0x0F;                     // clear T0 bits in TMOD
   TMOD  |=  0x01;                     // T0 uses prescaller
   CKCON &= ~0x03;                     // clear T0 bits in CKCON
   CKCON |=  0x02;                     // divide by 48 prescaller

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

   TMOD  &= ~0x0F;                     // clear T0 bits in TMOD
   TMOD  |=  0x01;                     // T0 uses prescaller
   CKCON &= ~0x03;                     // clear T0 bits in CKCON
   CKCON |=  0x02;                     // divide by 48 prescaller

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

   frequency  = divideWithRounding(frequency, 625);

   frequency  *= 625L;

   if (frequency > 480000000L )
   {
      frequency -= 480000000L;
      frequencyBandSelect  = frequency / 20000000L;
      frequency -= (U32)frequencyBandSelect * 20000000L;
      frequency /= 625L;
      frequency <<= 1;
      frequencyBandSelect |= 0x20;
   }
   else
   {
      frequency -= 240000000L;
      frequencyBandSelect = (frequency / 10000000L);
      frequency -= (U32)frequencyBandSelect * 10000000L;
      frequency /= 625L;
      frequency <<= 2;
   }

   frequencyBandSelect |= 0x40;        // set sbsel

   nominalCarrierFrequency.U16 = (U16)frequency;

   phyWrite(EZRADIOPRO_FREQUENCY_BAND_SELECT, frequencyBandSelect);
   phyWrite(EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_1, nominalCarrierFrequency.U8[MSB]);
   phyWrite(EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_0, nominalCarrierFrequency.U8[LSB]);

}

//-----------------------------------------------------------------------------
// lookUpFilterIndex()
//
// Parameters   : RxBandwidth
// Return Value : i (filter index)
//
// This function will search the RxBandwidth table to find the appropriate
// IF filter settings. This function will return the index for use with the
// TableRxBandwidth and TableIfFilterSettings
//
//-----------------------------------------------------------------------------
U8 lookUpFilterIndex (U32 RxBandwidth)
{
   U8 i;

   RxBandwidth  = divideWithRounding (RxBandwidth, 100);

   i=0;

   // Find largest value in table smaller than target.
   while (TableRxBandwidth[i] <  RxBandwidth)
   {
      i++;
   }

   return i;
}

//-----------------------------------------------------------------------------
// calcAFC_PullInRange()
//
// Parameters   : U32 rxBandwidth
// Return Value : U32 afcPullIn
//
//-----------------------------------------------------------------------------
U32 calcAFC_PullInRange (U32 rxBandwidth)
{
   U32 afcPullIn;

   afcPullIn = rxBandwidth * 7;
   afcPullIn = divideWithRounding (afcPullIn, 10);
   return afcPullIn;
}

//-----------------------------------------------------------------------------
// setAFC_Limit()
//
// Parameters   : U32 afcLimit
// Return Value : None
//
//-----------------------------------------------------------------------------
void setAFC_Limit (U32 afcLimit)
{
   U8 frequencyBandSelect;

   //TRX frequency must be set first
   frequencyBandSelect = phyRead(EZRADIOPRO_FREQUENCY_BAND_SELECT);

   if((frequencyBandSelect&0x20)==0x20)
   {
      afcLimit>>=1;
   }

   afcLimit = divideWithRounding (afcLimit, 625);

   if(afcLimit>0xFF)
      afcLimit = 0xFF;

   phyWrite(EZRADIOPRO_AFC_LIMITER, afcLimit);
}

//-----------------------------------------------------------------------------
// calcRxoverSamplingRatio()
//
// Parameters   : U8 filter
//                U32 rxDataRate
//
// Return Value : U16 rxOverSamplingRatio
//
//-----------------------------------------------------------------------------
U16 calcRxoverSamplingRatio (U8 filter, U32 rxDataRate)
{
   U32 rxOverSamplingRatio;
   U8 ndec_exp, dwn3_bypass;

   filter >>= 4;
   ndec_exp = filter & 0x07;
   filter >>= 3;
   dwn3_bypass = filter & 0x01;

   //calculate rxOverSamplingRatio
   if(dwn3_bypass)
      rxOverSamplingRatio = (500000L * 3 * 8);
   else
      rxOverSamplingRatio = (500000L * 8);

   rxOverSamplingRatio >>= ndec_exp;
   rxOverSamplingRatio = divideWithRounding(rxOverSamplingRatio, rxDataRate);

   if (rxOverSamplingRatio > 0x07FF)    // limit to 11 bits
      rxOverSamplingRatio = 0x07FF;

   return (U16) rxOverSamplingRatio;
}

//-----------------------------------------------------------------------------
// setRxoverSamplingRatio()
//
// Parameters   : U16 rxOverSamplingRatio
// Return Value : None
//
//-----------------------------------------------------------------------------
void setRxoverSamplingRatio (U16 rxOverSamplingRatio)
{
   U8 ClockRecoveryOffset2;

   if (rxOverSamplingRatio > 0x07FF)    // limit to 11 bits
      rxOverSamplingRatio = 0x07FF;

   ClockRecoveryOffset2 = phyRead(EZRADIOPRO_CLOCK_RECOVERY_OFFSET_2);
   ClockRecoveryOffset2 &= 0x1F; // clear rxOverSamplingRatio bits
   ClockRecoveryOffset2 |= ((rxOverSamplingRatio>>3)&0xE0);

   phyWrite(EZRADIOPRO_CLOCK_RECOVERY_OFFSET_2, ClockRecoveryOffset2);
   phyWrite(EZRADIOPRO_CLOCK_RECOVERY_OVERSAMPLING_RATIO, (U8)rxOverSamplingRatio);
}

//-----------------------------------------------------------------------------
// calcClockRecoveryOffset()
//
// Parameters   : U8    filter
//                U32   rxDataRate
//
// Return Value : U32   clockRecoveryOffset
//
//-----------------------------------------------------------------------------
U32 calcClockRecoveryOffset (U8 filter, U32 rxDataRate)
{
   U32 clockRecoveryOffset;

   U8 filset, ndec_exp, dwn3_bypass;

   filset = filter & 0x0F;
   filter >>= 4;
   ndec_exp = filter & 0x07;
   filter >>= 3;
   dwn3_bypass = filter & 0x01;

   // calculate clockRecoveryOffset
   clockRecoveryOffset = rxDataRate;

   // limit operands to 32-bits
   // clockRecoveryOffset = clockRecoveryOffset * 2^20 /500000 : exceeds 32-bit
   // clockRecoveryOffset = clockRecoveryOffset * 2^15 /15625  : OK
   clockRecoveryOffset <<= 15;
   clockRecoveryOffset = divideWithRounding (clockRecoveryOffset, 15625);
   clockRecoveryOffset <<= ndec_exp;
   if(dwn3_bypass)
   {
     clockRecoveryOffset = divideWithRounding (clockRecoveryOffset, 3);
   }

   if (clockRecoveryOffset > 0x000FFFFF)    // limit to 20 bits
      clockRecoveryOffset = 0x000FFFFF;

   return clockRecoveryOffset;
}

//-----------------------------------------------------------------------------
// setClockRecoveryOffset()
//
// Parameters   : U32 clockRecoveryOffset
// Return Value : None
//
//-----------------------------------------------------------------------------
void setClockRecoveryOffset (U32 clockRecoveryOffset)
{
   UU16 ncoff;
   U8 ClockRecoveryOffset2;

   if (clockRecoveryOffset > 0x000FFFFF)    // limit to 20 bits
      clockRecoveryOffset = 0x000FFFFF;

   ClockRecoveryOffset2 = phyRead(EZRADIOPRO_CLOCK_RECOVERY_OFFSET_2);
   ClockRecoveryOffset2 &= 0xF0; // clear ncoff bits
   ClockRecoveryOffset2 |= ((clockRecoveryOffset>>16)&0x0F);

   ncoff.U16 = (U16) clockRecoveryOffset;

   phyWrite(EZRADIOPRO_CLOCK_RECOVERY_OFFSET_2, ClockRecoveryOffset2);
   phyWrite(EZRADIOPRO_CLOCK_RECOVERY_OFFSET_1, ncoff.U8[MSB]);
   phyWrite(EZRADIOPRO_CLOCK_RECOVERY_OFFSET_0, ncoff.U8[LSB]);
}

//-----------------------------------------------------------------------------
// calcClockRecoveryTimingLoopGain()
//
// Parameters   : U32   rxDataRate
//                U16   RxOverSamplingRatio
//                U32   RxDeviation
//
// Return Value : U16   clockRecoveryTimingLoopGain
//
//-----------------------------------------------------------------------------
U16 calcClockRecoveryTimingLoopGain (U32 rxDataRate, U16 RxOverSamplingRatio, U32 RxDeviation)
{
   U32 clockRecoveryTimingLoopGain;

    // calulate clockRecoveryTimingLoopGain
   clockRecoveryTimingLoopGain = rxDataRate;
   clockRecoveryTimingLoopGain <<= 15;
   clockRecoveryTimingLoopGain = divideWithRounding (clockRecoveryTimingLoopGain, RxOverSamplingRatio);
   clockRecoveryTimingLoopGain = divideWithRounding (clockRecoveryTimingLoopGain, RxDeviation);
   clockRecoveryTimingLoopGain += 2;

   if (clockRecoveryTimingLoopGain > 0x07FF)    // limit to 11 bits
      clockRecoveryTimingLoopGain = 0x07FF;

   return (U16) clockRecoveryTimingLoopGain;
}

//-----------------------------------------------------------------------------
// FunctionName()
//
// Parameters   : U16   clockRecoveryTimingLoopGain
// Return Value : None
//
//-----------------------------------------------------------------------------
void setClockRecoveryTimingLoopGain (U16 clockRecoveryTimingLoopGain)
{
   UU16 crgain;

   crgain.U16 = clockRecoveryTimingLoopGain;

   phyWrite(EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1, crgain.U8[MSB]);
   phyWrite(EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0, crgain.U8[LSB]);
}

//-----------------------------------------------------------------------------
// sampleRSSI()
//
// Parameters   : None
// Return Value : U8 RSSI
//
// This function will find the average of a number of samples discarding the
// low and high values. This will discard the infrequent erroneous rssi
// samples which are known to occur.
//
//-----------------------------------------------------------------------------
U8  sampleRSSI (void)
{
   U8 i;
   U8 sample;

   U8    min = 255;
   U8    max = 0;
   U16   sum = 0;

   for(i=0;i<RSSI_SAMPLES;i++)
   {
      sample = phyRead(EZRADIOPRO_RECEIVED_SIGNAL_STRENGTH_INDICATOR);

      sum += sample;

      if(sample > max)
      {
         max = sample;
      }

      if(sample < min)
      {
         min = sample;
      }
   }

   sum -= min;
   sum -= max;

   sum /= (RSSI_SAMPLES-2);

   return sum;
}

//-----------------------------------------------------------------------------
// calcInputPower()
//
// Parameters   : U8    RSSI
// Return Value : S16   inputPower
//
// Equation for input power derived from linear trend line of figure in data
// sheet.
//                      10
// inputPower = rssi * ---- - 127
//                      19
//-----------------------------------------------------------------------------
S16   calcInputPower (U8 rssi)
{
   S16 value;

   value = rssi;
   value *= 10;
   value /= 19;
   value -=127;

   if(value< -120)
      value = -120;
   else if (value > 0)
      value = 0;

   return value;
}

//=============================================================================
