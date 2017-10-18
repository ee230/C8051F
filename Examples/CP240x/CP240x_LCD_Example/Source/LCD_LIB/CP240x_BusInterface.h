//-----------------------------------------------------------------------------
// CP240x_BusInterface.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// This file contains generic host interface function prototypes. 
// It may be used to add support for new microcontroller interfaces. To add support
// for a new interface, create a "CP240x_BusInterface_XXX.c" file and provide 
// implementations for the function prototypes in this header file.
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
// Bus Interface Constants
//-----------------------------------------------------------------------------

// Bus Commands
#define  REGPOLL	    		0x01
#define  REGREAD           0x02
#define  REGSET            0x03
#define  REGWRITE          0x04        
#define  RAMREAD        	0x06        
#define  RAMWRITE       	0x08       

// Operation Type
#define  WRITE             0x00
#define  READ              0x01

// Block Direction
#define FORWARD            0x00
#define BACKWARD           0x20

// Block Operation Macros
#define CP240x_RegBlockSet(addr, ptr, len)   CP240x_BlockOperation(REGSET, addr, ptr, len)
#define CP240x_RegBlockPoll(addr, ptr, len)  CP240x_BlockOperation(REGPOLL, addr, ptr, len)
#define CP240x_RegBlockWrite(addr, ptr, len) CP240x_BlockOperation(REGWRITE, addr, ptr, len)
#define CP240x_RegBlockRead(addr, ptr, len)  CP240x_BlockOperation(REGREAD,  addr, ptr, len)
#define CP240x_RAMBlockWrite(addr, ptr, len) CP240x_BlockOperation(RAMWRITE, addr, ptr, len)
#define CP240x_RAMBlockRead(addr, ptr, len)  CP240x_BlockOperation(RAMREAD,  addr, ptr, len)

//-----------------------------------------------------------------------------
// Ultra Low Power Mode Constants
//-----------------------------------------------------------------------------

// ULP Modes
#define ULP_LCD                     0x09
#define ULP_SMARTCLOCK              0x02
#define ULP_SHUTDOWN                0x12

// ULP Mode Entry Macros
#define CP240x_EnterULP_LCD()             CP240x_EnterULP(ULP_LCD)   
#define CP240x_EnterULP_SmaRTClock()      CP240x_EnterULP(ULP_SMARTCLOCK)
#define CP240x_Shutdown()                 CP240x_EnterULP(ULP_SHUTDOWN)

//-----------------------------------------------------------------------------
// Interface Initialization
//-----------------------------------------------------------------------------
extern void CP240x_BusInterface_Init();

//-----------------------------------------------------------------------------
// Single-Byte Register Read/Write
//-----------------------------------------------------------------------------
extern void CP240x_RegWrite(U8 addr, U8 value);
extern U8 CP240x_RegRead(U8 addr);

//-----------------------------------------------------------------------------
// Single-Byte RAM Read/Write
//-----------------------------------------------------------------------------
#if(SINGLE_BYTE_RAM_OPERATIONS)
extern void CP240x_RAMWrite(U8 addr, U8 value);
extern U8 CP240x_RAMRead(U8 addr);
#endif

//-----------------------------------------------------------------------------
// Block Read/Write
//-----------------------------------------------------------------------------
#if(AUTODECREMENT_BLOCK_RAM_OPERATIONS)
extern void CP240x_SetRAMBlockDirection(U8 dir);
#endif

extern void CP240x_BlockOperation(U8 cmd, U8 addr, U8 *ptr, U8 len);

//-----------------------------------------------------------------------------
// ULP Functions
//-----------------------------------------------------------------------------
// Functions to enter and exit Ultra Low Power mode on the CP240x
//
extern void CP240x_EnterULP(U8 mode);
extern void CP240x_ExitULP(void);


//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
