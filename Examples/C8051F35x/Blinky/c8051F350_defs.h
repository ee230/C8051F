//-----------------------------------------------------------------------------
// C8051F350_defs.h
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051F35x/'F35x family.
// **Important Note**: The compiler_defs.h header file should be included
// before including this header file.
//
// Target:         C8051F35x, 'F35x
// Tool chain:     Keil
// Command Line:   None
//
// Release 1.4
//    -28 APR 2008 (BW)
//    -Corrected SFR16 addresses for TMR3 and DP
// Release 1.3
//    -3 MAR 2008 (BW)
//    -Reordered some SFRs and defines to address order.
// Release 1.2
//    -29 FEB 2008
//    -Made compiler independant by AS
//    -Added 16-bit sfr definitions
//
//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051F350_DEFS_H
#define C8051F350_DEFS_H

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------

SFR(P0, 0x80);                            // PORT 0 LATCH
SFR(SP, 0x81);                            // STACK POINTER
SFR(DPL, 0x82);                           // DATA POINTER LOW
SFR(DPH, 0x83);                           // DATA POINTER HIGH
SFR(PCON, 0x87);                          // POWER CONTROL
SFR(TCON, 0x88);                          // TIMER/COUNTER CONTROL
SFR(TMOD, 0x89);                          // TIMER/COUNTER MODE
SFR(TL0, 0x8A);                           // TIMER/COUNTER 0 LOW
SFR(TL1, 0x8B);                           // TIMER/COUNTER 1 LOW
SFR(TH0, 0x8C);                           // TIMER/COUNTER 0 HIGH
SFR(TH1, 0x8D);                           // TIMER/COUNTER 1 HIGH
SFR(CKCON, 0x8E);                         // CLOCK CONTROL
SFR(PSCTL, 0x8F);                         // PROGRAM STORE R/W CONTROL
SFR(P1, 0x90);                            // PORT 1 LATCH
SFR(TMR3CN, 0x91);                        // TIMER/COUNTER 3CONTROL
SFR(TMR3RLL, 0x92);                       // TIMER/COUNTER 3 RELOAD LOW
SFR(TMR3RLH, 0x93);                       // TIMER/COUNTER 3 RELOAD HIGH
SFR(TMR3L, 0x94);                         // TIMER/COUNTER 3 LOW
SFR(TMR3H, 0x95);                         // TIMER/COUNTER 3 HIGH
SFR(IDA0, 0x96);                          // CURRENT MODE DAC0 LOW
SFR(SCON0, 0x98);                         // UART0 CONTROL
SFR(SBUF0, 0x99);                         // UART0 DATA BUFFER
SFR(ADC0DECL, 0x9A);                      // ADC0 DECIMATION LOW
SFR(ADC0DECH, 0x9B);                      // ADC0 DECIMATION HIGH
SFR(CPT0CN, 0x9C);                        // COMPARATOR0 CONTROL
SFR(CPT0MD, 0x9D);                        // COMPARATOR0 MODE SELECTION
SFR(CPT0MX, 0x9F);                        // COMPARATOR0 MUX SELECTION
SFR(P2, 0xA0);                            // PORT 2 LATCH
SFR(SPI0CFG, 0xA1);                       // SPI CONFIGURATION
SFR(SPI0CKR, 0xA2);                       // SPI CLOCK RATE CONTROL
SFR(SPI0DAT, 0xA3);                       // SPI DATA
SFR(P0MDOUT, 0xA4);                       // PORT 0 OUTPUT MODE CONFIGURATION
SFR(P1MDOUT, 0xA5);                       // PORT 1 OUTPUT MODE CONFIGURATION
SFR(P2MDOUT, 0xA6);                       // PORT 2 OUTPUT MODE CONFIGURATION
SFR(IE, 0xA8);                            // INTERRUPT ENABLE
SFR(CLKSEL, 0xA9);                        // CLOCK SELECT
SFR(EMI0CN, 0xAA);                        // EXTERNAL MEMORY INTERFACE CONTROL
SFR(ADC0CGL, 0xAB);                       // ADC0 GAIN CALIBRATION LOW
SFR(ADC0CGM, 0xAC);                       // ADC0 GAIN CALIBRATION MIDDLE
SFR(ADC0CGH, 0xAD);                       // ADC0 GAIN CALIBRATION HIGH
SFR(OSCXCN, 0xB1);                        // EXTERNAL OSCILLATOR CONTROL
SFR(OSCICN, 0xB2);                        // INTERNAL OSCILLATOR CONTROL
SFR(OSCICL, 0xB3);                        // INTERNAL OSCILLATOR CALIBRATION
SFR(FLSCL, 0xB6);                         // FLASH SCALE
SFR(FLKEY, 0xB7);                         // FLASH LOCK AND KEY
SFR(IP, 0xB8);                            // INTERRUPT PRIORITY
SFR(IDA0CN, 0xB9);                        // CURRENT MODE DAC0 CONTROL
SFR(ADC0COL, 0xBA);                       // ADC0 OFFSET CALIBRATION LOW
SFR(ADC0COM, 0xBB);                       // ADC0 OFFSET CALIBRATION MIDDLE
SFR(ADC0COH, 0xBC);                       // ADC0 OFFSET CALIBRATION HIGH
SFR(ADC0BUF, 0xBD);                       // ADC0 BUFFER CONTROL
SFR(CLKMUL, 0xBE);                        // CLOCK MULTIPLIER
SFR(ADC0DAC, 0xBF);                       // ADC0 OFFSET DAC
SFR(SMB0CN, 0xC0);                        // SMBUS CONTROL
SFR(SMB0CF, 0xC1);                        // SMBUS CONFIGURATION
SFR(SMB0DAT, 0xC2);                       // SMBUS DATA
SFR(ADC0L, 0xC3);                         // ADC0 OUTPUT LOW
SFR(ADC0M, 0xC4);                         // ADC0 OUTPUT MIDDLE
SFR(ADC0H, 0xC5);                         // ADC0 OUTPUT HIGH
SFR(ADC0MUX, 0xC6);                       // ADC0 MULTIPLEXER
SFR(TMR2CN, 0xC8);                        // TIMER/COUNTER 2 CONTROL
SFR(TMR2RLL, 0xCA);                       // TIMER/COUNTER 2 RELOAD LOW
SFR(TMR2RLH, 0xCB);                       // TIMER/COUNTER 2 RELOAD HIGH
SFR(TMR2L, 0xCC);                         // TIMER/COUNTER 2 LOW
SFR(TMR2H, 0xCD);                         // TIMER/COUNTER 2 HIGH
SFR(PSW, 0xD0);                           // PROGRAM STATUS WORD
SFR(REF0CN, 0xD1);                        // VOLTAGE REFERENCE CONTROL
SFR(P0SKIP, 0xD4);                        // PORT 0 SKIP
SFR(P1SKIP, 0xD5);                        // PORT 1 SKIP
SFR(IDA1CN, 0xD7);                        // CURRENT MODE DAC1 CONTROL
SFR(PCA0CN, 0xD8);                        // PCA CONTROL
SFR(PCA0MD, 0xD9);                        // PCA MODE
SFR(PCA0CPM0, 0xDA);                      // PCA MODULE 0 MODE
SFR(PCA0CPM1, 0xDB);                      // PCA MODULE 1 MODE
SFR(PCA0CPM2, 0xDC);                      // PCA MODULE 2 MODE
SFR(IDA1, 0xDD);                          // CURRENT MODE DAC1 LOW
SFR(ACC, 0xE0);                           // ACCUMULATOR
SFR(XBR0, 0xE1);                          // PORT I/O CROSSBAR CONTROL 0
SFR(XBR1, 0xE2);                          // PORT I/O CROSSBAR CONTROL 1
SFR(PFE0CN, 0xE3);                        // PREFETCH ENGINE CONTROL
SFR(IT01CF, 0xE4);                        // INT0/INT1 CONFIGURATION
SFR(EIE1, 0xE6);                          // EXTENDED INTERRUPT ENABLE 1
SFR(ADC0STA, 0xE8);                       // ADC0 STATUS
SFR(PCA0CPL0, 0xE9);                      // PCA CAPTURE 0 LOW
SFR(PCA0CPH0, 0xEA);                      // PCA CAPTURE 0 HIGH
SFR(PCA0CPL1, 0xEB);                      // PCA CAPTURE 1 LOW
SFR(PCA0CPH1, 0xEC);                      // PCA CAPTURE 1 HIGH
SFR(PCA0CPL2, 0xED);                      // PCA CAPTURE 2 LOW
SFR(PCA0CPH2, 0xEE);                      // PCA CAPTURE 2 HIGH
SFR(RSTSRC, 0xEF);                        // RESET SOURCE CONFIGURATION/STATUS
SFR(B, 0xF0);                             // B REGISTER
SFR(P0MDIN, 0xF1);                        // PORT 0 INPUT MODE CONFIGURATION
SFR(P1MDIN, 0xF2);                        // PORT 1 INPUT MODE CONFIGURATION
SFR(ADC0MD, 0xF3);                        // ADC0 MODE
SFR(ADC0CN, 0xF4);                        // ADC0 CONTROL
SFR(EIP1, 0xF6);                          // EXTENDED INTERRUPT PRIORITY 1
SFR(ADC0CLK, 0xF7);                       // ADC0 CLOCK
SFR(SPI0CN, 0xF8);                        // SPI CONTROL
SFR(PCA0L, 0xF9);                         // PCA COUNTER LOW
SFR(PCA0H, 0xFA);                         // PCA COUNTER HIGH
SFR(ADC0CF, 0xFB);                        // ADC0 CONFIGURATION
SFR(ADC0FL, 0xFC);                        // ADC0 FAST FILTER OUTPUT LOW
SFR(ADC0FM, 0xFD);                        // ADC0 FAST FILTER OUTPUT MIDDLE
SFR(ADC0FH, 0xFE);                        // ADC0 FAST FILTER OUTPUT HIGH
SFR(VDM0CN, 0xFF);                        // VDD MONITOR CONTROL

//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16 (PCA0, 0xF9);                       // PCA0 Counter
SFR16 (PCA0CP0, 0xE9);                    // PCA0 Module 0 Capture/Compare
SFR16 (PCA0CP1, 0xEB);                    // PCA0 Module 1 Capture/Compare
SFR16 (PCA0CP2, 0xED);                    // PCA0 Module 2 Capture/Compare
SFR16 (TMR2RL, 0xCA);                     // Timer 2 Reload
SFR16 (TMR2, 0xCC);                       // Timer 2 Counter
SFR16 (ADC0DEC, 0x9A);                    // ADC0 Decimation
SFR16 (TMR3RL, 0x92);                     // Timer 3 Reload
SFR16 (TMR3, 0x94);                       // Timer 3 Counter
SFR16 (DP, 0x82);                         // Data Pointer

//-----------------------------------------------------------------------------
// Address Definitions for Bit-addressable Registers
//-----------------------------------------------------------------------------

#define SFR_P0       0x80
#define SFR_TCON     0x88
#define SFR_P1       0x90
#define SFR_SCON0    0x98
#define SFR_P2       0xA0
#define SFR_IE       0xA8
#define SFR_IP       0xB8
#define SFR_SMB0CN   0xC0
#define SFR_TMR2CN   0xC8
#define SFR_PSW      0xD0
#define SFR_PCA0CN   0xD8
#define SFR_ACC      0xE0
#define SFR_ADC0STA  0xE8
#define SFR_B        0xF0
#define SFR_SPI0CN   0xF8


//-----------------------------------------------------------------------------
// Bit Definitions
//-----------------------------------------------------------------------------

// TCON 0x88
SBIT (TF1, SFR_TCON, 7);                  // TIMER 1 OVERFLOW FLAG
SBIT (TR1, SFR_TCON, 6);                  // TIMER 1 ON/OFF CONTROL
SBIT (TF0, SFR_TCON, 5);                  // TIMER 0 OVERFLOW FLAG
SBIT (TR0, SFR_TCON, 4);                  // TIMER 0 ON/OFF CONTROL
SBIT (IE1, SFR_TCON, 3);                  // EXT. INTERRUPT 1 EDGE FLAG
SBIT (IT1, SFR_TCON, 2);                  // EXT. INTERRUPT 1 TYPE
SBIT (IE0, SFR_TCON, 1);                  // EXT. INTERRUPT 0 EDGE FLAG
SBIT (IT0, SFR_TCON, 0);                  // EXT. INTERRUPT 0 TYPE

// SCON0 0x98
SBIT (S0MODE, SFR_SCON0, 7);              // UART 0 MODE
                                          // bit 6 not used
SBIT (MCE0, SFR_SCON0, 5);                // UART 0 MCE
SBIT (REN0, SFR_SCON0, 4);                // UART 0 RX ENABLE
SBIT (TB80, SFR_SCON0, 3);                // UART 0 TX BIT 8
SBIT (RB80, SFR_SCON0, 2);                // UART 0 RX BIT 8
SBIT (TI0, SFR_SCON0, 1);                 // UART 0 TX INTERRUPT FLAG
SBIT (RI0, SFR_SCON0, 0);                 // UART 0 RX INTERRUPT FLAG

// IE 0xA8
SBIT (EA, SFR_IE, 7);                     // GLOBAL INTERRUPT ENABLE
SBIT (ESPI0, SFR_IE, 6);                  // SPI0 INTERRUPT ENABLE
SBIT (ET2, SFR_IE, 5);                    // TIMER 2 INTERRUPT ENABLE
SBIT (ES0, SFR_IE, 4);                    // UART0 INTERRUPT ENABLE
SBIT (ET1, SFR_IE, 3);                    // TIMER 1 INTERRUPT ENABLE
SBIT (EX1, SFR_IE, 2);                    // EXTERNAL INTERRUPT 1 ENABLE
SBIT (ET0, SFR_IE, 1);                    // TIMER 0 INTERRUPT ENABLE
SBIT (EX0, SFR_IE, 0);                    // EXTERNAL INTERRUPT 0 ENABLE

// IP 0xB8
                                          // bit 7 not used
SBIT (PSPI0, SFR_IP, 6);                  // SPI0 PRIORITY
SBIT (PT2, SFR_IP, 5);                    // TIMER 2 PRIORITY
SBIT (PS0, SFR_IP, 4);                    // UART0 PRIORITY
SBIT (PT1, SFR_IP, 3);                    // TIMER 1 PRIORITY
SBIT (PX1, SFR_IP, 2);                    // EXTERNAL INTERRUPT 1 PRIORITY
SBIT (PT0, SFR_IP, 1);                    // TIMER 0 PRIORITY
SBIT (PX0, SFR_IP, 0);                    // EXTERNAL INTERRUPT 0 PRIORITY

// SMB0CN 0xC0
SBIT (MASTER, SFR_SMB0CN, 7);             // SMBUS 0 MASTER/SLAVE
SBIT (TXMODE, SFR_SMB0CN, 6);             // SMBUS 0 TRANSMIT MODE
SBIT (STA, SFR_SMB0CN, 5);                // SMBUS 0 START FLAG
SBIT (STO, SFR_SMB0CN, 4);                // SMBUS 0 STOP FLAG
SBIT (ACKRQ, SFR_SMB0CN, 3);              // SMBUS 0 ACKNOWLEDGE REQUEST
SBIT (ARBLOST, SFR_SMB0CN, 2);            // SMBUS 0 ARBITRATION LOST
SBIT (ACK, SFR_SMB0CN, 1);                // SMBUS 0 ACKNOWLEDGE FLAG
SBIT (SI, SFR_SMB0CN, 0);                 // SMBUS 0 INTERRUPT PENDING FLAG

// TMR2CN 0xC8
SBIT (TF2H, SFR_TMR2CN, 7);               // TIMER 2 HIGH BYTE OVERFLOW FLAG
SBIT (TF2L, SFR_TMR2CN, 6);               // TIMER 2 LOW BYTE OVERFLOW FLAG
SBIT (TF2LEN, SFR_TMR2CN, 5);             // TIMER 2 LOW BYTE INTERRUPT ENABLE
                                          // bit 4 not used
SBIT (T2SPLIT, SFR_TMR2CN, 3);            // TIMER 2 SPLIT MODE ENABLE
SBIT (TR2, SFR_TMR2CN, 2);                // TIMER 2 ON/OFF CONTROL
                                          // bit 1 not used
SBIT (T2XCLK, SFR_TMR2CN, 0);             // TIMER 2 EXTERNAL CLOCK SELECT

// PSW 0xD0
SBIT(CY, SFR_PSW, 7);                     // CARRY FLAG
SBIT(AC, SFR_PSW, 6);                     // AUXILIARY CARRY FLAG
SBIT(F0, SFR_PSW, 5);                     // USER FLAG 0
SBIT(RS1, SFR_PSW, 4);                    // REGISTER BANK SELECT 1
SBIT(RS0, SFR_PSW, 3);                    // REGISTER BANK SELECT 0
SBIT(OV, SFR_PSW, 2);                     // OVERFLOW FLAG
SBIT(F1, SFR_PSW, 1);                     // USER FLAG 1
SBIT(P, SFR_PSW, 0);                      // ACCUMULATOR PARITY FLAG

// PCA0CN 0xD8
SBIT (CF, SFR_PCA0CN, 7);                 // PCA 0 COUNTER OVERFLOW FLAG
SBIT (CR, SFR_PCA0CN, 6);                 // PCA 0 COUNTER RUN CONTROL BIT
                                          // bit 5 not used
                                          // bit 4 not used
                                          // bit 3 not used
SBIT (CCF2, SFR_PCA0CN, 2);               // PCA 0 MODULE 2 INTERRUPT FLAG
SBIT (CCF1, SFR_PCA0CN, 1);               // PCA 0 MODULE 1 INTERRUPT FLAG
SBIT (CCF0, SFR_PCA0CN, 0);               // PCA 0 MODULE 0 INTERRUPT FLAG

// ADC0STA 0xE8
SBIT (AD0BUSY, SFR_ADC0STA, 7);           // ADC 0 CONVERSION IN PROGRESS FLAG
SBIT (AD0CBSY, SFR_ADC0STA, 6);           // ADC 0 CALIBRATION IN PROGRESS FLAG
SBIT (AD0INT, SFR_ADC0STA, 5);            // ADC 0 CONVERSION COMPLETE FLAG
SBIT (AD0S3C, SFR_ADC0STA, 4);            // ADC 0 SINC3 FILTER ERROR FLAG
SBIT (AD0FFC, SFR_ADC0STA, 3);            // ADC 0 FAST FILTER ERROR FLAG
SBIT (AD0CALC, SFR_ADC0STA, 2);           // ADC 0 CALIBRATION COMPLETE FLAG
SBIT (AD0ERR, SFR_ADC0STA, 1);            // ADC 0 ERROR FLAG
SBIT (AD0OVR, SFR_ADC0STA, 0);            // ADC 0 OVERRUN FLAG

// SPI0CN 0xF8
SBIT (SPIF, SFR_SPI0CN, 7);               // SPI 0 INTERRUPT FLAG
SBIT (WCOL, SFR_SPI0CN, 6);               // SPI 0 WRITE COLLISION FLAG
SBIT (MODF, SFR_SPI0CN, 5);               // SPI 0 MODE FAULT FLAG
SBIT (RXOVRN, SFR_SPI0CN, 4);             // SPI 0 RX OVERRUN FLAG
SBIT (NSSMD1, SFR_SPI0CN, 3);             // SPI 0 SLAVE SELECT MODE 1
SBIT (NSSMD0, SFR_SPI0CN, 2);             // SPI 0 SLAVE SELECT MODE 0
SBIT (TXBMT, SFR_SPI0CN, 1);              // SPI 0 TX BUFFER EMPTY FLAG
SBIT (SPIEN, SFR_SPI0CN, 0);              // SPI 0 SPI ENABLE

//-----------------------------------------------------------------------------
// Interrupt Priorities
//-----------------------------------------------------------------------------

#define INTERRUPT_INT0                 0  // External Interrupt 0
#define INTERRUPT_TIMER0               1  // Timer0 Overflow
#define INTERRUPT_INT1                 2  // External Interrupt 1
#define INTERRUPT_TIMER1               3  // Timer1 Overflow
#define INTERRUPT_UART0                4  // Serial Port 0
#define INTERRUPT_TIMER2               5  // Timer2 Overflow
#define INTERRUPT_SPI0                 6  // Serial Peripheral Interface 0
#define INTERRUPT_SMB0                 7  // SMB0 Interrupt
                                          // 8 reserved
                                          // 9 reserved
#define INTERRUPT_ADC0                10  // ADC0 Interrupt
#define INTERRUPT_PCA0                11  // PCA0 Peripheral
#define INTERRUPT_COMPARATOR0         12  // Comparator
                                          // 13 reserved
#define INTERRUPT_TIMER3              14  // Timer3 Overflow

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                    // #define C8051F350_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
