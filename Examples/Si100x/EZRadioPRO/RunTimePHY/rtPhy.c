//=============================================================================
// rtPhy.c
//=============================================================================
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// Target:
//    Any Silicon Labs C8051 MCU.
//
// IDE:
//    Silicon Laboratories IDE   version 3.3
//
// Tool Chains:
//    Keil
//       c51.exe     version 8.0.8
//       bl51.exe    version 6.0.5
//    SDCC
//       sdcc.exe    version 2.8.0
//       aslink.exe  version 1.75
//
// Project Name:
//    Run Time PHY
//
// Release 2.0
//    - 04 NOV 2011
//
// This software must be used in accordance with the End User License Agreement.
//
//-----------------------------------------------------------------------------
// EZRadioPRO Run Time PHY
// Version 2.0 Release Notes
//
// * Added automatic RX bandwidth calculator
// * Extended range to 256 kbps
// * Added optional 32-bit overflow checking
// * Added optional high modulation-index tables
// * Fully tested against known-good Test Vectors
//
// The parameters now must be set in a specific order.
// 1. Set the TRX_FREQUENCY
// 2. Set the TRX_DATA_RATE
// 3. Set the TRX_DEVIATION (optional)
// 4. Set the RX_BAND_WIDTH (optional)
// 5. Set the TRX_CHANNEL_SPACING (optional)
//
// Setting the data rate will automatically set the default
// deviation to one half the data rate (modulation index=1)
// and automatically calculate the required RX bandwidth.
//
// Manually setting the deviation will override default value
// and automatically calculate the required RX bandwidth.
// This must be done after setting the data rate.
//
// Manually setting the RX bandwidth will override the automatic
// RX bandwidth calculation. This must be done after setting the
// data rate and deviation.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//
// These includes must be in a specific order. Dependencies listed in comments.
//-----------------------------------------------------------------------------
#include "compiler_defs.h"
#include "hardware_defs.h"                    // requires compiler_defs.h
#include "rtPhy.h"
#include "rtPhy_const.h"
//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
bit RxPacketReceived;
SEGMENT_VARIABLE (RxPacketLength, U8, BUFFER_MSPACE);
SEGMENT_VARIABLE (RxErrors, U8, BUFFER_MSPACE);
//-----------------------------------------------------------------------------
// Receive interrupt buffer
//-----------------------------------------------------------------------------
SEGMENT_VARIABLE (RxIntBuffer[64], U8, BUFFER_MSPACE);
//-----------------------------------------------------------------------------
// Internal interrupt functions
//-----------------------------------------------------------------------------
U8    RxIntPhyRead (U8);
void  RxIntPhyWrite (U8, U8);
void  RxIntphyReadFIFO (U8, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE));
//=============================================================================
//
// API Functions
//
//=============================================================================
rtPhySettingsStruct SEG_XDATA rtPhySettings;
bit PhyInitialized = 0;
//=============================================================================
// local functions
//=============================================================================
U8    InitSoftwareReset(void);
void  SetTimeOut (U16);
void  ClearTimeOut (void);
void  delay (U16);


void  SetTRxFrequency (U32);
void  SetTRxChannelSpacing  (U32);
void  SetTxFrequencyDeviation (U32);
void  SetTxDataRate (U32);

void  AutoRxBandwith (void);
void  UpdateRxModemSettings(void);
U8    LookUpFilterSetting(U32, U32 , U32 );
U32   CalcAFC_PullInRange(U32);
void  SetAFC_Limit (U32);
U16   CalcRxOverSamplingRatio (U8, U32);
void  SetRxOverSamplingRatio (U16);
U32   CalcClockRecoveryOffset (U8, U32);
void  SetClockRecoveryOffset (U32);
U16   CalcClockRecoveryTimingLoopGain (U32, U16, U32);
void  SetClockRecoveryTimingLoopGain (U16);

void InitConfigSettings(void);

U32   Divide (U32, U32);
U8    CountBits (U32);
U32   Multiply (U32, U32);
U32   LeftShift (U32, U8);
void  MathError(void);
//-----------------------------------------------------------------------------
// Function Name
//    rtPhyInit()
//
// Parameters   : none
// Return Value : status
//
// After a pin reset or other reset the state of the Radio is unknown.
// The MCU will wait for full POR duration, then figure out if the
// Radio needs a software reset.
//
//-----------------------------------------------------------------------------
PHY_STATUS rtPhyInit(void)
{
   U8 status;

   SDN = 0;

   delay(MILLISECONDS(25));

   status = phyRead(EZRADIOPRO_DEVICE_VERSION); // check version
   if(status == 0xFF)
      return PHY_STATUS_ERROR_SPI;
   else if (status == 0x00)
      return  PHY_STATUS_ERROR_SPI;
   else if (status < MIN_RADIO_VERSION)
      return PHY_STATUS_ERROR_UNSUPPORTED_RADIO;

   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   if((status & EZRADIOPRO_IPOR)==0)
   {
      // radio needs a software reset
      return InitSoftwareReset();
   }
   else if((status & EZRADIOPRO_ICHIPRDY)==0)
   {
      // enable Chip read only
      phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, 0);
      phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, EZRADIOPRO_ENCHIPRDY);

      // wait on IRQ with 2 MS timeout
      SetTimeOut(MILLISECONDS(2));
      while(IRQ)
      {
         if(TIMEOUT_T0)
            return PHY_STATUS_ERROR_RADIO_XTAL;
      }
      ClearTimeOut();
   }
   return PHY_STATUS_SUCCESS; // success
}
//-----------------------------------------------------------------------------
// Function Name
//    RadioInitSWReset()
//
// Parameters   : none
// Return Value : status
//
// This function uses a software reset to reset the radio. This can be used
// to reset the radio when a radio POR has not occured.
// A T0 interrupt timeout is used to minimize start-up time.
//-----------------------------------------------------------------------------
U8 InitSoftwareReset(void)
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
   SetTimeOut(MILLISECONDS(2));
   while(IRQ)
   {
      if(TIMEOUT_T0)
         return PHY_STATUS_ERROR_NO_IRQ;
   }
   ClearTimeOut();

   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   if((status & EZRADIOPRO_ICHIPRDY)==0)
   {
      // enable Chip read only
      phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, 0);
      phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, EZRADIOPRO_ENCHIPRDY);

      // wait on IRQ with 2 MS timeout
      SetTimeOut(MILLISECONDS(2));
      while(IRQ)
      {
         if(TIMEOUT_T0)
            return PHY_STATUS_ERROR_RADIO_XTAL;
      }
      ClearTimeOut();
   }

   return PHY_STATUS_SUCCESS; // success
}
//-----------------------------------------------------------------------------
// Function Name
//    rtPhyIdle(void)
//
// Return Value : PHY_STATUS
// Parameters   : none
//
//-----------------------------------------------------------------------------
PHY_STATUS rtPhyIdle(void)
{
   U8 status;

   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   if((status & EZRADIOPRO_ICHIPRDY)==EZRADIOPRO_ICHIPRDY)
      return PHY_STATUS_SUCCESS;
   else
   {
      // enable just the chip ready IRQ
      phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, 0x00);
      phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, EZRADIOPRO_ENCHIPRDY);

      // read Si4432 interrupts to clear
      status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_1);
      status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

      // enable XTON
      phyWrite (EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1, EZRADIOPRO_XTON);

      // wait on IRQ with 2 MS timeout
      SetTimeOut(MILLISECONDS(2));
      while(IRQ)
      {
         if(TIMEOUT_T0)
            return PHY_STATUS_ERROR_RADIO_XTAL;
      }
      ClearTimeOut();

      return PHY_STATUS_SUCCESS;
   }
}
//-----------------------------------------------------------------------------
// Function Name
//    rtPhyStandby()
//
// Return Value : PHY_STATUS
// Parameters   : none
//
//-----------------------------------------------------------------------------
PHY_STATUS rtPhyStandby (void)
{
   U8 status;

   // disable interrupts
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, 0x00);
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, 0x00);

   // read Si4432 interrupts to clear
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_1);
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   // stop XTAL
   phyWrite (EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1, 0);

   return PHY_STATUS_SUCCESS; // success
}
//-----------------------------------------------------------------------------
// Function Name
//    rtPhyShutDown()
//
// Return Value : PHY_STATUS
// Parameters   : none
//
//-----------------------------------------------------------------------------
PHY_STATUS rtPhyShutDown (void)
{
   P2MDOUT |= 0x40;                    // Enable SDN push pull
   SDN = 1;
   PhyInitialized = 0;

   return PHY_STATUS_SUCCESS; // success
}
//-----------------------------------------------------------------------------
// Function Name
//    rtPhyReStart()
//
// Return Value : PHY_STATUS
// Parameters   : none
//
//-----------------------------------------------------------------------------
PHY_STATUS rtPhyReStart(void)
{
   U8 status;

   SDN = 0;

   delay(MILLISECONDS(2));

   // wait on IRQ with 25 MS timeout
   SetTimeOut(MILLISECONDS(25));
   while(IRQ)
   {
      if(TIMEOUT_T0)
         return PHY_STATUS_ERROR_RADIO_XTAL;
   }
   ClearTimeOut();

   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   if((status & EZRADIOPRO_IPOR)==0)
   {
      // radio needs a software reset
      return InitSoftwareReset();
   }
   else if((status & EZRADIOPRO_ICHIPRDY)==0)
   {
      // disable POR interrupt
      phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, EZRADIOPRO_ENCHIPRDY);

      // wait on IRQ with 2 MS timeout
      SetTimeOut(MILLISECONDS(2));
      while(IRQ)
      {
         if(TIMEOUT_T0)
            return PHY_STATUS_ERROR_RADIO_XTAL;
      }
      ClearTimeOut();
   }
   return PHY_STATUS_SUCCESS; // success
}
//-----------------------------------------------------------------------------
// Function Name
//    rtPhySet()
//
// Return Value : PHY_STATUS
// Parameters   : U8 addr, U32 value
//
//-----------------------------------------------------------------------------
PHY_STATUS rtPhySet (U8 addr, U32 value)
{
   switch (addr)
   {
      // case 0x00:
      case (TRX_FREQUENCY):
         if (value < 240000000L)
            return PHY_STATUS_ERROR_INVALID_VALUE;
         else if (value > 960000000L )
            return PHY_STATUS_ERROR_INVALID_VALUE;
         else
         {
            rtPhySettings.TRxFrequency = value;
            if(PhyInitialized)
               SetTRxFrequency(value);

            return PHY_STATUS_SUCCESS;
         }

      // case 0x01:
      case (TRX_DATA_RATE):
         if (value > 256000L)
            return PHY_STATUS_ERROR_INVALID_VALUE;
         else
         {
            rtPhySettings.TRxDataRate = value;
            // automatically set deviation to half of data rate
            rtPhySettings.TRxDeviation = value>>1;
            AutoRxBandwith();
            if(PhyInitialized)
            {
               SetTxDataRate(value);
               SetTxFrequencyDeviation(value);
               UpdateRxModemSettings();
            }

            return PHY_STATUS_SUCCESS;
         }

      // case 0x02:
      case (TRX_DEVIATION):
         if (value > 319375L)
            return PHY_STATUS_ERROR_INVALID_VALUE;
         else
         {
            rtPhySettings.TRxDeviation = value;
            AutoRxBandwith();
            if(PhyInitialized)
            {
               SetTxFrequencyDeviation(value);
               UpdateRxModemSettings();
            }
            return PHY_STATUS_SUCCESS;
         }

      // case 0x03:
      case (RX_BAND_WIDTH):
         if (value > 620700L)
            return PHY_STATUS_ERROR_INVALID_VALUE;
         else
         {
            rtPhySettings.RxBandWidth = value;
            if(PhyInitialized)
               UpdateRxModemSettings();

            return PHY_STATUS_SUCCESS;
         }

      // case 0x04:
      case (AFC_BAND_WIDTH):
         if (value > 200000L)
            return PHY_STATUS_ERROR_INVALID_VALUE;
         else
         {
            rtPhySettings.AFCBandWidth = value;
            if(PhyInitialized)
               UpdateRxModemSettings();

            return PHY_STATUS_SUCCESS;
         }

      // case 0x05:
      case (TRX_CHANNEL_SPACING):
         if (value > 2550000L)
            return PHY_STATUS_ERROR_INVALID_VALUE;
         else
         {
            rtPhySettings.TRxChannelSpacing = value;
            if(PhyInitialized)
               SetTRxChannelSpacing(value);

            return PHY_STATUS_SUCCESS;
         }


      default:
            return PHY_STATUS_ERROR_INVALID_ADDRESS;
   }
}
//-----------------------------------------------------------------------------
// Function Name
//    rtPhyInitRadio()
//
// Return Value : PHY_STATUS
// Parameters   : none
//
//-----------------------------------------------------------------------------
PHY_STATUS rtPhyInitRadio (void)
{
   U8 status;

   // disable interrupts
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, 0x00);
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, 0x00);

      // read Si4432 interrupts to clear
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_1);
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

#ifdef ENABLE_RF_SWITCH
   //set GPIO0 to GND
   phyWrite(EZRADIOPRO_GPIO0_CONFIGURATION, 0x14);
   //set GPIO1 & GPIO2 to control the TRX switch
   phyWrite(EZRADIOPRO_GPIO1_CONFIGURATION, 0x12);
   phyWrite(EZRADIOPRO_GPIO2_CONFIGURATION, 0x15);
#else
   //set GPIOx to GND
   phyWrite(EZRADIOPRO_GPIO0_CONFIGURATION, 0x14);
   phyWrite(EZRADIOPRO_GPIO1_CONFIGURATION, 0x14);
   phyWrite(EZRADIOPRO_GPIO2_CONFIGURATION, 0x14);
#endif

   //set  cap. bank
   //status = phyRead(EZRADIOPRO_CRYSTAL_OSCILLATOR_LOAD_CAPACITANCE);
   phyWrite(EZRADIOPRO_CRYSTAL_OSCILLATOR_LOAD_CAPACITANCE, EZRADIOPRO_OSC_CAP_VALUE);

   InitConfigSettings();

   //Init Radio registers using current settings
   // TRX Frequency & Modem Settings
   SetTRxFrequency(rtPhySettings.TRxFrequency);
   SetTRxChannelSpacing(rtPhySettings.TRxChannelSpacing);
   SetTxFrequencyDeviation(rtPhySettings.TRxDeviation);
   SetTxDataRate(rtPhySettings.TRxDataRate);
   // RX Modem Settings
   UpdateRxModemSettings();

   // errata for B1 radio
   phyWrite(0x59, 0x40);
   phyWrite(0x69, 0x60);

   PhyInitialized = 1;

   return PHY_STATUS_SUCCESS;
}
//-----------------------------------------------------------------------------
// Function Name
//    InitConfigSettings()
//
// Return Value : none
// Parameters   : none
//
//-----------------------------------------------------------------------------
void InitConfigSettings (void)
{
   U8 i;
   U8 addr;
   U8 value;

  //Set the Radio Parameters from rtPhy_const
   for(i = 0; i < NUMBER_OF_INIT_REGISTERS; i++)
   {
      addr = rtPhyInitRegisters[i];
      value = rtPhyInitSettings[i];
      phyWrite(addr, value);
   }
}
//-----------------------------------------------------------------------------
// Function Name
//    SetTRxFrequency()
//
// Return Value : none
// Parameters   : U32 frequency
//
//-----------------------------------------------------------------------------
void SetTRxFrequency (U32 frequency)
{

   U8 frequencyBandSelect;
   UU16 nominalCarrierFrequency;

   if (frequency >= 480000000L )
   {
      frequency -= 480000000L;
      frequencyBandSelect  = frequency / 20000000L;
      frequency -= Multiply ((U32)frequencyBandSelect, 20000000L);
      frequency  = Divide(frequency, 625);
      frequency = LeftShift(frequency, 1);
      frequencyBandSelect |= 0x20;
   }
   else
   {
      frequency -= 240000000L;
      frequencyBandSelect  = frequency / 10000000L;
      frequency -= Multiply ((U32)frequencyBandSelect, 10000000L);
      frequency  = Divide(frequency, 625);
      frequency = LeftShift(frequency,2);
   }

   frequencyBandSelect |= 0x40;        // set sbsel

   nominalCarrierFrequency.U16 = (U16)frequency;

   phyWrite(EZRADIOPRO_FREQUENCY_BAND_SELECT, frequencyBandSelect);
   phyWrite(EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_1, nominalCarrierFrequency.U8[MSB]);
   phyWrite(EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_0, nominalCarrierFrequency.U8[LSB]);

}
//-----------------------------------------------------------------------------
// Function Name
//    SetTRxChannelSpacing()
//
// Return Value : None
// Parameters   : U32 frequency
//
//-----------------------------------------------------------------------------
void   SetTRxChannelSpacing  (U32 channelSpacing)
{
   channelSpacing = Divide(channelSpacing, 10000);

   phyWrite(EZRADIOPRO_FREQUENCY_HOPPING_STEP_SIZE, channelSpacing);
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
void SetTxFrequencyDeviation (U32 deviation)
{
   U8 txFrequencyDeviation;
   U8 modulationControl2Mask;

   deviation = Divide (deviation, 625);

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
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
void SetTxDataRate (U32 dataRate)
{
   UU16  txDataRate;
   U8    modulationControl1Mask;

   if(dataRate >= 200000)
   {
      phyWrite(EZRADIOPRO_CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE,0xED);
      phyWrite(EZRADIOPRO_AFC_TIMING_CONTROL,0x02);
   }
   else if (dataRate >= 100000)
   {
      phyWrite(EZRADIOPRO_CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE,0xC0);
      phyWrite(EZRADIOPRO_AFC_TIMING_CONTROL,0x0A);
   }
   else
   {
      phyWrite(EZRADIOPRO_CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE,0x80);
      phyWrite(EZRADIOPRO_AFC_TIMING_CONTROL,0x0A);
   }

  if (dataRate < 30000)
   {
      // dataRate = dataRate * 2^21 / 10^6  : exceeds 32-bits
      // Find largest factor of 2^n for 10^6.
      // n =  2^6 = 64
      // 10^6 = 2^6 * 15625
      // divide top and bottom by 2^6.
      // dataRate = dataRate * 2^15 / 15625
      //
      dataRate = LeftShift(dataRate, 15);
      dataRate = Divide (dataRate, 15625L);
      modulationControl1Mask = 0x20; //set txdtrtscale
   }
   else
   {
      // dataRate = dataRate * 2^16 / 10^6   : exceeds 32-bits
      // Find largest exponent of 2^n for 10^6.
      // n =  2^6 = 64
      // 10^6 = 2^6 * 15625
      // divide top and bottom by 2^6.
      // dataRate = dataRate * 2^10 / 15625 : OK for 32 bit math
      //
      dataRate = LeftShift(dataRate, 10);
      dataRate = Divide (dataRate, 15625L);
      modulationControl1Mask = 0x00; //don't set txdtrtscale
   }

   txDataRate.U16 = (U16)dataRate;

   // TX Modem Settings
   phyWrite(EZRADIOPRO_TX_DATA_RATE_1, txDataRate.U8[MSB]);
   phyWrite(EZRADIOPRO_TX_DATA_RATE_0, txDataRate.U8[LSB]);
   phyWrite(EZRADIOPRO_MODULATION_MODE_CONTROL_1,
      (phyRead(EZRADIOPRO_MODULATION_MODE_CONTROL_1)|modulationControl1Mask));

}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
void AutoRxBandwith (void)
{
   U32   BW_Modulation;
   U32   BW_Error;
   U32   BW_ModTimesP9;

   BW_Modulation = LeftShift(rtPhySettings.TRxDeviation, 1);
   BW_Modulation += rtPhySettings.TRxDataRate;

   if(BW_Modulation > 620700L)
      BW_Modulation = 620700L;

   BW_ModTimesP9 = Multiply (BW_Modulation, 9);
   BW_ModTimesP9 = Divide (BW_ModTimesP9, 10);

   BW_Error = rtPhySettings.TRxFrequency;
   BW_Error = Divide (BW_Error, 1000L);
   BW_Error = Multiply (BW_Error,  (4 * XTAL_TOLERANCE));
   BW_Error = Divide (BW_Error, 1000L);


   if(BW_Error > BW_Modulation)
   {
      rtPhySettings.RxBandWidth = BW_Error;
      rtPhySettings.AFCBandWidth = BW_Error;
   }
   else
   {
      rtPhySettings.RxBandWidth = BW_Modulation;
      rtPhySettings.AFCBandWidth = BW_ModTimesP9;
   }
}

//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
void UpdateRxModemSettings()
{
   // local rX modem variables

   U8    filterSetting;            // used with look-up table
   U16   rxOverSamplingRatio;
   U32   clockRecoveryOffset;
   U16   loopGain;

   // Use RX bandwidth to look-up filter index.
   filterSetting = LookUpFilterSetting (rtPhySettings.RxBandWidth, rtPhySettings.TRxDeviation, rtPhySettings.TRxDataRate);
   phyWrite(EZRADIOPRO_IF_FILTER_BANDWIDTH, filterSetting);

   // Use updated Rxbandwidth to calculate afc pull in and set afc limit.
   SetAFC_Limit(rtPhySettings.AFCBandWidth);

   // Use Filter setting and RxDataRate to set RxoverSamplingRatio
   rxOverSamplingRatio = CalcRxOverSamplingRatio (filterSetting, rtPhySettings.TRxDataRate);
   SetRxOverSamplingRatio(rxOverSamplingRatio);

   // Use Filter setting and RxDataRate to set ClockRecoveryOffset
   clockRecoveryOffset = CalcClockRecoveryOffset (filterSetting, rtPhySettings.TRxDataRate);
   SetClockRecoveryOffset (clockRecoveryOffset);

   // Use RxDataRate, RxoverSamplingRatio, and RxDeviation to set Loop Gain
   loopGain = CalcClockRecoveryTimingLoopGain (rtPhySettings.TRxDataRate, rxOverSamplingRatio, rtPhySettings.TRxDeviation);
   SetClockRecoveryTimingLoopGain(loopGain);
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifdef HIGH_MODULATION_INDEX_TABLES
U8 LookUpFilterSetting(U32 bandwidth, U32 deviation, U32 dataRate)
{
   U8 i;
   U32 modIndex;
   U8 filterSetting;


   bandwidth  = Divide (bandwidth, 100);

   modIndex = Divide ((deviation * 20), dataRate);

   if(modIndex < 20)
   {
      // Find largest value in table smaller than target.
      i=0;
      while (rtPhyTableRxBandwidthLT2[i] <  bandwidth)
      {
         i++;
      }

      filterSetting = rtPhyTableIF_FilterSettingLT2[i];
      // also use index to update Rxbandwidth
      rtPhySettings.RxBandWidth = (rtPhyTableRxBandwidthLT2[i]) * (100L);

   }
   else if(modIndex < 100)
   {
      // Find largest value in table smaller than target.
      i=0;
      while (rtPhyTableRxBandwidthLT10[i] <  bandwidth)
      {
         i++;
      }

      filterSetting = rtPhyTableIF_FilterSettingLT10[i];
      // also use index to update Rxbandwidth
      rtPhySettings.RxBandWidth = (rtPhyTableRxBandwidthLT10[i]) * (100L);

   }
   else
   {
      // Find largest value in table smaller than target.
      i=0;
      while (rtPhyTableRxBandwidthGE10[i] <  bandwidth)
      {
         i++;
      }

      filterSetting = rtPhyTableIF_FilterSettingGE10[i];
      // also use index to update Rxbandwidth
      rtPhySettings.RxBandWidth = (rtPhyTableRxBandwidthGE10[i]) * (100L);
   }

   return filterSetting;
}
#else
U8 LookUpFilterSetting(U32 bandwidth, U32 deviation, U32 dataRate)
{
   U8 i;
   U8 filterSetting;

   bandwidth  = Divide (bandwidth, 100);

   deviation = deviation;
   dataRate = dataRate;

   // Find largest value in table smaller than target.
   i=0;
   while (rtPhyTableRxBandwidthLT2[i] <  bandwidth)
   {
      i++;
   }

   filterSetting = rtPhyTableIF_FilterSettingLT2[i];
   // also use index to update Rxbandwidth
   rtPhySettings.RxBandWidth = (rtPhyTableRxBandwidthLT2[i]) * (100L);

   return filterSetting;
}
#endif
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
U32 CalcAFC_PullInRange (U32 rxBandwidth)
{
   U32 afcPullIn;

   afcPullIn = rxBandwidth * 8;
   afcPullIn = Divide (afcPullIn, 18);
   return afcPullIn;
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
void SetAFC_Limit (U32 afcBandWidth)
{
   U8 frequencyBandSelect;

   //TRX frequency must be set first
   frequencyBandSelect = phyRead(EZRADIOPRO_FREQUENCY_BAND_SELECT);

   if((frequencyBandSelect&0x20)==0x20)
   {
      afcBandWidth>>=1;
   }

   afcBandWidth = Divide (afcBandWidth, 1250);

   if(afcBandWidth>80)
      afcBandWidth = 80;

   phyWrite(EZRADIOPRO_AFC_LIMITER, afcBandWidth);
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
U16 CalcRxOverSamplingRatio (U8 filter, U32 rxDataRate)
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
   rxOverSamplingRatio = Divide(rxOverSamplingRatio, rxDataRate);

   if (rxOverSamplingRatio > 0x07FF)    // limit to 11 bits
      rxOverSamplingRatio = 0x07FF;

   return (U16) rxOverSamplingRatio;
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
void SetRxOverSamplingRatio (U16 rxOverSamplingRatio)
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
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
U32 CalcClockRecoveryOffset (U8 filter, U32 rxDataRate)
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
   // clockRecoveryOffset = rxDataRate * 2^20 /500000 : exceeds 32-bit
   // Find largest exponent of 2^n for 500000.
   // n =  2^5 = 32
   // 500000 = 2^5 * 15625
   // divide top and bottom by 2^5.
   // clockRecoveryOffset = dataRate * 2^15 / 15625
   // max data rate 256000
   // clockRecoveryOffset = dataRate * 2^14 / 15625 * 2^1
   //
   clockRecoveryOffset = LeftShift(clockRecoveryOffset, 14);
   clockRecoveryOffset = Divide (clockRecoveryOffset, 15625);
   clockRecoveryOffset = LeftShift(clockRecoveryOffset, 1);

   clockRecoveryOffset = LeftShift(clockRecoveryOffset, ndec_exp);

   if(dwn3_bypass)
   {
     clockRecoveryOffset = Divide (clockRecoveryOffset, 3);
   }

   if (clockRecoveryOffset > 0x000FFFFF)    // limit to 20 bits
      clockRecoveryOffset = 0x000FFFFF;

   return clockRecoveryOffset;
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
void SetClockRecoveryOffset (U32 clockRecoveryOffset)
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
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
U16 CalcClockRecoveryTimingLoopGain (U32 rxDataRate, U16 RxOverSamplingRatio, U32 RxDeviation)
{
   U32 clockRecoveryTimingLoopGain;

   // calulate clockRecoveryTimingLoopGain
   // supports data rates up to 256 kbps (0x3FFFF)
   // multiple by 2^14 first, then divide, then * 2^2, then divide again
   clockRecoveryTimingLoopGain = LeftShift(rxDataRate, 14);
   clockRecoveryTimingLoopGain = Divide (clockRecoveryTimingLoopGain, RxOverSamplingRatio);
   clockRecoveryTimingLoopGain = LeftShift(clockRecoveryTimingLoopGain, 2);
   clockRecoveryTimingLoopGain = Divide (clockRecoveryTimingLoopGain, RxDeviation);
   clockRecoveryTimingLoopGain += 2;

   if (clockRecoveryTimingLoopGain > 0x07FF)    // limit to 11 bits
      clockRecoveryTimingLoopGain = 0x07FF;

   return (U16) clockRecoveryTimingLoopGain;
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
void SetClockRecoveryTimingLoopGain (U16 clockRecoveryTimingLoopGain)
{
   UU16 crgain;

   if (clockRecoveryTimingLoopGain > 0x07FF)    // limit to 11 bits
      clockRecoveryTimingLoopGain = 0x07FF;

   crgain.U16 = clockRecoveryTimingLoopGain;

   phyWrite(EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1, crgain.U8[MSB]);
   phyWrite(EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0, crgain.U8[LSB]);
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
S8 PhySetTxPower (S8 power)
{
   if(power > 20)
   {
      power = 20;
   }
   else if (power < -1)
   {
      power = -1;
   }

   power = (power+1)/3;

   phyWrite(EZRADIOPRO_TX_POWER, power);

   power = power*3 - 1;

   return power;
}

//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
// PHY_STATUS rtPhyHopChannel (U8 channel)
// {
//    phyWrite(EZRADIOPRO_FREQUENCY_HOPPING_CHANNEL_SELECT, channel);
//    return PHY_STATUS_SUCCESS;
// }
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifndef RECEIVER_ONLY
U8 rtPhyTx (U8 length, VARIABLE_SEGMENT_POINTER(txBuffer, U8, BUFFER_MSPACE))
{
   U8 status;

   phyWrite(EZRADIOPRO_TRANSMIT_PACKET_LENGTH, length);

   phyWriteFIFO(length, txBuffer);

   // enable just the packet sent IRQ
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, EZRADIOPRO_ENPKSENT);
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, 0x00);

   // read Si4432 interrupts to clear
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_1);
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   // start TX
   phyWrite(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,(EZRADIOPRO_TXON|EZRADIOPRO_XTON));

   // wait on IRQ with 70 MS timeout
   // time out based on longest message and lowest data rate
   SetTimeOut(MILLISECONDS(70));
   while(IRQ)
   {
      if(TIMEOUT_T0)
       return PHY_STATUS_ERROR_RADIO_XTAL;
   }
   ClearTimeOut();

   return 0;
}
#endif

//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY
PHY_STATUS rtPhyRxOn (void)
{
   U8 status;

   RxPacketReceived = 0;

   // enable packet valid and CRC error IRQ
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, EZRADIOPRO_ENPKVALID|EZRADIOPRO_ENCRCERROR);
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, 0x00);

   // read Si4432 interrupts to clear
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_1);
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   // enable RX
   phyWrite(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,(EZRADIOPRO_RXON|EZRADIOPRO_XTON));

   EX0 = 1;

   return PHY_STATUS_SUCCESS;
}
#endif
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY
PHY_STATUS rtPhyRxOff (void)
{
   U8 status;

   EX0 = 0;

   // clear interrupt enables
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, 0x00);
   phyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, 0x00);

   // read Si4432 interrupts to clear
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_1);
   status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_2);

   // disable RX
   phyWrite(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,(EZRADIOPRO_XTON));

   return PHY_STATUS_SUCCESS;
}
#endif
//-----------------------------------------------------------------------------
// delay ()
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
// timeout ()
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
// T0_ISR() used with timeout
//-----------------------------------------------------------------------------
INTERRUPT(T0_ISR, INTERRUPT_TIMER0)
{
   ET0 = 0;
   TF0 = 0;
   TR0 = 0;
}
//-----------------------------------------------------------------------------
// ClearTimeOut() used to cancel TimeOut
//-----------------------------------------------------------------------------
void ClearTimeOut (void)
{
   ET0 = 0;
   TF0 = 0;
   TR0 = 0;
}
//=============================================================================
//
// spi Functions for rtPhy.c module
//
//=============================================================================
//
// Notes:
//
// The spi functions in this module are for use in the main thread. The EZMacPro API calls should
// only be used in the main thread. The SPI is used by the main thread, as well as the external
// interrupt INT0 thread, and the T0 interrupt. Since all SPI tranfers are multiple bytes. It is
// important that MAC interrupts are disabled when using the SPI from the main thread.
//
// These SPI functions may be interrupted by other interrupts, so the double buffered transfers
// are managed with this in mind.
//
// The double buffered transfer maximizes the data throughput and elimiates any software
// delay between btyes. The clock is continuous for the two byte transfer. Instead of using the
// SPIF flag for each byte, the TXBMT is used to keep the transmit buffer full, then the SPIBSY
// bit is used to determine when all bits have been transfered. The SPIF flag should not be
// polled in double buffered transfers.
//
//-----------------------------------------------------------------------------
// Function Name phyWrite()
//
// Return Value   : None
// Parameters :
//    U8 reg - register address from the si4432.h file.
//    U8 value - value to write to register
//
// Notes:
//
//    MAC interrupts are preserved and restored.
//    Write uses a Double buffered transfer.
//
//-----------------------------------------------------------------------------
void phyWrite (U8 reg, U8 value)
{
   bit restoreEA;

   // disable interrupts during SPI transfer
   restoreEA = EA;
   EA = 0;

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

   // Restore interrupts after SPI transfer
   EA = restoreEA;
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : U8 value - value returned from the si4432 register
// Parameters   : U8 reg - register address from the si4432.h file.
//
//-----------------------------------------------------------------------------
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

   bit restoreEA;

   // disable interrupts during SPI transfer
   restoreEA = EA;
   EA = 0;

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

   // Restore interrupts after SPI transfer
   EA = restoreEA;

   return value;
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifndef RECEIVER_ONLY
void phyWriteFIFO (U8 n, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE))
{
   bit restoreEA;

   // disable interrupts during SPI transfer
   restoreEA = EA;
   EA = 0;

   NSS1 = 0;                            // drive NSS low
   SPIF1 = 0;                           // clear SPIF
   SPI1DAT = (0x80 | EZRADIOPRO_FIFO_ACCESS);

   while(n--)
   {
      while(!TXBMT1);                   // wait on TXBMT
      SPI1DAT = *buffer++;             // write buffer
   }

   while(!TXBMT1);                      // wait on TXBMT
   while((SPI1CFG & 0x80) == 0x80);    // wait on SPIBSY

   SPIF1 = 0;                           // leave SPI  cleared
   NSS1 = 1;                            // drive NSS high

   // Restore interrupts after SPI transfer
   EA = restoreEA;
}
#endif
//=============================================================================
//
// Receiver Functions
//
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY
PHY_STATUS  rtPhyGetRxPacket(U8 *pLength, VARIABLE_SEGMENT_POINTER(rxBuffer, U8, BUFFER_MSPACE))
{
   bit restoreEX0;
   U8 i;

   if(RxPacketReceived)
   {
      // disable interrupts during copy
      restoreEX0 = EX0;
      EX0 = 0;

      for(i=0;i<RxPacketLength;i++)
      {
         rxBuffer[i]=RxIntBuffer[i];
      }

      RxPacketReceived = 0;

      EX0 = restoreEX0;

      *pLength = RxPacketLength;

      return PHY_STATUS_SUCCESS;
   }
   else
   {
      return PHY_STATUS_ERROR_NO_PACKET;
   }
}

#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY
INTERRUPT(Receiver_ISR, INTERRUPT_INT0)
{
   U8 status;

   IE0 = 0;

   status = RxIntPhyRead(EZRADIOPRO_INTERRUPT_STATUS_2);
   status = RxIntPhyRead(EZRADIOPRO_INTERRUPT_STATUS_1);

   if((status & EZRADIOPRO_IPKVALID)==EZRADIOPRO_IPKVALID)
   {
      if(RxPacketReceived==0)
      {
         RxPacketLength = RxIntPhyRead(EZRADIOPRO_RECEIVED_PACKET_LENGTH);
         RxIntphyReadFIFO(RxPacketLength, RxIntBuffer);
         RxPacketReceived = 1;
      }
      else
      {
         // Clear RX FIFO
         status = RxIntPhyRead(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2);
         status |= EZRADIOPRO_FFCLRRX;
         RxIntPhyWrite(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2, status);
         status &= ~EZRADIOPRO_FFCLRRX;
         RxIntPhyWrite(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2, status);
      }
   }
   else if((status & EZRADIOPRO_ICRCERROR)==EZRADIOPRO_ICRCERROR)
   {
      RxErrors++;
   }
   else
   {
   }

   // enable packet valid and CRC error IRQ
   RxIntPhyWrite(EZRADIOPRO_INTERRUPT_ENABLE_1, EZRADIOPRO_ENPKVALID|EZRADIOPRO_ENCRCERROR);
   RxIntPhyWrite(EZRADIOPRO_INTERRUPT_ENABLE_2, 0x00);

   // enable RX again
   RxIntPhyWrite(EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1,(EZRADIOPRO_RXON|EZRADIOPRO_XTON));
}
#endif
//=============================================================================
//
// spi Functions for Rx Receiver interrupt
//
//=============================================================================
//-----------------------------------------------------------------------------
// Function Name RxIntPhyWrite()
//
// Return Value   : None
// Parameters :
//    U8 reg - register address from the si4432.h file.
//    U8 value - value to write to register
//
// Notes:
//
//    MAC interrupts are preserved and restored.
//    Write uses a Double buffered transfer.
//
//-----------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY
void RxIntPhyWrite (U8 reg, U8 value)
{
   bit restoreEA;

   // disable interrupts during SPI transfer
   restoreEA = EA;
   EA = 0;

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

   // Restore interrupts after SPI transfer
   EA = restoreEA;
}
#endif
//-----------------------------------------------------------------------------
// Function Name
//    RxIntPhyRead()
//
// Parameters   : U8 reg - register address from the si4432.h file.
// Return Value : U8 value - value returned from the si4432 register
//
//-----------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY
U8 RxIntPhyRead (U8 reg)
{
   U8 value;

   bit restoreEA;

   // disable interrupts during SPI transfer
   restoreEA = EA;
   EA = 0;

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

   // Restore interrupts after SPI transfer
   EA = restoreEA;

   return value;
}
#endif
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
// Note that this function does no use double buffered data transfers to prevent data loss.
// This function may be interrupted by another process and should not loose dat or hang on the
// SPIF flag.
//
//-----------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY
void RxIntphyReadFIFO (U8 n, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE))
{
   bit restoreEA;

   // disable interrupts during SPI transfer
   restoreEA = EA;
   EA = 0;

   NSS1 = 0;                            // drive NSS low
   SPIF1 = 0;                           // clear SPIF
   SPI1DAT = (EZRADIOPRO_FIFO_ACCESS);
   while(!SPIF1);                       // wait on SPIF
   ACC = SPI1DAT;                      // discard first byte

   while(n--)
   {
      SPIF1 = 0;                        // clear SPIF
      SPI1DAT = 0x00;                  // write anything
      while(!SPIF1);                    // wait on SPIF
      *buffer++ = SPI1DAT;             // copy to buffer
   }

   SPIF1 = 0;                           // leave SPIF cleared
   NSS1 = 1;                            // drive NSS high

   // Restore interrupts after SPI transfer
   EA = restoreEA;
}
#endif
//-----------------------------------------------------------------------------
// Function Divide
//
// Return Value : quotient
// Parameters   : dividend, divisor
//
//-----------------------------------------------------------------------------
#ifdef CHECK_32_BIT_MATH
U32   Divide (U32 dividend, U32 divisor)
{
   U32 quotient;

   if(divisor==0)
   {
      MathError();
      return (0xFFFFFFFF);             // indicate overflow
   }
   else
   {
      dividend += (divisor >> 1);
      quotient = dividend / divisor;
      return quotient;
   }
}
#else
U32   Divide (U32 dividend, U32 divisor)
{
   U32 quotient;

   dividend += (divisor >> 1);
   quotient = dividend / divisor;
   return quotient;
}
#endif
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifdef CHECK_32_BIT_MATH
U8   CountBits (U32 number)
{
   U8 count;
   U8 i;

   count = 0;
   i=32;
   while(number && i)
   {
      number>>=1;
      count++;
      i--;
   }

   return count;
}
#endif
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifdef CHECK_32_BIT_MATH
U32   Multiply (U32 x, U32 y)
{
   U8 n;

   n = CountBits(x) + CountBits(y);

   if(n<=32)
   {
      return (x * y);
   }
   else
   {
      MathError();
      return (0xFFFFFFFF);             // indicate overflow
   }
}
#else
U32   Multiply (U32 x, U32 y)
{
   return (x * y);
}
#endif
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifdef CHECK_32_BIT_MATH
U32   LeftShift (U32 number, U8 shift)
{
   U8 n;

   n = CountBits(number);
   n += shift;
   if(n <=32)
   {
      return (number<<shift);
   }
   else
   {
      MathError();
      return (0xFFFFFFFF);             // indicate overflow
   }
}
#else
U32   LeftShift (U32 number, U8 shift)
{
   return (number<<shift);
}
#endif
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifdef CHECK_32_BIT_MATH
void  MathError(void)
{
   //RSTSRC = 0x10;                    // SW reset
}
#endif
//-----------------------------------------------------------------------------
//=============================================================================
// end rtPhy..c
//=============================================================================
