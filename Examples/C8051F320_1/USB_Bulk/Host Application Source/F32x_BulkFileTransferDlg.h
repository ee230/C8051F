// F32x_BulkFileTransferDlg.h : header file
//

#if !defined(AFX_F32X_BULKFILETRANSFERDLG_H__03E03B93_6F73_420A_8D62_F36E2D7D75A7__INCLUDED_)
#define AFX_F32X_BULKFILETRANSFERDLG_H__03E03B93_6F73_420A_8D62_F36E2D7D75A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define MAX_PACKET_SIZE_READ		64
#define MAX_PACKET_SIZE_READ		(64 *1024 )
//#define MAX_PACKET_SIZE_WRITE		64
#define MAX_PACKET_SIZE_WRITE		512

//#define MAX_WRITE_PKTS		0x08
#define MAX_WRITE_PKTS		0x01

#define FT_READ_MSG			0x00
#define FT_WRITE_MSG		0x01
#define FT_READ_ACK			0x02

#define FT_MSG_SIZE			0x03

/////////////////////////////////////////////////////////////////////////////
// CF32x_BulkFileTransferDlg dialog

class CF32x_BulkFileTransferDlg : public CDialog
{
// Construction
public:
	CF32x_BulkFileTransferDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CF32x_BulkFileTransferDlg)
	enum { IDD = IDD_F32X_BULKFILETRANSFER_DIALOG };
	CString	m_sTXFileName;
	CString	m_sRXFileName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CF32x_BulkFileTransferDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CF32x_BulkFileTransferDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTransferData();
	afx_msg void OnReceiveData();
	afx_msg void OnBrowseTxFile();
	afx_msg void OnBrowseRxFile();
	virtual void OnOK();
	afx_msg void OnUpdateDeviceList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void FillDeviceList();
	BOOL WriteFileData();
	BOOL ReadFileData();
	BOOL DeviceRead(BYTE* buffer, DWORD dwSize, DWORD* lpdwBytesRead, DWORD dwTimeout = 0);
	BOOL DeviceWrite(BYTE* buffer, DWORD dwSize, DWORD* lpdwBytesWritten, DWORD dwTimeout = 0);

private:
	HANDLE m_hUSBDevice;
	HANDLE m_hUSBWrite;
	HANDLE m_hUSBRead;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_F32X_BULKFILETRANSFERDLG_H__03E03B93_6F73_420A_8D62_F36E2D7D75A7__INCLUDED_)
