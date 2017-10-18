//-----------------------------------------------------------------------------
// T620_USB0_ReportHandler.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Includes functions called by USB_ISR.c to handle input and output reports.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051T620/1 or 'T320/1/2/3
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 3.4x
// Command Line:   See Readme.txt
// Project Name:   T620_USB0_Suspend
//
// Release 1.0
//    -Initial Revision (PD / TP)
//    -15 SEP 2008
//

#ifndef  _UUSB_REPORTHANDLER_H_
#define  _USB_REPORTHANDLER_H_


typedef struct {
   unsigned char ReportID;
   void (*hdlr)();
} VectorTableEntry;

typedef struct{
   unsigned char Length;
   unsigned char* Ptr;
} BufferStructure;

extern void ReportHandler_IN_ISR(unsigned char);
extern void ReportHandler_IN_Foreground(unsigned char);
extern void ReportHandler_OUT(unsigned char);
extern void Setup_OUT_BUFFER(void);

extern BufferStructure IN_BUFFER, OUT_BUFFER;

#endif

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
