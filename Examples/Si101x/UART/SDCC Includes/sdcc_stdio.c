//-----------------------------------------------------------------------------
// sdcc_stdio.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This set of functions includes putchar (), getchar (), and gets ()
// functionality for the SDCC compiler.
//
// FID:
// Target:         C8051F33x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (BW)
//    -17 OCT 2006
//
//-----------------------------------------------------------------------------

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//-----------------------------------------------------------------------------
// GETS
//-----------------------------------------------------------------------------
//
// Return Value : Pointer to string containing buffer read from UART.
// Parameters   : <buf> to store string; <len> maximum number of characters to
//                read; <len> must be 2 or greater.
//
// This function returns a string of maximum length <n>.
//-----------------------------------------------------------------------------
char * GETS (char *buf, unsigned int len)
{
   unsigned char temp;
   unsigned char i;
   unsigned char done;

   done = FALSE;
   i = 0;

   while (done == FALSE)
   {
      temp = getchar ();

      if (temp == '\b')
      {
         if (i != 0)                   // backspace if possible
         {
            i = i - 1;
            putchar ('\b');
         }
      }
      else
      if (temp == '\r')                // handle newline
      {
         buf[i] = '\0';                // add null terminator to string
         done = TRUE;
      }
      else
      if (temp == '\0')                // handle EOF
      {
         buf[i] = '\0';                // add null terminator to string
      }
      else                             // handle new character
      {
         buf[i] = temp;
         putchar (temp);               // echo character
         i = i + 1;
         if (i == (len-1))
         {
            buf[i] = '\0';
            done = TRUE;
          }
       }
   }

   return buf;

}

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : character to send to UART
//
// This function outputs a character to the UART.
//-----------------------------------------------------------------------------
void putchar (char c)
{
   if (c == '\n')
   {
      while (!TI0);
      TI0 = 0;
      SBUF0 = 0x0d;
   }
   while (!TI0);
   TI0 = 0;
   SBUF0 = c;
}

//-----------------------------------------------------------------------------
// getchar
//-----------------------------------------------------------------------------
//
// Return Value : character received from UART
// Parameters   : None
//
// This function returns a character from the UART.
//-----------------------------------------------------------------------------
char getchar (void)
{
   char c;

   while (!RI0);
   c = SBUF0;
   RI0 = 0;

   return c;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------