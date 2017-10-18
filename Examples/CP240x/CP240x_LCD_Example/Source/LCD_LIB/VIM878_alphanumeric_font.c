//-----------------------------------------------------------------------------
// VIM878_alphanumeric_font.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// This file implements an alphanumeric font on the VIM-878 LCD
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
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>             // compiler declarations
#include <C8051F930_defs.h>
#include "lcd_lib.h"

#if (LCD == VIM878)
//-----------------------------------------------------------------------------
// Segment constants for the VIM-878 LCD
//-----------------------------------------------------------------------------
//
// This section changes if the mapping between LCD segment/driver pins changes.
//
// LCD Segment Constants (Bit Mapping)
// I J K L    H G N M    CA F E D    A B C DP

#define A_seg 0x0008
#define B_seg 0x0004
#define C_seg 0x0002
#define D_seg 0x0010
#define E_seg 0x0020
#define F_seg 0x0040
#define G_seg 0x0400
#define H_seg 0x0800
#define I_seg 0x8000
#define J_seg 0x4000
#define K_seg 0x2000
#define L_seg 0x1000
#define M_seg 0x0100
#define N_seg 0x0200
#define DP_seg 0x0001
#define CA_seg 0x0080
#define CL_seg	0x0000


//-----------------------------------------------------------------------------
// Font Arrays for the VIM-878
//-----------------------------------------------------------------------------
//
// This section should only be changed when the font needs to change. It will 
// most likely always remain constant as long as a VIM-878 is used.

code U16 ALPHA_ARRAY[] = 
{	
/*A*/	A_seg + B_seg + C_seg + E_seg + F_seg + G_seg + K_seg,   
/*b*/	C_seg + D_seg + E_seg + F_seg + G_seg + K_seg,			
/*C*/	A_seg + F_seg + E_seg + D_seg, 
/*d*/	B_seg + C_seg + D_seg + E_seg + G_seg + K_seg,
/*E*/	A_seg + D_seg + E_seg + F_seg + G_seg,
/*F*/	A_seg + E_seg + F_seg + G_seg,
/*G*/	A_seg + C_seg + D_seg + E_seg + F_seg + K_seg,
/*H*/	B_seg + C_seg + E_seg + F_seg + G_seg + K_seg,
/*I*/	A_seg + D_seg + I_seg + M_seg,
/*J*/	B_seg + C_seg + D_seg + E_seg,
/*K*/	E_seg + F_seg + G_seg + J_seg + L_seg,
/*L*/	D_seg + E_seg + F_seg,
/*m*/	C_seg + E_seg + G_seg + K_seg + M_seg,
/*N*/	B_seg + C_seg + E_seg + F_seg + H_seg + L_seg,
/*o*/	C_seg + D_seg + E_seg + G_seg + K_seg,
/*P*/	A_seg + B_seg + E_seg + F_seg + G_seg + K_seg,
/*Q*/	A_seg + B_seg + C_seg + D_seg + E_seg + F_seg + L_seg,
/*R*/	A_seg + B_seg + E_seg + F_seg + G_seg + K_seg + L_seg,
/*S*/	A_seg + C_seg + D_seg + F_seg + G_seg + K_seg,
/*T*/	A_seg + I_seg + M_seg,
/*U*/	B_seg + C_seg + D_seg + E_seg + F_seg,
/*V*/	B_seg + C_seg + H_seg + L_seg,
/*W*/	B_seg + C_seg + D_seg + E_seg + F_seg + M_seg,
/*X*/	H_seg + J_seg + L_seg + N_seg,
/*Y*/	H_seg + J_seg + M_seg,
/*Z*/	A_seg + D_seg + J_seg + N_seg,
};


code U16 NUMBER_ARRAY[] = 
{
/*0*/	A_seg + B_seg + C_seg + D_seg + E_seg + F_seg,   
/*1*/	B_seg + C_seg,				
/*2*/	A_seg + B_seg + D_seg + E_seg + G_seg + K_seg,
/*3*/	A_seg + B_seg + C_seg + D_seg + K_seg,
/*4*/	B_seg + C_seg + F_seg + G_seg + K_seg,
/*5*/	A_seg + C_seg + D_seg + F_seg + G_seg + K_seg,
/*6*/	A_seg + C_seg + D_seg + E_seg + F_seg + G_seg + K_seg,
/*7*/	A_seg + B_seg + C_seg,
/*8*/	A_seg + B_seg + C_seg + D_seg + E_seg + F_seg + G_seg + K_seg,
/*9*/	A_seg + B_seg + C_seg + D_seg + F_seg + G_seg + K_seg
};


//-----------------------------------------------------------------------------
// LCD Specific Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// LCD_OutChar
//-----------------------------------------------------------------------------
//
// Parameter: none
// Return Value: ASCII Character
//
// Prints the character to the LCD
//
void LCD_OutChar(U8 position, U8 dat)
{
	U16 seg_char;
	if(dat >= 'A' && dat <= 'Z')
	{
		seg_char = ALPHA_ARRAY[dat - 'A'];
	}
	else if(dat >= 'a' && dat <= 'z')
	{
		seg_char = ALPHA_ARRAY[dat - 'a'];
	}
	else if(dat >= '0' && dat <= '9')
	{
		seg_char = NUMBER_ARRAY[dat - '0'];
	}
	else
	{	
      seg_char = 0;	
   }

	CP240x_RegWrite(ULPMEM00+1 + (position*2), seg_char >> 8);
	CP240x_RegWrite(ULPMEM00 + position*2, seg_char & 0xFF);
}
#endif