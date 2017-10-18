// HID_Blinky.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CHID_BlinkyApp:
// See HID_Blinky.cpp for the implementation of this class
//

class CHID_BlinkyApp : public CWinApp
{
public:
	CHID_BlinkyApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CHID_BlinkyApp theApp;