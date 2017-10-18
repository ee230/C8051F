//-----------------------------------------------------------------------------
// C8051F200_defs.h
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051F200 family.
// **Important Note**: The compiler_defs.h header file should be included
// before including this header file.
//
// Target:         C8051F200
// Tool chain:     Keil
// Command Line:   None
//
// Release 1.4 - 22 July 2008 (ES)
//    -Added missing byte register definitions
//    -Updated ADC0CN bits
// Release 1.3
//    -29 FEB 2008
//    -Made compiler independant by AS
//    -Added 16-bit sfr definitions
//
//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051F200_DEFS_H
#define C8051F200_DEFS_H

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------

SFR(P0, 0x80);          // PORT 0
SFR(SP, 0x81);          // STACK POINTER
SFR(DPL, 0x82);         // DATA POINTER - LOW BYTE
SFR(DPH, 0x83);         // DATA POINTER - HIGH BYTE
SFR(PCON, 0x87);        // POWER CONTROL
SFR(TCON, 0x88);        // TIMER CONTROL
SFR(TMOD, 0x89);        // TIMER MODE
SFR(TL0, 0x8A);         // TIMER 0 - LOW BYTE
SFR(TL1, 0x8B);         // TIMER 1 - LOW BYTE
SFR(TH0, 0x8C);         // TIMER 0 - HIGH BYTE
SFR(TH1, 0x8D);         // TIMER 1 - HIGH BYTE
SFR(CKCON, 0x8E);       // CLOCK CONTROL
SFR(PSCTL, 0x8F);       // PROGRAM STORE R/W CONTROL
SFR(P1, 0x90);          // PORT 1
SFR(SCON, 0x98);        // SERIAL PORT CONTROL
SFR(SBUF, 0x99);        // SERIAL PORT BUFFER
SFR(SPI0CFG, 0x9A);     // SERIAL PERIPHERAL INTERFACE 0 CONFIGURATION
SFR(SPI0DAT, 0x9B);     // SERIAL PERIPHERAL INTERFACE 0 DATA
SFR(SPI0CKR, 0x9D);     // SERIAL PERIPHERAL INTERFACE 0 CLOCK RATE CONTROL
SFR(CPT0CN, 0x9E);      // COMPARATOR 0 CONTROL
SFR(CPT1CN, 0x9F);      // COMPARATOR 1 CONTROL
SFR(P2, 0xA0);          // PORT 2
SFR(PRT0CF, 0xA4);      // PORT 0 OUTPUT MODE CONFIGURATION
SFR(PRT1CF, 0xA5);      // PORT 1 OUTPUT MODE CONFIGURATION
SFR(PRT2CF, 0xA6);      // PORT 2 OUTPUT MODE CONFIGURATION
SFR(PRT3CF, 0xA7);      // PORT 3 OUTPUT MODE CONFIGURATION
SFR(IE, 0xA8);          // INTERRUPT ENABLE
SFR(SWCINT, 0xAD);      // SOFTWARE-CONTROLLED INTERRUPT FLAGS
SFR(PRT1IF, 0xAD);      // SOFTWARE-CONTROLLED INTERRUPT FLAGS (LEGACY NAME)
SFR(EMI0CN, 0xAF);      // EXTERNAL MEMORY INTERFACE CONTROL
SFR(P3, 0xB0);          // PORT 3
SFR(OSCXCN, 0xB1);      // EXTERNAL OSCILLATOR CONTROL
SFR(OSCICN, 0xB2);      // INTERNAL OSCILLATOR CONTROL
SFR(FLSCL, 0xB6);       // FLASH MEMORY TIMING PRESCALER
SFR(FLACL, 0xB7);       // FLASH ACESS LIMIT
SFR(IP, 0xB8);          // INTERRUPT PRIORITY
SFR(AMX0SL, 0xBB);      // ADC 0 MUX CHANNEL SELECTION
SFR(ADC0CF, 0xBC);      // ADC 0 CONFIGURATION
SFR(ADC0L, 0xBE);	    // ADC 0 DATA - LOW BYTE
SFR(ADC0H, 0xBF);       // ADC 0 DATA - HIGH BYTE
SFR(ADC0GTL, 0xC4);	    // ADC 0 GREATER-THAN REGISTER - LOW BYTE
SFR(ADC0GTH, 0xC5);     // ADC 0 GREATER-THAN REGISTER - HIGH BYTE
SFR(ADC0LTL, 0xC6);	    // ADC 0 LESS-THAN REGISTER - LOW BYTE
SFR(ADC0LTH, 0xC7);     // ADC 0 LESS-THAN REGISTER - HIGH BYTE
SFR(T2CON, 0xC8);       // TIMER 2 CONTROL
SFR(RCAP2L, 0xCA);      // TIMER 2 CAPTURE REGISTER - LOW BYTE
SFR(RCAP2H, 0xCB);      // TIMER 2 CAPTURE REGISTER - HIGH BYTE
SFR(TL2, 0xCC);         // TIMER 2 - LOW BYTE
SFR(TH2, 0xCD);         // TIMER 2 - HIGH BYTE
SFR(PSW, 0xD0);         // PROGRAM STATUS WORD
SFR(REF0CN, 0xD1);      // VOLTAGE REFERENCE 0 CONTROL
SFR(ACC, 0xE0);         // ACCUMULATOR
SFR(PRT0MX, 0xE1);      // PORT MUX CONFIGURATION REGISTER 0
SFR(PRT1MX, 0xE2);      // PORT MUX CONFIGURATION REGISTER 1
SFR(PRT2MX, 0xE3);      // PORT MUX CONFIGURATION REGISTER 2
SFR(EIE1, 0xE6);        // EXTERNAL INTERRUPT ENABLE 1
SFR(EIE2, 0xE7);        // EXTERNAL INTERRUPT ENABLE 2
SFR(ADC0CN, 0xE8);      // ADC 0 CONTROL
SFR(RSTSRC, 0xEF);      // RESET SOURCE
SFR(B, 0xF0);           // B REGISTER
SFR(P0MODE, 0xF1);      // PORT 0 INPUT MODE CONFIGURATION
SFR(P1MODE, 0xF2);      // PORT 1 INPUT MODE CONFIGURATION
SFR(P2MODE, 0xF3);      // PORT 2 INPUT MODE CONFIGURATION
SFR(P3MODE, 0xF4);      // PORT 3 INPUT MODE CONFIGURATION
SFR(EIP1, 0xF6);        // EXTERNAL INTERRUPT PRIORITY REGISTER 1
SFR(EIP2, 0xF7);        // EXTERNAL INTERRUPT PRIORITY REGISTER 2
SFR(SPI0CN, 0xF8);      // SERIAL PERIPHERAL INTERFACE 0 CONTROL
SFR(WDTCN, 0xFF);       // WATCHDOG TIMER CONTROL

//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16(DP, 0x82);        // Data Pointer
SFR16(ADC0, 0xBE);      // ADC0 Data
SFR16(ADC0GT, 0xC4);    // ADC0 Greater-Than Compare
SFR16(ADC0LT, 0xC6);    // ADC0 Less-Than Compare
SFR16(RCAP2, 0xCA);     // Counter/Timer 2 Capture
SFR16(T2, 0xCC);        // Timer 2 Data

//-----------------------------------------------------------------------------
// Address Definitions for Bit-addressable Registers
//-----------------------------------------------------------------------------

#define SFR_P0          0x80
#define SFR_TCON        0x88
#define SFR_P1          0x90
#define SFR_SCON        0x98
#define SFR_P2          0xA0
#define SFR_IE          0xA8
#define SFR_P3          0xB0
#define SFR_IP          0xB8
#define SFR_T2CON       0xC8
#define SFR_PSW         0xD0
#define SFR_ACC         0xE0
#define SFR_ADC0CN      0xE8
#define SFR_B           0xF0
#define SFR_SPI0CN      0xF8

//-----------------------------------------------------------------------------
// Bit Definitions
//-----------------------------------------------------------------------------

//  TCON  0x88
SBIT(TF1, SFR_TCON, 7);               // TIMER 1 OVERFLOW FLAG
SBIT(TR1, SFR_TCON, 6);               // TIMER 1 ON/OFF CONTROL
SBIT(TF0, SFR_TCON, 5);               // TIMER 0 OVERFLOW FLAG
SBIT(TR0, SFR_TCON, 4);               // TIMER 0 ON/OFF CONTROL
SBIT(IE1, SFR_TCON, 3);               // EXT. INTERRUPT 1 EDGE FLAG
SBIT(IT1, SFR_TCON, 2);               // EXT. INTERRUPT 1 TYPE
SBIT(IE0, SFR_TCON, 1);               // EXT. INTERRUPT 0 EDGE FLAG
SBIT(IT0, SFR_TCON, 0);               // EXT. INTERRUPT 0 TYPE

//  SCON  0x98
SBIT(SM0, SFR_SCON, 7);               // SERIAL MODE CONTROL BIT 0
SBIT(SM1, SFR_SCON, 6);               // SERIAL MODE CONTROL BIT 1
SBIT(SM2, SFR_SCON, 5);               // MULTIPROCESSOR COMMUNICATION ENABLE
SBIT(REN, SFR_SCON, 4);               // RECEIVE ENABLE
SBIT(TB8, SFR_SCON, 3);               // TRANSMIT BIT 8
SBIT(RB8, SFR_SCON, 2);               // RECEIVE BIT 8
SBIT(TI, SFR_SCON, 1);                // TRANSMIT INTERRUPT FLAG
SBIT(RI, SFR_SCON, 0);                // RECEIVE INTERRUPT FLAG

//  IE  0xA8
SBIT(EA, SFR_IE, 7);                  // GLOBAL INTERRUPT ENABLE
                                      // Bit 6 Unused
SBIT(ET2, SFR_IE, 5);                 // TIMER 2 INTERRUPT ENABLE
SBIT(ES, SFR_IE, 4);                  // SERIAL PORT INTERRUPT ENABLE
SBIT(ET1, SFR_IE, 3);                 // TIMER 1 INTERRUPT ENABLE
SBIT(EX1, SFR_IE, 2);                 // EXTERNAL INTERRUPT 1 ENABLE
SBIT(ET0, SFR_IE, 1);                 // TIMER 0 INTERRUPT ENABLE
SBIT(EX0, SFR_IE, 0);                 // EXTERNAL INTERRUPT 0 ENABLE

//  IP  0xB8
                                      // Bit 7 Unused
                                      // Bit 6 Unused
SBIT(PT2, SFR_IP, 5);                 // TIMER 2 PRIORITY
SBIT(PS, SFR_IP, 4);                  // SERIAL PORT PRIORITY
SBIT(PT1, SFR_IP, 3);                 // TIMER 1 PRIORITY
SBIT(PX1, SFR_IP, 2);                 // EXTERNAL INTERRUPT 1 PRIORITY
SBIT(PT0, SFR_IP, 1);                 // TIMER 0 PRIORITY
SBIT(PX0, SFR_IP, 0);                 // EXTERNAL INTERRUPT 0 PRIORITY

//  T2CON  0xC8
SBIT(TF2, SFR_T2CON, 7);              // TIMER 2 OVERFLOW FLAG
SBIT(EXF2, SFR_T2CON, 6);             // EXTERNAL FLAG
SBIT(RCLK, SFR_T2CON, 5);             // RECEIVE CLOCK FLAG
SBIT(TCLK, SFR_T2CON, 4);             // TRANSMIT CLOCK FLAG
SBIT(EXEN2, SFR_T2CON, 3);            // TIMER 2 EXTERNAL ENABLE FLAG
SBIT(TR2, SFR_T2CON, 2);              // TIMER 2 ON/OFF CONTROL
SBIT(CT2, SFR_T2CON, 1);              // TIMER OR COUNTER SELECT
SBIT(CPRL2, SFR_T2CON, 0);            // CAPTURE OR RELOAD SELECT

//  PSW
SBIT(CY, SFR_PSW, 7);                 // CARRY FLAG
SBIT(AC, SFR_PSW, 6);                 // AUXILIARY CARRY FLAG
SBIT(F0, SFR_PSW, 5);                 // USER FLAG 0
SBIT(RS1, SFR_PSW, 4);                // REGISTER BANK SELECT 1
SBIT(RS0, SFR_PSW, 3);                // REGISTER BANK SELECT 0
SBIT(OV, SFR_PSW, 2);                 // OVERFLOW FLAG
SBIT(F1, SFR_PSW, 1);                 // USER FLAG 1
SBIT(P, SFR_PSW, 0);                  // ACCUMULATOR PARITY FLAG

// ADC0CN E8H
SBIT(ADCEN, SFR_ADC0CN, 7);           // ADC 0 ENABLE
SBIT(ADCTM, SFR_ADC0CN, 6);           // ADC 0 TRACK MODE
SBIT(ADCINT, SFR_ADC0CN, 5);          // ADC 0 CONVERISION COMPLETE INTERRUPT FLAG
SBIT(ADBUSY, SFR_ADC0CN, 4);          // ADC 0 BUSY FLAG
SBIT(ADSTM1, SFR_ADC0CN, 3);          // ADC 0 START OF CONVERSION MODE BIT 1
SBIT(ADSTM0, SFR_ADC0CN, 2);          // ADC 0 START OF CONVERSION MODE BIT 0
SBIT(ADWINT, SFR_ADC0CN, 1);          // ADC 0 WINDOW COMPARE INTERRUPT FLAG
SBIT(ADLJST, SFR_ADC0CN, 0);	        // ADC 0 LEFT JUSTIFY SELECT

// SPI0CN F8H
SBIT(SPIF, SFR_SPI0CN, 7);            // SPI 0 INTERRUPT FLAG
SBIT(WCOL, SFR_SPI0CN, 6);            // SPI 0 WRITE COLLISION FLAG
SBIT(MODF, SFR_SPI0CN, 5);            // SPI 0 MODE FAULT FLAG
SBIT(RXOVRN, SFR_SPI0CN, 4);          // SPI 0 RX OVERRUN FLAG
SBIT(TXBSY, SFR_SPI0CN, 3);           // SPI 0 TX BUSY FLAG
SBIT(SLVSEL, SFR_SPI0CN, 2);          // SPI 0 SLAVE SELECT
SBIT(MSTEN, SFR_SPI0CN, 1);           // SPI 0 MASTER ENABLE
SBIT(SPIEN, SFR_SPI0CN, 0);           // SPI 0 SPI ENABLE

//-----------------------------------------------------------------------------
// Interrupt Priorities
//-----------------------------------------------------------------------------

#define INTERRUPT_INT0                 0  // External Interrupt 0
#define INTERRUPT_TIMER0               1  // Timer0 Overflow
#define INTERRUPT_INT1                 2  // External Interrupt 1
#define INTERRUPT_TIMER1               3  // Timer1 Overflow
#define INTERRUPT_UART                 4  // UART Interrupt
#define INTERRUPT_TIMER2               5  // Timer2 Overflow
#define INTERRUPT_SPI                  6  // SPI Interrupt
                                          // IP 7 Unused
#define INTERRUPT_ADC0_WINDOW          8  // ADC0 Window Comparison
                                          // IP 9 Unused
#define INTERRUPT_COMPARATOR0_FALLING 10  // Comparator0 Falling Interrupt
#define INTERRUPT_COMPARATOR0_RISING  11  // Comparator0 Rising Interrupt
#define INTERRUPT_COMPARATOR1_FALLING 12  // Comparator1 Falling Interrupt
#define INTERRUPT_COMPARATOR1_RISING  13  // Comparator1 Rising Interrupt
#define INTERRUPT_ADC0_EOC            15  // ADC0 End of Conversion
#define INTERRUPT_SCI0                16  // Software Controlled Interrupt 0
#define INTERRUPT_SCI1                17  // Software Controlled Interrupt 1
#define INTERRUPT_SCI2                18  // Software Controlled Interrupt 2
#define INTERRUPT_SCI3                19  // Software Controlled Interrupt 3
                                          // IP 20 Unused
#define INTERRUPT_EXT_OSC_RDY         21  // External Crystal Oscillator Ready

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define C8051F200_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
