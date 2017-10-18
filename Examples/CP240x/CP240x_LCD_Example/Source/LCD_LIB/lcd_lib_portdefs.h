//-----------------------------------------------------------------------------
// lcd_lib_portdefs.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// Contains port definitions for the CP240x LCD controller
//
// Target:         Multiple
// Tool chain:     Generic
//                 Silicon Laboratories IDE version 3.40 
// Project Name:   CP240x_Firmware
//
// 
//
// Release 1.0
//    -26 OCT 2009
//

//-----------------------------------------------------------------------------
// Development Board Definition
//-----------------------------------------------------------------------------
//
// The global <DEV_BOARD> definition can be one of the following values:
// 
//    F930DK -- Selects the 'F930 Development Kit Configuration
//    F912DK -- Selects the 'F912 Development Kit Configuration
//    SI1000DK -- Selects the Si1000 Development Kit Configuration
//    SI1050DK -- Selects the Si1050 Development Kit Configuration
//    CUSTOM   -- Not using a Silicon Laboratories Development Kit

#define DEV_BOARD              F930DK

//-----------------------------------------------------------------------------
// Interrupt and Reset Pin
//-----------------------------------------------------------------------------

#if(DEV_BOARD == F930DK || DEV_BOARD == F912DK)
   SBIT (INT, SFR_P0, 1);
   SBIT (RST, SFR_P0, 0);
#elif(DEV_BOARD == SI1000DK)
   SBIT (INT, SFR_P0, 7);
   SBIT (RST, SFR_P0, 0);
#elif(DEV_BOARD == SI1050DK)
   SBIT (INT, SFR_P1, 4);
   SBIT (RST, SFR_P1, 5);
#else
   #error "Please define location of INT and RST"

   // Uncomment the lines below and modify to select the correct
   // Port I/O pins connected to the /INT and /RST signal.  If a 
   // power on reset is used instead of the reset pin, please 
   // assign the port pin which powers the CP240x to the "RST" label

   //SBIT (INT, SFR_P0, 1);
   //SBIT (RST, SFR_P0, 0);

#endif


//-----------------------------------------------------------------------------
// Interface Definition
//-----------------------------------------------------------------------------
//
// The global <BUS_INTERFACE> definition can be one of the following values:
// 
//    SPI -- Selects the SPI interface for commmuniction with the CP240x
//    SMBUS -- Selects the SMBUS interface for commmuniction with the CP240x
//
#define BUS_INTERFACE              SPI


//-----------------------------------------------------------------------------
// Definitions when using SPI
//-----------------------------------------------------------------------------
#if (BUS_INTERFACE == SPI)

   // Define the SPI peripheral to use { SPI0 or SPI1 }   
   #if(DEV_BOARD == F930DK || DEV_BOARD == F912DK || DEV_BOARD == SI1050DK)
      #define SPI_N                 SPI1
   #elif(DEV_BOARD == SI1000DK)
      #define SPI_N                 SPI0
   #else
      #error "Please define the SPI peripheral to use"
      //#define SPI_N                 SPI0
   #endif
         

   // The SPI bit rate in Hz
   #define SPI_FREQUENCY           1000000

   // Define the port pin to be used for the /NSS signal
   #if(DEV_BOARD == F930DK || DEV_BOARD == F912DK || DEV_BOARD == SI1050DK)
      SBIT (NSS, SFR_P1, 3);
   #elif(DEV_BOARD == SI1000DK)
      SBIT (NSS, SFR_P2, 5);
   #else
      #error "Please define location of NSS"
      //SBIT (NSS, SFR_P1, 3);
   #endif
   

#endif

//-----------------------------------------------------------------------------
// Definitions when using SMBUS
//-----------------------------------------------------------------------------
#if (BUS_INTERFACE == SMBUS)

   // The timer used for SMBus. Can be { TIMER0, TIMER1, TIMER2, 
   // TIMER2_HB, and TIMER2_LB }
   // The timer will be automatically configured by the library.  The timer
   // registers should not be accessed from outside the library.  When using
   // split mode on timer2, the other "unused" 8-bits may be used in the
   // system as a general purpose 8-bit timer. If "TIMER2" is selected, then
   // all 16-bits of the timer will be used.
   #define SMBUS_TIMER           TIMER0

   // The SMBus bit rate in Hz
   #define SMBUS_FREQUENCY       400000

   // Define the port pin to be used for the /PWR signal
   #if(DEV_BOARD == F930DK || DEV_BOARD == F912DK)
      SBIT (PWR, SFR_P1, 3);
   #elif(DEV_BOARD == SI1000DK)
      SBIT (PWR, SFR_P2, 5);
   #else
      #error "Please define location of PWR"
      //SBIT (PWR, SFR_P1, 3);
   #endif
   
   // This timeout requires all 16 bits of Timer3. Unless SMBus is a shared
   // bus with multiple devices, the recommended value is 0 to free up
   // on-chip timers.
   #define ENABLE_SCL_LOW_TIMEOUT  0

   // Device addresses (7 bits, lsb is a don't care)
   #define  CP240X_SLAVE_ADDR     0x74       // Device address for CP240x slave

#endif


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
