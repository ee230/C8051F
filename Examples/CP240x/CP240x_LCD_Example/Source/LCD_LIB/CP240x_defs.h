//-----------------------------------------------------------------------------
// CP240x_defs.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// CP240x Register Definitions
//
// Target:         Multiple
// Tool chain:     Generic
//                 Silicon Laboratories IDE version 3.40 
// Project Name:   CP240x_Firmware
//
// 
//
// Release 1.0
//    -26 OCT 2009
//

// SmaRTClock Registers
#define RTCKEY                         0x0A  //  RTC0 Indirect Address
#define RTCADR                         0x0B  //  RTC0 Indirect Data
#define RTCDAT                         0x0C  //  RTC0 Lock and Key

// Interrupt Mask and Clocking Registers
#define INT0EN                         0x30  //  Interrupt Enable Register 0
#define INT1EN                         0x31  //  Interrupt Enable Register 1
#define CLKSL                          0x32  //  Clock Select
#define IOSCCN                         0x33  //  Internal Oscillator Control
#define REVID                          0x34  //  Revision Identifier

// Interrupt Status Registers
#define INT0RD                         0x40  //  Interrupt Status Register 0 (read-only)
#define INT1RD                         0x41  //  Interrupt Status Register 1 (read-only)
#define ULPST                          0x42  //  Ultra Low Power Status
#define INT0                           0x43  //  Interrupt Status Register 0 (self-clearing)
#define INT1                           0x44  //  Interrupt Status Register 1 (self-clearing)

// Timer 0 and Timer 1 Registers
#define TMR0RLL                        0x50  //  Timer 0 Reload Register Low Byte
#define TMR0RLH                        0x51  //  Timer 0 Reload Register High Byte
#define TMR0L                          0x52  //  Timer 0 Low Byte
#define TMR0H                          0x53  //  Timer 0 High Byte
#define TMR0CN                         0x54  //  Timer 0 Control
#define TMR1RLL                        0x55  //  Timer 1 Reload Register Low Byte
#define TMR1RLH                        0x56  //  Timer 1 Reload Register High Byte
#define TMR1L                          0x57  //  Timer 1 Low Byte
#define TMR1H                          0x58  //  Timer 1 High Byte
#define TMR1CN                         0x59  //  Timer 1 Control

// SMBus Registers
#define SMBCF                          0x68  //  SMBus Configuration

// ULP/LCD0 Data Registers
#define LCD0BLINK                      0x80  //  LCD0 Segment Blink
#define ULPMEM00                       0x81  //  ULP Memory Byte 0
#define ULPMEM01                       0x82  //  ULP Memory Byte 1
#define ULPMEM02                       0x83  //  ULP Memory Byte 2
#define ULPMEM03                       0x84  //  ULP Memory Byte 3
#define ULPMEM04                       0x85  //  ULP Memory Byte 4
#define ULPMEM05                       0x86  //  ULP Memory Byte 5
#define ULPMEM06                       0x87  //  ULP Memory Byte 6
#define ULPMEM07                       0x88  //  ULP Memory Byte 7
#define ULPMEM08                       0x89  //  ULP Memory Byte 8
#define ULPMEM09                       0x8A  //  ULP Memory Byte 9
#define ULPMEM10                       0x8B  //  ULP Memory Byte 10
#define ULPMEM11                       0x8C  //  ULP Memory Byte 11
#define ULPMEM12                       0x8D  //  ULP Memory Byte 12
#define ULPMEM13                       0x8E  //  ULP Memory Byte 13
#define ULPMEM14                       0x8F  //  ULP Memory Byte 14
#define ULPMEM15                       0x90  //  ULP Memory Byte 15

// LCD Control Registers
#define LCD0CN                         0x95  //  LCD0 Control
#define CONTRAST                       0x96  //  LCD0 Contrast Adjustment
#define LCD0CF                         0x97  //  LCD0 Configuration
#define LCD0DIVL                       0x98  //  LCD0 Clock Divider High Byte
#define LCD0DIVH                       0x99  //  LCD0 Clock Divider Low Byte
#define LCD0TOGR                       0x9A  //  LCD0 Toggle Rate
#define LCD0PWR                        0x9B  //  LCD0 Power Mode

// Ultra Low Power Control Registers
#define MSCN                           0xA0  //  Master Control
#define MSCF                           0xA1  //  Master Configuration
#define ULPCN                          0xA2  //  Ultra Low Power Control

// Port I/O Configuration Registers
#define P0OUT                          0xB0  //  Port 0 Output Data Latch
#define P1OUT                          0xB1  //  Port 1 Output Data Latch
#define P2OUT                          0xB2  //  Port 2 Output Data Latch
#define P3OUT                          0xB3  //  Port 3 Output Data Latch
#define P4OUT                          0xB4  //  Port 4 Output Data Latch
#define P0MDI                          0xB5  //  Port 0 Input Mode
#define P1MDI                          0xB6  //  Port 1 Input Mode
#define P2MDI                          0xB7  //  Port 2 Input Mode
#define P3MDI                          0xB8  //  Port 3 Input Mode
#define P4MDI                          0xB9  //  Port 4 Input Mode
#define P0MDO                          0xBA  //  Port 0 Output Mode
#define P1MDO                          0xBB  //  Port 1 Output Mode
#define P2MDO                          0xBC  //  Port 2 Output Mode
#define P3MDO                          0xBD  //  Port 3 Output Mode
#define P4MDO                          0xBE  //  Port 4 Output Mode
#define P0DRIVE                        0xBF  //  Port 0 Drive Strength
#define P1DRIVE                        0xC0  //  Port 1 Drive Strength
#define P2DRIVE                        0xC1  //  Port 2 Drive Strength
#define P3DRIVE                        0xC2  //  Port 3 Drive Strength
#define P4DRIVE                        0xC3  //  Port 4 Drive Strength
#define P0MATCH                        0xC4  //  Port 0 Match
#define P1MATCH                        0xC5  //  Port 1 Match
#define P2MATCH                        0xC6  //  Port 2 Match
#define P3MATCH                        0xC7  //  Port 3 Match
#define P4MATCH                        0xC8  //  Port 4 Match
#define P0MSK                          0xC9  //  Port 0 Mask
#define P1MSK                          0xCA  //  Port 1 Mask
#define P2MSK                          0xCB  //  Port 2 Mask
#define P3MSK                          0xCC  //  Port 3 Mask
#define P4MSK                          0xCD  //  Port 4 Mask

// Port I/O Input and Status Registers
#define PMATCHST                       0xD0  //  Port Match Status
#define P0IN                           0xD1  //  Port 0 Input
#define P1IN                           0xD2  //  Port 1 Input
#define P2IN                           0xD3  //  Port 2 Input
#define P3IN                           0xD4  //  Port 3 Input
#define P4IN                           0xD5  //  Port 4 Input

//-----------------------------------------------------------------------------
// SmaRTClock Register Definitions
//-----------------------------------------------------------------------------
#define CAPTURE0  0x00                 // RTC address of CAPTURE0 register
#define CAPTURE1  0x01                 // RTC address of CAPTURE1 register
#define CAPTURE2  0x02                 // RTC address of CAPTURE2 register
#define CAPTURE3  0x03                 // RTC address of CAPTURE3 register
#define RTC0CN    0x04                 // RTC address of RTC0CN register                
#define RTC0XCN   0x05                 // RTC address of RTC0XCN register 
#define RTC0XCF   0x06                 // RTC address of RTC0XCF register
#define ALARM0    0x08                 // RTC address of ALARM0 register
#define ALARM1    0x09                 // RTC address of ALARM1 register
#define ALARM2    0x0A                 // RTC address of ALARM2 register
#define ALARM3    0x0B                 // RTC address of ALARM3 register

// SmaRTClock Bit Definitions
#define RTC0CAP   0x01
#define RTC0SET   0x02
#define ALRM      0x04
#define RTC0AEN   0x08
#define RTC0TR    0x10
#define OSCFAIL   0x20
#define MCLKEN    0x40
#define RTC0EN    0x80
