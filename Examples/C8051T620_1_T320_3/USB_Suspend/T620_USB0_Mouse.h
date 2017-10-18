//-----------------------------------------------------------------------------
// T620_USB0_Mouse.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Header file for mouse routines.
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

#ifndef  _MOUSE_H_
#define  _MOUSE_H_

extern signed char MOUSE_VECTOR;
extern unsigned char MOUSE_AXIS;
extern unsigned char MOUSE_BUTTON;
unsigned char IN_PACKET[];


#define X_Axis 0
#define Y_Axis 1


void System_Init(void);
void USB0_Init(void);

#endif  /* _MOUSE_H_ */

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
