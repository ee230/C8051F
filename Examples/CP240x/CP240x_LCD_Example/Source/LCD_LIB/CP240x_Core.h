//-----------------------------------------------------------------------------
// CP240x_Core.h
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
// Type Definitions
//-----------------------------------------------------------------------------

#define CP240X_NUM_PORTREGS_CONFIG   30
#define CP240X_NUM_PORTREGS_READ     5

typedef struct CP240X_PORTREGS 
{  
   // Writable Registers
   U8 P0OUT_Local;  
   U8 P1OUT_Local;  
   U8 P2OUT_Local;  
   U8 P3OUT_Local;  
   U8 P4OUT_Local;  
   U8 P0MDI_Local;  
   U8 P1MDI_Local;  
   U8 P2MDI_Local;  
   U8 P3MDI_Local;  
   U8 P4MDI_Local;  
   U8 P0MDO_Local;  
   U8 P1MDO_Local;  
   U8 P2MDO_Local;  
   U8 P3MDO_Local;  
   U8 P4MDO_Local;  
   U8 P0DRIVE_Local;
   U8 P1DRIVE_Local;
   U8 P2DRIVE_Local;
   U8 P3DRIVE_Local;
   U8 P4DRIVE_Local;
   U8 P0MATCH_Local;
   U8 P1MATCH_Local;
   U8 P2MATCH_Local;
   U8 P3MATCH_Local;
   U8 P4MATCH_Local;
   U8 P0MSK_Local;  
   U8 P1MSK_Local;  
   U8 P2MSK_Local;  
   U8 P3MSK_Local;  
   U8 P4MSK_Local;
   
   // Read Only Registers
   U8 PMATCHST_Local;  
   U8 P0IN_Local;      
   U8 P1IN_Local;      
   U8 P2IN_Local;      
   U8 P3IN_Local;      
   U8 P4IN_Local;        

} CP240X_PORTREGS;


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

extern U16 CP240x_Measured_RTCCLK;
extern U8 CP240x_MSCN_Local;
extern U8 CP240x_RTC0CN_Local;

extern CP240X_PORTREGS xdata CP240x_PortIO_Registers;

//-----------------------------------------------------------------------------
// Hardware Reset
//-----------------------------------------------------------------------------
extern S8 CP240x_Reset(void);

//-----------------------------------------------------------------------------
// Interrupt Management
//-----------------------------------------------------------------------------
extern void CP240x_ClearAllFlags(void);
extern S8   CP240x_ClearAllInterrupts(void);

//-----------------------------------------------------------------------------
// MSCN Management
//-----------------------------------------------------------------------------

#define CP240x_MSCN_SetBits(value)   CP240x_MSCN_Update(CP240x_MSCN_Local | value)
#define CP240x_MSCN_ClearBits(value) CP240x_MSCN_Update(CP240x_MSCN_Local & value)
 
void CP240x_MSCN_Update (U8 value);

//-----------------------------------------------------------------------------
// RTC0CN Management
//-----------------------------------------------------------------------------

#define CP240x_RTC0CN_SetBits(value)   CP240x_RTC0CN_Update(CP240x_RTC0CN_Local | value)
#define CP240x_RTC0CN_ClearBits(value) CP240x_RTC0CN_Update(CP240x_RTC0CN_Local & value)
 
void CP240x_RTC0CN_Update (U8 value);


//-----------------------------------------------------------------------------
// RTC Functions
//-----------------------------------------------------------------------------
extern void CP240x_RTC_Write (U8 addr, U8 value);
extern U8   CP240x_RTC_Read (U8 addr);
extern void CP240x_RTC_Init (void);
extern void CP240x_RTC_WriteAlarm (U32 alarm);
extern U32  CP240x_RTC_GetCurrentTime (void);
extern void CP240x_RTC_SetCurrentTime (U32 time);

#if ALLOW_STOPWATCH_CALLBACKS 
extern U16 CP240x_RTC_MeasureFrequency (void);
#endif

//-----------------------------------------------------------------------------
// Timer Functions
//-----------------------------------------------------------------------------
// Initialize Timer 0 or Timer 1
//
extern void CP240x_Timer0_Init(U16 counts);
extern void CP240x_Timer1_Init(U16 counts);

//-----------------------------------------------------------------------------
// Port I/O Functions
//-----------------------------------------------------------------------------
//
extern void CP240x_PortIO_Configure(void);
extern void CP240x_PortIO_Read(void);

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------                                          
