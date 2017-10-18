//-----------------------------------------------------------------------------
// C8051F700_defs.h
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051F70x family.
//
//
// Target:         C8051F700/01/02/03/04/05/06/07/08/09/10/11/12/13/14/15/16/17
// Tool chain:     Keil, SDCC
// Command Line:   None
//
// Release 1.4 - PD
//    -Removed SFRNEXT and SFRLAST, added list of all target MCUs in family
// Release 1.3 - PD
//    -Added P6MDIN
// Release 1.2 - TP
//    -Added CS0PM, CS0MD1, CS0MD2, and CS0MD3
// Release 1.1 - BD
//    -Corrected CRC0AUTO0 and CRC0AUTO1 Register Names to CRC0AUTO and CRC0CNT
//    -Shortened HARDWAREID and REVISIONID Register Names to HWID and REVID
//    -11 FEB 2009
// Release 1.0
//    -Ported from 'F93x DEFS rev 1.0 by FB
//    -2 JUL 2008
//

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051F700_DEFS_H
#define C8051F700_DEFS_H

#include <compiler_defs.h>

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------

SFR (P0, 0x80);                        // Port 0 Latch
SFR (SP, 0x81);                        // Stack Pointer
SFR (DPL, 0x82);                       // Data Pointer Low
SFR (DPH, 0x83);                       // Data Pointer High
SFR (PCON, 0x87);                      // Power Control
SFR (TCON, 0x88);                      // Timer/Counter Control
SFR (TMOD, 0x89);                      // Timer/Counter Mode
SFR (TL0, 0x8A);                       // Timer/Counter 0 Low
SFR (TL1, 0x8B);                       // Timer/Counter 1 Low
SFR (TH0, 0x8C);                       // Timer/Counter 0 High
SFR (TH1, 0x8D);                       // Timer/Counter 1 High
SFR (CKCON, 0x8E);                     // Clock Control
SFR (PSCTL, 0x8F);                     // Program Store R/W Control
SFR (P1, 0x90);                        // Port 1 Latch
SFR (TMR3CN, 0x91);                    // Timer/Counter 3 Control
SFR (CRC0CN, 0x91);                    // CRC Control
SFR (TMR3RLL, 0x92);                   // Timer/Counter 3 Reload Low
SFR (CS0SS, 0x92);                     // CS0 Auto-Scan Start Channel
SFR (TMR3RLH, 0x93);                   // Timer/Counter 3 Reload High
SFR (CS0SE, 0x93);                     // CS0 Auto-Scan End Channel
SFR (TMR3L, 0x94);                     // Timer/Counter 3 Low
SFR (CRC0IN, 0x94);                    // CRC Data Input
SFR (TMR3H, 0x95);                     // Timer/Counter 3 High
SFR (CRC0FLIP, 0x95);                  // CRC Flip
SFR (CS0THL, 0x96);                    // CS0 Comparator Threshold Low Byte
SFR (CRC0AUTO, 0x96);                  // CRC Automatic Control
SFR (CS0THH, 0x97);                    // CS0 Comparator Threshold High Byte
SFR (CRC0CNT, 0x97);                   // CRC Automatic Flash Sector Count
SFR (SCON0, 0x98);                     // UART0 Control
SFR (SBUF0, 0x99);                     // UART0 Data Buffer
SFR (CS0CN, 0x9A);                     // CS0 Control
SFR (P4MDOUT, 0x9A);                   // Port 4 Output Mode
SFR (CPT0CN, 0x9B);                    // Comparator0 Control
SFR (P5MDOUT, 0x9B);                   // Port 5 Output Mode
SFR (CS0MX, 0x9C);                     // CS0 Mux Channel Select
SFR (P6MDOUT, 0x9C);                   // Port 6 Output Mode
SFR (CPT0MD, 0x9D);                    // Comparator0 Mode Selection
SFR (CS0CF, 0x9E);                     // CS0 Configuration
SFR (CPT0MX, 0x9F);                    // Comparator0 Mux Selection
SFR (CS0PM, 0x9F);                     // CS0 Port Monitor
SFR (P2, 0xA0);                        // Port 2 Latch
SFR (SPI0CFG, 0xA1);                   // SPI0 Configuration
SFR (PCA0PWM, 0xA1);                   // PCA PWM Configuration
SFR (SPI0CKR, 0xA2);                   // SPI0 Clock Rate Control
SFR (SPI0DAT, 0xA3);                   // SPI0 Data
SFR (P0MDOUT, 0xA4);                   // Port 0 Output Mode Configuration
SFR (P1MDOUT, 0xA5);                   // Port 1 Output Mode Configuration
SFR (P2MDOUT, 0xA6);                   // Port 2 Output Mode Configuration
SFR (SFRPAGE, 0xA7);                   // SFR Page
SFR (IE, 0xA8);                        // Interrupt Enable
SFR (CS0DL, 0xA9);                     // CS0 Data Low Byte
SFR (OSCICN, 0xA9);                    // Internal H-F Oscillator Control
SFR (CS0DH, 0xAA);                     // CS0 Data High Byte
SFR (EMI0CN, 0xAA);                    // EMIF Control
SFR (P4, 0xAC);                        // Port 4
SFR (REVID, 0xAD);                     // Hardware Revision Identification Byte
SFR (CS0MD1, 0xAD);                    // CS0 Mode 1
SFR (CS0MD3, 0xAE);                    // CS0 Mode 3
SFR (P3MDOUT, 0xAF);                   // Port 3 Output Mode Configuration
SFR (P3, 0xB0);                        // Port 3
SFR (P6, 0xB2);                        // Port 6
SFR (P5, 0xB3);                        // Port 5
SFR (OSCXCN, 0xB5);                    // External Oscillator Control
SFR (EEADDR, 0xB6);                    // EEPROM Byte Address
SFR (FLKEY, 0xB7);                     // Flash Lock And Key
SFR (IP, 0xB8);                        // Interrupt Priority
SFR (REG0CN, 0xB9);                    // Voltage Regulator Control
SFR (SMB0ADR, 0xBA);                   // SMBus Slave Address
SFR (ADC0MX, 0xBB);                    // AMUX0 Positive Channel Select
SFR (SMB0ADM, 0xBB);                   // SMBus Slave Address Mask
SFR (ADC0CF, 0xBC);                    // ADC0 Configuration
SFR (ADC0L, 0xBD);                     // ADC0 Low
SFR (CLKSEL, 0xBD);                    // Clock Select
SFR (ADC0H, 0xBE);                     // ADC0 High
SFR (CS0MD2, 0xBE);                    // CS0 Mode 2
SFR (OSCICL, 0xBF);                    // Internal Oscillator Calibration
SFR (SMB0CN, 0xC0);                    // SMBus0 Control
SFR (SMB0CF, 0xC1);                    // SMBus0 Configuration
SFR (P6DRV, 0xC1);                     // Port 6 Drive Strength
SFR (SMB0DAT, 0xC2);                   // SMBus0 Data
SFR (ADC0GTL, 0xC3);                   // ADC0 Greater-Than Compare Low
SFR (ADC0GTH, 0xC4);                   // ADC0 Greater-Than Compare High
SFR (HWID, 0xC4);                      // Hardware Identification
SFR (ADC0LTL, 0xC5);                   // ADC0 Less-Than Compare Word Low
SFR (EECNTL, 0xC5);                    // EEPROM Control
SFR (ADC0LTH, 0xC6);                   // ADC0 Less-Than Compare Word High
SFR (EEKEY, 0xC6);                     // EEPROM Protect Key
SFR (EMI0CF, 0xC7);                    // EMIF Configuration
SFR (TMR2CN, 0xC8);                    // Timer/Counter 2 Control
SFR (TMR2RLL, 0xCA);                   // Timer/Counter 2 Reload Low
SFR (TMR2RLH, 0xCB);                   // Timer/Counter 2 Reload High
SFR (TMR2L, 0xCC);                     // Timer/Counter 2 Low
SFR (TMR2H, 0xCD);                     // Timer/Counter 2 High
SFR (EIP1, 0xCE);                      // Extended Interrupt Priority 1
SFR (EIP2, 0xCF);                      // Extended Interrupt Priority 2
SFR (PSW, 0xD0);                       // Program Status Word
SFR (EEDATA, 0xD1);                    // EEPROM Byte Data
SFR (REF0CN, 0xD2);                    // Voltage Reference Control
SFR (P0SKIP, 0xD4);                    // Port 0 Skip
SFR (P1SKIP, 0xD5);                    // Port 1 Skip
SFR (P2SKIP, 0xD6);                    // Port 2 Skip
SFR (PCA0CN, 0xD8);                    // PCA0 Control
SFR (CRC0DATA, 0xD9);                   // CRC Data Output
SFR (PCA0CPM0, 0xDA);                  // PCA0 Module 0 Mode Register
SFR (PCA0CPM1, 0xDB);                  // PCA0 Module 1 Mode Register
SFR (PCA0CPM2, 0xDC);                  // PCA0 Module 2 Mode Register
SFR (ACC, 0xE0);                       // Accumulator
SFR (P1MAT, 0xE1);                     // Port 1 Match
SFR (XBR0, 0xE1);                      // Port I/O Crossbar Control 0
SFR (XBR1, 0xE2);                      // Port I/O Crossbar Control 1
SFR (P1MASK, 0xE2);                    // Port 1 Mask Register
SFR (WDTCN, 0xE3);                     // Watchdog Timer Control
SFR (IT01CF, 0xE4);                    // INT0/INT1 Configuration
SFR (SFR0CN, 0xE5);                    // SFR Page Control
SFR (EIE1, 0xE6);                      // Extended Interrupt Enable 1
SFR (EIE2, 0xE7);                      // Extended Interrupt Enable 2
SFR (ADC0CN, 0xE8);                    // ADC0 Control
SFR (PCA0CPL1, 0xE9);                  // PCA0 Capture 1 Low
SFR (PCA0CPH1, 0xEA);                  // PCA0 Capture 1 High
SFR (PCA0CPL2, 0xEB);                  // PCA0 Capture 2 Low
SFR (PCA0CPH2, 0xEC);                  // PCA0 Capture 2 High
SFR (DERIVID, 0xEC);                   // Derivitive Identification
SFR (PCA0MD, 0xED);                    // PCA0 Mode
SFR (EMI0TC, 0xEE);                    // EMIF Timing Control
SFR (RSTSRC, 0xEF);                    // Reset Source Configuration/Status
SFR (B, 0xF0);                         // B Register
SFR (P0MDIN, 0xF1);                    // Port 0 Input Mode Configuration
SFR (P1MDIN, 0xF2);                    // Port 1 Input Mode Configuration
SFR (P0MAT, 0xF3);                     // Port 0 Match
SFR (P2MDIN, 0xF3);                    // Port 2 Input Mode Configuration
SFR (P0MASK, 0xF4);                    // Port 0 Mask
SFR (P3MDIN, 0xF4);                    // Port 3 Input Mode Configuration
SFR (P4MDIN, 0xF5);                    // Port 4 Input Mode Configuration
SFR (P5MDIN, 0xF6);                    // Port 5 Input Mode Configuration
SFR (P6MDIN, 0xF7);                    // Port 6 Input Mode Configuration
SFR (SPI0CN, 0xF8);                    // SPI0 Control
SFR (PCA0L, 0xF9);                     // PCA0 Counter Low
SFR (P0DRV, 0xF9);                     // Port 0 Drive Strength
SFR (PCA0H, 0xFA);                     // PCA0 Counter High
SFR (P1DRV, 0xFA);                     // Port 1 Drive Strength
SFR (PCA0CPL0, 0xFB);                  // PCA0 Capture 0 Low
SFR (P2DRV, 0xFB);                     // Port 2 Drive Strength
SFR (PCA0CPH0, 0xFC);                  // PCA0 Capture 0 High
SFR (P3DRV, 0xFC);                     // Port 3 Drive Strength
SFR (P4DRV, 0xFD);                     // Port 4 Drive Strength
SFR (P5DRV, 0xFE);                     // Port 5 Drive Strength
SFR (VDM0CN, 0xFF);                    // VDD Monitor Control



//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16 (DP, 0x82);                      // Data Pointer
SFR16 (TMR3RL, 0x92);                  // Timer 3 Reload
SFR16 (TMR3, 0x94);                    // Timer 3 Counter
SFR16 (CS0TH, 0x96);                   // CS0 Comparator Threshold
SFR16 (CS0D, 0xA9);                    // CS0 Data
SFR16 (ADC0, 0xBD);                    // ADC0 Data
SFR16 (ADC0GT, 0xC3);                  // ADC0 Greater-Than Compare
SFR16 (ADC0LT, 0xC5);                  // ADC0 Less-Than Compare
SFR16 (TMR2RL, 0xCA);                  // Timer 2 Reload
SFR16 (TMR2, 0xCC);                    // Timer 2 Counter
SFR16 (PCA0CP1, 0xE9);                 // PCA0 Module 1 Capture/Compare
SFR16 (PCA0CP2, 0xEB);                 // PCA0 Module 2 Capture/Compare
SFR16 (PCA0, 0xF9);                    // PCA0 Counter
SFR16 (PCA0CP0, 0xFB);                 // PCA0 Module 0 Capture/Compare

//-----------------------------------------------------------------------------
// Address Definitions for Bit-addressable Registers
//-----------------------------------------------------------------------------

#define SFR_P0       0x80
#define SFR_TCON     0x88
#define SFR_P1       0x90
#define SFR_SCON0    0x98
#define SFR_P2       0xA0
#define SFR_IE       0xA8
#define SFR_P3       0xB0
#define SFR_IP       0xB8
#define SFR_SMB0CN   0xC0
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
SBIT (S0MODE, SFR_SCON0, 7);           // UART0 Mode
                                       // Bit6 UNUSED
SBIT (MCE0, SFR_SCON0, 5);             // UART0 MCE
SBIT (REN0, SFR_SCON0, 4);             // UART0 RX Enable
SBIT (TB80, SFR_SCON0, 3);             // UART0 TX Bit 8
SBIT (RB80, SFR_SCON0, 2);             // UART0 RX Bit 8
SBIT (TI0, SFR_SCON0, 1);              // UART0 TX Interrupt Flag
SBIT (RI0, SFR_SCON0, 0);              // UART0 RX Interrupt Flag

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
                                       // Bit7 UNUSED
SBIT (PSPI0, SFR_IP, 6);               // SPI0 Priority
SBIT (PT2, SFR_IP, 5);                 // Timer 2 Priority
SBIT (PS0, SFR_IP, 4);                 // UART0 Priority
SBIT (PT1, SFR_IP, 3);                 // Timer 1 Priority
SBIT (PX1, SFR_IP, 2);                 // External Interrupt 1 Priority
SBIT (PT0, SFR_IP, 1);                 // Timer 0 Priority
SBIT (PX0, SFR_IP, 0);                 // External Interrupt 0 Priority

// SMB0CN 0xC0
SBIT (MASTER, SFR_SMB0CN, 7);          // SMBus0 Master/Slave
SBIT (TXMODE, SFR_SMB0CN, 6);          // SMBus0 Transmit Mode
SBIT (STA, SFR_SMB0CN, 5);             // SMBus0 Start Flag
SBIT (STO, SFR_SMB0CN, 4);             // SMBus0 Stop Flag
SBIT (ACKRQ, SFR_SMB0CN, 3);           // SMBus0 Acknowledge Request
SBIT (ARBLOST, SFR_SMB0CN, 2);         // SMBus0 Arbitration Lost
SBIT (ACK, SFR_SMB0CN, 1);             // SMBus0 Acknowledge Flag
SBIT (SI, SFR_SMB0CN, 0);              // SMBus0 Interrupt Pending Flag

// TMR2CN 0xC8
SBIT (TF2H, SFR_TMR2CN, 7);            // Timer 2 High Byte Overflow Flag
SBIT (TF2L, SFR_TMR2CN, 6);            // Timer 2 Low Byte Overflow Flag
SBIT (TF2LEN, SFR_TMR2CN, 5);          // Timer 2 Low Byte Interrupt Enable
SBIT (TF2CEN, SFR_TMR2CN, 4);          // Timer 2 Lfo Capture Enable
SBIT (T2SPLIT, SFR_TMR2CN, 3);         // Timer 2 Split Mode Enable
SBIT (TR2, SFR_TMR2CN, 2);             // Timer 2 On/Off Control
SBIT (T2XCLK, SFR_TMR2CN, 0);          // Timer 2 External Clock Select

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
SBIT (CCF2, SFR_PCA0CN, 2);            // PCA0 Module 2 Interrupt Flag
SBIT (CCF1, SFR_PCA0CN, 1);            // PCA0 Module 1 Interrupt Flag
SBIT (CCF0, SFR_PCA0CN, 0);            // PCA0 Module 0 Interrupt Flag

// ADC0CN 0xE8
SBIT (AD0EN, SFR_ADC0CN, 7);           // ADC0 Enable
SBIT (AD0TM, SFR_ADC0CN, 6);           // ADC0 Track Mode Bit
SBIT (AD0INT, SFR_ADC0CN, 5);          // ADC0 EOC Interrupt Flag
SBIT (AD0BUSY, SFR_ADC0CN, 4);         // ADC0 Busy Flag
SBIT (AD0WINT, SFR_ADC0CN, 3);         // ADC0 Window Interrupt Flag
SBIT (AD0CM2, SFR_ADC0CN, 2);          // ADC0 Convert Start Mode Bit 2
SBIT (AD0CM1, SFR_ADC0CN, 1);          // ADC0 Convert Start Mode Bit 1
SBIT (AD0CM0, SFR_ADC0CN, 0);          // ADC0 Convert Start Mode Bit 0

// SPI0CN 0xF8
SBIT (SPIF, SFR_SPI0CN, 7);            // SPI0 Interrupt Flag
SBIT (WCOL, SFR_SPI0CN, 6);            // SPI0 Write Collision Flag
SBIT (MODF, SFR_SPI0CN, 5);            // SPI0 Mode Fault Flag
SBIT (RXOVRN, SFR_SPI0CN, 4);          // SPI0 RX Overrun Flag
SBIT (NSSMD1, SFR_SPI0CN, 3);          // SPI0 Slave Select Mode 1
SBIT (NSSMD0, SFR_SPI0CN, 2);          // SPI0 Slave Select Mode 0
SBIT (TXBMT, SFR_SPI0CN, 1);           // SPI0 TX Buffer Empty Flag
SBIT (SPI0EN, SFR_SPI0CN, 0);          // SPI0 Enable

//-----------------------------------------------------------------------------
// Interrupt Priorities
//-----------------------------------------------------------------------------

#define INTERRUPT_INT0             0   // External Interrupt 0
#define INTERRUPT_TIMER0           1   // Timer0 Overflow
#define INTERRUPT_INT1             2   // External Interrupt 1
#define INTERRUPT_TIMER1           3   // Timer1 Overflow
#define INTERRUPT_UART0            4   // Serial Port 0
#define INTERRUPT_TIMER2           5   // Timer2 Overflow
#define INTERRUPT_SPI0             6   // Serial Peripheral Interface 0
#define INTERRUPT_SMBUS0           7   // SMBus0 Interface
#define INTERRUPT_PORT_MATCH       8   // Port Match
#define INTERRUPT_ADC0_WINDOW      9   // ADC0 Window Comparison
#define INTERRUPT_ADC0_EOC         10  // ADC0 End Of Conversion
#define INTERRUPT_PCA0             11  // PCA0 Peripheral
#define INTERRUPT_COMPARATOR0      12  // Comparator0
#define INTERRUPT_TIMER3           14  // Timer3 Overflow
#define INTERRUPT_CS0_EOC          15  // CS0 End of Conversion
#define INTERRUPT_CS0_GRT          16  // CS0 Comparator Greater Than

//-----------------------------------------------------------------------------
// SFR Page Definitions
//-----------------------------------------------------------------------------
#define CONFIG_PAGE       0x0F         // SYSTEM AND PORT CONFIGURATION PAGE
#define LEGACY_PAGE       0x00         // LEGACY SFR PAGE

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define C8051F700_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------