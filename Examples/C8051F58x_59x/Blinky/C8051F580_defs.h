//-----------------------------------------------------------------------------
// C8051F580_defs.h
//-----------------------------------------------------------------------------
// Copyright 2008, Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051F58x family.
// **Important Note**: The compiler_defs.h header file should be included
// before including this header file.
//
// Target:         C8051F580/1/2/3/4/5/6/7,8/9-F590/1
// Tool chain:     Generic
// Command Line:   None
//
// Release 0.5 - 10 SEP 2011 (GP)
//    - Fixed SFR16 definition for CAN0IF2MC
//
// Release 0.4 - 08 OCT 2010 (TP)
//    - Fixed bit definitions for SCON0
//
// Release 0.3 - 01 APR 2009 (GP)
//    - Added SN0-SN3
//
// Release 0.2 - 29 JUL 2008 (ADT)
//    - Added Timer 4 bit definitions
//
// Release 0.1 - 09 JUL 2008 (GP)
//    - Initial Revision
//
//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051F580_DEFS_H
#define C8051F580_DEFS_H

//-----------------------------------------------------------------------------
// Page 0, Page 1 and Page F Registers
//-----------------------------------------------------------------------------

SFR (P0, 0x80);                        // Port 0 Latch
SFR (SP, 0x81);                        // Stack Pointer
SFR (DPL, 0x82);                       // Data Pointer Low
SFR (DPH, 0x83);                       // Data Pointer High
SFR (SFR0CN, 0x84);                    // SFR Page Control
SFR (SFRNEXT, 0x85);                   // SFR stack next page
SFR (SFRLAST, 0x86);                   // SFR stack last page
SFR (PCON, 0x87);                      // Power Control
SFR (TCON, 0x88);                      // Timer/Counter Control
SFR (TMOD, 0x89);                      // Timer/Counter Mode
SFR (TL0, 0x8A);                       // Timer/Counter 0 Low
SFR (TL1, 0x8B);                       // Timer/Counter 1 Low
SFR (TH0, 0x8C);                       // Timer/Counter 0 High
SFR (TH1, 0x8D);                       // Timer/Counter 1 High
SFR (CKCON, 0x8E);                     // Clock Control
SFR (PSCTL, 0x8F);                     // Program Store R/W Control
SFR (CLKSEL, 0x8F);                    // System clock select
SFR (P1, 0x90);                        // Port 1 Latch
SFR (TMR3CN, 0x91);                    // Timer/Counter 3 Control
SFR (TMR5CN, 0x91);                    // Timer/Counter 5 Control
SFR (TMR3RLL, 0x92);                   // Timer/Counter 3 Reload Low
SFR (TMR5CAPL, 0x92);                  // Timer/Counter 5 Capture Low
SFR (TMR3RLH, 0x93);                   // Timer/Counter 3 Reload High
SFR (TMR5CAPH, 0x93);                  // Timer/Counter 5 Capture High
SFR (TMR3L, 0x94);                     // Timer/Counter 3 Low
SFR (TMR5L, 0x94);                     // Timer/Counter 5 Low
SFR (TMR3H, 0x95);                     // Timer/Counter 3 High
SFR (TMR5H, 0x95);                     // Timer/Counter 5 High
SFR (TMR5CF, 0x96);                    // Timer/Counter 5 Configuration
SFR (CLKMUL, 0x97);                    // Clock Multiplier
SFR (SCON0, 0x98);                     // UART0 Control
SFR (SCON1, 0x98);                     // UART1 Control
SFR (SBUF0, 0x99);                     // UART0 Data Buffer
SFR (SBUF1, 0x99);                     // UART1 Data Buffer
SFR (CPT0CN, 0x9A);                    // Comparator 0 Control
SFR (CPT2CN, 0x9A);                    // Comparator 2 Control
SFR (CPT0MD, 0x9B);                    // Comparator 0 Mode
SFR (CPT2MD, 0x9B);                    // Comparator 2 Mode
SFR (CPT0MX, 0x9C);                    // Comparator 0 Mux
SFR (CPT2MX, 0x9C);                    // Comparator 2 Mux
SFR (CPT1CN, 0x9D);                    // Comparator 1 Control
SFR (CPT1MD, 0x9E);                    // Comparator 0 Mode
SFR (OSCIFIN, 0x9E);                   // Internal Oscillator Fine Control
SFR (CPT1MX, 0x9F);                    // Comparator 1 Mux
SFR (OSCXCN, 0x9F);                    // External Oscillator Control
SFR (P2, 0xA0);                        // Port 2 Latch
SFR (SPI0CFG, 0xA1);                   // SPI0 Configuration
SFR (OSCICN, 0xA1);                    // Internal Oscillator Control
SFR (SPI0CKR, 0xA2);                   // SPI0 Clock rate control
SFR (OSCICRS, 0xA2);                   // Internal Oscillator Coarse Control
SFR (SPI0DAT, 0xA3);                   // SPI0 Data Buffer
SFR (P0MDOUT, 0xA4);                   // Port 0 Output Mode
SFR (P1MDOUT, 0xA5);                   // Port 1 Output Mode
SFR (P2MDOUT, 0xA6);                   // Port 2 Output Mode
SFR (SFRPAGE, 0xA7);                   // SFR Page Select
SFR (IE, 0xA8);                        // Interrupt Enable
SFR (SMOD0, 0xA9);                     // Serial Port 0 Control
SFR (EMI0CN, 0xAA);                    // EMIF Control
SFR (EMI0TC, 0xAA);                    // EMIF Timing control
SFR (SBCON0, 0xAB);                    // UART0 Baud Rate Generator Control
SFR (SBRLL0, 0xAC);                    // UART0 Baud Rate Generator Low
SFR (SBRLH0, 0xAD);                    // UART0 Baud Rate Generator High
SFR (P3MAT, 0xAE);                     // Port 3 Match
SFR (P3MDOUT, 0xAE);                   // Port 3 Mode
SFR (P3MASK, 0xAF);                    // Port 3 Mask
SFR (P4MDOUT, 0xAF);                   // Port 4 Mode
SFR (P3, 0xB0);                        // Port 3 Latch
SFR (P2MAT, 0xB1);                     // Port 2 Match
SFR (P2MASK, 0xB2);                    // Port 2 Mask
SFR (EMI0CF, 0xB2);                    // EMIF Configuration
SFR (P4, 0xB5);                        // Port 4 Latch
SFR (FLSCL, 0xB6);                     // Flash Scale
SFR (FLKEY, 0xB7);                     // Flash access limit
SFR (IP, 0xB8);                        // Interrupt Priority
SFR (SMB0ADR, 0xB9);                   // SMBus0 Slave address
SFR (ADC0TK, 0xBA);                    // ADC0 Tracking Mode Select
SFR (SMB0ADM, 0xBA);                   // SMBus0 Address Mask
SFR (ADC0MX, 0xBB);                    // AMUX0 Channel select
SFR (ADC0CF, 0xBC);                    // AMUX0 Channel configuration
SFR (ADC0L, 0xBD);                     // ADC0 Data Low
SFR (ADC0H, 0xBE);                     // ADC0 Data High
SFR (SMB0CN, 0xC0);                    // SMBus0 Control
SFR (SMB0CF, 0xC1);                    // SMBus0 Configuration
SFR (SMB0DAT, 0xC2);                   // SMBus0 Data
SFR (ADC0GTL, 0xC3);                   // ADC0 Greater-Than Compare Low
SFR (ADC0GTH, 0xC4);                   // ADC0 Greater-Than Compare High
SFR (ADC0LTL, 0xC5);                   // ADC0 Less-Than Compare Word Low
SFR (ADC0LTH, 0xC6);                   // ADC0 Less-Than Compare Word High
SFR (XBR3, 0xC6);                      // Port I/O Crossbar Control 3
SFR (XBR2, 0xC7);                      // Port I/O Crossbar Control 2
SFR (TMR2CN, 0xC8);                    // Timer/Counter 2 Control
SFR (TMR4CN, 0xC8);                    // Timer/Counter 4 Control
SFR (REG0CN, 0xC9);                    // Regulator Control
SFR (LIN0CF, 0xC9);                    // LIN 0 Configuration
SFR (TMR4CF, 0xC9);                    // Timer/Counter 4 Configuration
SFR (TMR2RLL, 0xCA);                   // Timer/Counter 2 Reload Low
SFR (TMR4CAPL, 0xCA);                  // Timer/Counter 4 Capture Low
SFR (TMR2RLH, 0xCB);                   // Timer/Counter 2 Reload High
SFR (TMR4CAPH, 0xCB);                  // Timer/Counter 4 Capture High
SFR (TMR2L, 0xCC);                     // Timer/Counter 2 Low
SFR (TMR4L, 0xCC);                     // Timer/Counter 4 Low
SFR (TMR2H, 0xCD);                     // Timer/Counter 2 High
SFR (TMR4H, 0xCD);                     // Timer/Counter 4 High
SFR (PCA0CPL5, 0xCE);                  // PCA0 Capture 5 Low
SFR (PCA1CPL11, 0xCE);                 // PCA1 Capture 11 Low
SFR (PCA0CPH5, 0xCF);                  // PCA0 Capture 5 High
SFR (PCA1CPH11, 0xCF);                 // PCA1 Capture 11 High
SFR (PSW, 0xD0);                       // Program Status Word
SFR (REF0CN, 0xD1);                    // Voltage Reference Control
SFR (LIN0DAT, 0xD2);                   // LIN0 Data
SFR (LIN0ADR, 0xD3);                   // LIN0 Address
SFR (P0SKIP, 0xD4);                    // Port 0 Skip
SFR (P1SKIP, 0xD5);                    // Port 1 Skip
SFR (P2SKIP, 0xD6);                    // Port 2 Skip
SFR (P3SKIP, 0xD7);                    // Port 3 Skip
SFR (PCA0CN, 0xD8);                    // PCA0 Control
SFR (PCA1CN, 0xD8);                    // PCA1 Control
SFR (PCA0MD, 0xD9);                    // PCA0 Mode
SFR (PCA1MD, 0xD9);                    // PCA1 Mode
SFR (PCA0PWM, 0xD9);                   // PCA0 PWM Control
SFR (PCA0CPM0, 0xDA);                  // PCA0 Module 0 Mode
SFR (PCA1CPM6, 0xDA);                  // PCA1 Module 6 Mode
SFR (PCA1PWM, 0xDA);                   // PCA1 PWM Control
SFR (PCA0CPM1, 0xDB);                  // PCA0 Module 1  Mode
SFR (PCA1CPM7, 0xDB);                  // PCA1 Module 7  Mode
SFR (PCA0CPM2, 0xDC);                  // PCA0 Module 2  Mode
SFR (PCA1CPM8, 0xDC);                  // PCA1 Module 8  Mode
SFR (PCA0CPM3, 0xDD);                  // PCA0 Module 3  Mode
SFR (PCA1CPM9, 0xDD);                  // PCA1 Module 9  Mode
SFR (PCA0CPM4, 0xDE);                  // PCA0 Module 4  Mode
SFR (PCA1CPM10, 0xDE);                  // PCA1 Module 10 Mode
SFR (PCA0CPM5, 0xDF);                  // PCA0 Module 5  Mode
SFR (PCA1CPM11, 0xDF);                  // PCA1 Module 11 Mode
SFR (ACC, 0xE0);                       // Accumulator
SFR (XBR0, 0xE1);                      // Port I/O Crossbar Control 0
SFR (XBR1, 0xE2);                      // Port I/O Crossbar Control 1
SFR (CCH0CN, 0xE3);                    // Cache control
SFR (IT01CF, 0xE4);                    // INT0/INT1 Configuration
SFR (EIE1, 0xE6);                      // Extended Interrupt Enable 2
SFR (EIE2, 0xE7);                      // Extended Interrupt Enable 2
SFR (ADC0CN, 0xE8);                    // ADC0 Control
SFR (PCA0CPL1, 0xE9);                  // PCA0 Capture 1 Low
SFR (PCA1CPL1, 0xE9);                  // PCA1 Capture 7 Low
SFR (PCA0CPH1, 0xEA);                  // PCA0 Capture 1 High
SFR (PCA1CPH1, 0xEA);                  // PCA1 Capture 7 High
SFR (PCA0CPL2, 0xEB);                  // PCA0 Capture 2 Low
SFR (PCA1CPL2, 0xEB);                  // PCA1 Capture 8 Low
SFR (PCA0CPH2, 0xEC);                  // PCA0 Capture 2 High
SFR (PCA1CPH2, 0xEC);                  // PCA1 Capture 8 High
SFR (PCA0CPL3, 0xED);                  // PCA0 Capture 3 Low
SFR (PCA1CPL3, 0xED);                  // PCA1 Capture 9 Low
SFR (PCA0CPH3, 0xEE);                  // PCA0 Capture 3 High
SFR (PCA1CPH3, 0xEE);                  // PCA1 Capture 9 High
SFR (RSTSRC, 0xEF);                    // Reset Source Configuration/Status
SFR (B, 0xF0);                         // B Register
SFR (P0MAT, 0xF1);                     // Port 0 Match
SFR (P0MDIN, 0xF1);                    // Port 0 Input Mode
SFR (P0MASK, 0xF2);                    // Port 0 Mask
SFR (P1MDIN, 0xF2);                    // Port 1 Input Mode
SFR (P1MAT, 0xF3);                     // Port 1 Match
SFR (P2MDIN, 0xF3);                    // Port 2 Input Mode
SFR (P1MASK, 0xF4);                    // Port 1 Mask
SFR (P3MDIN, 0xF4);                    // Port 3 Input Mode
SFR (PSBANK, 0xF5);                    // Program Space Bank Select
SFR (EIP1, 0xF6);                      // External Interrupt Priority 1
SFR (EIP2, 0xF7);                      // External Interrupt Priority 2
SFR (SPI0CN, 0xF8);                    // SPI0 Control
SFR (PCA0L, 0xF9);                     // PCA0 Counter Low
SFR (PCA1L, 0xF9);                     // PCA1 Counter Low
SFR (SN0, 0xF9);                       // Serial Number 0
SFR (PCA0H, 0xFA);                     // PCA0 Counter High
SFR (PCA1H, 0xFA);                     // PCA1 Counter High
SFR (SN1, 0xFA);                       // Serial Number 1
SFR (PCA0CPL0, 0xFB);                  // PCA0 Capture 0 Low
SFR (PCA1CPL6, 0xFB);                  // PCA1 Capture 6 Low
SFR (SN2, 0xFB);                       // Serial Number 2
SFR (PCA0CPH0, 0xFC);                  // PCA0 Capture 0 High
SFR (PCA1CPH6, 0xFC);                  // PCA1 Capture 6 High
SFR (SN3, 0xFC);                       // Serial Number 3
SFR (PCA0CPL4, 0xFD);                  // PCA0 Capture 4 Low
SFR (PCA1CPL10, 0xFD);                 // PCA1 Capture 10 Low
SFR (PCA0CPH4, 0xFE);                  // PCA0 Capture 4 High
SFR (PCA1CPH10, 0xFE);                 // PCA1 Capture 10 High
SFR (VDM0CN, 0xFF);                    // VDD Monitor Control

//-----------------------------------------------------------------------------
// Page C (CAN0) Registers
//-----------------------------------------------------------------------------

SFR (CAN0CFG, 0x92);                   // CAN0 Clock Configuration
SFR (CAN0STAT, 0x94);                  // Status Register Low Byte
SFR (CAN0ERRL, 0x96);                  // Error Counter Low Byte
SFR (CAN0ERRH, 0x97);                  // Error Counter High Byte
SFR (CAN0BTL, 0x9A);                   // Bit Timing Register Low Byte
SFR (CAN0BTH, 0x9B);                   // Bit Timing Register High Byte
SFR (CAN0IIDL, 0x9C);                  // Interrupt Register Low Byte
SFR (CAN0IIDH, 0x9D);                  // Interrupt Register High Byte
SFR (CAN0TST, 0x9E);                   // Test Register Low Byte
SFR (CAN0BRPE, 0xA1);                  // BRP Extension Register Low Byte
SFR (CAN0TR1L, 0xA2);                  // Transmission Request 1 Low Byte
SFR (CAN0TR1H, 0xA3);                  // Transmission Request 1 High Byte
SFR (CAN0TR2L, 0xA4);                  // Transmission Request 2 Low Byte
SFR (CAN0TR2H, 0xA5);                  // Transmission Request 2 High Byte
SFR (CAN0ND1L, 0xAA);                  // New Data 1 Low Byte
SFR (CAN0ND1H, 0xAB);                  // New Data 1 High Byte
SFR (CAN0ND2L, 0xAC);                  // New Data 2 Low Byte
SFR (CAN0ND2H, 0xAD);                  // New Data 2 High Byte
SFR (CAN0IP1L, 0xAE);                  // Interrupt Pending 1 Low Byte
SFR (CAN0IP1H, 0xAF);                  // Interrupt Pending 1 High Byte
SFR (CAN0IP2L, 0xB2);                  // Interrupt Pending 2 Low Byte
SFR (CAN0IP2H, 0xB3);                  // Interrupt Pending 2 High Byte
SFR (CAN0MV1L, 0xBA);                  // Message Valid 1 Low Byte
SFR (CAN0MV1H, 0xBB);                  // Message Valid 1 High Byte
SFR (CAN0MV2L, 0xBC);                  // Message Valid 2 Low Byte
SFR (CAN0MV2H, 0xBD);                  // Message Valid 2 High Byte
SFR (CAN0IF1CRL, 0xBE);                // IF1 Command Request Low Byte
SFR (CAN0IF1CRH, 0xBF);                // IF1 Command Request High Byte
SFR (CAN0CN, 0xC0);                    // CAN Control Register Low Byte
SFR (CAN0IF1CML, 0xC2);                // IF1 Command Mask Low Byte
SFR (CAN0IF1CMH, 0xC3);                // IF1 Command Mask High Byte
SFR (CAN0IF1M1L, 0xC4);                // IF1 Mask 1 Low Byte
SFR (CAN0IF1M1H, 0xC5);                // IF1 Mask 1 High Byte
SFR (CAN0IF1M2L, 0xC6);                // IF1 Mask 2 Low Byte
SFR (CAN0IF1M2H, 0xC7);                // IF1 Mask 2 High Byte
SFR (CAN0IF1A1L, 0xCA);                // IF1 Arbitration 1 Low Byte
SFR (CAN0IF1A1H, 0xCB);                // IF1 Arbitration 1 High Byte
SFR (CAN0IF1A2L, 0xCC);                // IF1 Arbitration 2 Low Byte
SFR (CAN0IF1A2H, 0xCD);                // IF1 Arbitration 2 High Byte
SFR (CAN0IF2MCL, 0xCE);                // IF2 Message Control Low Byte
SFR (CAN0IF2MCH, 0xCF);                // IF2 Message Control High Byte
SFR (CAN0IF1MCL, 0xD2);                // IF1 Message Control Low Byte
SFR (CAN0IF1MCH, 0xD3);                // IF1 Message Control High Byte
SFR (CAN0IF1DA1L, 0xD4);               // IF1 Data A 1 Low Byte
SFR (CAN0IF1DA1H, 0xD5);               // IF1 Data A 1 High Byte
SFR (CAN0IF1DA2L, 0xD6);               // IF1 Data A 2 Low Byte
SFR (CAN0IF1DA2H, 0xD7);               // IF1 Data A 2 High Byte
SFR (CAN0IF1DB1L, 0xDA);               // IF1 Data B 1 Low Byte
SFR (CAN0IF1DB1H, 0xDB);               // IF1 Data B 1 High Byte
SFR (CAN0IF1DB2L, 0xDC);               // IF1 Data B 2 Low Byte
SFR (CAN0IF1DB2H, 0xDD);               // IF1 Data B 2 High Byte
SFR (CAN0IF2CRL, 0xDE);                // IF2 Command Request Low Byte
SFR (CAN0IF2CRH, 0xDF);                // IF2 Command Request High Byte
SFR (CAN0IF2CML, 0xE2);                // IF2 Command Mask Low Byte
SFR (CAN0IF2CMH, 0xE3);                // IF2 Command Mask High Byte
SFR (CAN0IF2M1L, 0xEA);                // IF2 Mask 1 Low Byte
SFR (CAN0IF2M1H, 0xEB);                // IF2 Mask 1 High Byte
SFR (CAN0IF2M2L, 0xEC);                // IF2 Mask 2 Low Byte
SFR (CAN0IF2M2H, 0xED);                // IF2 Mask 2 High Byte
SFR (CAN0IF2A1L, 0xEE);                // IF2 Arbitration 1 Low Byte
SFR (CAN0IF2A1H, 0xEF);                // IF2 Arbitration 1 High Byte
SFR (CAN0IF2A2L, 0xF2);                // IF2 Arbitration 2 Low Byte
SFR (CAN0IF2A2H, 0xF3);                // IF2 Arbitration 2 High Byte
SFR (CAN0IF2DA1L, 0xF6);               // IF2 Data A 1 Low Byte
SFR (CAN0IF2DA1H, 0xF7);               // IF2 Data A 1 High Byte
SFR (CAN0IF2DA2L, 0xFA);               // IF2 Data A 2 Low Byte
SFR (CAN0IF2DA2H, 0xFB);               // IF2 Data A 2 High Byte
SFR (CAN0IF2DB1L, 0xFC);               // IF2 Data B 1 Low Byte
SFR (CAN0IF2DB1H, 0xFD);               // IF2 Data B 1 High Byte
SFR (CAN0IF2DB2L, 0xFE);               // IF2 Data B 2 Low Byte
SFR (CAN0IF2DB2H, 0xFF);               // IF2 Data B 2 High Byte


//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16 (DP, 0x82);                      // Data Pointer
SFR16 (TMR3RL, 0x92);                  // Timer 3 Reload
SFR16 (TMR5CAP, 0x92);                 // Timer 5 Capture
SFR16 (TMR3, 0x94);                    // Timer 3 Capture / Reload
SFR16 (TMR5, 0x94);                    // Timer 5
SFR16 (SBRL0, 0xAC);                   // UART0 Reload
SFR16 (ADC0, 0xBD);                    // ADC0 data
SFR16 (ADC0GT, 0xC3);                  // ADC0 Greater Than Window
SFR16 (ADC0LT, 0xC5);                  // ADC0 Less Than Window
SFR16 (TMR2RL, 0xCA);                  // Timer 2 Reload
SFR16 (TMR4CAP, 0xCA);                 // Timer 4 Capture
SFR16 (TMR2, 0xCC);                    // Timer 2 Capture / Reload
SFR16 (TMR4, 0xCC);                    // Timer 4
SFR16 (PCA0CP5, 0xCE);                 // PCA0 Module 5 Capture
SFR16 (PCA1CP11, 0xCE);                // PCA1 Module 11 Capture
SFR16 (PCA0CP1, 0xE9);                 // PCA0 Module 1 Capture
SFR16 (PCA1CP7, 0xE9);                 // PCA1 Module 7 Capture
SFR16 (PCA0CP2, 0xEB);                 // PCA0 Module 2 Capture
SFR16 (PCA1CP8, 0xEB);                 // PCA1 Module 8 Capture
SFR16 (PCA0CP3, 0xED);                 // PCA0 Module 3 Capture
SFR16 (PCA1CP9, 0xED);                 // PCA1 Module 9 Capture
SFR16 (PCA0, 0xF9);                    // PCA0 Counter
SFR16 (PCA1, 0xF9);                    // PCA1 Counter
SFR16 (PCA0CP0, 0xFB);                 // PCA0 Module 0 Capture
SFR16 (PCA1CP6, 0xFB);                 // PCA1 Module 6 Capture
SFR16 (PCA0CP4, 0xFD);                 // PCA0 Module 4 Capture
SFR16 (PCA1CP10, 0xFD);                 // PCA1 Module 10 Capture

SFR16 (CAN0ERR, 0x96);                 // Error Counter
SFR16 (CAN0BT, 0x9A);                  // Bit Timing Register
SFR16 (CAN0IID, 0x9C);                 // Interrupt Register
SFR16 (CAN0TR1, 0xA2);                 // Transmission Request 1
SFR16 (CAN0TR2, 0xA4);                 // Transmission Request 2
SFR16 (CAN0ND1, 0xAA);                 // New Data 1
SFR16 (CAN0ND2, 0xAC);                 // New Data 2
SFR16 (CAN0IP1, 0xAE);                 // Interrupt Pending 1
SFR16 (CAN0IP2, 0xB2);                 // Interrupt Pending 2
SFR16 (CAN0MV1, 0xBA);                 // Message Valid 1
SFR16 (CAN0MV2, 0xBC);                 // Message Valid 2
SFR16 (CAN0IF1CR, 0xBE);               // IF1 Command Request
SFR16 (CAN0IF1CM, 0xC2);               // IF1 Command Mask
SFR16 (CAN0IF1M1, 0xC4);               // IF1 Mask 1
SFR16 (CAN0IF1M2, 0xC6);               // IF1 Mask 2
SFR16 (CAN0IF1A1, 0xCA);               // IF1 Arbitration 1
SFR16 (CAN0IF1A2, 0xCC);               // IF1 Arbitration 2
SFR16 (CAN0IF1MC, 0xD2);               // IF1 Message Control
SFR16 (CAN0IF1DA1, 0xD4);              // IF1 Data A 1
SFR16 (CAN0IF1DA2, 0xD6);              // IF1 Data A 2
SFR16 (CAN0IF1DB1, 0xDA);              // IF1 Data B 1
SFR16 (CAN0IF1DB2, 0xDC);              // IF1 Data B 2
SFR16 (CAN0IF2CR, 0xDE);               // IF2 Command Request
SFR16 (CAN0IF2CM, 0xE2);               // IF2 Command Mask
SFR16 (CAN0IF2M1, 0xEA);               // IF2 Mask 1
SFR16 (CAN0IF2M2, 0xEC);               // IF2 Mask 2
SFR16 (CAN0IF2A1, 0xEE);               // IF2 Arbitration 1
SFR16 (CAN0IF2A2, 0xF2);               // IF2 Arbitration 2
SFR16 (CAN0IF2MC, 0xCE);               // IF2 Message Control
SFR16 (CAN0IF2DA1, 0xF6);              // IF2 Data A 1
SFR16 (CAN0IF2DA2, 0xFA);              // IF2 Data A 2
SFR16 (CAN0IF2DB1, 0xFC);              // IF2 Data B 1
SFR16 (CAN0IF2DB2, 0xFE);              // IF2 Data B 2

//-----------------------------------------------------------------------------
// LIN0 Indirect Registers
//-----------------------------------------------------------------------------

#define  LIN0DT1   0x00                // LIN0 Data Byte 1
#define  LIN0DT2   0x01                // LIN0 Data Byte 2
#define  LIN0DT3   0x02                // LIN0 Data Byte 3
#define  LIN0DT4   0x03                // LIN0 Data Byte 4
#define  LIN0DT5   0x04                // LIN0 Data Byte 5
#define  LIN0DT6   0x05                // LIN0 Data Byte 6
#define  LIN0DT7   0x06                // LIN0 Data Byte 7
#define  LIN0DT8   0x07                // LIN0 Data Byte 8
#define  LIN0CTRL  0x08                // LIN0 Control
#define  LIN0ST    0x09                // LIN0 Status
#define  LIN0ERR   0x0A                // LIN0 Error
#define  LIN0SIZE  0x0B                // LIN0 Message Size
#define  LIN0DIV   0x0C                // LIN0 Divider
#define  LIN0MUL   0x0D                // LIN0 Multiplier
#define  LIN0ID    0x0E                // LIN0 Identifier

//-----------------------------------------------------------------------------
// Address Definitions for Bit-addressable Registers
//-----------------------------------------------------------------------------

#define SFR_P0       0x80
#define SFR_TCON     0x88
#define SFR_P1       0x90
#define SFR_SCON0    0x98
#define SFR_SCON1    0x98
#define SFR_P2       0xA0
#define SFR_IE       0xA8
#define SFR_P3       0xB0
#define SFR_IP       0xB8
#define SFR_SMB0CN   0xC0
#define SFR_TMR2CN   0xC8
#define SFR_TMR4CN   0xC8
#define SFR_PSW      0xD0
#define SFR_PCA0CN   0xD8
#define SFR_PCA1CN   0xD8
#define SFR_ACC      0xE0
#define SFR_ADC0CN   0xE8
#define SFR_B        0xF0
#define SFR_SPI0CN   0xF8

//-----------------------------------------------------------------------------
// Bit Definitions
//-----------------------------------------------------------------------------

// TCON 0x88
SBIT (TF1, SFR_TCON, 7);               // Timer 1 Overflow Flag
SBIT (TR1, SFR_TCON, 6);               // Timer 1 On/Off Control
SBIT (TF0, SFR_TCON, 5);               // Timer 0 Overflow Flag
SBIT (TR0, SFR_TCON, 4);               // Timer 0 On/Off Control
SBIT (IE1, SFR_TCON, 3);               // Ext. Interrupt 1 Edge Flag
SBIT (IT1, SFR_TCON, 2);               // Ext. Interrupt 1 Type
SBIT (IE0, SFR_TCON, 1);               // Ext. Interrupt 0 Edge Flag
SBIT (IT0, SFR_TCON, 0);               // Ext. Interrupt 0 Type

// SCON0 0x98
SBIT (OVR0, SFR_SCON0, 7);             // UART0 Receive FIFO Overrun Flag
SBIT (PERR0, SFR_SCON0, 6);            // UART0 Parity Error Flag
SBIT (THRE0, SFR_SCON0, 5);            // UART0 Transmit Register Empty Flag
SBIT (REN0, SFR_SCON0, 4);             // UART0 RX Enable
SBIT (TBX0, SFR_SCON0, 3);             // UART0 Extra Transmission Bit
SBIT (RBX0, SFR_SCON0, 2);             // UART0 Extra Receive Bit
SBIT (TI0, SFR_SCON0, 1);              // UART0 TX Interrupt Flag
SBIT (RI0, SFR_SCON0, 0);              // UART0 RX Interrupt Flag

// SCON1 0x98
SBIT (S1MODE, SFR_SCON1, 7);           // UART1 Mode 0
                                       // Bit 6 UNUSED
SBIT (MCE1, SFR_SCON1, 5);             // UART1 Multiprocessor enable
SBIT (REN1, SFR_SCON1, 4);             // UART1 RX Enable
SBIT (TB81, SFR_SCON1, 3);             // UART1 TX Bit 8
SBIT (RB81, SFR_SCON1, 2);             // UART1 RX Bit 8
SBIT (TI1, SFR_SCON1, 1);              // UART1 TX Interrupt Flag
SBIT (RI1, SFR_SCON1, 0);              // UART1 RX Interrupt Flag

// IE 0xA8
SBIT (EA, SFR_IE, 7);                  // Global Interrupt Enable
SBIT (ESPI0, SFR_IE, 6);               // SPI0 Interrupt Enable
SBIT (ET2, SFR_IE, 5);                 // Timer 2 Interrupt Enable
SBIT (ES0, SFR_IE, 4);                 // UART0 Interrupt Enable
SBIT (ET1, SFR_IE, 3);                 // Timer 1 Interrupt Enable
SBIT (EX1, SFR_IE, 2);                 // External Interrupt 1 Enable
SBIT (ET0, SFR_IE, 1);                 // Timer 0 Interrupt Enable
SBIT (EX0, SFR_IE, 0);                 // External Interrupt 0 Enable

// IP 0xB8
                                       // Bit 7 unused
SBIT (PSPI0, SFR_IP, 6);               // SPI0 Interrupt Priority
SBIT (PT2, SFR_IP, 5);                 // Timer 2 Priority
SBIT (PS0, SFR_IP, 4);                 // UART0 Priority
SBIT (PS, SFR_IP, 4);                  // UART0 Priority
SBIT (PT1, SFR_IP, 3);                 // Timer 1 Priority
SBIT (PX1, SFR_IP, 2);                 // External Interrupt 1 Priority
SBIT (PT0, SFR_IP, 1);                 // Timer 0 Priority
SBIT (PX0, SFR_IP, 0);                 // External Interrupt 0 Priority

// SMB0CN 0xC0
SBIT (MASTER, SFR_SMB0CN, 7);          // SMBus0 Master/Slave Indicator
SBIT (TXMODE, SFR_SMB0CN, 6);          // SMBus0 Transmit Mode Indicator
SBIT (STA, SFR_SMB0CN, 5);             // SMBus0 Start Flag
SBIT (STO, SFR_SMB0CN, 4);             // SMBus0 Stop Flag
SBIT (ACKRQ, SFR_SMB0CN, 3);           // SMBus0 Acknowledge Request
SBIT (ARBLOST, SFR_SMB0CN, 2);         // SMBus0 Arbitration Lost Indicator
SBIT (ACK, SFR_SMB0CN, 1);             // SMBus0 Acknowledge
SBIT (SI, SFR_SMB0CN, 0);              // SMBus0 Interrupt Flag

// TMR2CN 0xC8
SBIT (TF2H, SFR_TMR2CN, 7);            // Timer 2 High-Byte Overflow Flag
SBIT (TF2L, SFR_TMR2CN, 6);            // Timer 2 Low-Byte  Overflow Flag
SBIT (TF2LEN, SFR_TMR2CN, 5);          // Timer 2 Low-Byte Flag Enable
SBIT (TF2CEN, SFR_TMR2CN, 4);          // Timer 2 Capture Enable
SBIT (T2SPLIT, SFR_TMR2CN, 3);         // Timer 2 Split-Mode Enable
SBIT (TR2, SFR_TMR2CN, 2);             // Timer 2 Run Enable
SBIT (T2RCLK, SFR_TMR2CN, 1);          // Timer 2 Xclk/Rclk Select
SBIT (T2XCLK, SFR_TMR2CN, 0);          // Timer 2 Clk/8 Clock Source

// TMR4CN 0xC8
SBIT (TF4, SFR_TMR4CN, 7);             // Timer 4 Overflow/Underflow Flag
SBIT (EXF4, SFR_TMR4CN, 6);            // Timer 4 External Flag
                                       // Bit 5 unused
                                       // Bit 4 unused
SBIT (EXE4, SFR_TMR4CN, 3);            // Timer 4 External Enable
SBIT (TR4, SFR_TMR4CN, 2);             // Timer 4 Run Enable
SBIT (CT4, SFR_TMR4CN, 1);             // Timer 4 Counter/Timer Select
SBIT (CPRL4, SFR_TMR4CN, 0);           // Timer 4 Capture/Reload Select

// PSW 0xD0
SBIT (CY, SFR_PSW, 7);                 // Carry Flag
SBIT (AC, SFR_PSW, 6);                 // Auxiliary Carry Flag
SBIT (F0, SFR_PSW, 5);                 // User Flag 0
SBIT (RS1, SFR_PSW, 4);                // Register Bank Select 1
SBIT (RS0, SFR_PSW, 3);                // Register Bank Select 0
SBIT (OV, SFR_PSW, 2);                 // Overflow Flag
SBIT (F1, SFR_PSW, 1);                 // User Flag 1
SBIT (P, SFR_PSW, 0);                  // Accumulator Parity Flag

// PCA0CN 0xD8
SBIT (CF, SFR_PCA0CN, 7);              // PCA0 Counter Overflow Flag
SBIT (CR, SFR_PCA0CN, 6);              // PCA0 Counter Run Control Bit
SBIT (CCF5, SFR_PCA0CN, 5);            // PCA0 Module 5 Interrupt Flag
SBIT (CCF4, SFR_PCA0CN, 4);            // PCA0 Module 4 Interrupt Flag
SBIT (CCF3, SFR_PCA0CN, 3);            // PCA0 Module 3 Interrupt Flag
SBIT (CCF2, SFR_PCA0CN, 2);            // PCA0 Module 2 Interrupt Flag
SBIT (CCF1, SFR_PCA0CN, 1);            // PCA0 Module 1 Interrupt Flag
SBIT (CCF0, SFR_PCA0CN, 0);            // PCA0 Module 0 Interrupt Flag

// PCA1CN 0xD8
SBIT (CF1, SFR_PCA1CN, 7);             // PCA1 Counter Overflow Flag
SBIT (CR1, SFR_PCA1CN, 6);             // PCA1 Counter Run Control Bit
SBIT (CCF11, SFR_PCA1CN, 5);           // PCA1 Module 11 Interrupt Flag
SBIT (CCF10, SFR_PCA1CN, 4);           // PCA1 Module 10 Interrupt Flag
SBIT (CCF9, SFR_PCA1CN, 3);            // PCA1 Module 9 Interrupt Flag
SBIT (CCF8, SFR_PCA1CN, 2);            // PCA1 Module 8 Interrupt Flag
SBIT (CCF7, SFR_PCA1CN, 1);            // PCA1 Module 7 Interrupt Flag
SBIT (CCF6, SFR_PCA1CN, 0);            // PCA1 Module 6 Interrupt Flag

// ADC0CN 0xE8
SBIT (AD0EN, SFR_ADC0CN, 7);           // ADC0 Enable
SBIT (BURSTEN, SFR_ADC0CN, 6);         // ADC0 Burst Enable
SBIT (AD0INT, SFR_ADC0CN, 5);          // ADC0 EOC Interrupt Flag
SBIT (AD0BUSY, SFR_ADC0CN, 4);         // ADC0 Busy Flag
SBIT (AD0WINT, SFR_ADC0CN, 3);         // ADC0 Window Compare Interrupt Flag
SBIT (AD0LJST, SFR_ADC0CN, 2);         // ADC0 Left Justified
SBIT (AD0CM1, SFR_ADC0CN, 1);          // ADC0 Start Of Conversion Mode Bit 1
SBIT (AD0CM0, SFR_ADC0CN, 0);          // ADC0 Start Of Conversion Mode Bit 0

// SPI0CN 0xF8
SBIT (SPIF, SFR_SPI0CN, 7);            // SPI0 Interrupt Flag
SBIT (WCOL, SFR_SPI0CN, 6);            // SPI0 Write Collision Flag
SBIT (MODF, SFR_SPI0CN, 5);            // SPI0 Mode Fault Flag
SBIT (RXOVRN, SFR_SPI0CN, 4);          // SPI0 RX Overrun Flag
SBIT (NSSMD1, SFR_SPI0CN, 3);          // SPI0 Slave Select Mode 1
SBIT (NSSMD0, SFR_SPI0CN, 2);          // SPI0 Slave Select Mode 0
SBIT (TXBMT, SFR_SPI0CN, 1);           // SPI0 TX Buffer Empty Flag
SBIT (SPIEN, SFR_SPI0CN, 0);           // SPI0 Enable

//-----------------------------------------------------------------------------
// Interrupt Priorities
//-----------------------------------------------------------------------------

#define INTERRUPT_INT0             0   // External Interrupt 0
#define INTERRUPT_TIMER0           1   // Timer 0 Overflow
#define INTERRUPT_INT1             2   // External Interrupt 1
#define INTERRUPT_TIMER1           3   // Timer 1 Overflow
#define INTERRUPT_UART0            4   // UART0
#define INTERRUPT_TIMER2           5   // Timer 2 Overflow
#define INTERRUPT_SPI0             6   // SPI0
#define INTERRUPT_SMBUS0           7   // SMBus0 Interface
#define INTERRUPT_ADC0_WINDOW      8   // ADC0 Window Comparison
#define INTERRUPT_ADC0_EOC         9   // ADC0 End Of Conversion
#define INTERRUPT_PCA0            10   // PCA0 Peripheral
#define INTERRUPT_COMPARATOR0     11   // Comparator 0 Comparison
#define INTERRUPT_COMPARATOR1     12   // Comparator 1 Comparison
#define INTERRUPT_TIMER3          13   // Timer 3 Overflow
#define INTERRUPT_LIN0            14   // LIN Bus Interrupt
#define INTERRUPT_VREG            15   // Voltage Regulator
#define INTERRUPT_CAN0            16   // CAN Bus Interrupt
#define INTERRUPT_PORT_MATCH      17   // Port Match
#define INTERRUPT_UART1           18   // UART1
#define INTERRUPT_PCA1            19   // PCA1 Peripheral
#define INTERRUPT_COMPARATOR2     20   // Comparator 2 Comparison
#define INTERRUPT_TIMER4          21   // Timer 4 Overflow
#define INTERRUPT_TIMER5          22   // Timer 5 Overflow


//-----------------------------------------------------------------------------
// SFR Page Definitions
//-----------------------------------------------------------------------------

#define  CONFIG_PAGE       0x0F        // System and Port Configuration Page
#define  ACTIVE_PAGE       0x00        // Active Use Page
#define  ACTIVE2_PAGE      0x10        // Active Use Page 2
#define  CAN0_PAGE         0x0C        // CAN0 Registers

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define C8051F580_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------