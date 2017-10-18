//=============================================================================
// SPI1_Master.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This reusable module includes API functions for SPI1 master mode
// transfers using the DMA. Include the header file for this module
// to use the API functions.
//
// This module does not include the SPI1 init or the DMA ISR. These
// functions are located in main and must be copied to the users
// main module.
//
// The DMA channels are configured in the DMA-defs.h header file.
//
// These functions are blocking and use IDLE mode to wait on the
// DMA transfer. The Low-power Idle mode can be used to save
// additional power.
//
// There is some overhead for setting up the DMA. Short one or two
// byte transfers may be more efficiently coded using the SPI1 in
// polled mode without the DMA. There is no known issues mixing
// master polled mode transfers and master DMA transfers.
//
// Target:
//    C8051F960
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
//    C8051F960 Example Code
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <C8051F960_defs.h>
#include "SPI1_Master.h"
#include "DMA_defs.h"
//=============================================================================
// API Functions
//=============================================================================
//-----------------------------------------------------------------------------
// SPI1_MasterOutIn()
//
// parameters:
//    size            - size for both input and output transfers
//    outPointer      - xdata pointer to SPI data transmitted on MOSI pin
//    inPointer       - xdata pointer to SPI data received on MISO pin
//
// returns:
//   nothing
//
// Description:
//
// This function facilitates a bidirectional master mode data transfer
// using the DMA. A bidirectional SPI transfer will receive a number of bytes
// equal to the number of bytes transmitted. The receive buffer must
// be large enough to contain the size of the transfer.
//
// The output data must be store in xram prior to calling this function.
// The input data will be in xram when this function returns.
//
// Note that the first byte to be transferred must be at the lowest address.
// Likewise the first byte to be received will be at the lowest address.
//
// This function uses SPI1 with the DMA. The static DMA channel
// allocation is defined in DMA_defs.h.
//
// Since two DMA channels are used, the DMA interrupt is enabled only
// for the channel receiving the MISO data. So an interrupt will not
// occur until all data is received.
//
// This function puts the MCU code into idle mode while the SPI transfer
// is in process.
//
// A interrupt is needed to wake up from Idle mode. This requires that the
// global interrupts are enabled. Also, a small DMA ISR that disables EIE2
// bit 5 is required in the main module.
//
// Note that the SPI interface does not have any handshaking or flow control.
// So the all the requested output data will be transmitted without waiting
// on the slave peripheral. The system designer must ensure that the slave
// peripheral can accept all the data at the transfer rate.
//
// Note there is considerable code overhead in setting up a DMA SPI transfer.
// So it may be more efficient to use polling for short transfers.
//
// The overhead of this function is about 30 SYSCLK cycles. This is about
// about two byte times using SYSCLK/2 for the SPI clock.
//
// This function does not manage the NSS pin. The NSS pin might be asserted
// before calling this function and negated afterward. This will depend
// on the peripheral requirements for NSS.
//
//-----------------------------------------------------------------------------
void SPI1_MasterOutIn (U16 size,
   VARIABLE_SEGMENT_POINTER (outPointer, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER (inPointer, U8, SEG_XDATA))
{
   UU16 addr;
   UU16 value;

   SFRPAGE = DPPE_PAGE;
   DMA0EN &= ~(SPI1_MASK);             // Disable SPI DMA channel for now.

   // Select channel and peripheral request.
   // Wrapping not used for any SPI operation.
   // SPI1_IN means XRAM to SPI1DAT
   // This data is transmitted on MOSI
   // So the out pointer is used.
   DMA0SEL = SPI1_IN_CHANNEL;
   DMA0NCF = SPI1_IN_PERIPHERAL_REQUEST;
   DMA0NMD = NO_WRAPPING;
   addr.U16 = (U16)(outPointer);
   DMA0NBAL = addr.U8[LSB];
   DMA0NBAH = addr.U8[MSB];
   value.U16 = size;
   DMA0NSZL = value.U8[LSB];
   DMA0NSZH = value.U8[MSB];
   DMA0NAOL = 0;
   DMA0NAOH = 0;

   // Configure DMA SPI OUT channel
   // Select channel and peripheral request
   // Enable interrupt
   // SPI1_OUT means SPI1DAT to XRAM
   // This data is received on MISO
   // So the in pointer is used.
   DMA0SEL = SPI1_OUT_CHANNEL;
   DMA0NCF = SPI1_OUT_PERIPHERAL_REQUEST|DMA_INT_EN;
   DMA0NMD = NO_WRAPPING;
   addr.U16 = (U16)(inPointer);
   DMA0NBAL = addr.U8[LSB];
   DMA0NBAH = addr.U8[MSB];
   value.U16 = size;
   DMA0NSZL = value.U8[LSB];
   DMA0NSZH = value.U8[MSB];
   DMA0NAOL = 0;
   DMA0NAOH = 0;

   // Clear SPI1 DMA channel bits in DMA0INT sfr using mask.
   DMA0INT &= ~(SPI1_MASK);

   // Set SPI1 DMA channel bits in DMA0EN sfr using mask.
   // This enables the DMA and initiates the SPI1 transfer.
   DMA0EN = (SPI1_MASK);

    // enable DMA interrupt to terminate Idle mode
    EIE2 |= 0x20;

   // This do...while loop ensures that the CPU will remain in Idle mode
   // until SPI1_OUT DMA channel transfer is complete.
   do
   {
      #ifdef DMA_TRANSFERS_USE_IDLE
      PCON |= 0x01;                    // go to Idle mode
      #endif
      // wait on DMA  interrupt
   }  while((DMA0INT & SPI1_OUT_MASK)==0);

   // Clear SPI1 bits (SPI1_IN & SPI1_OUT) bits in DMA0EN sfr using mask.
   DMA0EN &= ~(SPI1_MASK);
   // Clear SPI1 bits (SPI1_IN & SPI1_OUT) bits in DMA0INT sfr using mask.
   DMA0INT &= ~(SPI1_MASK);
}
//-----------------------------------------------------------------------------
// SPI1_MasterWriteOnly()
//
// parameters:
//    size            - size for output transfers
//    outPointer      - xdata pointer to SPI data transmitted on MOSI pin
//
// returns:
//   nothing
//
// Description:
//
// This function facilitates a unidirectional master mode data transfer
// using the DMA. Any data received on the MISO pin will be discarded.
//
// The output data must be stored in xram prior to calling this function.
//
// Note that the first byte to be transferred must be at the lowest address.
//
// This function uses SPI1 with the DMA. The static DMA channel
// allocation is defined in DMA_defs.h.
//
// This function puts the MCU code into idle mode while the SPI transfer
// is in process.
//
// Since only DMA channels are used, the DMA interrupt is enabled only
// for the channel transmitting the data. There will be one final byte
// left in the transmitter upon completion of the DMA transfer.
//
// A interrupt is needed to wake up from Idle mode. This requires that the
// global interrupts are enabled. Also, a small DMA ISR that disables EIE2
// bit 5 is required in the main module.
//
// Note that the SPI interface does not have any handshaking or flow control.
// So the all the requested output data will be transmitted without waiting
// on the slave peripheral. The system designer must ensure that the slave
// peripheral can accept all the data at the transfer rate.
//
// Note there is considerable code overhead in setting up a DMA SPI transfer.
// So it may be more efficient to use polling for short transfers.
//
// The overhead of this function is about 20 SYSCLK cycles. This is a little
// more than one byte times using SYSCLK/2 for the SPI clock.
//
// This function does not manage the NSS pin. This will depend on the
// peripheral requirements for NSS.  The NSS pin might be asserted
// before calling this function. The NSS should not be negated until after
// the last byte is transmitted. The TXBMT1 and SPIBUSY1 bits are used to
// wait until the last byte has been transmitted.
//
//-----------------------------------------------------------------------------
void SPI1_MasterOutOnly (U16 size,
   VARIABLE_SEGMENT_POINTER (outPointer, U8, SEG_XDATA))
{
   UU16 addr;
   UU16 value;

   SFRPAGE = DPPE_PAGE;

   DMA0EN &= ~SPI1_IN_MASK;            // Disable SPI DMA channel for now.

   // Select channel and peripheral request.
   // Wrapping not used for any SPI operation.
   // Enable interrupt
   // SPI1_IN means XRAM to SPI1DAT.
   // This data is transmitted on MOSI.
   // So it is really SPI1 Master Out data.
   DMA0SEL = SPI1_IN_CHANNEL;
   DMA0NCF = SPI1_IN_PERIPHERAL_REQUEST|DMA_INT_EN;
   DMA0NMD = NO_WRAPPING;
   addr.U16 = (U16)(outPointer);
   DMA0NBAL = addr.U8[LSB];
   DMA0NBAH = addr.U8[MSB];
   value.U16 = (size);
   DMA0NSZL = value.U8[LSB];
   DMA0NSZH = value.U8[MSB];
   DMA0NAOL = 0;
   DMA0NAOH = 0;

   // Note the SPI1_OUT peripheral request is not used.
   // Only one DMA channel is used.

   // Clear SPI1 DMA channel bits in DMA0INT sfr using mask.
   DMA0INT &= ~SPI1_IN_MASK;


   // Set SPI1 DMA channel bits in DMA0EN sfr using mask.
   // This enables the DMA and initiates the SPI1 transfer.
   DMA0EN = SPI1_IN_MASK;

    // enable DMA interrupt to terminate Idle mode
   EIE2 |= 0x20;                       // enable DMA interrupt to terminate Idle mode

   // This do...while loop ensures that the CPU will remain in Idle mode
   // until SPI1_IN DMA channel transfer is complete.
   do
   {
      #ifdef DMA_TRANSFERS_USE_IDLE
      PCON |= 0x01;                    // go to Idle mode
      #endif
      // wait on DMA  interrupt
   }  while((DMA0INT & SPI1_IN_MASK)==0);

   // Clear SPI1 bit (SPI1_IN) bit in DMA0EN sfr using mask.
   DMA0EN &= ~SPI1_IN_MASK;
   // Clear SPI1 bit (SPI1_IN) bit in DMA0INT sfr using mask.
   DMA0INT &= ~SPI1_IN_MASK;

   // Waiting on the TXBMT1 and SPIBSY bit will ensure that the last byte
   // has been transmitted. This step is not necessary for the DMA transfer,
   // but may be necessary if the NSS pin is toggled immediately after calling
   // this function. The slave peripheral may not receive the last byte if
   // the NSS pin is toggled before the last byte is transmitted on the MOSI
   // pin.
   SFRPAGE = SPI1_PAGE;
   while(!TXBMT1);                     //wait on TXBMT1
   while((SPI1CFG & 0x80) == 0x80);    //wait on SPIBSY1

}
//=============================================================================
// End of file
//=============================================================================

