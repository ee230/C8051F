/////////////////////////////////////////////////////////////////////////////
// HIDtoUARTDlg.h : header file
/////////////////////////////////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "SLABHIDDevice.h"
#include "afxwin.h"
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////
// Definitions
/////////////////////////////////////////////////////////////////////////////

// USB Parameters
#define VID								0x10C4
#define PID								0x8468
#define HID_READ_TIMEOUT				0
#define HID_WRITE_TIMEOUT				1000

// UART Parameters
#define UART_MIN_BAUD_RATE				1200
#define UART_MAX_BAUD_RATE				230600

// HID Report IDs
#define ID_IN_CONTROL					0xFE
#define ID_OUT_CONTROL					0xFD
#define ID_IN_DATA						0x01
#define ID_OUT_DATA						0x02

// HID Report Sizes
#define SIZE_IN_CONTROL					5
#define SIZE_OUT_CONTROL				5
#define SIZE_IN_DATA					61
#define SIZE_OUT_DATA					61
#define SIZE_MAX_WRITE					59
#define SIZE_MAX_READ					59

// Read Timer Definitions
#define TIMER_READ_ID					0
#define TIMER_READ_ELAPSE				15

/////////////////////////////////////////////////////////////////////////////
// CHIDtoUARTDlg dialog
/////////////////////////////////////////////////////////////////////////////

class CHIDtoUARTDlg : public CDialog
{
// Construction
public:
	CHIDtoUARTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HIDTOUART_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Protected Members
protected:
	HICON		m_hIcon;
	HICON		m_hSmallIcon;
	HID_DEVICE	m_hid;
	HDEVNOTIFY	m_hNotifyDevNode;

// Protected Methods
protected:
	void InitializeDialog();

	void RegisterDeviceChange();
	void UnregisterDeviceChange();

	BOOL Connect();
	BOOL Disconnect();
	BOOL GetSelectedDevice(CString& path);
	BOOL FindDevice(CString path, DWORD& deviceNum);

	void UpdateDeviceList();

	void EnableDeviceCtrls(BOOL bEnable);

	void StartReadTimer();
	void StopReadTimer();

	void AppendReceiveText(const CString& text);

// HID Report Methods
protected:
	BOOL SetBaudRate(DWORD baudRate);
	BOOL GetBaudRate(DWORD& baudRate);
	BOOL TransmitData(const BYTE* buffer, DWORD bufferSize);
	BOOL ReceiveData(BYTE* buffer, DWORD bufferSize, DWORD& bytesRead);

// Generated message map functions
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg void OnBnClickedCheckConnect();
	afx_msg void OnCbnDropdownComboDeviceList();
	afx_msg void OnCbnCloseupComboDeviceList();
	afx_msg void OnBnClickedButtonGetBaudRate();
	afx_msg void OnBnClickedButtonSetBaudRate();
	afx_msg void OnBnClickedButtonTransmit();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

// Control Variables
public:
	CComboBox		m_comboDeviceList;
	DWORD			m_baudRate;
	CRichEditCtrl	m_richReceive;
};
