//------------------------------------------------------------------------------
// F04x_CAN1.c
//------------------------------------------------------------------------------
//
//
// DEVICE: C8051F040
//
// AUTHOR: LS
//
// TOOLS:  Keil C-compiler and Silicon Labs IDE
//
//
// CAN1.c and CAN2.c are a simple example of configuring a CAN network to
// transmit and receive data on a CAN network, and how to move information to
// and from CAN RAM message objects.  Each C8051F040-TB CAN node is configured
// to send a message when it's P3.7 button is depressed/released, with a 0x11
// to indicate the button is pushed, and 0x00 when released. Each node also has
// a message object configured to receive messages. The C8051 tests the
// received data and will turn on/off the target board's LED. When one target
// is loaded with CAN2.c and the other is loaded with CAN1.c, one target
// board's push-button will control the other target board's LED, establishing
// a simple control link via the CAN bus and can be observed directly on the
// target boards.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <c8051f040.h>                          // SFR declarations

// CAN Protocol Register Index for CAN0ADR, from TABLE 18.1 of the C8051F040
// datasheet
////////////////////////////////////////////////////////////////////////////////
#define CANCTRL            0x00                 //Control Register
#define CANSTAT            0x01                 //Status register
#define ERRCNT             0x02                 //Error Counter Register
#define BITREG             0x03                 //Bit Timing Register
#define INTREG             0x04                 //Interrupt Low Byte Register
#define CANTSTR            0x05                 //Test register
#define BRPEXT             0x06                 //BRP Extension         Register
////////////////////////////////////////////////////////////////////////////////
//IF1 Interface Registers
////////////////////////////////////////////////////////////////////////////////
#define IF1CMDRQST         0x08                 //IF1 Command Rest      Register
#define IF1CMDMSK          0x09                 //IF1 Command Mask      Register
#define IF1MSK1            0x0A                 //IF1 Mask1             Register
#define IF1MSK2            0x0B                 //IF1 Mask2             Register
#define IF1ARB1            0x0C                 //IF1 Arbitration 1     Register
#define IF1ARB2            0x0D                 //IF1 Arbitration 2     Register
#define IF1MSGC            0x0E                 //IF1 Message Control   Register
#define IF1DATA1           0x0F                 //IF1 Data A1           Register
#define IF1DATA2           0x10                 //IF1 Data A2           Register
#define IF1DATB1           0x11                 //IF1 Data B1           Register
#define IF1DATB2           0x12                 //IF1 Data B2           Register
////////////////////////////////////////////////////////////////////////////////
//IF2 Interface Registers
////////////////////////////////////////////////////////////////////////////////
#define IF2CMDRQST         0x20                 //IF2 Command Rest      Register
#define IF2CMDMSK          0x21                 //IF2 Command Mask      Register
#define IF2MSK1            0x22                 //IF2 Mask1             Register
#define IF2MSK2            0x23                 //IF2 Mask2             Register
#define IF2ARB1            0x24                 //IF2 Arbitration 1     Register
#define IF2ARB2            0x25                 //IF2 Arbitration 2     Register
#define IF2MSGC            0x26                 //IF2 Message Control   Register
#define IF2DATA1           0x27                 //IF2 Data A1           Register
#define IF2DATA2           0x28                 //IF2 Data A2           Register
#define IF2DATB1           0x29                 //IF2 Data B1           Register
#define IF2DATB2           0x2A                 //IF2 Data B2           Register
////////////////////////////////////////////////////////////////////////////////
//Message Handler Registers
////////////////////////////////////////////////////////////////////////////////
#define TRANSREQ1          0x40                 //Transmission Rest1 Register
#define TRANSREQ2          0x41                 //Transmission Rest2 Register

#define NEWDAT1            0x48                 //New Data 1            Register
#define NEWDAT2            0x49                 //New Data 2            Register

#define INTPEND1           0x50                 //Interrupt Pending 1   Register
#define INTPEND2           0x51                 //Interrupt Pending 2   Register

#define MSGVAL1            0x58                 //Message Valid 1       Register
#define MSGVAL2            0x59                 //Message Valid 2       Register

////////////////////////////////////////////////////////////////////////////////
//Global Variables
////////////////////////////////////////////////////////////////////////////////
char MsgNum;
char status;
int i;
int MOTwoIndex = 0;
int MOOneIndex = 0;
int StatusCopy;
int RXbuffer [4];
int TXbuffer [8];
int MsgIntNum;
int Temperature;
sbit BUTTON = P3^7;
sbit LED = P1^6;
sfr16 CAN0DAT = 0xD8;



////////////////////////////////////////////////////////////////////////////////
// Function PROTOTYPES
////////////////////////////////////////////////////////////////////////////////

// Initialize Message Object
void clear_msg_objects (void);
void init_msg_object_TX (char MsgNum);
void init_msg_object_RX (char MsgNum);
void start_CAN (void);
void transmit_turn_LED_ON (char MsgNum);
void transmit_turn_LED_OFF (char MsgNum);
void receive_data (char MsgNum);
void external_osc (void);
void config_IO (void);
void flash_LED (void);
void test_reg_write (char test);
void stop_CAN (void);


////////////////////////////////////////////////////////////////////////////////
// MAIN Routine
////////////////////////////////////////////////////////////////////////////////
void main (void) {

  // disable watchdog timer
  WDTCN = 0xde;
  WDTCN = 0xad;

  //configure Port I/O
  config_IO();

  // switch to external oscillator
  external_osc();


////////////////////////////////////////////////////////////////////////////////
// Configure CAN communications
//
// IF1 used for procedures calles by main program
// IF2 used for interrupt service procedure receive_data
//
// Message Object assignments:
//  0x02: Used to transmit commands to toggle its LED, arbitration number 1
//
////////////////////////////////////////////////////////////////////////////////

  // Clear CAN RAM
  clear_msg_objects();

  // Initialize message object to transmit data
  init_msg_object_TX (0x02);

  // Initialize message object to receive data
  init_msg_object_RX (0x01);

  // Enable CAN interrupts in CIP-51
  EIE2 = 0x20;

  //Function call to start CAN
  start_CAN();

  //Global enable 8051 interrupts
  EA = 1;

  //Loop and wait for interrupts
  while (1)
    {
      if (BUTTON == 0){
        while (BUTTON == 0){}
        transmit_turn_LED_OFF(0x02);}
      else {
        while (BUTTON == 1){}
        transmit_turn_LED_ON(0x02);}
    }
}


////////////////////////////////////////////////////////////////////////////////
// Set up C8051F040
////////////////////////////////////////////////////////////////////////////////

// Switch to external oscillator
void external_osc (void)
{
  int n;                        // local variable used in delay FOR loop.
  SFRPAGE = CONFIG_PAGE;        // switch to config page to config oscillator
  OSCXCN  = 0x77;               // start external oscillator; 22.1 MHz Crystal
                                // system clock is 22.1 MHz / 2 = 11.05 MHz
  for (n=0;n<255;n++);          // delay about 1ms
  while ((OSCXCN & 0x80) == 0); // wait for oscillator to stabilize
  CLKSEL |= 0x01;               // switch to external oscillator
}

void config_IO (void)
{
  SFRPAGE  = CONFIG_PAGE;        //Port SFR's on Configuration page
  XBR3     = 0x80;     // Configure CAN TX pin (CTX) as push-pull digital output
  P1MDOUT |= 0x40;     // Configure P1.6 as push-pull to drive LED
  XBR2     = 0x40;     // Enable Crossbar/low ports
}

////////////////////////////////////////////////////////////////////////////////
//CAN Functions
////////////////////////////////////////////////////////////////////////////////


//Clear Message Objects
void clear_msg_objects (void)
{
  SFRPAGE  = CAN0_PAGE;
  CAN0ADR  = IF1CMDMSK;    // Point to Command Mask Register 1
  CAN0DATL = 0xFF;         // Set direction to WRITE all IF registers to Msg Obj
  for (i=1;i<33;i++)
    {
      CAN0ADR = IF1CMDRQST; // Write blank (reset) IF registers to each msg obj
      CAN0DATL = i;
    }
}

//Initialize Message Object for RX
void init_msg_object_RX (char MsgNum)
{
  SFRPAGE  = CAN0_PAGE;
  CAN0ADR  = IF1CMDMSK;  // Point to Command Mask 1
  CAN0DAT  = 0x00B8;     // Set to WRITE, and alter all Msg Obj except ID MASK
                         // and data bits
  CAN0ADR  = IF1ARB1;    // Point to arbitration1 register
  CAN0DAT  = 0x0000;     // Set arbitration1 ID to "0"
  CAN0DAT  = 0x8004;     // Arb2 high byte:Set MsgVal bit, no extended ID,
                         // Dir = RECEIVE
  CAN0DAT  = 0x0480;     // Msg Cntrl: set RXIE, remote frame function disabled
  CAN0ADR  = IF1CMDRQST; // Point to Command Request reg.
  CAN0DATL = MsgNum;     // Select Msg Obj passed into function parameter list
                         // --initiates write to Msg Obj
  // 3-6 CAN clock cycles to move IF register contents to the Msg Obj in CAN RAM
}

//Initialize Message Object for TX
void init_msg_object_TX (char MsgNum)
{
  SFRPAGE = CAN0_PAGE;
  CAN0ADR = IF1CMDMSK;  // Point to Command Mask 1
  CAN0DAT = 0x00B2;     // Set to WRITE, & alter all Msg Obj except ID MASK bits
  CAN0ADR = IF1ARB1;    // Point to arbitration1 register
  CAN0DAT = 0x0000;     // Set arbitration1 ID to highest priority
  CAN0DAT = 0xA000;     // Autoincrement to Arb2 high byte:
                        // Set MsgVal bit, no extended ID, Dir = WRITE
  CAN0DAT = 0x0081;     // Msg Cntrl: DLC = 1, remote frame function not enabled
  CAN0ADR = IF1CMDRQST; // Point to Command Request reg.
  CAN0DAT = MsgNum;     // Select Msg Obj passed into function parameter list
                        // --initiates write to Msg Obj
  // 3-6 CAN clock cycles to move IF reg contents to the Msg Obj in CAN RAM.
}

//Start CAN
void start_CAN (void)
{
  /* Calculation of the CAN bit timing :

  System clock        f_sys = 22.1184 MHz/2 = 11.0592 MHz.
  System clock period t_sys = 1/f_sys = 90.422454 ns.
  CAN time quantum       tq = t_sys (at BRP = 0)

  Desired bit rate is 1 MBit/s, desired bit time is 1000 ns.
  Actual bit time = 11 tq = 996.65ns ~ 1000 ns
  Actual bit rate is 1.005381818 MBit/s = Desired bit rate+0.5381%

  CAN bus length = 10 m, with 5 ns/m signal delay time.
  Propagation delay time : 2*(transceiver loop delay + bus line delay) = 400 ns
  (maximum loop delay between CAN nodes)

  Prop_Seg = 5 tq = 452 ns ( >= 400 ns).
  Sync_Seg = 1 tq

  Phase_seg1 + Phase_Seg2 = (11-6) tq = 5 tq
  Phase_seg1 <= Phase_Seg2,  =>  Phase_seg1 = 2 tq and Phase_Seg2 = 3 tq
  SJW = (min(Phase_Seg1, 4) tq = 2 tq

  TSEG1 = (Prop_Seg + Phase_Seg1 - 1) = 6
  TSEG2 = (Phase_Seg2 - 1)            = 2
  SJW_p = (SJW - 1)                   = 1

  Bit Timing Register = BRP + SJW_p*0x0040 = TSEG1*0x0100 + TSEG2*0x1000 = 2640

  Clock tolerance df :

  A: df < min(Phase_Seg1, Phase_Seg2) / (2 * (13*bit_time - Phase_Seg2))
  B: df < SJW / (20 * bit_time)

  A: df < 2/(2*(13*11-3)) = 1/(141-3) = 1/138 = 0.7246%
  B: df < 2/(20*11)                   = 1/110 = 0.9091%

  Actual clock tolerance is 0.7246% - 0.5381% = 0.1865% (no problem for quartz)
  */

  SFRPAGE  = CAN0_PAGE;
  CAN0CN  |= 0x41;       // Configuration Change Enable CCE and INIT
  CAN0ADR  = BITREG   ;  // Point to Bit Timing register
  CAN0DAT  = 0x2640;     // see above

  CAN0ADR  = IF1CMDMSK;  // Point to Command Mask 1
  CAN0DAT  = 0x0087;     // Config for TX : WRITE to CAN RAM, write data bytes,
                         // set TXrqst/NewDat, clr IntPnd

  // RX-IF2 operation may interrupt TX-IF1 operation
  CAN0ADR  = IF2CMDMSK;  // Point to Command Mask 2
  CAN0DATL = 0x1F;       // Config for RX : READ CAN RAM, read data bytes,
                         // clr NewDat and IntPnd
  CAN0CN  |= 0x06;       // Global Int. Enable IE and SIE
  CAN0CN  &= ~0x41;      // Clear CCE and INIT bits, starts CAN state machine
}

//Transmit CAN frame to turn other node's LED ON
void transmit_turn_LED_ON (char MsgNum)
{
  SFRPAGE  = CAN0_PAGE;  // IF1 already set up for TX
  CAN0ADR  = IF1CMDMSK;  // Point to Command Mask 1
  CAN0DAT  = 0x0087;     // Config to WRITE to CAN RAM, write data bytes,
                         // set TXrqst/NewDat, Clr IntPnd
  CAN0ADR  = IF1DATA1;   // Point to 1st byte of Data Field
  CAN0DATL = 0x11;       // Ones signals to turn LED's light ON in data A1 field
  CAN0ADR  = IF1CMDRQST; // Point to Command Request Reg.
  CAN0DATL = MsgNum;     // Move new data for TX to Msg Obj "MsgNum"
}

//Transmit CAN Frame to turn other node's LED OFF
void transmit_turn_LED_OFF (char MsgNum)
{
  SFRPAGE  = CAN0_PAGE;  // IF1 already set up for TX
  CAN0ADR  = IF1DATA1;   // Point to 1st byte of Data Field
  CAN0DATL = 0x00;       // Zero signals to turn LED's light ON in Data A1 field
  CAN0ADR  = IF1CMDRQST; // Point to Command Request Reg.
  CAN0DATL = MsgNum;     // Move new data for TX to Msg Obj "MsgNum"
}


// Receive Data from the IF2 buffer
void receive_data (char MsgNum)
{
  char virtual_button;
  SFRPAGE  = CAN0_PAGE; // IF1 already set up for RX
  CAN0ADR  = IF2CMDRQST;// Point to Command Request Reg.
  CAN0DATL = MsgNum;    // Move new data for RX from Msg Obj "MsgNum"
                        // Move new data to a
  CAN0ADR  = IF2DATA1;  // Point to 1st byte of Data Field

  virtual_button = CAN0DATL;
  if (virtual_button == 0x11)   //Ones is signal from other node to turn LED ON
    LED = 1;
  else  LED = 0;                //Otherwise turn LED OFF (message was one's)
}

////////////////////////////////////////////////////////////////////////////////
//Interrupt Service Routine
////////////////////////////////////////////////////////////////////////////////
void ISRname (void) interrupt 19
{
  status = CAN0STA;
  if ((status&0x10) != 0)
    {                            // RxOk is set, interrupt caused by reception
      CAN0STA = (CAN0STA&0xEF)|0x07;         // Reset RxOk, set LEC to NoChange
      /* read message number from CAN INTREG */
      receive_data (0x01);             // Up to now, we have only one RX message
    }
  if ((status&0x08) != 0)
    {                            // TxOk is set, interrupt caused by transmision
      CAN0STA = (CAN0STA&0xF7)|0x07;        // Reset TxOk, set LEC to NoChange
    }
  if (((status&0x07) != 0)&&((status&0x07) != 7))
    {                           // Error interrupt, LEC changed
      /* error handling ? */
      CAN0STA = CAN0STA|0x07;              // Set LEC to NoChange
    }
}

