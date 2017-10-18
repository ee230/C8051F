//-----------------------------------------------------------------------------
// Imeasure.a51
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This routine inserts an infinite loop that may be used to measure current.
// The loop has been located within a 64-byte boundary to represent average
// supply current.
// 
// Note: This assembly file is only compatible with Keil.
//
// Target:         Si10xx
// Tool chain:     Keil C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (FB)
//    -27 APR 2010
//
$include (Imeasure.h)


//-----------------------------------------------------------------------------
// Start of Module
//-----------------------------------------------------------------------------
NAME	IMEASURE                          


//-----------------------------------------------------------------------------
// Imeasure_Infinite_Loop
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Infinite Loop used for measuring current.
//
//-----------------------------------------------------------------------------
   CSEG AT IMEASURE_FUNCTION_LOCATION
   

	SJMP 	$
   RET   


//-----------------------------------------------------------------------------
// End of Module
//-----------------------------------------------------------------------------
END                                    //End of Module

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
