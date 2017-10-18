//-----------------------------------------------------------------------------
// F3xx_USB0_HIDtoUART.h
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Stub file for Firmware Template.
//
//
// How To Test:    See Readme.txt
//
//
// FID             
// Target:         C8051F32x/C8051F340
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   HIDtoUART
//
// Release 1.1
//    -Changed "Usb_Init" to "USB_Init" (TP)
//    -07 OCT 2010
//
// Release 1.0
//    -Initial Revision (PD)
//    -04 JUN 2008
//

#ifndef  _USBTOUART_H_
#define  _USBTOUART_H_

#define SYSTEMCLOCK 24000000

#define IN_CONTROL 0xFE
#define IN_CONTROL_SIZE 8

#define OUT_CONTROL 0xFD
#define OUT_CONTROL_SIZE 8

#define IN_DATA  0x01
#define IN_DATA_SIZE 60
#define OUT_DATA 0x02
#define OUT_DATA_SIZE 60

#define UART_INPUT_BUFFERSIZE 128
#define UART_OUTPUT_BUFFERSIZE 128

//#define BAUDRATE_HARDCODED



#ifndef BAUDRATE_HARDCODED
extern unsigned long BaudRate;
#else
#define BaudRate 630000
#endif


void System_Init (void);
void USB_Init (void);

void UART_INPUT_TEST(void);

extern unsigned char TX_Ready;


extern unsigned char xdata IN_PACKET[];
extern unsigned char xdata OUT_PACKET[];
extern unsigned char xdata UART_OUTPUT[];
extern unsigned char xdata UART_INPUT[];
extern unsigned char UART_INPUT_SIZE, UART_OUTPUT_SIZE, UART_INPUT_FIRST, UART_INPUT_LAST;
extern unsigned char UART_OUTPUT_FIRST, UART_OUTPUT_LAST;

extern unsigned char UART_OUTPUT_OVERFLOW_BOUNDARY;
extern unsigned char USB_OUT_SUSPENDED;

typedef union ULONG {
unsigned long L;
unsigned char C[4];
} ULONG;

void UART0_Init (void);
#endif  /* _USB_DESC_H_ */