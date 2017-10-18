//-----------------------------------------------------------------------------
// C8051T622_defs.h
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051T622/3 and 'T326/7 devices.
// **Important Note**: The compiler_defs.h header file should be included
// before including this header file.
//
//
// Target:         C8051T622, 'T623, 'T326, 'T327
// Tool chain:     Keil
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -28 AUG 2008
//

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051T622_DEFS_H
#define C8051T622_DEFS_H

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------

SFR (P0, 0x80);                        // Port 0 Latch
SFR (SP, 0x81);                        // Stack Pointer
SFR (DPL, 0x82);                       // Data Pointer Low
SFR (DPH, 0x83);                       // Data Pointer High
SFR (P0MAT, 0x84);                     // Port 0 Match
SFR (EMI0CF, 0x85);                    // EMIF Configuration
SFR (OSCLCN, 0x86);                    // Internal Low-Freq Oscillator Control
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
SFR (TMR3CN, 0x91);                    // Timer/Counter 3Control
SFR (TMR3RLL, 0x92);                   // Timer/Counter 3 Reload Low
SFR (TMR3RLH, 0x93);                   // Timer/Counter 3 Reload High
SFR (TMR3L, 0x94);                     // Timer/Counter 3Low
SFR (TMR3H, 0x95);                     // Timer/Counter 3 High
SFR (USB0ADR, 0x96);                   // USB0 Indirect Address Register
SFR (USB0DAT, 0x97);                   // USB0 Data Register
SFR (SCON0, 0x98);                     // UART0 Control
SFR (SBUF0, 0x99);                     // UART0 Data Buffer
SFR (P2, 0xA0);                        // Port 2 Latch
SFR (SPI0CFG, 0xA1);                   // SPI Configuration
SFR (SPI0CKR, 0xA2);                   // SPI Clock Rate Control
SFR (SPI0DAT, 0xA3);                   // SPI Data
SFR (P0MDOUT, 0xA4);                   // Port 0 Output Mode Configuration
SFR (P1MDOUT, 0xA5);                   // Port 1 Output Mode Configuration
SFR (P2MDOUT, 0xA6);                   // Port 2 Output Mode Configuration
SFR (IE, 0xA8);                        // Interrupt Enable
SFR (CLKSEL, 0xA9);                    // Clock Select
SFR (EMI0CN, 0xAA);                    // External Memory Interface Control
SFR (SBCON1, 0xAC);                    // UART1 Baud Rate Generator Control
SFR (P0MASK, 0xAE);                    // Port 0 Mask
SFR (PFE0CN, 0xAF);                    // Prefetch Engine Control
SFR (OSCXCN, 0xB1);                    // External Oscillator Control
SFR (OSCICN, 0xB2);                    // Internal Oscillator Control
SFR (OSCICL, 0xB3);                    // Internal Oscillator Calibration
SFR (SBRLL1, 0xB4);                    // UART1 Baud Rate Generator Low
SFR (SBRLH1, 0xB5);                    // UART1 Baud Rate Generator High
SFR (P1MAT, 0xB6);                     // Port 1 Match
SFR (MEMKEY, 0xB7);                    // EPROM Memory Lock and Key
SFR (IP, 0xB8);                        // Interrupt Priority
SFR (CLKMUL, 0xB9);                    // Clock Multiplier
SFR (P1MASK, 0xBA);                    // Port 1 Mask
SFR (SMB0CN, 0xC0);                    // SMBus Control
SFR (SMB0CF, 0xC1);                    // SMBus Configuration
SFR (SMB0DAT, 0xC2);                   // SMBus Data
SFR (SMB0ADR, 0xC7);                   // SMBus0 Address
SFR (TMR2CN, 0xC8);                    // Timer/Counter 2 Control
SFR (REG01CN, 0xC9);                   // Voltage Regulator Control
SFR (TMR2RLL, 0xCA);                   // Timer/Counter 2 Reload Low
SFR (TMR2RLH, 0xCB);                   // Timer/Counter 2 Reload High
SFR (TMR2L, 0xCC);                     // Timer/Counter 2 Low
SFR (TMR2H, 0xCD);                     // Timer/Counter 2 High
SFR (SMB0ADM, 0xCF);                   // SMBus0 Address Match
SFR (PSW, 0xD0);                       // Program Status Word
SFR (SCON1, 0xD2);                     // UART1 Control
SFR (SBUF1, 0xD3);                     // UART1 Data Buffer
SFR (P0SKIP, 0xD4);                    // Port 0 Skip
SFR (P1SKIP, 0xD5);                    // Port 1 Skip
SFR (USB0XCN, 0xD7);                   // USB0 Transceiver Control
SFR (PCA0CN, 0xD8);                    // PCA0 Control
SFR (PCA0MD, 0xD9);                    // PCA0 Mode
SFR (PCA0CPM0, 0xDA);                  // PCA0 Module 0 Mode Register
SFR (PCA0CPM1, 0xDB);                  // PCA0 Module 1 Mode Register
SFR (PCA0CPM2, 0xDC);                  // PCA0 Module 2 Mode Register
SFR (ACC, 0xE0);                       // Accumulator
SFR (XBR0, 0xE1);                      // Port I/O Crossbar Control 0
SFR (XBR1, 0xE2);                      // Port I/O Crossbar Control 1
SFR (XBR2, 0xE3);                      // Port I/O Crossbar Control 2
SFR (IT01CF, 0xE4);                    // INT0/INT1 Configuration
SFR (SMOD1, 0xE5);                     // UART1 Mode
SFR (EIE1, 0xE6);                      // Extended Interrupt Enable 1
SFR (EIE2, 0xE7);                      // Extended Interrupt Enable 2
SFR (PCA0CPL1, 0xE9);                  // PCA0 Capture 1 Low
SFR (PCA0CPH1, 0xEA);                  // PCA0 Capture 1 High
SFR (PCA0CPL2, 0xEB);                  // PCA0 Capture 2 Low
SFR (PCA0CPH2, 0xEC);                  // PCA0 Capture 2 High
SFR (RSTSRC, 0xEF);                    // Reset Source Configuration/Status
SFR (B, 0xF0);                         // B Register
SFR (P0MDIN, 0xF1);                    // Port 0 Input Mode Configuration
SFR (P1MDIN, 0xF2);                    // Port 1 Input Mode Configuration
SFR (PCA0PWM, 0xF4);                   // PCA0 PWM Configuration Register
SFR (IAPCN, 0xF5);                     // In-Application Programming Control
SFR (EIP1, 0xF6);                      // Extended Interrupt Priority 1
SFR (EIP2, 0xF7);                      // Extended Interrupt Priority 2
SFR (SPI0CN, 0xF8);                    // SPI0 Control
SFR (PCA0L, 0xF9);                     // PCA0 Counter Low
SFR (PCA0H, 0xFA);                     // PCA0 Counter High
SFR (PCA0CPL0, 0xFB);                  // PCA0 Capture 0 Low
SFR (PCA0CPH0, 0xFC);                  // PCA0 Capture 0 High
SFR (VDM0CN, 0xFF);                    // VDD Monitor Control

//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16 (DP, 0x82);                      // Data Pointer
SFR16 (TMR3RL, 0x92);                  // Timer 3 Reload
SFR16 (TMR3, 0x94);                    // Timer 3 Counter
SFR16 (SBRL1, 0xB4);                   // UART1 Baud Rate Generator
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
#define SFR_IP       0xB8
#define SFR_SMB0CN   0xC0
#define SFR_TMR2CN   0xC8
#define SFR_PSW      0xD0
#define SFR_PCA0CN   0xD8
#define SFR_ACC      0xE0
#define SFR_B        0xF0
#define SFR_SPI0CN   0xF8

//-----------------------------------------------------------------------------
// Bit Definitions
//-----------------------------------------------------------------------------

// TCON 0x88
SBIT (TF1, SFR_TCON, 7);               // Timer1 overflow flag
SBIT (TR1, SFR_TCON, 6);               // Timer1 on/off control
SBIT (TF0, SFR_TCON, 5);               // Timer0 overflow flag
SBIT (TR0, SFR_TCON, 4);               // Timer0 on/off control
SBIT (IE1, SFR_TCON, 3);               // Ext interrupt 1 edge flag
SBIT (IT1, SFR_TCON, 2);               // Ext interrupt 1 type
SBIT (IE0, SFR_TCON, 1);               // Ext interrupt 0 edge flag
SBIT (IT0, SFR_TCON, 0);               // Ext interrupt 0 type

// SCON0 0x98
SBIT (S0MODE, SFR_SCON0, 7);           // Serial mode control bit 0
                                       // Bit6 UNUSED
SBIT (MCE0, SFR_SCON0, 5);             // Multiprocessor communication enable
SBIT (REN0, SFR_SCON0, 4);             // Receive enable
SBIT (TB80, SFR_SCON0, 3);             // Transmit bit 8
SBIT (RB80, SFR_SCON0, 2);             // Receive bit 8
SBIT (TI0, SFR_SCON0, 1);              // Transmit interrupt flag
SBIT (RI0, SFR_SCON0, 0);              // Receive interrupt flag

// IE 0xA8
SBIT (EA, SFR_IE, 7);                  // Global interrupt enable
SBIT (ESPI0, SFR_IE, 6);               // SPI0 interrupt enable
SBIT (ET2, SFR_IE, 5);                 // Timer2 interrupt enable
SBIT (ES0, SFR_IE, 4);                 // UART0 interrupt enable
SBIT (ET1, SFR_IE, 3);                 // Timer1 interrupt enable
SBIT (EX1, SFR_IE, 2);                 // External interrupt 1 enable
SBIT (ET0, SFR_IE, 1);                 // Timer0 interrupt enable
SBIT (EX0, SFR_IE, 0);                 // External interrupt 0 enable

// IP 0xB8
                                       // Bit7 UNUSED
SBIT (PSPI0, SFR_IP, 6);               // SPI0 interrupt priority
SBIT (PT2, SFR_IP, 5);                 // Timer2 priority
SBIT (PS0, SFR_IP, 4);                 // UART0 priority
SBIT (PT1, SFR_IP, 3);                 // Timer1 priority
SBIT (PX1, SFR_IP, 2);                 // External interrupt 1 priority
SBIT (PT0, SFR_IP, 1);                 // Timer0 priority
SBIT (PX0, SFR_IP, 0);                 // External interrupt 0 priority

// SMB0CN 0xC0
SBIT (MASTER, SFR_SMB0CN, 7);          // Master/slave indicator
SBIT (TXMODE, SFR_SMB0CN, 6);          // Transmit mode indicator
SBIT (STA, SFR_SMB0CN, 5);             // Start flag
SBIT (STO, SFR_SMB0CN, 4);             // Stop flag
SBIT (ACKRQ, SFR_SMB0CN, 3);           // Acknowledge request
SBIT (ARBLOST, SFR_SMB0CN, 2);         // Arbitration lost indicator
SBIT (ACK, SFR_SMB0CN, 1);             // Acknowledge flag
SBIT (SI, SFR_SMB0CN, 0);              // SMBus interrupt flag

// TMR2CN 0xC8
SBIT (TF2H, SFR_TMR2CN, 7);            // Timer2 high byte overflow flag
SBIT (TF2L, SFR_TMR2CN, 6);            // Timer2 low byte overflow flag
SBIT (TF2LEN, SFR_TMR2CN, 5);          // Timer2 low byte interrupt enable
SBIT (TF2CEN, SFR_TMR2CN, 4);          // Timer2 LFO capture enable
SBIT (T2SPLIT, SFR_TMR2CN, 3);         // Timer2 split mode enable
SBIT (TR2, SFR_TMR2CN, 2);             // Timer2 on/off control
                                       // Bit1 UNUSED
SBIT (T2XCLK, SFR_TMR2CN, 0);          // Timer2 external clock select

// PSW 0xD0
SBIT (CY, SFR_PSW, 7);                 // Carry flag
SBIT (AC, SFR_PSW, 6);                 // Auxiliary carry flag
SBIT (F0, SFR_PSW, 5);                 // User flag 0
SBIT (RS1, SFR_PSW, 4);                // Register bank select 1
SBIT (RS0, SFR_PSW, 3);                // Register bank select 0
SBIT (OV, SFR_PSW, 2);                 // Overflow flag
SBIT (F1, SFR_PSW, 1);                 // User flag 1
SBIT (P, SFR_PSW, 0);                  // Accumulator parity flag

// PCA0CN 0xD8
SBIT (CF, SFR_PCA0CN, 7);              // PCA0 counter overflow flag
SBIT (CR, SFR_PCA0CN, 6);              // PCA0 counter run control
                                       // Bit5 UNUSED
SBIT (CCF4, SFR_PCA0CN, 4);            // PCA0 module4 capture/compare flag
SBIT (CCF3, SFR_PCA0CN, 3);            // PCA0 module3 capture/compare flag
SBIT (CCF2, SFR_PCA0CN, 2);            // PCA0 module2 capture/compare flag
SBIT (CCF1, SFR_PCA0CN, 1);            // PCA0 module1 capture/compare flag
SBIT (CCF0, SFR_PCA0CN, 0);            // PCA0 module0 capture/compare flag

// SPI0CN 0xF8
SBIT (SPIF, SFR_SPI0CN, 7);            // SPI0 interrupt flag
SBIT (WCOL, SFR_SPI0CN, 6);            // SPI0 write collision flag
SBIT (MODF, SFR_SPI0CN, 5);            // SPI0 mode fault flag
SBIT (RXOVRN, SFR_SPI0CN, 4);          // SPI0 rx overrun flag
SBIT (NSSMD1, SFR_SPI0CN, 3);          // SPI0 slave select mode 1
SBIT (NSSMD0, SFR_SPI0CN, 2);          // SPI0 slave select mode 0
SBIT (TXBMT, SFR_SPI0CN, 1);           // SPI0 transmit buffer empty
SBIT (SPIEN, SFR_SPI0CN, 0);           // SPI0 SPI enable

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
#define INTERRUPT_USB0             8   // USB0 Transceiver
                                       // RESERVED
                                       // RESERVED
#define INTERRUPT_PCA0             11  // PCA0 Peripheral
                                       // RESERVED
                                       // RESERVED
#define INTERRUPT_TIMER3           14  // Timer3 Overflow
#define INTERRUPT_VBUS_LEVEL       15  // VBUS level-triggered interrupt
#define INTERRUPT_UART1            16  // Serial Port 1
                                       // RESERVED
#define INTERRUPT_PORT_MATCH       18  // Port Match

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define C8051T622_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
