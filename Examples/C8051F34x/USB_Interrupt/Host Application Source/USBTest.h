// USBTest.h : main header file for the USBTEST application
//

#if !defined(AFX_USBTEST_H__103A12E9_207D_4B90_8502_FDB8588AE07A__INCLUDED_)
#define AFX_USBTEST_H__103A12E9_207D_4B90_8502_FDB8588AE07A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CUSBTestApp:
// See USBTest.cpp for the implementation of this class
//

class CUSBTestApp : public CWinApp
{
public:
	CUSBTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUSBTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBTEST_H__103A12E9_207D_4B90_8502_FDB8588AE07A__INCLUDED_)
