//-----------------------------------------------------------------------------
// F3xx_USB0_CustomApp.h
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This file shows an example of how an initialization routine
// can be made accessible to the USB_MAIN file.
//
// How To Test:    See Readme.txt
//
//
// FID             3XX000019
// Target:         C8051F32x/C8051F340
// Tool chain:     Keil / Raisonance
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_MouseExample
//
//
// Release 1.0
//    -Initial Revision (PD)
//    -07 DEC 2005
//


#ifndef _DEFAULT_CUSTOMAPP_H_
#define _DEFAULT_CUSTOMAPP_H_

void DEFAULT_InitRoutine(void);
void USB_Init(void);
void System_Init ();
#define ID_STUB 0

#endif  /* _USB_CUSTOMAPP_H_ */