//-----------------------------------------------------------------------------
// C8051F360_defs.h
//-----------------------------------------------------------------------------
// Copyright 2007, Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051F36x family.
// **Important Note**: The compiler_defs.h header file should be included
// before including this header file.
//
// Target:         C8051F360, 'F361, 'F362, 'F363, 'F364, 'F365, 'F366,
//                 'F367, 'F368
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.2 - 07 AUG 2007 (PKC)
//    -Removed #include <compiler_defs.h>. The C source file should include it.
//    -Removed FID and fixed formatting.
// Release 1.1 - 08 DEC 2006 (BW)
//    -Added comments and SFRPAGE definitions (BW)
//    -Corrected PCA0CPM3,4,5
// Release 1.0 - 09 NOV 2006 (BW)
//    -Ported from 'F330 DEFS rev 1.2

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051F360_DEFS_H
#define C8051F360_DEFS_H

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------

SFR (P0, 0x80);                        // Port 0 Latch
SFR (SP, 0x81);                        // Stack Pointer
SFR (DPL, 0x82);                       // Data Pointer Low
SFR (DPH, 0x83);                       // Data Pointer High
SFR (CCH0CN, 0x84);                    // Cache Control
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
SFR (CLKSEL, 0x8F);                    // Clock Select
SFR (P1, 0x90);                        // Port 1 Latch
SFR (TMR3CN, 0x91);                    // Timer/Counter 3 Control
SFR (TMR3RLL, 0x92);                   // Timer/Counter 3 Reload Low
SFR (TMR3RLH, 0x93);                   // Timer/Counter 3 Reload High
SFR (TMR3L, 0x94);                     // Timer/Counter 3 Low
SFR (TMR3H, 0x95);                     // Timer/Counter 3 High
SFR (IDA0L, 0x96);                     // Current Mode Dac0 Low
SFR (IDA0H, 0x97);                     // Current Mode Dac0 High
SFR (SCON0, 0x98);                     // UART0 Control
SFR (SBUF0, 0x99);                     // UART0 Data Buffer
SFR (CPT1CN, 0x9A);                    // Comparator1 Control
SFR (CPT0CN, 0x9B);                    // Comparator0 Control
SFR (CPT1MD, 0x9C);                    // Comparator1 Mode Selection
SFR (CPT0MD, 0x9D);                    // Comparator0 Mode Selection
SFR (CPT1MX, 0x9E);                    // Comparator1 Mux Selection
SFR (CPT0MX, 0x9F);                    // Comparator0 Mux Selection
SFR (P2, 0xA0);                        // Port 2 Latch
SFR (SPI0CFG, 0xA1);                   // SPI0 Configuration
SFR (SPI0CKR, 0xA2);                   // SPI0 Clock Rate Control
SFR (SPI0DAT, 0xA3);                   // SPI0 Data
SFR (MAC0AL, 0xA4);                    // MAC0 Accumulator Low
SFR (P0MDOUT, 0xA4);                   // Port 0 Output Mode Configuration
SFR (MAC0AH, 0xA5);                    // MAC0 Accumulator High
SFR (P1MDOUT, 0xA5);                   // Port 1 Output Mode Configuration
SFR (P2MDOUT, 0xA6);                   // Port 2 Output Mode Configuration
SFR (SFRPAGE, 0xA7);                   // SFR page select
SFR (IE, 0xA8);                        // Interrupt Enable
SFR (PLL0DIV, 0xA9);                   // PLL0 Divisor
SFR (EMI0CN, 0xAA);                    // External Memory Interface Control
SFR (FLSTAT, 0xAC);                    // Flash Status
SFR (OSCLCN, 0xAD);                    // Low-frequency oscillator Control
SFR (MAC0RNDL, 0xAE);                  // MAC0 Round Low
SFR (P4MDOUT, 0xAE);                   // P4 Output Mode Configuration
SFR (MAC0RNDH, 0xAF);                  // MAC0 Round High
SFR (P3MDOUT, 0xAF);                   // P3 Output Mode Configuration
SFR (P3, 0xB0);                        // Port 3 Latch
SFR (P2MAT, 0xB1);                     // Port 2 Match
SFR (PLL0MUL, 0xB1);                   // PLL0 Multiply
SFR (P2MASK, 0xB2);                    // Port 2 Mask
SFR (PLL0FLT, 0xB2);                   // PLL0 Filter Configuration
SFR (PLL0CN, 0xB3);                    // PLL0 Control
SFR (P4, 0xB5);                        // Port 4 Latch
SFR (FLSCL, 0xB6);                     // Flash Scale
SFR (OSCXCN, 0xB6);                    // External Oscillator Control
SFR (FLKEY, 0xB7);                     // Flash Lock And Key
SFR (OSCICN, 0xB7);                    // Internal Oscillator Control
SFR (IP, 0xB8);                        // Interrupt Priority
SFR (IDA0CN, 0xB9);                    // Current Mode DAC0 Control
SFR (AMX0N, 0xBA);                     // AMUX0 Negative Channel Select
SFR (AMX0P, 0xBB);                     // AMUX0 Positive Channel Select
SFR (ADC0CF, 0xBC);                    // ADC0 Configuration
SFR (ADC0L, 0xBD);                     // ADC0 Low
SFR (ADC0H, 0xBE);                     // ADC0 High
SFR (OSCICL, 0xBF);                    // Internal Oscillator Calibration
SFR (SMB0CN, 0xC0);                    // SMBus0 Control
SFR (SMB0CF, 0xC1);                    // SMBus0 Configuration
SFR (SMB0DAT, 0xC2);                   // SMBus0 Data
SFR (ADC0GTL, 0xC3);                   // ADC0 Greater-Than Compare Low
SFR (ADC0GTH, 0xC4);                   // ADC0 Greater-Than Compare High
SFR (ADC0LTL, 0xC5);                   // ADC0 Less-Than Compare Word Low
SFR (ADC0LTH, 0xC6);                   // ADC0 Less-Than Compare Word High
SFR (EMI0CF, 0xC7);                    // EMIF Configuration
SFR (ONESHOT, 0xC7);                   // Flash oneshot timing
SFR (TMR2CN, 0xC8);                    // Timer/Counter 2 Control
SFR (CCH0TN, 0xC9);                    // Cache Timing
SFR (TMR2RLL, 0xCA);                   // Timer/Counter 2 Reload Low
SFR (TMR2RLH, 0xCB);                   // Timer/Counter 2 Reload High
SFR (TMR2L, 0xCC);                     // Timer/Counter 2 Low
SFR (TMR2H, 0xCD);                     // Timer/Counter 2 High
SFR (EIP1, 0xCE);                      // Extended Interrupt Priority 1
SFR (MAC0STA, 0xCF);                   // MAC0 Status
SFR (EIP2, 0xCF);                      // Extended Interrupt Priority 2
SFR (PSW, 0xD0);                       // Program Status Word
SFR (REF0CN, 0xD1);                    // Voltage Reference Control
SFR (MAC0ACC0, 0xD2);                  // MAC0 Accumulator 0
SFR (CCH0LC, 0xD2);                    // Cache Lock Control
SFR (MAC0ACC1, 0xD3);                  // MAC0 Accumulator 1
SFR (CCH0MA, 0xD3);                    // Cache Miss Accumulator
SFR (MAC0ACC2, 0xD4);                  // MAC0 Accumulator 2
SFR (MAC0ACC3, 0xD5);                  // MAC0 Accumulator 3
SFR (P0SKIP, 0xD4);                    // Port 0 Skip
SFR (P1SKIP, 0xD5);                    // Port 1 Skip
SFR (MAC0OVR, 0xD6);                   // MAC0 Overflow
SFR (P2SKIP, 0xD6);                    // Port 2 Skip
SFR (MAC0CF, 0xD7);                    // MAC0 Configuration
SFR (P3SKIP, 0xD7);                    // Port 3 Skip
SFR (PCA0CN, 0xD8);                    // PCA0 Control
SFR (PCA0MD, 0xD9);                    // PCA0 Mode
SFR (PCA0CPM0, 0xDA);                  // PCA0 Module 0 Mode Register
SFR (PCA0CPM1, 0xDB);                  // PCA0 Module 1 Mode Register
SFR (PCA0CPM2, 0xDC);                  // PCA0 Module 2 Mode Register
SFR (PCA0CPM3, 0xDD);                  // PCA0 Module 3 Mode Register
SFR (PCA0CPM4, 0xDE);                  // PCA0 Module 4 Mode Register
SFR (PCA0CPM5, 0xDF);                  // PCA0 Module 5 Mode Register
SFR (ACC, 0xE0);                       // Accumulator
SFR (P1MAT, 0xE1);                     // Port 1 Match
SFR (XBR0, 0xE1);                      // Port I/O Crossbar Control 0
SFR (P1MASK, 0xE2);                    // Port 1 Mask
SFR (XBR1, 0xE2);                      // Port I/O Crossbar Control 1
SFR (IT01CF, 0xE4);                    // INT0/INT1 Configuration
SFR (SFR0CN, 0xE5);                    // SFR page control
SFR (EIE1, 0xE6);                      // Extended Interrupt Enable 1
SFR (EIE2, 0xE7);                      // Extended Interrupt Enable 2
SFR (ADC0CN, 0xE8);                    // ADC0 Control
SFR (PCA0CPL1, 0xE9);                  // PCA0 Capture 1 Low
SFR (PCA0CPH1, 0xEA);                  // PCA0 Capture 1 High
SFR (PCA0CPL2, 0xEB);                  // PCA0 Capture 2 Low
SFR (PCA0CPH2, 0xEC);                  // PCA0 Capture 2 High
SFR (PCA0CPL3, 0xED);                  // PCA0 Capture 3 Low
SFR (PCA0CPH3, 0xEE);                  // PCA0 Capture 3 High
SFR (RSTSRC, 0xEF);                    // Reset Source Configuration/Status
SFR (B, 0xF0);                         // B Register
SFR (MAC0BL, 0xF1);                    // MAC0 B Low
SFR (P0MDIN, 0xF1);                    // Port 0 Input Mode Configuration
SFR (MAC0BH, 0xF2);                    // MAC0 B High
SFR (P1MDIN, 0xF2);                    // Port 1 Input Mode Configuration
SFR (P0MAT, 0xF3);                     // Port 0 Match
SFR (P2MDIN, 0xF3);                    // Port 2 Input Mode Configuration
SFR (P0MASK, 0xF4);                    // Port 0 Mask
SFR (P3MDIN, 0xF4);                    // Port 3 Input Mode Configuration
SFR (PCA0CPL5, 0xF5);                  // PCA0 Capture 5 Low
SFR (PCA0CPH5, 0xF6);                  // PCA0 Capture 5 High
SFR (EMI0TC, 0xF7);                    // EMIF Timing
SFR (SPI0CN, 0xF8);                    // SPI0 Control
SFR (PCA0L, 0xF9);                     // PCA0 Counter Low
SFR (PCA0H, 0xFA);                     // PCA0 Counter High
SFR (PCA0CPL0, 0xFB);                  // PCA0 Capture 0 Low
SFR (PCA0CPH0, 0xFC);                  // PCA0 Capture 0 High
SFR (PCA0CPL4, 0xFD);                  // PCA0 Capture 4 Low
SFR (PCA0CPH4, 0xFE);                  // PCA0 Capture 4 High
SFR (VDM0CN, 0xFF);                    // VDD Monitor Control

//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16 (DP, 0x82);                      // Data Pointer
SFR16 (TMR3RL, 0x92);                  // Timer 3 Reload
SFR16 (TMR3, 0x94);                    // Timer 3 Counter
SFR16 (IDA0, 0x96);                    // IDAC0 Data
SFR16 (MAC0A, 0xA4);                   // MAC0 A Operand
SFR16 (MAC0RND, 0xAE);                 // MAC0 Round
SFR16 (ADC0, 0xBD);                    // ADC0 Data
SFR16 (ADC0GT, 0xC3);                  // ADC0 Greater-Than Compare
SFR16 (ADC0LT, 0xC5);                  // ADC0 Less-Than Compare
SFR16 (TMR2RL, 0xCA);                  // Timer 2 Reload
SFR16 (TMR2, 0xCC);                    // Timer 2 Counter
SFR16 (MAC0ACCL, 0xD2);                // MAC0 Accumulator Low
SFR16 (MAC0ACCH, 0xD4);                // MAC0 Accumulator High
SFR16 (PCA0CP1, 0xE9);                 // PCA0 Module 1 Capture/Compare
SFR16 (PCA0CP2, 0xEB);                 // PCA0 Module 2 Capture/Compare
SFR16 (PCA0CP3, 0xED);                 // PCA0 Module 3 Capture/Compare
SFR16 (MAC0B, 0xF1);                   // MAC0 B Operand
SFR16 (PCA0CP5, 0xF5);                 // PCA0 Module 5 Capture/Compare
SFR16 (PCA0, 0xF9);                    // PCA0 Counter
SFR16 (PCA0CP0, 0xFB);                 // PCA0 Module 0 Capture/Compare
SFR16 (PCA0CP4, 0xFD);                 // PCA0 Module 4 Capture/Compare

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
                                       // Bit6 UNUSED
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
SBIT (CCF5, SFR_PCA0CN, 5);            // PCA0 Module 5 Interrupt Flag
SBIT (CCF4, SFR_PCA0CN, 4);            // PCA0 Module 4 Interrupt Flag
SBIT (CCF3, SFR_PCA0CN, 3);            // PCA0 Module 3 Interrupt Flag
SBIT (CCF2, SFR_PCA0CN, 2);            // PCA0 Module 2 Interrupt Flag
SBIT (CCF1, SFR_PCA0CN, 1);            // PCA0 Module 1 Interrupt Flag
SBIT (CCF0, SFR_PCA0CN, 0);            // PCA0 Module 0 Interrupt Flag

// ADC0CN 0xE8
SBIT (AD0EN, SFR_ADC0CN, 7);           // ADC0 Enable
SBIT (AD0TM, SFR_ADC0CN, 6);           // ADC0 Track Mode
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
SBIT (SPIEN, SFR_SPI0CN, 0);           // SPI0 SPI0 Enable

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
                                       // Interrupt 8 is RESERVED
#define INTERRUPT_ADC0_WINDOW      9   // ADC0 Window Comparison
#define INTERRUPT_ADC0_EOC         10  // ADC0 End Of Conversion
#define INTERRUPT_PCA0             11  // PCA0 Peripheral
#define INTERRUPT_COMPARATOR0      12  // Comparator0
#define INTERRUPT_COMPARATOR1      13  // Comparator1
#define INTERRUPT_TIMER3           14  // Timer3 Overflow
                                       // Interrupt 15 is RESERVED
#define INTERRUPT_PORT_MATCH       16  // Port Match

//-----------------------------------------------------------------------------
// SFR Page Definitions
//-----------------------------------------------------------------------------

#define CONFIG_PAGE       0x0F         // SYSTEM AND PORT CONFIGURATION PAGE
#define LEGACY_PAGE       0x00         // LEGACY SFR PAGE
#define TIMER01_PAGE      0x00         // TIMER 0 AND TIMER 1
#define CPT0_PAGE         0x00         // COMPARATOR 0
#define CPT1_PAGE         0x00         // COMPARATOR 1
#define UART0_PAGE        0x00         // UART 0
#define SPI0_PAGE         0x00         // SPI 0
#define EMI0_PAGE         0x0F         // EXTERNAL MEMORY INTERFACE
#define ADC0_PAGE         0x00         // ADC 0
#define SMB0_PAGE         0x00         // SMBUS 0
#define TMR2_PAGE         0x00         // TIMER 2
#define TMR3_PAGE         0x00         // TIMER 3
#define DAC0_PAGE         0x00         // DAC 0
#define PCA0_PAGE         0x00         // PCA 0
#define PLL0_PAGE         0x0F         // PLL 0
#define MAC0_PAGE         0x00         // MAC 0
#define MATCH_PAGE        0x00         // PORT0, PORT1, PORT2 MATCH

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define C8051F360_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------