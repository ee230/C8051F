// F32x_BulkFileTransfer.h : main header file for the F32X_BULKFILETRANSFER application
//

#if !defined(AFX_F32X_BULKFILETRANSFER_H__BD3D2F8A_C144_46CC_AA88_AB03FD20313E__INCLUDED_)
#define AFX_F32X_BULKFILETRANSFER_H__BD3D2F8A_C144_46CC_AA88_AB03FD20313E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CF32x_BulkFileTransferApp:
// See F32x_BulkFileTransfer.cpp for the implementation of this class
//

class CF32x_BulkFileTransferApp : public CWinApp
{
public:
	CF32x_BulkFileTransferApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CF32x_BulkFileTransferApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CF32x_BulkFileTransferApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_F32X_BULKFILETRANSFER_H__BD3D2F8A_C144_46CC_AA88_AB03FD20313E__INCLUDED_)
