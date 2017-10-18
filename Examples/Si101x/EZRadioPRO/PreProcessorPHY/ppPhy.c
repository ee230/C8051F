//=============================================================================
// ppPhy.c
//=============================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// Target:
//    Any Silicon Labs C8051 MCU.
//
// IDE:
//    Silicon Laboratories IDE   Version 3.6
//
// Tool Chains:
//    Keil        9
//    SDCC        3
//    Raisonance  7
//
// Project Name:
//    PreProcessorPHY
//
// Version 2.0
//    - 08NOV2011
//
// This software must be used in accordance with the End User License
// Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// EZRadioPRO PreProcessor PHY
// Version 2.0 Release Notes
//
// * Added automatic RX bandwidth calculator
// * Extended range to 256 kbps
// * Added high modulation-index tables
// * Fully tested against known-good Test Vectors
//
// The mandatory definitions are:
// * TRX_FREQUENCY (240000000L to 959999999L Hz)
// * TRX_DATA_RATE (1200L to 256000L bps)
//
// ALL DEFINITIONS MUST HAVE A TRAILING L FOR CORRECT 32-BIT CALCUALTION.
//
// The TRX_FREQUENCY should match the hardware matching network frequency.
//
// A lower data rate will generally provide a greater sensitivity and longer
// range. But the RX bandwidth may be limited by the frequency error. A data
// rate of 40 kbps provides the best frequency utilization for 868/915 MHz
// carrier and a 20 ppm xtal. A data rate of 20 kbps provides the best
// frequency utilization for a 343/47 MHz carrier and a 20 ppm xtal.
// Data rates above 100 kHz should be used cautiously. Please test the
// hardware using the default settings first and then increase the data rate
// cautiously.
//
//
// The optional parameters are:
// * TRX_DEVIATION (625L to 319375 Hz)
// * RX_BAND_WIDTH (2600L to 620000L Hz)
// * TRX_CHANNEL_SPACING (0L to 2550000L Hz)
//
// Leaving TRX_DEVIATION undefined with automatically use a deviation of
// one-half the data rate. This will provide good results for most
// applications. This provides a modulation index of unity, which is ideal
// for GFSK. Use higher deviations cautiously. Higher deviations may provide
// better frequency utilization at lower data rates and may also provide some
// frequency spreading. If in doubt, leave TRX_DEVIATION undefined.
//
// Leaving the RX_BAND_WIDTH undefined will automatically calculate the
// receiver bandwidth based on the specified data rate deviation and crystal
// tolerance. Manually defining the RX bandwidth will override the automatic
// RX bandwidth calculation.
//
// Note that manually setting the RX_BAND_WIDTH to a value smaller that the
// frequency error may result in less than  optimum performance. For example,
// using a data rate of 1250 bps with a deviation of 625 Hz, the modulation
// bandwidth is only 2500 Hz. But the frequency error is 72 kHz
// (900 MHz 20ppm). So using a RX_BAND_WIDTH of 2500 may be to narrow to
// receive anything.
//
// When in doubt, use the automatic RX_BAND_WIDTH calculation.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//
// These includes must be in a specific order. Dependencies listed in comments.
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include "hardware_defs.h"                    // requires compiler_defs.h
#include "ppPhy.h"
#include "ppPhy_const.h"
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bit RxPacketReceived;
SEGMENT_VARIABLE (RxPacketLength, U8, BUFFER_MSPACE);
SEGMENT_VARIABLE (RxErrors, U8, BUFFER_MSPACE);
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
SEGMENT_VARIABLE (RxIntBuffer[64], U8, BUFFER_MSPACE);
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void  Delay (U16);
void  SetTimeOut (U16);
void  ClearTimeOut (void);
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PHY_STATUS InitSoftwareReset (void);
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
U8    RxIntPhyRead (U8);
void  RxIntPhyWrite (U8, U8);
void  RxIntphyReadFIFO (U8, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE));
//=============================================================================
//
// API Functions
//
//=============================================================================
//-----------------------------------------------------------------------------
// Function Name
//    ppPhyInit()
//
// Parameters   : none
// Return Value : status
//
// After a pin reset or other reset the state of the Radio is unknown.
// The MCU will wait for full POR duration, then figure out if the
// Radio needs a software reset.
//
//-----------------------------------------------------------------------------
PHY_STATUS ppPhyInit(void)
{
   U8 status;

   SDN = 0;

   Delay(MILLISECONDS(25));

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
//    InitSoftwareReset()
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
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
PHY_STATUS ppPhyIdle(void)
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
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
PHY_STATUS ppPhyStandby (void)
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
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
PHY_STATUS ppPhyShutDown (void)
{
   P2MDOUT |= 0x40;                    // Enable SDN push pull
   SDN = 1;

   return PHY_STATUS_SUCCESS; // success
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
PHY_STATUS ppPhyReStart(void)
{
   U8 status;

   SDN = 0;

   Delay(MILLISECONDS(2));

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
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
PHY_STATUS ppPhyInitRadio (void)
{
   U8 status;
   U8 i;
   U8 addr;
   U8 value;

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

   //Set the Radio Parameters from ppPhy_const
   for(i = 0; i < NUMBER_OF_REGISTERS; i++)
   {
      addr = ppPhyRegisters[i];
      value = ppPhySettings[i];
      phyWrite(addr, value);
   }

   // errata for B1 radio
   phyWrite(0x59, 0x40);

   return PHY_STATUS_SUCCESS;
}
//-----------------------------------------------------------------------------
// Function Name
//
// Return Value : None
// Parameters   :
//
//-----------------------------------------------------------------------------
#ifndef RECEIVER_ONLY
PHY_STATUS ppPhyTx (U8 length, VARIABLE_SEGMENT_POINTER(txBuffer, U8, BUFFER_MSPACE))
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
       return PHY_STATUS_TRANSMIT_ERROR;
   }
   ClearTimeOut();

   //status = phyRead(EZRADIOPRO_INTERRUPT_STATUS_1);

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
PHY_STATUS ppPhyRxOn (void)
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
PHY_STATUS ppPhyRxOff (void)
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
// Delay ()
//-----------------------------------------------------------------------------
void Delay (U16 ticks)
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
// spi Functions for ppPhy.c module
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
PHY_STATUS  ppPhyGetRxPacket(U8 *pLength, VARIABLE_SEGMENT_POINTER(rxBuffer, U8, BUFFER_MSPACE))
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
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
// Function Name
//    RxIntPhyRead()
//
// Parameters   : U8 reg - register address from the si4432.h file.
// Return Value : U8 value - value returned from the si4432 register
//
//-----------------------------------------------------------------------------
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
//=============================================================================
// end ppPhy..c
//=============================================================================
