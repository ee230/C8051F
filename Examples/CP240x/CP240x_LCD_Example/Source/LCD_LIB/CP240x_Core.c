//-----------------------------------------------------------------------------
// CP240x_Core.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// This file contains the implementation for common tasks that may be 
// performed on the CP240x.
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
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F930_defs.h>
#include "lcd_lib.h"

//-----------------------------------------------------------------------------
// Global Variables 
//-----------------------------------------------------------------------------
static U8 CP240x_CLK;                  // Save CP240x clock setting whenever
                                       // entering ULP mode. Will restore
                                       // CP240x clock whenever exiting ULP. 

U16 CP240x_Measured_RTCCLK;            // Measured value of RTC

U8 CP240x_MSCN_Local;                  // Local copy of the MSCN register
U8 CP240x_RTC0CN_Local;                // Local copy of RTC0CN register

                                       // Local copy of PortIO registers
CP240X_PORTREGS xdata CP240x_PortIO_Registers;

//-----------------------------------------------------------------------------
// CP240x_Reset
//-----------------------------------------------------------------------------
//
// Return Value : Can return various error codes to be defined later.
//    
//   
// Parameters   : N/A
//
// Resets the CP240x
//
//-----------------------------------------------------------------------------
S8 CP240x_Reset(void)
{
   U8 rev;
   //----------------------------------
   // Generate a 15 us pulse on reset 
   //----------------------------------
   RST = 0;                            // Assert the CP240x Reset

   Wait_US(15);                        // Allow the CP240x enough time
                                       // to recognize the reset event                                                                          

   RST = 1;                            // De-Assert the CP240x Reset   
   
   while(INT);                         // Wait for reset complete int.   


   //----------------------------------
   // Verify that the serial interface
   // is working properly
   //----------------------------------
   rev = CP240x_RegRead(REVID);

   if(rev != 0x01)
   {
      while(1);                        // Check Port I/O Config
   } 

   //---------------------------------
   // Disable all interrupts by 
   // clearing INT0EN and INT1EN
   //---------------------------------
   CP240x_RegBlockWrite(INT0EN, "\0", 2);   


   //---------------------------------
   // Initialize Registers
   //---------------------------------

   
   CP240x_MSCN_Local = 0x00;           // Reset value of the MSCN register
   
   // Read the reset values of all writable Port I/O configuration registers
   CP240x_RegBlockRead(P0OUT, &CP240x_PortIO_Registers.P0OUT_Local, CP240X_NUM_PORTREGS_CONFIG);
   
   CP240x_RegWrite(RTCKEY,0xA5);       // Unlock the SmaRTClock interface
   CP240x_RegWrite(RTCKEY,0xF1);  

   return NO_ERROR;


}


//-----------------------------------------------------------------------------
// CP240x_ClearAllFlags
//-----------------------------------------------------------------------------
//
// Return Value : none
//    
//   
// Parameters   : N/A
//
// Clears All CP240x Interrupts and ensures that /INT goes high
//
//-----------------------------------------------------------------------------
void CP240x_ClearAllFlags(void)
{

   CP240x_MSCN_SetBits(0x40);
   
}

//-----------------------------------------------------------------------------
// CP240x_ClearAllInterrupts
//-----------------------------------------------------------------------------
//
// Return Value : Can return various error codes to be defined later.
//    
//   
// Parameters   : N/A
//
// Clears All CP240x Interrupts and ensures that /INT goes high
//
//-----------------------------------------------------------------------------
S8 CP240x_ClearAllInterrupts(void)
{
   U16 interrupt_status;
   
   do
   {
      CP240x_MSCN_SetBits(0x40);
      CP240x_RegBlockRead(INT0, (U8*) &interrupt_status, 2); 
      
      // Check for clock failure
      if(interrupt_status & 0x0800)
      {
         CP240x_RTC_Failure();
      }

   } while(!INT); 
   
   return NO_ERROR;
}

//-----------------------------------------------------------------------------
// MSCN Management
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// CP240x_MSCN_Update
//-----------------------------------------------------------------------------
//
// Return Value : None
//    
//   
// Parameters   : value
//
// Updates MSCN with the new value and stores a copy in MSCN_Local.
//
//-----------------------------------------------------------------------------
void CP240x_MSCN_Update (U8 value)
{
   CP240x_MSCN_Local = (value & ~0x42);
   CP240x_RegWrite(MSCN, value);
}

//-----------------------------------------------------------------------------
// RTC0CN Management
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// CP240x_RTC0CN_Update
//-----------------------------------------------------------------------------
//
// Return Value : None
//    
//   
// Parameters   : value
//
// Updates RTC0CN with the new value and stores a copy in RTC0CN_Local.
//
//-----------------------------------------------------------------------------
void CP240x_RTC0CN_Update (U8 value)
{
   CP240x_RTC0CN_Local = (value & ~0x03);
   CP240x_RTC_Write(RTC0CN, value);
}

//-----------------------------------------------------------------------------
// RTC Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CP240x_RTC_Write ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : 1) unsigned char reg - address of RTC register to write
//                2) unsigned char value - the value to write to <reg>
//
// This function will Write one byte to the specified RTC register.
// Using a register number greater that 0x0F is not permited.
//-----------------------------------------------------------------------------
void CP240x_RTC_Write (U8 addr, U8 value)
{
   UU16 addr_value;
     
   addr_value.U8[MSB] = (0x10 | addr) ;
   addr_value.U8[LSB] = value;   
   
   // Write to the adjacent RTCADR and RTCDAT registers
   CP240x_RegBlockWrite(RTCADR, &addr_value.U8[MSB], 2); 

}

//-----------------------------------------------------------------------------
// CP240x_RTC_Read
//-----------------------------------------------------------------------------
//
// Return Value : RTC0DAT
// Parameters   : 1) unsigned char reg - address of RTC register to read
//
// This function will read one byte from the specified RTC register.
// Using a register number greater that 0x0F is not permited.
//
//-----------------------------------------------------------------------------
U8 CP240x_RTC_Read (U8 addr)
{
   U8 value;
   
   value = addr;

   // Select the register to read and initiate transfer to the data register
   CP240x_RegWrite(RTCADR, (0x90 | addr));

   // Read the value from the data register
   value = CP240x_RegRead(RTCDAT);                      

   return value; 
}

//-----------------------------------------------------------------------------
// CP240x_RTC_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will initialize the smaRTClock. 
//
//-----------------------------------------------------------------------------
void CP240x_RTC_Init ()
{  

   U16 interrupt_status;
                          
   //----------------------------------------
   // Configure the SmaRTClock to crystal
   // or self-oscillate mode
   //---------------------------------------- 
   
   // Disable SmaRTClock
   CP240x_RTC_Write(RTC0CN, 0x00);       

   #if(CP240X_RTC_CLKSRC == CP240X_CRYSTAL)            

      // Configure the smaRTClock oscillator for crystal mode
      // Bias Doubling Enabled, AGC Disabled
      CP240x_RTC_Write (RTC0XCN, 0x60);   
                                       
      // Enable Auto Load Cap Stepping
      CP240x_RTC_Write (RTC0XCF, (0x80 | CP240X_LOADCAP_VALUE));   
   
   #elif(CP240X_RTC_CLKSRC == CP240X_SELFOSC)

      // Configure smaRTClock to self-oscillate mode with bias X2 disabled
      CP240x_RTC_Write (RTC0XCN, 0x00);   
                                      
      // Disable Auto Load Cap Stepping
      CP240x_RTC_Write (RTC0XCF, (0x00 | CP240X_LOADCAP_VALUE));   
   
   #elif(CP240X_RTC_CLKSRC == CP240X_CMOSCLK)
      
      // Configure the smaRTClock oscillator for crystal mode
      // Bias Doubling Enabled, AGC Disabled
      CP240x_RTC_Write (RTC0XCN, 0x60); 

      // Disable Auto Load Cap Stepping
      CP240x_RTC_Write (RTC0XCF, (0x00 | CP240X_LOADCAP_VALUE));   

   #else
      #error "Must select crystal or self oscillate mode"                                   
   #endif

   
   // Enable SmaRTClock oscillator
   CP240x_RTC_Write (RTC0CN, 0x80);    
  
   //----------------------------------------
   // Wait for crystal to start
   // No need to wait in self-oscillate mode
   //----------------------------------------  

   #if (CP240X_RTC_CLKSRC == CP240X_CRYSTAL)

      // Wait > 20 ms
      Wait_MS (200);

      // Wait for smaRTClock valid
      while ((CP240x_RTC_Read (RTC0XCN) & 0x10)== 0x00); 
   
      // Wait for Load Capacitance Ready
      while ((CP240x_RTC_Read(RTC0XCF) & 0x40)== 0x00); 
      
      // Enable Automatic Gain Control and disable bias doubling
      CP240x_RTC_Write (RTC0XCN, 0xC0);   
                                       
   #endif

  
   CP240x_RTC_Write (RTC0CN, 0xC0);    // Enable missing smaRTClock detector
                                       // and leave smaRTClock oscillator
                                       // enabled. 

   Wait_MS(2);                         // Wait for missing clock detector 
                                       // to stabilize
   
   CP240x_RTC_Write(RTC0CN, 0xC0);     // Clear the OSCFAIL flag
 
   CP240x_MSCN_SetBits(0x40);          // Clear interrupt flag
   CP240x_RegBlockRead(INT0, (U8*) &interrupt_status, 2);

   CP240x_RTC0CN_Local = 0xC0;         // Initialize local copy of RTC0CN

   
 
}

#if ALLOW_STOPWATCH_CALLBACKS 
//-----------------------------------------------------------------------------
// CP240x_RTC_MeasureFrequency ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function will configure Timer1 to capture the SmaRTClock oscillator
// A timer 1 interrupt will be generated every 8 RTC cycles and the value 
// of timer 1 at the time of the interrupt will be available for reading
// in the TMR1RLL registers.
//
//-----------------------------------------------------------------------------
U16 CP240x_RTC_MeasureFrequency (void)
{
   UU16 INTEN_save;
   UU16 INTEN_registers;
   
   UU16 INT_registers; 
 

   UU16 period_counts;
   UU32 rtc_frequency;
   
      
   // Save the current value of the interrupt enable registers
   CP240x_RegBlockRead(INT0EN, &INTEN_save.U8[MSB], 2);
      
   // Enable the Timer 1 and RTC Interrupts, disable all others
   INTEN_registers.U16 = 0x1808;
   CP240x_RegBlockWrite(INT0EN, &INTEN_registers.U8[MSB], 2);

   // Clear All Interrupt Flags
   CP240x_ClearAllFlags();
   CP240x_ClearAllInterrupts();

   CP240x_Timer1_Init(8);              // Timer overflows every 8 counts   
   CP240x_RegWrite(TMR1CN, 0x05);      // Timer counts SmaRTClock / 8

   // Wait for the next interrupt
   while(INT);

   // Read the interrupt registers
   CP240x_RegBlockRead(INT0, &INT_registers.U8[MSB], 2);

   // Check for a clock failure or alarm
   if(INT_registers.U8[MSB] != 0x00)
   {
      return UNEXPECTED_SMARTCLOCK_EVENT;
   } 


   //----------------------------------------
   // Obtain first timer reading
   //----------------------------------------

   Stopwatch_Reset();

   // Wait for the next interrupt
   while(INT);

   Stopwatch_Start();

   // Read the interrupt registers
   CP240x_RegBlockRead(INT0, &INT_registers.U8[MSB], 2);

   // Check for a clock failure or alarm
   if(INT_registers.U8[MSB] != 0x00)
   {
      return UNEXPECTED_SMARTCLOCK_EVENT;
   } 
   

   //----------------------------------------
   // Obtain second timer reading
   //----------------------------------------

   // Wait for the next interrupt
   while(INT);
   
   period_counts.U16 = Stopwatch_Stop();

   // Read the interrupt registers
   CP240x_RegBlockRead(INT0, &INT_registers.U8[MSB], 2);
   
   // Check for a clock failure or alarm
   if(INT_registers.U8[MSB] != 0x00)
   {
      return UNEXPECTED_SMARTCLOCK_EVENT;
   } 


   // Restore Interrupts
   CP240x_RegWrite(TMR1CN, 0x00);      // Disable Timer 1
   CP240x_RegBlockWrite(INT0EN, &INTEN_save.U8[MSB], 2);

   //--------------------------------------------
   // Determine the counts in 64 periods
   //--------------------------------------------
   
   //--------------------------------------------
   // Return the SmaRTClock frequency
   //--------------------------------------------

   //#define SYSCLK_CALIBRATED 24500000L
   
   rtc_frequency.U32 = ((SYSCLK*64L)/12L);
   rtc_frequency.U32 /= period_counts.U16;
   
   CP240x_Measured_RTCCLK = rtc_frequency.U16[LSB];

   return rtc_frequency.U16[LSB];

}
#endif

//-----------------------------------------------------------------------------
// CP240x_RTC_WriteAlarm ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//    U32 alarm - the value to write to <ALRM>
//
// This function will write the 32-bit value to the ALRM registers
//
//-----------------------------------------------------------------------------
void CP240x_RTC_WriteAlarm (U32 alarm)
{

   UU32 alarm_big_endian;
   UU32 alarm_value;
   
   alarm_big_endian.U32 = alarm;   

   // Flip the byte ordering of the array to little endian (LSB first)
   alarm_value.U8[b0] = alarm_big_endian.U8[b3]; // LSB <- MSB
   alarm_value.U8[b1] = alarm_big_endian.U8[b2]; 
   alarm_value.U8[b2] = alarm_big_endian.U8[b1]; 
   alarm_value.U8[b3] = alarm_big_endian.U8[b0]; // MSB <- LSB

   // Disable SmaRTClock Alarms
   CP240x_RTC_Write(RTC0CN, CP240x_RTC0CN_Local & ~RTC0AEN);
      
   // Copy the little endian alarm value to the alarm registers
   CP240x_RegWrite(RTCADR, (0x10 | ALARM0));
   CP240x_RegBlockSet(RTCDAT, &alarm_value.U8[b3], 4);

   // Restore the smaRTClock Alarm enable state
   CP240x_RTC_Write (RTC0CN, CP240x_RTC0CN_Local);
}

//-----------------------------------------------------------------------------
// CP240x_RTC_GetCurrentTime()
//-----------------------------------------------------------------------------
//
// Return Value : U32 time - the current smaRTClock time
// Parameters   : None
//    
//
// This function will read the current timer value
//
//-----------------------------------------------------------------------------
U32 CP240x_RTC_GetCurrentTime (void)
{

   UU32 time_big_endian;
   UU32 time_little_endian;
   
   // Initiate an RTC capture
   CP240x_RTC0CN_SetBits(RTC0CAP);  
   while((CP240x_RTC_Read(RTC0CN) & RTC0CAP)); // Wait for RTC0CAP -> 0

   // Read out the time from the capture registers (little endian)
   CP240x_RegWrite(RTCADR, (0xD0 | CAPTURE0));
   CP240x_RegBlockPoll(RTCDAT, &time_little_endian.U8[b3], 4);

   // Flip the byte ordering of the array to big endian (MSB first)
   time_big_endian.U8[b0] = time_little_endian.U8[b3]; // LSB <- MSB
   time_big_endian.U8[b1] = time_little_endian.U8[b2]; 
   time_big_endian.U8[b2] = time_little_endian.U8[b1]; 
   time_big_endian.U8[b3] = time_little_endian.U8[b0]; // MSB <- LSB

   return time_big_endian.U32;

}

//-----------------------------------------------------------------------------
// CP240x_RTC_SetCurrentTime()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : U32 time - the current smaRTClock time
//    
//
// This function will write the 32-bit value to the TIMER registers
//
//-----------------------------------------------------------------------------
void CP240x_RTC_SetCurrentTime (U32 time)
{

   UU32 time_big_endian;
   UU32 time_little_endian;
   
   time_big_endian.U32 = time;

   // Flip the byte ordering of the array to big endian (MSB first)
   time_little_endian.U8[b0] = time_big_endian.U8[b3]; // LSB <- MSB
   time_little_endian.U8[b1] = time_big_endian.U8[b2]; 
   time_little_endian.U8[b2] = time_big_endian.U8[b1]; 
   time_little_endian.U8[b3] = time_big_endian.U8[b0]; // MSB <- LSB

   // Copy the little endian alarm value to the capture registers
   CP240x_RegWrite(RTCADR, (0x10 | CAPTURE0));
   CP240x_RegBlockSet(RTCDAT, &time_little_endian.U8[b3], 4);

   // Initiate an RTC set operation
   CP240x_RTC0CN_SetBits(RTC0SET);  
   while((CP240x_RTC_Read(RTC0CN) & RTC0SET)); // Wait for RTC0SET -> 0
   

}

//-----------------------------------------------------------------------------
// Timer Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// CP240x_Timer0_Init
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : 16 bit reload value
//
// Initializes Timer 0 to overflow every (0xFFFF - <counts>). Timer0
// is clocked every CP240x_SYSCLK / 12.
// 
//-----------------------------------------------------------------------------
void CP240x_Timer0_Init(U16 counts)
{  
   UU16 reload_value;
   U8 timer_reg_array[5];
   
   reload_value.U16 = -counts;
   
   // Initialize timer to reload value
   timer_reg_array[0] = reload_value.U8[LSB];  // TMR0RLL
   timer_reg_array[1] = reload_value.U8[MSB];  // TMR0RLH   
   timer_reg_array[2] = reload_value.U8[LSB];  // TMR0L
   timer_reg_array[3] = reload_value.U8[MSB];  // TMR0H   
   
   // Enable Timer, Disable Low Byte Interrupts
   timer_reg_array[4] = 0x04;                  // TMR0CN
   
   // Write the timer registers
   //CP240x_RegBlockWrite(TMR0RLL, timer_reg_array, 5);

   CP240x_RegWrite(TMR0RLL+0, timer_reg_array[0]);
   CP240x_RegWrite(TMR0RLL+1, timer_reg_array[1]);
   CP240x_RegWrite(TMR0RLL+2, timer_reg_array[2]);
   CP240x_RegWrite(TMR0RLL+3, timer_reg_array[3]);

   
}

//-----------------------------------------------------------------------------
// CP240x_Timer1_Init
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : 16 bit reload value
//
// Initializes Timer 1 to overflow every (0xFFFF - <reload_value>). Timer0
// is clocked every CP240x_SYSCLK / 12.
// 
//-----------------------------------------------------------------------------
void CP240x_Timer1_Init(U16 counts)
{
   UU16 reload_value;
   U8 timer_reg_array[5];
   
   reload_value.U16 = -counts;
   
   // Initialize timer to reload value
   timer_reg_array[0] = reload_value.U8[LSB];  // TMR1RLL
   timer_reg_array[1] = reload_value.U8[MSB];  // TMR1RLH   
   timer_reg_array[2] = reload_value.U8[LSB];  // TMR1L
   timer_reg_array[3] = reload_value.U8[MSB];  // TMR1H   
   
   // Enable Timer, Disable Low Byte Interrupts
   timer_reg_array[4] = 0x04;                  // TMR1CN
   
   // Write the timer registers
   CP240x_RegBlockWrite(TMR1RLL, timer_reg_array, 5);
}


//-----------------------------------------------------------------------------
// Port I/O Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CP240x_PortIO_Configure
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : none
//
// Transfers the contents of the CP240x_PortIO_Registers array to the writable 
// registers on the CP240x 
//-----------------------------------------------------------------------------
void CP240x_PortIO_Configure(void)
{
   CP240x_RegBlockWrite(P0OUT, &CP240x_PortIO_Registers.P0OUT_Local,
                        CP240X_NUM_PORTREGS_CONFIG);
}

//-----------------------------------------------------------------------------
// CP240x_PortIO_Read
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : none
//
// Transfers the contents of the PMATCHST and Port I/O Input Registers
// to the CP240x_PortIO_Registers array.
//-----------------------------------------------------------------------------

void CP240x_PortIO_Read(void)
{
   CP240x_RegBlockRead(PMATCHST, &CP240x_PortIO_Registers.PMATCHST_Local,
                        CP240X_NUM_PORTREGS_READ);

}


//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
