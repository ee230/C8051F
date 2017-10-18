//-----------------------------------------------------------------------------
// C8051F520A_defs.h
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051F52xA/'F53xA family.
//
// Target:         C8051F52xA, 'F53xA
// Tool chain:     Keil, SDCC
// Command Line:   None
//
// Release 1.4
//    -22 JULY 2008
//    -Updated TMR2CN bit definitions (ES)
//
// Release 1.3
//    -All changes by TP
//    -25 JUN 2008
//    Added LIN indirect registers
//
// Release 1.2
//    -All changes by TP
//    -29 JAN 2008
//    Updated to use "compiler_defs.h"
//
// Release 1.1
//    -All changes by CG
//    -07 AUG 2007
//    Changed the name of the register AMX0SL to ADC0MX. (0xBB address)
//
// Release 1.0
//    -All changes by CG/PKC
//    -04 OCT 2006
//    Initial Release
//

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051F520A_H
#define C8051F520A_H

#include <compiler_defs.h>

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------


SFR (P0, 0x80);                        // Port 0 Latch
SFR (SP, 0x81);                        // Stack Pointer
SFR (DPL, 0x82);                       // Data Pointer - Low byte
SFR (DPH, 0x83);                       // Data Pointer - High byte
SFR (PCON, 0x87);                      // Power Control
SFR (TCON, 0x88);                      // Timer Control
SFR (TMOD, 0x89);                      // Timer Mode
SFR (TL0, 0x8A);                       // Timer 0 - Low byte
SFR (TL1, 0x8B);                       // Timer 1 - Low byte
SFR (TH0, 0x8C);                       // Timer 0 - High byte
SFR (TH1, 0x8D);                       // Timer 1 - High byte
SFR (CKCON, 0x8E);                     // Clock Control
SFR (PSCTL, 0x8F);                     // Program Store R/W Control
SFR (P1, 0x90);                        // Port 1 Latch
SFR (LINADDR, 0x92);                   // LIN Indirect Access Address
SFR (LINDATA, 0x93);                   // LIN Indirect Access Data
SFR (LINCF, 0x95);                     // LIN Configuration
SFR (SCON0, 0x98);                     // UART0 Control
SFR (SBUF0, 0x99);                     // UART0 Buffer
SFR (CPT0CN, 0x9B);                    // Comparator 0 Control
SFR (CPT0MD, 0x9D);                    // Comparator 0 Mode
SFR (CPT0MX, 0x9F);                    // Comparator 0 Mux
SFR (SPI0CFG, 0xA1);                   // SPI0 Configuration
SFR (SPI0CKR, 0xA2);                   // SPI0 Clock Rate
SFR (SPI0DAT, 0xA3);                   // SPI0 Data
SFR (P0MDOUT, 0xA4);                   // Port 0 Output Mode Configuration
SFR (P1MDOUT, 0xA5);                   // Port 1 Output Mode Configuration
SFR (IE, 0xA8);                        // Interrupt Enable
SFR (CLKSEL, 0xA9);                    // Clock Select
SFR (OSCIFIN, 0xB0);                   // Internal Fine Oscillator Calibration
SFR (OSCXCN, 0xB1);                    // External Oscillator Control
SFR (OSCICN, 0xB2);                    // Internal Oscillator Control
SFR (OSCICL, 0xB3);                    // Internal Oscillator Calibration
SFR (FLKEY, 0xB7);                     // Flash Lock & Key
SFR (IP, 0xB8);                        // Interrupt Priority
SFR (ADC0TK, 0xBA);                    // ADC0 Tracking
SFR (ADC0MX, 0xBB);                    // ADC0 Mux Channel Selection
SFR (ADC0CF, 0xBC);                    // ADC0 CONFIGURATION
SFR (ADC0L, 0xBD);                     // ADC0 LSB Result
SFR (ADC0H, 0xBE);                     // ADC0 Data
SFR (P1MASK, 0xBF);                    // Port 1 Mask
SFR (ADC0GTL, 0xC3);                   // ADC0 Greater-Than Compare Low
SFR (ADC0GTH, 0xC4);                   // ADC0 Greater-Than Compare High
SFR (ADC0LTL, 0xC5);                   // ADC0 Less-Than Compare Word Low
SFR (ADC0LTH, 0xC6);                   // ADC0 Less-Than Compare Word High
SFR (P0MASK, 0xC7);                    // Port 1 Mask
SFR (TMR2CN, 0xC8);                    // Timer 2 Control
SFR (REG0CN, 0xC9);                    // Regulator Control
SFR (TMR2RLL, 0xCA);                   // Timer 2 Reload Low
SFR (TMR2RLH, 0xCB);                   // Timer 2 Reload High
SFR (TMR2L, 0xCC);                     // Timer 2 Low Byte
SFR (TMR2H, 0xCD);                     // Timer 2 High Byte
SFR (P1MAT, 0xCF);                     // Port1 Match
SFR (PSW, 0xD0);                       // Program Status Word
SFR (REF0CN, 0xD1);                    // Voltage Reference 0 Control
SFR (P0SKIP, 0xD4);                    // Port 0 Skip
SFR (P1SKIP, 0xD5);                    // Port 1 Skip
SFR (P0MAT, 0xD7);                     // Port 0 Match
SFR (PCA0CN, 0xD8);                    // PCA0 Control
SFR (PCA0MD, 0xD9);                    // PCA0 Mode
SFR (PCA0CPM0, 0xDA);                  // PCA0 Module 0 Mode
SFR (PCA0CPM1, 0xDB);                  // PCA0 Module 1 Mode
SFR (PCA0CPM2, 0xDC);                  // PCA0 Module 2 Mode
SFR (ACC, 0xE0);                       // Accumulator
SFR (XBR0, 0xE1);                      // Digital Crossbar Configuration 0
SFR (XBR1, 0xE2);                      // Digital Crossbar Configuration 1
SFR (IT01CF, 0xE4);                    // INT0/INT1 Configuration
SFR (EIE1, 0xE6);                      // Extended Interrupt Enable 1
SFR (ADC0CN, 0xE8);                    // ADC 0 Control
SFR (PCA0CPL1, 0xE9);                  // PCA0 Module 1 Capture/Compare Low Byte
SFR (PCA0CPH1, 0xEA);                  // PCA0 Module 1 Capture/Compare High Byte
SFR (PCA0CPL2, 0xEB);                  // PCA0 Module 2 Capture/Compare Low Byte
SFR (PCA0CPH2, 0xEC);                  // PCA0 Module 2 Capture/Compare High Byte
SFR (RSTSRC, 0xEF);                    // Reset Source Configuration/Status
SFR (B, 0xF0);                         // B Register
SFR (P0MDIN, 0xF1);                    // Port 0 Input Mode
SFR (P1MDIN, 0xF2);                    // Port 1 Input Mode
SFR (EIP1, 0xF6);                      // Extended Interrupt Priority 1
SFR (SPI0CN, 0xF8);                    // SPI0 Control
SFR (PCA0L, 0xF9);                     // PCA0 Counter Low Byte
SFR (PCA0H, 0xFA);                     // PCA0 Counter High Byte
SFR (PCA0CPL0, 0xFB);                  // PCA Module 0 Capture/Compare Low Byte
SFR (PCA0CPH0, 0xFC);                  // PCA Module 0 Capture/Compare High Byte
SFR (VDDMON, 0xFF);                    // VDD Monitor

//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16 (DP, 0x82);                      // Data Pointer
SFR16 (ADC0, 0xBD);                    // ADC0 Data
SFR16 (ADC0GT, 0xC3);                  // ADC0 Greater-Than Compare
SFR16 (ADC0LT, 0xC5);                  // ADC0 Less-Than Compare
SFR16 (TMR2RL, 0xCA);                  // Timer 2 Reload
SFR16 (TMR2, 0xCC);                    // Timer 2 Counter
SFR16 (PCA0, 0xF9);                    // PCA0 Counter
SFR16 (PCA0CP0, 0xFB);                 // PCA0 Module 0 Capture/Compare
SFR16 (PCA0CP1, 0xE9);                 // PCA0 Module 1 Capture/Compare
SFR16 (PCA0CP2, 0xEB);                 // PCA0 Module 2 Capture/Compare

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
#define SFR_IE       0xA8
#define SFR_OSCIFIN  0xB0
#define SFR_IP       0xB8
#define SFR_TMR2CN   0xC8
#define SFR_PSW      0xD0
#define SFR_PCA0CN   0xD8
#define SFR_ACC      0xE0
#define SFR_ADC0CN   0xE8
#define SFR_B        0xF0
#define SFR_SPI0CN   0xF8

//-----------------------------------------------------------------------------
// Bit Definitions
//-----------------------------------------------------------------------------

// TCON  0x88
SBIT (TF1, SFR_TCON, 7);               // Timer 1 Overflow Flag
SBIT (TR1, SFR_TCON, 6);               // Timer 1 On/Off Control
SBIT (TF0, SFR_TCON, 5);               // Timer 0 Overflow Flag
SBIT (TR0, SFR_TCON, 4);               // Timer 0 On/Off Control
SBIT (IE1, SFR_TCON, 3);               // External Interrupt 1 Edge Flag
SBIT (IT1, SFR_TCON, 2);               // External Interrupt 1 Type
SBIT (IE0, SFR_TCON, 1);               // External Interrupt 0 Edge Flag
SBIT (IT0, SFR_TCON, 0);               // External Interrupt 0 Type

// SCON0  0x98
SBIT (S0MODE, SFR_SCON0, 7);           // Serial Mode Control Bit 0
                                       // Bit6 UNUSED
SBIT (MCE0, SFR_SCON0, 5);             // Multiprocessor Communication Enable
SBIT (REN0, SFR_SCON0, 4);             // Receive Enable
SBIT (TB80, SFR_SCON0, 3);             // Transmit Bit 8
SBIT (RB80, SFR_SCON0, 2);             // Receive Bit 8
SBIT (TI0, SFR_SCON0, 1);              // Transmit Interrupt Flag
SBIT (RI0, SFR_SCON0, 0);              // Receive Interrupt Flag

// IE  0xA8
SBIT (EA, SFR_IE, 7);                  // Global Interrupt Enable
SBIT (ESPI0, SFR_IE, 6);               // SPI0 Interrupt Enable
SBIT (ET2, SFR_IE, 5);                 // Timer 2 Interrupt Enable
SBIT (ES0, SFR_IE, 4);                 // UART0 Interrupt Enable
SBIT (ET1, SFR_IE, 3);                 // Timer 1 Interrupt Enable
SBIT (EX1, SFR_IE, 2);                 // External Interrupt 1 Enable
SBIT (ET0, SFR_IE, 1);                 // Timer 0 Interrupt Enable
SBIT (EX0, SFR_IE, 0);                 // External Interrupt 0 Enable

// IP  0xB8
                                       // Bit7 UNUSED
SBIT (PSPI0, SFR_IP, 6);               // SPI0 Interrupt Priority
SBIT (PT2, SFR_IP, 5);                 // Timer 2 Priority
SBIT (PS0, SFR_IP, 4);                 // UART0 Priority
SBIT (PT1, SFR_IP, 3);                 // Timer 1 Priority
SBIT (PX1, SFR_IP, 2);                 // External Interrupt 1 Priority
SBIT (PT0, SFR_IP, 1);                 // Timer 0 Priority
SBIT (PX0, SFR_IP, 0);                 // External Interrupt 0 Priority

// TMR2CN 0xC8
SBIT (TF2H, SFR_TMR2CN, 7);            // Timer 2 High-Byte Overflow Flag
SBIT (TF2L, SFR_TMR2CN, 6);            // Timer 2 Low-Byte  Overflow Flag
SBIT (TF2LEN, SFR_TMR2CN, 5);          // Timer 2 Low-Byte Flag Enable
SBIT (TF2CEN, SFR_TMR2CN, 4);          // Timer 2 Capture Enable
SBIT (T2SPLIT, SFR_TMR2CN, 3);         // Timer 2 Split-Mode Enable
SBIT (TR2, SFR_TMR2CN, 2);             // Timer 2 On/Off Control
                                       // Bit1 UNUSED
SBIT (T2XCLK, SFR_TMR2CN, 0);          // Timer 2 Clk/8 Clock Source

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
                                       // Bit5 UNUSED
                                       // Bit4 UNUSED
                                       // Bit3 UNUSED
SBIT (CCF2, SFR_PCA0CN, 2);            // PCA0 Module 2 Interrupt Flag
SBIT (CCF1, SFR_PCA0CN, 1);            // PCA0 Module 1 Interrupt Flag
SBIT (CCF0, SFR_PCA0CN, 0);            // PCA0 Module 0 Interrupt Flag

// ADC0CN 0xE8
SBIT (AD0EN, SFR_ADC0CN, 7);           // ADC0 Enable
SBIT (BURSTEN, SFR_ADC0CN, 6);         // ADC0 Burst Enable
SBIT (AD0INT, SFR_ADC0CN, 5);          // ADC0 Conversion Complete Interrupt Flag
SBIT (AD0BUSY, SFR_ADC0CN, 4);         // ADC0 Busy Flag
SBIT (AD0WINT, SFR_ADC0CN, 3);         // ADC0 Window Compare Interrupt Flag
SBIT (AD0LJST, SFR_ADC0CN, 2);         // ADC0 Left Justified
SBIT (AD0CM1, SFR_ADC0CN, 1);          // ADC0 Start Of Conversion Mode Bit 1
SBIT (AD0CM0, SFR_ADC0CN, 0);          // ADC0 Start Of Conversion Mode Bit 0

// SPI0CN 0xF8
SBIT (SPIF, SFR_SPI0CN, 7);            // SPI0 Interrupt Flag
SBIT (WCOL, SFR_SPI0CN, 6);            // SPI0 Write Collision Flag
SBIT (MODF, SFR_SPI0CN, 5);            // SPI0 Mode Fault Flag
SBIT (RXOVRN, SFR_SPI0CN, 4);          // SPI0 Rx Overrun Flag
SBIT (NSSMD1, SFR_SPI0CN, 3);          // SPI0 NSS Mode Bit 1
SBIT (NSSMD0, SFR_SPI0CN, 2);          // SPI0 NSS Mode Bit 0
SBIT (TXBMT, SFR_SPI0CN, 1);           // SPI0 Transmit Buffer Empty Flag
SBIT (SPIEN, SFR_SPI0CN, 0);           // SPI0 Enable

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
#define INTERRUPT_ADC0_WINDOW          7  // ADC0 Window Comparison
#define INTERRUPT_ADC0_EOC             8  // ADC0 End Of Conversion
#define INTERRUPT_PCA0                 9  // PCA0 Peripheral
#define INTERRUPT_COMPARATOR_FALLING  10  // Comparator Falling edge
#define INTERRUPT_COMPARATOR_RISING   11  // Comparator Rising edge
#define INTERRUPT_LIN                 12  // LIN interrupt
#define INTERRUPT_VREG                13  // Voltage Regulator Dropout
#define INTERRUPT_PORT_MATCH          14  // Port Match

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define C8051F520A_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------