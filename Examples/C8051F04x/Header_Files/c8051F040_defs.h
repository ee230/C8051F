//-----------------------------------------------------------------------------
// C8051F040_defs.h
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051F04x/'F04x family.
// **Important Note**: The compiler_defs.h header file should be included
// before including this header file.
//
// Target:         C8051F04x, 'F04x
// Tool chain:     Keil
// Command Line:   None
//
// Release 1.4 - 22 July 2008 (ES)
//    -Updated SCON1, IP, ADC0CN, ADC2CN bit definitions
// Release 1.3
//    -29 FEB 2008
//    -Made compiler independant by AS
//    -Added 16-bit sfr definitions
//
//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051F040_DEFS_H
#define C8051F040_DEFS_H

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------

SFR (P0, 0x80);           // PORT 0
SFR (SP, 0x81);           // STACK POINTER
SFR (DPL, 0x82);          // DATA POINTER - LOW BYTE
SFR (DPH, 0x83);          // DATA POINTER - HIGH BYTE
SFR (SFRPAGE, 0x84);      // SFR PAGE SELECT
SFR (SFRNEXT, 0x85);      // SFR STACK NEXT PAGE
SFR (SFRLAST, 0x86);      // SFR STACK LAST PAGE
SFR (PCON, 0x87);         // POWER CONTROL
SFR (TCON, 0x88);         // TIMER CONTROL
SFR (CPT0CN, 0x88);       // COMPARATOR 0 CONTROL
SFR (CPT1CN, 0x88);       // COMPARATOR 1 CONTROL
SFR (CPT2CN, 0x88);       // COMPARATOR 2 CONTROL
SFR (TMOD, 0x89);         // TIMER MODE
SFR (CPT0MD, 0x89);       // COMPARATOR 0 MODE
SFR (CPT1MD, 0x89);       // COMPARATOR 1 MODE
SFR (CPT2MD, 0x89);       // COMPARATOR 2 MODE
SFR (TL0, 0x8A);          // TIMER 0 - LOW BYTE
SFR (OSCICN, 0x8A);       // INTERNAL OSCILLATOR CONTROL
SFR (TL1, 0x8B);          // TIMER 1 - LOW BYTE
SFR (OSCICL, 0x8B);       // INTERNAL OSCILLATOR CALIBRATION
SFR (TH0, 0x8C);          // TIMER 0 - HIGH BYTE
SFR (OSCXCN, 0x8C);       // EXTERNAL OSCILLATOR CONTROL
SFR (TH1, 0x8D);          // TIMER 1 - HIGH BYTE
SFR (CKCON, 0x8E);        // TIMER 0/1 CLOCK CONTROL
SFR (PSCTL, 0x8F);        // FLASH WRITE/ERASE CONTROL
SFR (P1, 0x90);           // PORT 1
SFR (SSTA0, 0x91);        // UART 0 STATUS
SFR (SFRPGCN, 0x96);      // SFR PAGE CONTROL
SFR (CLKSEL, 0x97);       // SYSTEM CLOCK SELECT
SFR (SCON0, 0x98);        // UART 0 CONTROL
SFR (SCON1, 0x98);        // UART 1 CONTROL
SFR (SBUF0, 0x99);        // UART 0 BUFFER
SFR (SBUF1, 0x99);        // UART 1 BUFFER
SFR (SPI0CFG, 0x9A);      // SPI 0 CONFIGURATION
SFR (SPI0DAT, 0x9B);      // SPI 0 DATA
SFR (P4MDOUT, 0x9C);      // PORT 4 OUTPUT MODE
SFR (SPI0CKR, 0x9D);      // SPI 0 CLOCK RATE CONTROL
SFR (P5MDOUT, 0x9D);      // PORT 5 OUTPUT MODE
SFR (P6MDOUT, 0x9E);      // PORT 6 OUTPUT MODE
SFR (P7MDOUT, 0x9F);      // PORT 7 OUTPUT MODE
SFR (P2, 0xA0);           // PORT 2
SFR (EMI0TC, 0xA1);       // EMIF TIMING CONTROL
SFR (EMI0CN, 0xA2);       // EMIF CONTROL
SFR (EMI0CF, 0xA3);       // EMIF CONFIGURATION
SFR (P0MDOUT, 0xA4);      // PORT 0 OUTPUT MODE
SFR (P1MDOUT, 0xA5);      // PORT 1 OUTPUT MODE
SFR (P2MDOUT, 0xA6);      // PORT 2 OUTPUT MODE CONFIGURATION
SFR (P3MDOUT, 0xA7);      // PORT 3 OUTPUT MODE CONFIGURATION
SFR (IE, 0xA8);           // INTERRUPT ENABLE
SFR (SADDR0, 0xA9);       // UART 0 SLAVE ADDRESS
SFR (SADDR1, 0xA9);       // UART 1 SLAVE ADDRESS
SFR (P1MDIN, 0xAD);       // PORT 1 INPUT MODE
SFR (P2MDIN, 0xAE);       // PORT 2 INPUT MODE
SFR (P3MDIN, 0xAF);       // PORT 3 INPUT MODE
SFR (P3, 0xB0);           // PORT 3
SFR (FLSCL, 0xB7);        // FLASH TIMING PRESCALAR
SFR (FLACL, 0xB7);        // FLASH ACCESS LIMIT
SFR (IP, 0xB8);           // INTERRUPT PRIORITY
SFR (SADEN0, 0xB9);       // UART 0 SLAVE ADDRESS MASK
SFR (AMX2CF, 0xBA);       // ADC 2 MUX CONFIGURATION
SFR (AMX0PRT, 0xBD);      // ADC 0 MUX PORT PIN SELECT REGISTER
SFR (AMX0CF, 0xBA);       // ADC 0 CONFIGURATION REGISTER
SFR (AMX0SL, 0xBB);       // ADC 0 AND ADC 1 MODE SELECTION
SFR (AMX2SL, 0xBB);       // ADC 2 MUX CHANNEL SELECTION
SFR (ADC0CF, 0xBC);       // ADC 0 CONFIGURATION
SFR (ADC2CF, 0xBC);       // ADC 2 CONFIGURATION
SFR (ADC0L, 0xBE);        // ADC 0 DATA - LOW BYTE
SFR (ADC2, 0xBE);         // ADC 2 DATA - LOW BYTE
SFR (ADC0H, 0xBF);        // ADC 0 DATA - HIGH BYTE
SFR (SMB0CN, 0xC0);       // SMBUS 0 CONTROL
SFR (CAN0STA, 0xC0);      // CAN 0 STATUS
SFR (SMB0STA, 0xC1);      // SMBUS 0 STATUS
SFR (SMB0DAT, 0xC2);      // SMBUS 0 DATA
SFR (SMB0ADR, 0xC3);      // SMBUS 0 SLAVE ADDRESS
SFR (ADC0GTL, 0xC4);      // ADC 0 GREATER-THAN REGISTER - LOW BYTE
SFR (ADC2GT, 0xC4);       // ADC 2 GREATER-THAN REGISTER - LOW BYTE
SFR (ADC0GTH, 0xC5);      // ADC 0 GREATER-THAN REGISTER - HIGH BYTE
SFR (ADC0LTL, 0xC6);      // ADC 0 LESS-THAN REGISTER - LOW BYTE
SFR (ADC2LT, 0xC6);       // ADC 2 LESS-THAN REGISTER - LOW BYTE
SFR (ADC0LTH, 0xC7);      // ADC 0 LESS-THAN REGISTER - HIGH BYTE
SFR (TMR2CN, 0xC8);       // TIMER 2 CONTROL
SFR (TMR3CN, 0xC8);       // TIMER 3 CONTROL
SFR (TMR4CN, 0xC8);       // TIMER 4 CONTROL
SFR (P4, 0xC8);           // PORT 4
SFR (TMR2CF, 0xC9);       // TIMER 2 CONFIGURATION
SFR (TMR3CF, 0xC9);       // TIMER 3 CONFIGURATION
SFR (TMR4CF, 0xC9);       // TIMER 4 CONFIGURATION
SFR (RCAP2L, 0xCA);       // TIMER 2 CAPTURE REGISTER - LOW BYTE
SFR (RCAP3L, 0xCA);       // TIMER 3 CAPTURE REGISTER - LOW BYTE
SFR (RCAP4L, 0xCA);       // TIMER 4 CAPTURE REGISTER - LOW BYTE
SFR (RCAP2H, 0xCB);       // TIMER 2 CAPTURE REGISTER - HIGH BYTE
SFR (RCAP3H, 0xCB);       // TIMER 3 CAPTURE REGISTER - HIGH BYTE
SFR (RCAP4H, 0xCB);       // TIMER 4 CAPTURE REGISTER - HIGH BYTE
SFR (TMR2L, 0xCC);        // TIMER 2 - LOW BYTE
SFR (TMR3L, 0xCC);        // TIMER 3 - LOW BYTE
SFR (TMR4L, 0xCC);        // TIMER 4 - LOW BYTE
SFR (TMR2H, 0xCD);        // TIMER 2 - HIGH BYTE
SFR (TMR3H, 0xCD);        // TIMER 3 - HIGH BYTE
SFR (TMR4H, 0xCD);        // TIMER 4 - HIGH BYTE
SFR (SMB0CR, 0xCF);       // SMBUS 0 CLOCK RATE
SFR (PSW, 0xD0);          // PROGRAM STATUS WORD
SFR (REF0CN, 0xD1);       // VOLTAGE REFERENCE 0 CONTROL
SFR (DAC0L, 0xD2);        // DAC 0 REGISTER - LOW BYTE
SFR (DAC1L, 0xD2);        // DAC 1 REGISTER - LOW BYTE
SFR (DAC0H, 0xD3);        // DAC 0 REGISTER - HIGH BYTE
SFR (DAC1H, 0xD3);        // DAC 1 REGISTER - HIGH BYTE
SFR (DAC0CN, 0xD4);       // DAC 0 CONTROL
SFR (DAC1CN, 0xD4);       // DAC 1 CONTROL
SFR (HVA0CN, 0xD6);       // HVDA CONTROL REGISTER
SFR (PCA0CN, 0xD8);       // PCA 0 COUNTER CONTROL
SFR (CAN0DATL, 0xD8);     // CAN 0 DATA - LOW BYTE
SFR (P5, 0xD8);           // PORT 5
SFR (PCA0MD, 0xD9);       // PCA 0 COUNTER MODE
SFR (CAN0DATH, 0xD9);     // CAN 0 DATA - HIGH BYTE
SFR (PCA0CPM0, 0xDA);     // PCA 0 MODULE 0 CONTROL
SFR (CAN0ADR, 0xDA);      // CAN 0 ADDRESS
SFR (PCA0CPM1, 0xDB);     // PCA 0 MODULE 1 CONTROL
SFR (CAN0TST, 0xDB);      // CAN 0 TEST
SFR (PCA0CPM2, 0xDC);     // PCA 0 MODULE 2 CONTROL
SFR (PCA0CPM3, 0xDD);     // PCA 0 MODULE 3 CONTROL
SFR (PCA0CPM4, 0xDE);     // PCA 0 MODULE 4 CONTROL
SFR (PCA0CPM5, 0xDF);     // PCA 0 MODULE 5 CONTROL
SFR (ACC, 0xE0);          // ACCUMULATOR
SFR (PCA0CPL5, 0xE1);     // PCA 0 MODULE 5 CAPTURE/COMPARE - LOW BYTE
SFR (XBR0, 0xE1);         // CROSSBAR CONFIGURATION REGISTER 0
SFR (PCA0CPH5, 0xE2);     // PCA 0 MODULE 5 CAPTURE/COMPARE - HIGH BYTE
SFR (XBR1, 0xE2);         // CROSSBAR CONFIGURATION REGISTER 1
SFR (XBR2, 0xE3);         // CROSSBAR CONFIGURATION REGISTER 2
SFR (XBR3, 0xE4);         // CROSSBAR CONFIGURATION REGISTER 3
SFR (EIE1, 0xE6);         // EXTERNAL INTERRUPT ENABLE 1
SFR (EIE2, 0xE7);         // EXTERNAL INTERRUPT ENABLE 2
SFR (ADC0CN, 0xE8);       // ADC 0 CONTROL
SFR (ADC2CN, 0xE8);       // ADC 2 CONTROL
SFR (P6, 0xE8);           // PORT 6
SFR (PCA0CPL2, 0xE9);     // PCA 0 MODULE 2 CAPTURE/COMPARE - LOW BYTE
SFR (PCA0CPH2, 0xEA);     // PCA 0 MODULE 2 CAPTURE/COMPARE - HIGH BYTE
SFR (PCA0CPL3, 0xEB);     // PCA 0 MODULE 3 CAPTURE/COMPARE - LOW BYTE
SFR (PCA0CPH3, 0xEC);     // PCA 0 MODULE 3 CAPTURE/COMPARE - HIGH BYTE
SFR (PCA0CPL4, 0xED);     // PCA 0 MODULE 4 CAPTURE/COMPARE - LOW BYTE
SFR (PCA0CPH4, 0xEE);     // PCA 0 MODULE 4 CAPTURE/COMPARE - HIGH BYTE
SFR (RSTSRC, 0xEF);       // RESET SOURCE
SFR (B, 0xF0);            // B REGISTER
SFR (EIP1, 0xF6);         // EXTERNAL INTERRUPT PRIORITY REGISTER 1
SFR (EIP2, 0xF7);         // EXTERNAL INTERRUPT PRIORITY REGISTER 2
SFR (SPI0CN, 0xF8);       // SPI 0 CONTROL
SFR (CAN0CN, 0xF8);       // CAN 0 CONTROL
SFR (P7, 0xF8);           // PORT 7
SFR (PCA0L, 0xF9);        // PCA 0 TIMER - LOW BYTE
SFR (PCA0H, 0xFA);        // PCA 0 TIMER - HIGH BYTE
SFR (PCA0CPL0, 0xFB);     // PCA 0 MODULE 0 CAPTURE/COMPARE - LOW BYTE
SFR (PCA0CPH0, 0xFC);     // PCA 0 MODULE 0 CAPTURE/COMPARE - HIGH BYTE
SFR (PCA0CPL1, 0xFD);     // PCA 0 MODULE 1 CAPTURE/COMPARE - LOW BYTE
SFR (PCA0CPH1, 0xFE);     // PCA 0 MODULE 1 CAPTURE/COMPARE - HIGH BYTE
SFR (WDTCN, 0xFF);        // WATCHDOG TIMER CONTROL

//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16(PCA0, 0xF9);        // PCA0 Counter
SFR16(PCA0CP0, 0xFB);     // PCA0 Module 0 Capture/Compare
SFR16(PCA0CP1, 0xFD);     // PCA0 Module 1 Capture/Compare
SFR16(PCA0CP2, 0xE9);     // PCA0 Module 2 Capture/Compare
SFR16(PCA0CP3, 0xEB);     // PCA0 Module 3 Capture/Compare
SFR16(PCA0CP4, 0xED);     // PCA0 Module 4 Capture/Compare
SFR16(PCA0CP5, 0xE1);     // PCA0 Module 5 Capture/Compare
SFR16(CAN0DAT, 0xD8);     // CAN 0 DATA
SFR16(DAC0, 0xD2);        // DAC 0 REGISTER
SFR16(DAC1, 0xD2);        // DAC 1 REGISTER
SFR16(RCAP2, 0xCA);       // TIMER 2 CAPTURE REGISTER
SFR16(RCAP3, 0xCA);       // TIMER 3 CAPTURE REGISTER
SFR16(RCAP4, 0xCA);       // TIMER 4 CAPTURE REGISTER
SFR16(TMR2, 0xCC);        // TIMER 2
SFR16(TMR3, 0xCC);        // TIMER 3
SFR16(TMR4, 0xCC);        // TIMER 4
SFR16(ADC0GT, 0xC4);      // ADC0 Greater-Than Compare
SFR16(ADC0LT, 0xC6);      // ADC0 Less-Than Compare
SFR16(ADC0, 0xBE);        // ADC0 Data
SFR16(DP, 0x82);          // Data Pointer

//-----------------------------------------------------------------------------
// Address Definitions for Bit-addressable Registers
//-----------------------------------------------------------------------------

#define SFR_P0      0x80
#define SFR_TCON    0x88
#define SFR_CPT0CN  0x88
#define SFR_CPT1CN  0x88
#define SFR_CPT2CN  0x88
#define SFR_P1      0x90
#define SFR_SCON0   0x98
#define SFR_SCON1   0x98
#define SFR_P2      0xA0
#define SFR_IE      0xA8
#define SFR_P3      0xB0
#define SFR_IP      0xB8
#define SFR_SMB0CN  0xC0
#define SFR_CAN0STA 0xC0
#define SFR_P4      0xC8
#define SFR_TMR2CN  0xC8
#define SFR_TMR3CN  0xC8
#define SFR_TMR4CN  0xC8
#define SFR_PSW     0xD0
#define SFR_P5      0xD8
#define SFR_PCA0CN  0xD8
#define SFR_ACC     0xE0
#define SFR_P6      0xE8
#define SFR_ADC0CN  0xE8
#define SFR_ADC2CN  0xE8
#define SFR_B       0xF0
#define SFR_SPI0CN  0xF8
#define SFR_CAN0CN  0xF8
#define SFR_P7      0xF8

//-----------------------------------------------------------------------------
// Bit Definitions
//-----------------------------------------------------------------------------

//  TCON  0x88
SBIT (TF1, SFR_TCON, 7);             // TIMER 1 OVERFLOW FLAG
SBIT (TR1, SFR_TCON, 6);             // TIMER 1 ON/OFF CONTROL
SBIT (TF0, SFR_TCON, 5);             // TIMER 0 OVERFLOW FLAG
SBIT (TR0, SFR_TCON, 4);             // TIMER 0 ON/OFF CONTROL
SBIT (IE1, SFR_TCON, 3);             // EXT. INTERRUPT 1 EDGE FLAG
SBIT (IT1, SFR_TCON, 2);             // EXT. INTERRUPT 1 TYPE
SBIT (IE0, SFR_TCON, 1);             // EXT. INTERRUPT 0 EDGE FLAG
SBIT (IT0, SFR_TCON, 0);             // EXT. INTERRUPT 0 TYPE

//  CPT0CN  0x88
SBIT (CP0EN, SFR_CPT0CN, 7);         // COMPARATOR 0 ENABLE
SBIT (CP0OUT, SFR_CPT0CN, 6);        // COMPARATOR 0 OUTPUT
SBIT (CP0RIF, SFR_CPT0CN, 5);        // COMPARATOR 0 RISING EDGE INTERRUPT
SBIT (CP0FIF, SFR_CPT0CN, 4);        // COMPARATOR 0 FALLING EDGE INTERRUPT
SBIT (CP0HYP1, SFR_CPT0CN, 3);       // COMPARATOR 0 POSITIVE HYSTERESIS 1
SBIT (CP0HYP0, SFR_CPT0CN, 2);       // COMPARATOR 0 POSITIVE HYSTERESIS 0
SBIT (CP0HYN1, SFR_CPT0CN, 1);       // COMPARATOR 0 NEGATIVE HYSTERESIS 1
SBIT (CP0HYN0, SFR_CPT0CN, 0);       // COMPARATOR 0 NEGATIVE HYSTERESIS 0

//  CPT1CN  0x88
SBIT (CP1EN, SFR_CPT1CN, 7);         // COMPARATOR 1 ENABLE
SBIT (CP1OUT, SFR_CPT1CN, 6);        // COMPARATOR 1 OUTPUT
SBIT (CP1RIF, SFR_CPT1CN, 5);        // COMPARATOR 1 RISING EDGE INTERRUPT
SBIT (CP1FIF, SFR_CPT1CN, 4);        // COMPARATOR 1 FALLING EDGE INTERRUPT
SBIT (CP1HYP1, SFR_CPT1CN, 3);       // COMPARATOR 1 POSITIVE HYSTERESIS 1
SBIT (CP1HYP0, SFR_CPT1CN, 2);       // COMPARATOR 1 POSITIVE HYSTERESIS 0
SBIT (CP1HYN1, SFR_CPT1CN, 1);       // COMPARATOR 1 NEGATIVE HYSTERESIS 1
SBIT (CP1HYN0, SFR_CPT1CN, 0);       // COMPARATOR 1 NEGATIVE HYSTERESIS 0

//  CPT2CN  0x88
SBIT (CP2EN, SFR_CPT2CN, 7);         // COMPARATOR 2 ENABLE
SBIT (CP2OUT, SFR_CPT2CN, 6);        // COMPARATOR 2 OUTPUT
SBIT (CP2RIF, SFR_CPT2CN, 5);        // COMPARATOR 2 RISING EDGE INTERRUPT
SBIT (CP2FIF, SFR_CPT2CN, 4);        // COMPARATOR 2 FALLING EDGE INTERRUPT
SBIT (CP2HYP1, SFR_CPT2CN, 3);       // COMPARATOR 2 POSITIVE HYSTERESIS 1
SBIT (CP2HYP0, SFR_CPT2CN, 2);       // COMPARATOR 2 POSITIVE HYSTERESIS 0
SBIT (CP2HYN1, SFR_CPT2CN, 1);       // COMPARATOR 2 NEGATIVE HYSTERESIS 1
SBIT (CP2HYN0, SFR_CPT2CN, 0);       // COMPARATOR 2 NEGATIVE HYSTERESIS 0

//  SCON0  0x98
SBIT (SM00, SFR_SCON0, 7);           // UART 0 MODE 0
SBIT (SM10, SFR_SCON0, 6);           // UART 0 MODE 1
SBIT (SM20, SFR_SCON0, 5);           // UART 0 MCE
SBIT (REN0, SFR_SCON0, 4);           // UART 0 RX ENABLE
SBIT (TB80, SFR_SCON0, 3);           // UART 0 TX BIT 8
SBIT (RB80, SFR_SCON0, 2);           // UART 0 RX BIT 8
SBIT (TI0, SFR_SCON0, 1);            // UART 0 TX INTERRUPT FLAG
SBIT (RI0, SFR_SCON0, 0);            // UART 0 RX INTERRUPT FLAG

//  SCON1  0x98
SBIT (S1MODE, SFR_SCON1, 7);         // UART 1 MODE
                                     // Bit6 unused
SBIT (MCE1, SFR_SCON1, 5);           // UART 1 MCE
SBIT (REN1, SFR_SCON1, 4);           // UART 1 RX ENABLE
SBIT (TB81, SFR_SCON1, 3);           // UART 1 TX BIT 8
SBIT (RB81, SFR_SCON1, 2);           // UART 1 RX BIT 8
SBIT (TI1, SFR_SCON1, 1);            // UART 1 TX INTERRUPT FLAG
SBIT (RI1, SFR_SCON1, 0);            // UART 1 RX INTERRUPT FLAG

//  IE  0xA8
SBIT (EA, SFR_IE, 7);                // GLOBAL INTERRUPT ENABLE
                                     // Bit6 unused
SBIT (ET2, SFR_IE, 5);               // TIMER 2 INTERRUPT ENABLE
SBIT (ES0, SFR_IE, 4);               // UART0 INTERRUPT ENABLE
SBIT (ET1, SFR_IE, 3);               // TIMER 1 INTERRUPT ENABLE
SBIT (EX1, SFR_IE, 2);               // EXTERNAL INTERRUPT 1 ENABLE
SBIT (ET0, SFR_IE, 1);               // TIMER 0 INTERRUPT ENABLE
SBIT (EX0, SFR_IE, 0);               // EXTERNAL INTERRUPT 0 ENABLE

//  IP  0xB8
                                     // Bit7 unused
                                     // Bit6 unused
SBIT (PT2, SFR_IP, 5);               // TIMER 2 PRIORITY
SBIT (PS0, SFR_IP, 4);                // SERIAL PORT PRIORITY
SBIT (PT1, SFR_IP, 3);               // TIMER 1 PRIORITY
SBIT (PX1, SFR_IP, 2);               // EXTERNAL INTERRUPT 1 PRIORITY
SBIT (PT0, SFR_IP, 1);               // TIMER 0 PRIORITY
SBIT (PX0, SFR_IP, 0);               // EXTERNAL INTERRUPT 0 PRIORITY

// SMB0CN 0xC0
SBIT (BUSY, SFR_SMB0CN, 7);          // SMBUS 0 BUSY
SBIT (ENSMB, SFR_SMB0CN, 6);         // SMBUS 0 ENABLE
SBIT (STA, SFR_SMB0CN, 5);           // SMBUS 0 START FLAG
SBIT (STO, SFR_SMB0CN, 4);           // SMBUS 0 STOP FLAG
SBIT (SI, SFR_SMB0CN, 3);            // SMBUS 0 INTERRUPT PENDING FLAG
SBIT (AA, SFR_SMB0CN, 2);            // SMBUS 0 ASSERT/ACKNOWLEDGE FLAG
SBIT (SMBFTE, SFR_SMB0CN, 1);        // SMBUS 0 FREE TIMER ENABLE
SBIT (SMBTOE, SFR_SMB0CN, 0);        // SMBUS 0 TIMEOUT ENABLE

// CAN0STA 0xC0
SBIT (BOFF, SFR_CAN0STA, 7);         // Bus Off Status
SBIT (EWARN, SFR_CAN0STA, 6);        // Warning Status
SBIT (EPASS, SFR_CAN0STA, 5);        // Error Passive
SBIT (RXOK, SFR_CAN0STA, 4);         // Received Message Successfully
SBIT (TXOK, SFR_CAN0STA, 3);         // Transmit a Message Successfully
SBIT (LEC2, SFR_CAN0STA, 2);         // Last error code bit 2
SBIT (LEC1, SFR_CAN0STA, 1);         // Last error code bit 1
SBIT (LEC0, SFR_CAN0STA, 0);         // Last error code bit

//  TMR2CN  0xC8
SBIT (TF2, SFR_TMR2CN, 7);           // TIMER 2 OVERFLOW FLAG
SBIT (EXF2, SFR_TMR2CN, 6);          // TIMER 2 EXTERNAL FLAG
                                     // Bit5 unused
                                     // Bit4 unused
SBIT (EXEN2, SFR_TMR2CN, 3);         // TIMER 2 EXTERNAL ENABLE FLAG
SBIT (TR2, SFR_TMR2CN, 2);           // TIMER 2 ON/OFF CONTROL
SBIT (CT2, SFR_TMR2CN, 1);           // TIMER 2 COUNTER SELECT
SBIT (CPRL2, SFR_TMR2CN, 0);         // TIMER 2 CAPTURE SELECT

//  TMR3CN  0xC8
SBIT (TF3, SFR_TMR3CN, 7);           // TIMER 3 OVERFLOW FLAG
SBIT (EXF3, SFR_TMR3CN, 6);          // TIMER 3 EXTERNAL FLAG
                                     // Bit5 unused
                                     // Bit4 unused
SBIT (EXEN3, SFR_TMR3CN, 3);         // TIMER 3 EXTERNAL ENABLE FLAG
SBIT (TR3, SFR_TMR3CN, 2);           // TIMER 3 ON/OFF CONTROL
SBIT (CT3, SFR_TMR3CN, 1);           // TIMER 3 COUNTER SELECT
SBIT (CPRL3, SFR_TMR3CN, 0);         // TIMER 3 CAPTURE SELECT

//  TMR4CN  0xC8
SBIT (TF4, SFR_TMR4CN, 7);           // TIMER 4 OVERFLOW FLAG
SBIT (EXF4, SFR_TMR4CN, 6);          // TIMER 4 EXTERNAL FLAG
                                     // Bit5 unused
                                     // Bit4 unused
SBIT (EXEN4, SFR_TMR4CN, 3);         // TIMER 4 EXTERNAL ENABLE FLAG
SBIT (TR4, SFR_TMR4CN, 2);           // TIMER 4 ON/OFF CONTROL
SBIT (CT4, SFR_TMR4CN, 1);           // TIMER 4 COUNTER SELECT
SBIT (CPRL4, SFR_TMR4CN, 0);         // TIMER 4 CAPTURE SELECT

//  PSW 0xD0
SBIT (CY, SFR_PSW, 7);               // CARRY FLAG
SBIT (AC, SFR_PSW, 6);               // AUXILIARY CARRY FLAG
SBIT (F0, SFR_PSW, 5);               // USER FLAG 0
SBIT (RS1, SFR_PSW, 4);              // REGISTER BANK SELECT 1
SBIT (RS0, SFR_PSW, 3);              // REGISTER BANK SELECT 0
SBIT (OV, SFR_PSW, 2);               // OVERFLOW FLAG
SBIT (F1, SFR_PSW, 1);               // USER FLAG 1
SBIT (P, SFR_PSW, 0);                // ACCUMULATOR PARITY FLAG

// PCA0CN 0xD8
SBIT (CF, SFR_PCA0CN, 7);            // PCA 0 COUNTER OVERFLOW FLAG
SBIT (CR, SFR_PCA0CN, 6);            // PCA 0 COUNTER RUN CONTROL BIT
SBIT (CCF5, SFR_PCA0CN, 5);          // PCA 0 MODULE 5 INTERRUPT FLAG
SBIT (CCF4, SFR_PCA0CN, 4);          // PCA 0 MODULE 4 INTERRUPT FLAG
SBIT (CCF3, SFR_PCA0CN, 3);          // PCA 0 MODULE 3 INTERRUPT FLAG
SBIT (CCF2, SFR_PCA0CN, 2);          // PCA 0 MODULE 2 INTERRUPT FLAG
SBIT (CCF1, SFR_PCA0CN, 1);          // PCA 0 MODULE 1 INTERRUPT FLAG
SBIT (CCF0, SFR_PCA0CN, 0);          // PCA 0 MODULE 0 INTERRUPT FLAG

// ADC0CN 0xE8
SBIT (AD0EN, SFR_ADC0CN, 7);         // ADC 0 ENABLE
SBIT (AD0TM, SFR_ADC0CN, 6);         // ADC 0 TRACK MODE
SBIT (AD0INT, SFR_ADC0CN, 5);        // ADC 0 EOC INTERRUPT FLAG
SBIT (AD0BUSY, SFR_ADC0CN, 4);       // ADC 0 BUSY FLAG
SBIT (AD0CM1, SFR_ADC0CN, 3);        // ADC 0 CONVERT START MODE BIT 1
SBIT (AD0CM0, SFR_ADC0CN, 2);        // ADC 0 CONVERT START MODE BIT 0
SBIT (AD0WINT, SFR_ADC0CN, 1);       // ADC 0 WINDOW INTERRUPT FLAG
SBIT (AD0LJST, SFR_ADC0CN, 0);       // ADC 0 LEFT JUSTIFY SELECT

// ADC2CN 0xE8
SBIT (AD2EN, SFR_ADC2CN, 7);         // ADC 2 ENABLE
SBIT (AD2TM, SFR_ADC2CN, 6);         // ADC 2 TRACK MODE
SBIT (AD2INT, SFR_ADC2CN, 5);        // ADC 2 EOC INTERRUPT FLAG
SBIT (AD2BUSY, SFR_ADC2CN, 4);       // ADC 2 BUSY FLAG
SBIT (AD2CM2, SFR_ADC2CN, 3);        // ADC 2 CONVERT START MODE BIT 2
SBIT (AD2CM1, SFR_ADC2CN, 2);        // ADC 2 CONVERT START MODE BIT 1
SBIT (AD2CM0, SFR_ADC2CN, 1);        // ADC 2 CONVERT START MODE BIT 0
SBIT (AD2WINT, SFR_ADC2CN, 0);       // ADC 2 WINDOW INTERRUPT FLAG

// SPI0CN 0xF8
SBIT (SPIF, SFR_SPI0CN, 7);          // SPI 0 INTERRUPT FLAG
SBIT (WCOL, SFR_SPI0CN, 6);          // SPI 0 WRITE COLLISION FLAG
SBIT (MODF, SFR_SPI0CN, 5);          // SPI 0 MODE FAULT FLAG
SBIT (RXOVRN, SFR_SPI0CN, 4);        // SPI 0 RX OVERRUN FLAG
SBIT (NSSMD1, SFR_SPI0CN, 3); 		 // SPI 0 SLAVE SELECT MODE BIT 1
SBIT (NSSMD0, SFR_SPI0CN, 2); 		 // SPI 0 SLAVE SELECT MODE BIT 0
SBIT (TXBMT, SFR_SPI0CN, 1);         // SPI 0 TX BUFFER EMPTY
SBIT (SPIEN, SFR_SPI0CN, 0);         // SPI 0 SPI ENABLE

// CAN0CN 0xF8
SBIT (CANINIT, SFR_CAN0CN, 0);       // CAN Initialization bit
SBIT (CANIE, SFR_CAN0CN, 1);         // CAN Module Interrupt Enable Bit
SBIT (CANSIE, SFR_CAN0CN, 2);        // CAN Status change Interrupt Enable Bit
SBIT (CANEIE, SFR_CAN0CN, 3);        // CAN Module Error Interrupt Enable Bit
SBIT (CANIF, SFR_CAN0CN, 4);         // CAN Module Interrupt Flag
SBIT (CANDAR, SFR_CAN0CN, 5);        // CAN Disable Automatic Retransmission bit
SBIT (CANCCE, SFR_CAN0CN, 6);        // CAN Configuration Change Enable bit
SBIT (CANTEST, SFR_CAN0CN, 7);       // CAN Test Mode Enable bit


//-----------------------------------------------------------------------------
// SFR PAGE DEFINITIONS
//-----------------------------------------------------------------------------

#define  CONFIG_PAGE       0x0F      // SYSTEM AND PORT CONFIGURATION PAGE
#define  LEGACY_PAGE       0x00      // LEGACY SFR PAGE
#define  TIMER01_PAGE      0x00      // TIMER 0 AND TIMER 1
#define  CPT0_PAGE         0x01      // COMPARATOR 0
#define  CPT1_PAGE         0x02      // COMPARATOR 1
#define  CPT2_PAGE         0x03      // COMPARATOR 2
#define  UART0_PAGE        0x00      // UART 0
#define  UART1_PAGE        0x01      // UART 1
#define  SPI0_PAGE         0x00      // SPI 0
#define  EMI0_PAGE         0x00      // EXTERNAL MEMORY INTERFACE
#define  ADC0_PAGE         0x00      // ADC 0
#define  ADC2_PAGE         0x02      // ADC 2
#define  SMB0_PAGE         0x00      // SMBUS 0
#define  TMR2_PAGE         0x00      // TIMER 2
#define  TMR3_PAGE         0x01      // TIMER 3
#define  TMR4_PAGE         0x02      // TIMER 4
#define  DAC0_PAGE         0x00      // DAC 0
#define  DAC1_PAGE         0x01      // DAC 1
#define  PCA0_PAGE         0x00      // PCA 0
#define  CAN0_PAGE         0x01      // CAN 0


//-----------------------------------------------------------------------------
// Interrupt Priorities
//-----------------------------------------------------------------------------

#define INTERRUPT_INT0                 0  // External Interrupt 0
#define INTERRUPT_TIMER0               1  // Timer0 Overflow
#define INTERRUPT_INT1                 2  // External Interrupt 1
#define INTERRUPT_TIMER1               3  // Timer1 Overflow
#define INTERRUPT_UART0                4  // UART0 Interrupt
#define INTERRUPT_TIMER2               5  // Timer2 Overflow
#define INTERRUPT_SPI                  6  // SPI Interrupt
#define INTERRUPT_SMBUS                7  // SMBus Interrupt
#define INTERRUPT_ADC0_WINDOW          8  // ADC0 Window Comparison
#define INTERRUPT_PCA0                 9  // PCA0 Peripheral
#define INTERRUPT_COMPARATOR0         10  // Comparator0 Interrupt
#define INTERRUPT_COMPARATOR1         11  // Comparator1 Interrupt
#define INTERRUPT_COMPARATOR2         12  // Comparator2 Interrupt
#define INTERRUPT_TIMER3              14  // Timer3 Overflow
#define INTERRUPT_ADC0_EOC            15  // ADC0 End of Conversion
#define INTERRUPT_TIMER4              16  // Timer4 Overflow
#define INTERRUPT_ADC2_WINDOW         17  // ADC2 Window Comparison
#define INTERRUPT_ADC2_EOC            18  // ADC2 End of Conversion
#define INTERRUPT_CAN                 19  // CAN Interrupt
#define INTERRUPT_UART1               20  // UART1 Interrupt

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define C8051F040_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
