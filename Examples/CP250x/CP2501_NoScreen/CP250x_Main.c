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
// The purpose of this example is to create a multitouch device that 
// simulates touch data.  This example will run on any CP2501 hardware and 
// can be used to test USB portion of the hardware without requiring a touch
// screen
//
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

#define SPI_CLOCK  100000

#define UPDATE_RATE    80    // Send new data every 40 ms. Each tick is 500 us.

enum TOUCH_STATE {ST_NO_TOUCH, ST_IN_RANGE, ST_TIP_SWITCH, ST_TOUCH, ST_RELEASE};

// Indexes into USB Position Buffer
#define CONTROLS      0x00
#define CONTACT_ID    0x01
#define X_LSB         0x02
#define X_MSB         0x03
#define Y_LSB         0x04
#define Y_MSB         0x05

#define WIDTH_LSB     0x06
#define WIDTH_MSB     0x07
#define HEIGHT_LSB    0x08
#define HEIGHT_MSB    0x09
#define PRESSURE_LSB  0x0A
#define PRESSURE_MSB  0x0B

// Masks for Controls
#define M_TIP_SWITCH  0x01
#define M_IN_RANGE    0x02
#define M_CONFIDENCE  0x04

// Where to start the touch point
#define X_START       0x0000
#define Y_START       0x0000

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
// Function Prototypes
//-----------------------------------------------------------------------------

U8 CreateSingleTouchBuffer (void);
void CreateMultiTouchBuffer (void);

U8 DrawDiagonal (void);

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

void main (void)
{
   UU32 SPI_clock;

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

   // The SPI Interface is not used in this example, but one of the three
   // interface initialization functions must be called for the CP2501
   // to operate correctly
   SPI_clock.U32 = SPI_CLOCK;
   CP250x_SPI_Master_Init (SPI_clock, PH1_POLLO);    // Initialize SPI
   if (CP250x_Status != FUNC_SUCCESS)
   {
     // !Handle SPI initialization error
   }

   CP250x_USB_Init(ENUM_SINGLE_POINT_MODE);    // Initialize USB peripheral

   //--User Initialization-----------------------------------------------------

   CP250x_Stop_Counter ();
   CP250x_Counter = 0; 
   CP250x_Start_Counter ();

   // --Main Loop--------------------------------------------------------------

   while (1)
   {

      // If operating system supports only one touch point
      if (CP250x_SingleTouch_Mode)
      {
         // Check if buffer data has been sent
         if (CP250x_USB_Touch_Send_Pending == 0)
         {
            if (CP250x_Counter >= UPDATE_RATE)
            {
               CP250x_Counter = 0;

               // Fill USB Position Buffer with artificial touch points
               if (CreateSingleTouchBuffer())
               {
                  // Let <System FW> know data is ready
                  CP250x_USB_Touch_Send_Pending = 1;
               }
            }
         }
      }

      // If operating system supports multiple touch points
      if (CP250x_MultiTouch_Mode)
      {
         // Check if buffer data has been sent
         if (CP250x_USB_Touch_Send_Pending == 0)
         {
            if (CP250x_Counter >= UPDATE_RATE)
            {
               CP250x_Counter = 0;

               // Fill USB Position Buffer with artificial touch points
               if (CreateSingleTouchBuffer())
               {
                  // Let <System FW> know data is ready
                  CP250x_USB_Touch_Send_Pending = 1;
               }
            }
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
// CreateSingleTouchBuffer ()
//-----------------------------------------------------------------------------
//
// Simulates a touch point that draws a diagonal centered around the midpoint
// of the screen.  The touch points draws the diagonal and then lifts for
// 1 second and repeats the process
//
//
// The function runs through the following state machine:
// 1) No touch
// 2) In Range
// 3) Tip Switch
// 4) Touching (stays in this state while drawing rectangle)
// 5) Release
//
// 
U8 CreateSingleTouchBuffer()
{
   static U8  touch_state = ST_NO_TOUCH;    
   static U16 counter     = 0;

   U8 return_val = 1;     // Yes, send the data

   // Only one touch point, so ID is always 0x01
   USB_Position_Buffer[CONTACT_ID] = 0x01;

   if (touch_state == ST_NO_TOUCH)
   {
      return_val = 0;

      counter++;
      if (counter == 50)         // One second
      {
         counter = 0;
         touch_state = ST_IN_RANGE;   // next state
      }
   }

   else if (touch_state == ST_IN_RANGE)
   {
      // In this state, set the Controls to indicate no touching
      USB_Position_Buffer[CONTROLS] = M_IN_RANGE | M_CONFIDENCE;

      // Set X coordinate to 100 pixels left of the center
      USB_Position_Buffer[X_LSB] = X_START & 0x00FF;
      USB_Position_Buffer[X_MSB] = X_START  >> 8;

      // Set Y coordinate to 100 pixels above of the center
      USB_Position_Buffer[Y_LSB] = Y_START & 0x00FF;
      USB_Position_Buffer[Y_MSB] = Y_START  >> 8;

      // Only need to send this packet once, so switch states
      touch_state = ST_TIP_SWITCH;     // next state
   }

   else if (touch_state == ST_TIP_SWITCH)
   {
      // Send the same X-Y coordinate, but with TIP_SWITCH also set
      USB_Position_Buffer[CONTROLS] = M_TIP_SWITCH | M_IN_RANGE | M_CONFIDENCE;

      touch_state = ST_TOUCH;         // next state
   }

   else if (touch_state == ST_TOUCH)  // Change the X-Y coordianate
   {
      if (DrawDiagonal() == 0x00)  // 0 if no more to draw
      {
         touch_state = ST_RELEASE;    // next state
      }      
   }

   else if (touch_state == ST_RELEASE)
   {
      // Send the same last point, but with TIP_SWITCH removed
      USB_Position_Buffer[CONTROLS] = M_IN_RANGE | M_CONFIDENCE;

      counter = 0;                 // set for use in next state
      touch_state = ST_NO_TOUCH;      // next state
   }

   return return_val;
}

//-----------------------------------------------------------------------------
// DrawDiagonal ()
//-----------------------------------------------------------------------------
//
// Simulates a touch point that draws a diagonal centered around the midpoint
// of the screen.  The touch points draws the diagonal and then lifts for
// 1 second and repeats the process
//
// Return 1 if there is more of the diagonal to draw, else return 0
//
U8 DrawDiagonal (void)
{
   UU16 coord_temp;

   coord_temp.U8[MSB] = USB_Position_Buffer[X_MSB];
   coord_temp.U8[LSB] = USB_Position_Buffer[X_LSB];
   coord_temp.U16 += 10;
   USB_Position_Buffer[X_MSB] = coord_temp.U8[MSB];
   USB_Position_Buffer[X_LSB] = coord_temp.U8[LSB];

   coord_temp.U8[MSB] = USB_Position_Buffer[Y_MSB];
   coord_temp.U8[LSB] = USB_Position_Buffer[Y_LSB];
   coord_temp.U16 += 10;
   USB_Position_Buffer[Y_MSB] = coord_temp.U8[MSB];
   USB_Position_Buffer[Y_LSB] = coord_temp.U8[LSB];

   if (coord_temp.U16 > (Y_START+600)) {
      return 0; }
   else {
      return 1; }
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------