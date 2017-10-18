//------------------------------------------------------------------------------------
// F00x_Blinky.c
//------------------------------------------------------------------------------------
// Copyright (C) 2007 Silicon Laboratories, Inc.
//
// This program flashes the green LED on the C8051F000 target board about five times
// a second using the interrupt handler for Timer3.
// Target: C8051F000, C8051F010
//
// Tool chain: KEIL Eval 'c'
//

//#pragma CD OE DB SB							// Compilation directives

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <c8051f000.h>							// SFR declarations
#include <stdio.h>

//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------
sbit	LED = P1^6;									// green LED: '1' = ON; '0' = OFF
#define TC_100ms	16000							// approx number of counts of 1.9MHz/12
														//  to get 100ms.
//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------
void Timer3_ISR (void);

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main (void) {

	// disable watchdog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	// set up Crossbar and GPIO
	XBR2 = 0x40;									// Enable crossbar and weak pull-ups
	PRT1CF |= 0x40;								// enable P1.6 (LED) as a push-pull output

	// set up Timer3 for auto-reload to generate interrupts at 100ms intervals
	TMR3CN = 0x00;									// stop Timer3
	TMR3RLH = ((-TC_100ms >> 8) & 0xff);	// init Timer3 reload value
	TMR3RLL = (-TC_100ms & 0xff);
	TMR3H = 0xff;									// init Timer3 to reload immediately
	TMR3L = 0xff;
	EIE2 = 0x01;									// enable Timer3 OVR interrupt
	TMR3CN = 0x04;									// start Timer3

	EA = 1;											// enable global interrupts

	while (1) {
		PCON |= 0x01;								// set IDLE mode
	}
}

//------------------------------------------------------------------------------------
// Interrupt Service Routines
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Timer3_ISR
//------------------------------------------------------------------------------------
// This routine changes the state of the LED.
//
void Timer3_ISR (void) interrupt 14
{
	TMR3CN &= ~(0x80);							// clear TF3
	LED = ~LED;										// change state of LED
}