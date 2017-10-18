//-----------------------------------------------------------------------------
// CP250x_Main.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Target: CP2501
//
// Tool chain: Raisonance
//
// The purpose of this firmware is to show how to call the UART functions 
// provided by the System Firwmare API
//
// This code interfaces to a hypothetical touch-screen module and so it is not
// a functionally complete example.  No touch data is sent to the USB host.
//

//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "CP250x_API.h"
#include "CP250x_Buffer_Struct.h"
#include "CP250x_Configuration.h"

//-----------------------------------------------------------------------------
// Global Defines
//-----------------------------------------------------------------------------

#define UART_BAUD  115200

//-----------------------------------------------------------------------------
// Global Variables - Shared with System Firmware
//-----------------------------------------------------------------------------

// CP250x_Status is used for function return values
LOCATED_VARIABLE_NO_INIT (CP250x_Status,            U8, SEG_XDATA, 0x01F0);

// Function parameter variables
LOCATED_VARIABLE_NO_INIT (CP250x_Param1,            U8, SEG_XDATA, 0x01F1);
LOCATED_VARIABLE_NO_INIT (CP250x_Param2,            U8, SEG_XDATA, 0x01F2);
LOCATED_VARIABLE_NO_INIT (CP250x_Param3,            U8, SEG_XDATA, 0x01F3);
LOCATED_VARIABLE_NO_INIT (CP250x_Param4,            U8, SEG_XDATA, 0x01F4);
LOCATED_VARIABLE_NO_INIT (CP250x_Param5,            U8, SEG_XDATA, 0x01F5);
LOCATED_VARIABLE_NO_INIT (CP250x_Param6,            U8, SEG_XDATA, 0x01F6);
LOCATED_VARIABLE_NO_INIT (CP250x_Param7,            U8, SEG_XDATA, 0x01F7);

LOCATED_VARIABLE_NO_INIT (CP250x_Counter,           U8, SEG_XDATA, 0x01F8);
LOCATED_VARIABLE_NO_INIT (CP250x_Counter_Status,    U8, SEG_XDATA, 0x01F9);

// Buffer Pointers and Variables
LOCATED_VARIABLE_NO_INIT (Comm_Rx_Buffer[128],      U8, SEG_PDATA, 0x0000);
LOCATED_VARIABLE_NO_INIT (Comm_Tx_Buffer[128],      U8, SEG_PDATA, 0x0080);
LOCATED_VARIABLE_NO_INIT (USB_Position_Buffer[64],  U8, SEG_XDATA, 0x0100);
LOCATED_VARIABLE_NO_INIT (USB_Control_Buffer[64],   U8, SEG_XDATA, 0x0140);

      at ( 0) bit CP250x_USB_Touch_Send_Pending;       // 0x20.0
      at ( 1) bit CP250x_USB_Mouse_Send_Pending;       // 0x20.1
      at ( 2) bit CP250x_USB_Pen_Send_Pending;         // 0x20.2
      at ( 3) bit CP250x_UART_RX_Data_Ready;           // 0x20.3
      at ( 4) bit CP250x_USB_Enter_Suspend;            // 0x20.4
      at ( 5) bit CP250x_Reserved1;                    // 0x20.5
      at ( 6) bit CP250x_Reserved2;                    // 0x20.6
      at ( 7) bit CP250x_Reserved3;                    // 0x20.7

      at ( 8) bit CP250x_Mouse_Mode;                   // 0x21.0
      at ( 9) bit CP250x_SingleTouch_Mode;             // 0x21.1
      at (10) bit CP250x_MultiTouch_Mode;              // 0x21.2
      at (11) bit CP250x_SinglePen_Mode;               // 0x21.3
      at (12) bit CP250x_MultiPen_Mode;                // 0x21.4

      at (16) bit CP250x_USB_Control_Write;            // 0x22.0
      at (17) bit CP250x_USB_Control_Read;             // 0x22.1
      at (18) bit CP250x_USB_Control_Data_Ready;       // 0x22.2

//-----------------------------------------------------------------------------
// User #defines and variables
//-----------------------------------------------------------------------------

// How often to request touch data from the touch-screen module
#define  GET_TOUCH_DATA_TIMEOUT   20     // 10 ms, each tick is 500 us

// Commands to send touch-screen module over UART
#define  RETRIEVE_DATA           0x01

#define SIZE_UART_BUFFER 8  // Total bytes received from UART screen
U16 UART_Buffer[4];         // X1, Y1, X2, Y2 coordinates from touch screen

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

void main (void)
{
   UU32 UART_baud;
   U8 data_rx_count = 0;
   U8 *buf_ptr;

   // --System Initialization--------------------------------------------------

   CP250x_System_Init (BUSPOW, OSC_24);        // Init Regulator and Oscillator
   if (CP250x_Status != FUNC_SUCCESS)
   {
     // !Handle System initialization error
   }

   CP250x_GPIO_Init (0x00, 0x00, 0xFF, 0xFF);  // Initialize GPIO pins
   if (CP250x_Status != FUNC_SUCCESS)
   {
     // !Handle GPIO initialization error
   }

   UART_baud.U32 = UART_BAUD;
   CP250x_UART_Init (UART_baud, EIGHT_DATA_BITS, NO_PARITY, ONE_STOP_BIT);
   if (CP250x_Status != FUNC_SUCCESS)
   {
     // !Handle SPI initialization error
   }

   CP250x_USB_Init(ENUM_MOUSE_MODE);             // Initialize USB peripheral

   //--User Initialization-----------------------------------------------------

   // Start SystemFirmware provided timer. 
   CP250x_Stop_Counter ();
   CP250x_Counter = 0; 
   CP250x_Start_Counter ();

   // --Main Loop--------------------------------------------------------------

   while (1)
   {

      // Every GET_TOUCH_DATA_TIMEOUT, request touch screen data from module
      if (CP250x_Counter == GET_TOUCH_DATA_TIMEOUT)
      {
         // Reset pointer to UART_Buffer to store received data
         buf_ptr = (U8 *) &UART_Buffer;
         data_rx_count = 0;

         // Send a command to UART module to get data
         Comm_Tx_Buffer[0] = RETRIEVE_DATA;
         CP250x_UART_Write (1);      // Command is only 1 byte in length

         CP250x_Stop_Counter ();
         CP250x_Counter = 0; 
         CP250x_Start_Counter ();
      }

      // If data is available, dequeue and put in local buffer
      while (CP250x_UART_RX_Data_Ready)
      {
         // Dequeue 1 byte of data received from touch-screen module
         CP250x_UART_Dequeue();

         // Dequeued data is stored in CP250x_Param1
         *buf_ptr++ = CP250x_Param1;
         data_rx_count++;  
      }

      // If operating system only support mouse mode
      if (CP250x_Mouse_Mode)
      {
         // Check if buffer data has been sent
         if (CP250x_USB_Mouse_Send_Pending == 0)
         {
            if (data_rx_count == SIZE_UART_BUFFER)
            {
               // !Add code here to process data and 
               // put in USB_Position_Buffer

               // Let <System FW> know data is ready
               // CP250x_USB_Mouse_Send_Pending = 1;
            }
         }
      }

      // If operating system supports only one touch point
      if (CP250x_SingleTouch_Mode)
      {
         // Check if buffer data has been sent
         if (CP250x_USB_Touch_Send_Pending == 0)
         {
               // !Add code here to process data and 
               // put in USB_Position_Buffer

            // Let <System FW> know data is ready
            // CP250x_USB_Touch_Send_Pending = 1;
         }
      }

      // If operating system supports multiple touch points
      if (CP250x_MultiTouch_Mode)
      {
         // Check if buffer data has been sent
         if (CP250x_USB_Touch_Send_Pending == 0)
         {
               // !Add code here to process data and 
               // put in USB_Position_Buffer

            // Let <System FW> know data is ready
            // CP250x_USB_Touch_Send_Pending = 1;
         }
      }

      if (CP250x_USB_Enter_Suspend)
      {
         // !Configure GPIO pins as necessary before entering Suspend

         CP250x_USB_Suspend();

         // !Reset GPIO pins after exiting USB Suspend
      }
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------