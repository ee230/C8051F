//-----------------------------------------------------------------------------
// C8051F326_defs.h
//-----------------------------------------------------------------------------
// Copyright 2008, Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the C8051F326/7 family.
// **Important Note**: The compiler_defs.h header file should be included
// before including this header file.
//
// Target:         C8051F326, 'F327
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.1 - 22 JULY 2008 (ES)
//    -Added ACC under bit addressable registers
// Release 1.0
//    -Initial Release (GP)
//    -11 SEP 2007
//

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef C8051F326_DEFS_H
#define C8051F326_DEFS_H

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------


SFR (P0, 0x80);                        // Port0
SFR (SP, 0x81);                        // Stack pointer
SFR (DPL, 0x82);                       // Data pointer - Low byte
SFR (DPH, 0x83);                       // Data pointer - High byte
SFR (PCON, 0x87);                      // Power control
SFR (TCON, 0x88);                      // Timer control
SFR (TMOD, 0x89);                      // Timer mode
SFR (TL0, 0x8A);                       // Timer0 - Low byte
SFR (TL1, 0x8B);                       // Timer1 - Low byte
SFR (TH0, 0x8C);                       // Timer0 - High byte
SFR (TH1, 0x8D);                       // Timer1 - High byte
SFR (CKCON, 0x8E);                     // Clock Control
SFR (PSCTL, 0x8F);                     // Program store R/W Control
SFR (SBCON0, 0x91);                    // Baud Rate Generator 0 Control
SFR (SBRLL0, 0x93);                    // Baud Rate Generator 0 Low Byte
SFR (SBRLH0, 0x94);                    // Baud Rate Generator 0 High Byte
SFR (USB0ADR, 0x96);                   // USB0 Address Port
SFR (USB0DAT, 0x97);                   // USB0 Data Port
SFR (SCON0, 0x98);                     // Serial Port 0 Control
SFR (SBUF0, 0x99);                     // Serial Port 0 Data Buffer
SFR (SMOD0, 0x9A);                     // Serial Port 0 Mode
SFR (P2, 0xA0);                        // Port2
SFR (P0MDOUT, 0xA4);                   // Port 0 Output Mode
SFR (P2MDOUT, 0xA6);                   // Port 2 Output Mode
SFR (P3MDOUT, 0xA7);                   // Port 3 Output Mode
SFR (IE, 0xA8);                        // Interrupt Enable
SFR (CLKSEL, 0xA9);                    // Clock Source Select
SFR (EMI0CN, 0xAA);                    // External Memory Interface Control
SFR (P3, 0xB0);                        // Port 3
SFR (OSCICN, 0xB2);                    // Internal Oscillator Control
SFR (OSCICL, 0xB3);                    // Internal Oscillator Calibration
SFR (FLSCL, 0xB6);                     // Flash Scale
SFR (FLKEY, 0xB7);                     // Flash Lock & Key
SFR (IP, 0xB8);                        // Interrupt Priority
SFR (CLKMUL, 0xB9);                    // Clock Multiplier Control
SFR (REG0CN, 0xC9);                    // Regulator Control
SFR (PSW, 0xD0);                       // Program Status Word
SFR (USB0XCN, 0xD7);                   // USB0 Tranceiver Control
SFR (ACC, 0xE0);                       // Accumulator
SFR (GPIOCN, 0xE2);                    // Global Port I/O Control
SFR (OSCLCN, 0xE3);                    // Low Frequency Internal Osc. Control
SFR (EIE1, 0xE6);                      // Extended Interrupt Enable 1
SFR (EIE2, 0xE7);                      // Extended Interrupt Enable 2
SFR (RSTSRC, 0xEF);                    // Reset Source
SFR (B, 0xF0);                         // B register
SFR (EIP1, 0xF6);                      // Extended Interrupt Priority 1
SFR (EIP2, 0xF7);                      // Extended Interrupt Priority 2
SFR (VDM0CN, 0xFF);                    // Vdd monitor control


//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

SFR16 (DP, 0x82);
SFR16 (SBRL0, 0x93);

//-----------------------------------------------------------------------------
// Address Definitions for Bit-addressable Registers
//-----------------------------------------------------------------------------

#define SFR_P0       0x80
#define SFR_TCON     0x88
#define SFR_SCON0    0x98
#define SFR_P2       0xA0
#define SFR_IE       0xA8
#define SPR_P3       0xB0
#define SFR_IP       0xB8
#define SFR_PSW      0xD0
#define SFR_ACC      0xE0
#define SFR_B        0xF0

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
SBIT (OVR0, SFR_SCON0, 7);             // Receive FIFO Overrun Flag
SBIT (PERR0, SFR_SCON0, 7);            // Parity Error Flag
                                       // Bit 5 UNUSED
SBIT (REN0, SFR_SCON0, 4);             // Receive enable
SBIT (TBX0, SFR_SCON0, 3);             // Transmit Bit 8
SBIT (RBX0, SFR_SCON0, 2);             // Receive Bit 8
SBIT (TI0, SFR_SCON0, 1);              // Transmit Interrupt Flag
SBIT (RI0, SFR_SCON0, 0);              // Receive Interrupt Flag


// IE 0xA8
SBIT (EA, SFR_IE, 7);                  // Global interrupt enable
                                       // Bit 6 UNUSED
                                       // Bit 5 UNUSED
SBIT (ES0, SFR_IE, 4);                 // UART0 interrupt enable
SBIT (ET1, SFR_IE, 3);                 // Timer1 interrupt enable
SBIT (EX1, SFR_IE, 2);                 // External interrupt 1 enable
SBIT (ET0, SFR_IE, 1);                 // Timer0 interrupt enable
SBIT (EX0, SFR_IE, 0);                 // External interrupt 0 enable


// IP 0xB8
                                       // Bit 7 UNUSED
                                       // Bit 6 UNUSED
                                       // Bit 5 UNUSED
SBIT (PS0, SFR_IP, 4);                 // UART0 priority
SBIT (PT1, SFR_IP, 3);                 // Timer1 priority
SBIT (PX1, SFR_IP, 2);                 // External interrupt 1 priority
SBIT (PT0, SFR_IP, 1);                 // Timer0 priority
SBIT (PX0, SFR_IP, 0);                 // External interrupt 0 priority


// PSW 0xD0
SBIT (CY, SFR_PSW, 7);                 // Carry flag
SBIT (AC, SFR_PSW, 6);                 // Auxiliary carry flag
SBIT (F0, SFR_PSW, 5);                 // User flag 0
SBIT (RS1, SFR_PSW, 4);                // Register bank select 1
SBIT (RS0, SFR_PSW, 3);                // Register bank select 0
SBIT (OV, SFR_PSW, 2);                 // Overflow flag
SBIT (F1, SFR_PSW, 1);                 // User flag 1
SBIT (P, SFR_PSW, 0);                  // Accumulator parity flag


//-----------------------------------------------------------------------------
// Interrupt Priorities
//-----------------------------------------------------------------------------

#define INTERRUPT_INT0             0   // External Interrupt 0
#define INTERRUPT_TIMER0           1   // Timer0 Overflow
#define INTERRUPT_INT1             2   // External Interrupt 1
#define INTERRUPT_TIMER1           3   // Timer1 Overflow
#define INTERRUPT_UART0            4   // Serial Port 0
#define INTERRUPT_USB0             8   // USB Interface
#define INTERRUPT_VBUS_LEVEL       15  // VBUS level-triggered interrupt

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#endif                                 // #define C8051F326_DEFS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
