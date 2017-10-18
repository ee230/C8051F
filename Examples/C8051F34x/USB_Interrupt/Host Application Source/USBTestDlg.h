// USBTestDlg.h : header file
//

#if !defined(AFX_USBTESTDLG_H__A6A66517_BBBE_4B75_BA11_67CFC720468B__INCLUDED_)
#define AFX_USBTESTDLG_H__A6A66517_BBBE_4B75_BA11_67CFC720468B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Custom Controls
#include "DynamicLED.h"
#include "3DMeterCtrl.h"

//
// buffer to transmit/receive USB data
//
struct USB_iobuf {     
  unsigned char led1, led2;
  unsigned char	port;
  unsigned char analog1, analog2;
  unsigned char not_used[59];
};

/////////////////////////////////////////////////////////////////////////////
// CUSBTestDlg dialog

class CUSBTestDlg : public CDialog
{
// Construction
public:
	CUSBTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUSBTestDlg)
	enum { IDD = IDD_USBTEST_DIALOG };
	C3DMeterCtrl	m_Analog_Meter1;
	C3DMeterCtrl	m_Analog_Meter2;
	CDynamicLED		m_ButtonLED1;
	CDynamicLED		m_ButtonLED2;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
	HICON m_hIcon;
	HANDLE hUSBRead;
	HANDLE hUSBWrite;

	BOOL m_bReadError, m_bWriteError;

	// USB data buffer
	USB_iobuf io_buffer;

	// USB I/F class
	CUsbIF	UsbIF;
	void CloseConnection(); 

	// Generated message map functions
	//{{AFX_MSG(CUSBTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOKExit();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBTESTDLG_H__A6A66517_BBBE_4B75_BA11_67CFC720468B__INCLUDED_)
