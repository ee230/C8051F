//-----------------------------------------------------------------------------
// C8051F540_defs.h
//-----------------------------------------------------------------------------
// Copyright 2008, Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051F54x family.
// **Important Note**: The compiler_defs.h header file should be included
// before including this header file.
//
// Target:         C8051F540/1/2/3/4/5/6/7
// Tool chain:     Generic
// Command Line:   None
//
// Release 0.2 - 01 APR 2009 (GP)
//    -Initial Revision;
//
// Release 0.1 - 18 AUG 2008 (GP)
//    -Initial Revision;
//

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051F540_DEFS_H
#define C8051F540_DEFS_H

//-----------------------------------------------------------------------------
// SFRPage 0x00 and SFRPage 0x0F Registers
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
SFR (TMR3RLL, 0x92);                   // Timer/Counter 3 Reload Low
SFR (TMR3RLH, 0x93);                   // Timer/Counter 3 Reload High
SFR (TMR3L, 0x94);                     // Timer/Counter 3 Low
SFR (TMR3H, 0x95);                     // Timer/Counter 3 High
SFR (CLKMUL, 0x97);                    // Clock Multiplier
SFR (SCON0, 0x98);                     // UART0 Control
SFR (SBUF0, 0x99);                     // UART0 Data Buffer
SFR (CPT0CN, 0x9A);                    // Comparator 0 Control
SFR (CPT0MD, 0x9B);                    // Comparator 0 Mode
SFR (CPT0MX, 0x9C);                    // Comparator 0 Mux
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
SFR (SBCON0, 0xAB);                    // UART0 Baud Rate Generator Control
SFR (SBRLL0, 0xAC);                    // UART0 Baud Rate Generator Low
SFR (SBRLH0, 0xAD);                    // UART0 Baud Rate Generator High
SFR (P3MAT, 0xAE);                     // Port 3 Match
SFR (P3MDOUT, 0xAE);                   // Port 3 Mode
SFR (P3MASK, 0xAF);                    // Port 3 Mask
SFR (P3, 0xB0);                        // Port 3 Latch
SFR (P2MAT, 0xB1);                     // Port 2 Match
SFR (P2MASK, 0xB2);                    // Port 2 Mask
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
SFR (XBR2, 0xC7);                      // Port I/O Crossbar Control 2
SFR (TMR2CN, 0xC8);                    // Timer/Counter 2 Control
SFR (REG0CN, 0xC9);                    // Regulator Control
SFR (LIN0CF, 0xC9);                    // LIN 0 Configuration
SFR (TMR2RLL, 0xCA);                   // Timer/Counter 2 Reload Low
SFR (TMR2RLH, 0xCB);                   // Timer/Counter 2 Reload High
SFR (TMR2L, 0xCC);                     // Timer/Counter 2 Low
SFR (TMR2H, 0xCD);                     // Timer/Counter 2 High
SFR (PCA0CPL5, 0xCE);                  // PCA0 Capture 5 Low
SFR (PCA1CPL5, 0xCE);                  // PCA1 Capture 5 Low
SFR (PCA0CPH5, 0xCF);                  // PCA0 Capture 5 High
SFR (PCA1CPH5, 0xCF);                  // PCA1 Capture 5 High
SFR (PSW, 0xD0);                       // Program Status Word
SFR (REF0CN, 0xD1);                    // Voltage Reference Control
SFR (LIN0DAT, 0xD2);                   // LIN0 Data
SFR (LIN0ADR, 0xD3);                   // LIN0 Address
SFR (P0SKIP, 0xD4);                    // Port 0 Skip
SFR (P1SKIP, 0xD5);                    // Port 1 Skip
SFR (P2SKIP, 0xD6);                    // Port 2 Skip
SFR (P3SKIP, 0xD7);                    // Port 3 Skip
SFR (PCA0CN, 0xD8);                    // PCA0 Control
SFR (PCA0MD, 0xD9);                    // PCA0 Mode
SFR (PCA0PWM, 0xD9);                   // PCA0 PWM Control
SFR (PCA0CPM0, 0xDA);                  // PCA0 Module 0 Mode
SFR (PCA0CPM1, 0xDB);                  // PCA0 Module 1 Mode
SFR (PCA0CPM2, 0xDC);                  // PCA0 Module 2 Mode
SFR (PCA0CPM3, 0xDD);                  // PCA0 Module 3 Mode
SFR (PCA0CPM4, 0xDE);                  // PCA0 Module 4 Mode
SFR (PCA0CPM5, 0xDF);                  // PCA0 Module 5 Mode
SFR (ACC, 0xE0);                       // Accumulator
SFR (XBR0, 0xE1);                      // Port I/O Crossbar Control 0
SFR (XBR1, 0xE2);                      // Port I/O Crossbar Control 1
SFR (CCH0CN, 0xE3);                    // Cache control
SFR (IT01CF, 0xE4);                    // INT0/INT1 Configuration
SFR (EIE1, 0xE6);                      // Extended Interrupt Enable 2
SFR (EIE2, 0xE7);                      // Extended Interrupt Enable 2
SFR (ADC0CN, 0xE8);                    // ADC0 Control
SFR (PCA0CPL1, 0xE9);                  // PCA0 Capture 2 Low
SFR (PCA0CPH1, 0xEA);                  // PCA0 Capture 2 High
SFR (PCA0CPL2, 0xEB);                  // PCA0 Capture 3 Low
SFR (PCA0CPH2, 0xEC);                  // PCA0 Capture 3 High
SFR (PCA0CPL3, 0xED);                  // PCA0 Capture 4 Low
SFR (PCA0CPH3, 0xEE);                  // PCA0 Capture 4 High
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
SFR (SN0, 0xF9);                       // Serial Number 0
SFR (PCA0H, 0xFA);                     // PCA0 Counter High
SFR (SN1, 0xFA);                       // Serial Number 1
SFR (PCA0CPL0, 0xFB);                  // PCA0 Capture 0 Low
SFR (SN2, 0xFB);                       // Serial Number 2
SFR (PCA0CPH0, 0xFC);                  // PCA0 Capture 0 High
SFR (SN3, 0xFC);                       // Serial Number 3
SFR (PCA0CPL4, 0xFD);                  // PCA0 Capture 4 Low
SFR (PCA0CPH4, 0xFE);                  // PCA0 Capture 4 High
SFR (VDM0CN, 0xFF);                    // VDD Monitor Control

//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16 (DP, 0x82);                      // Data Pointer
SFR16 (TMR3RL, 0x92);                  // Timer 3 Reload
SFR16 (TMR3, 0x94);                    // Timer 3 Capture / Reload
SFR16 (SBRL0, 0xAC);                   // UART0 Reload
SFR16 (ADC0, 0xBD);                    // ADC0 data
SFR16 (ADC0GT, 0xC3);                  // ADC0 Greater Than Window
SFR16 (ADC0LT, 0xC5);                  // ADC0 Less Than Window
SFR16 (TMR2RL, 0xCA);                  // Timer 2 Reload
SFR16 (TMR2, 0xCC);                    // Timer 2 Capture / Reload
SFR16 (PCA0CP5, 0xCE);                 // PCA0 Module 5 Capture
SFR16 (PCA0CP1, 0xE9);                 // PCA0 Module 1 Capture
SFR16 (PCA0CP2, 0xEB);                 // PCA0 Module 2 Capture
SFR16 (PCA0CP3, 0xED);                 // PCA0 Module 3 Capture
SFR16 (PCA0, 0xF9);                    // PCA0 Counter
SFR16 (PCA0CP0, 0xFB);                 // PCA0 Module 0 Capture
SFR16 (PCA0CP4, 0xFD);                 // PCA0 Module 4 Capture

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
SBIT (OVR0, SFR_SCON0, 7);             // UART0 Mode 0
SBIT (PERR0, SFR_SCON0, 6);            // UART0 Parity Error Flag
SBIT (THRE0, SFR_SCON0, 5);            // UART0 Transmit Holding Reg. Empty
SBIT (REN0, SFR_SCON0, 4);             // UART0 RX Enable
SBIT (TBX0, SFR_SCON0, 3);             // UART0 TX Bit 8
SBIT (RBX0, SFR_SCON0, 2);             // UART0 RX Bit 8
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
SBIT (TR2, SFR_TMR2CN, 2);             // Timer2 Run Enable
SBIT (T2RCLK, SFR_TMR2CN, 1);          // Timer 2 Xclk/Rclk Select
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
SBIT (CCF5, SFR_PCA0CN, 5);            // PCA0 Module 5 Interrupt Flag
SBIT (CCF4, SFR_PCA0CN, 4);            // PCA0 Module 4 Interrupt Flag
SBIT (CCF3, SFR_PCA0CN, 3);            // PCA0 Module 3 Interrupt Flag
SBIT (CCF2, SFR_PCA0CN, 2);            // PCA0 Module 2 Interrupt Flag
SBIT (CCF1, SFR_PCA0CN, 1);            // PCA0 Module 1 Interrupt Flag
SBIT (CCF0, SFR_PCA0CN, 0);            // PCA0 Module 0 Interrupt Flag

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
#define INTERRUPT_PORT_MATCH      17   // Port Match

//-----------------------------------------------------------------------------
// SFR Page Definitions
//-----------------------------------------------------------------------------

#define  CONFIG_PAGE       0x0F        // System and Port Configuration Page
#define  ACTIVE_PAGE       0x00        // Active Use Page

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define C8051F540_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------