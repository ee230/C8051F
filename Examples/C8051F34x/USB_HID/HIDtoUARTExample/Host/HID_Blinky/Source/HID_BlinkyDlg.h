/////////////////////////////////////////////////////////////////////////////
// HID_BlinkyDlg.h : header file
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
#define PID								0x82CD
#define HID_READ_TIMEOUT				0
#define HID_WRITE_TIMEOUT				1000

// Pattern Definitions
#define PATTERN_MAX_BYTES				8
#define	PATTERN_END_PATTERN				0xFF
#define PATTERN_LED1_ON					0x01
#define PATTERN_LED2_ON					0x02
#define PATTERN_NUM_SELECTOR_PATTERNS	5

// HID Report IDs
#define ID_OUT_BLINK_PATTERN			0x01
#define ID_OUT_BLINK_ENABLE				0x02
#define ID_OUT_BLINK_RATE				0x03
#define ID_IN_BLINK_SELECTOR			0x04
#define ID_IN_BLINK_STATS				0x05
#define ID_FEATURE_BLINK_DIMMER			0x06

// Blink Selector Timer Definitions
#define SELECTOR_TIMER_ID				0
#define SELECTOR_TIMER_ELAPSE			15

/////////////////////////////////////////////////////////////////////////////
// Enumerations
/////////////////////////////////////////////////////////////////////////////

enum PatternSelect {
	PS_50_DUTY_CYCLE,
	PS_ALTERNATING_LEDS,
	PS_ALL_PERMUTATIONS,
	PS_ABSOLUTE_CRAZINESS,
	PS_PULSE
};

/////////////////////////////////////////////////////////////////////////////
// CHID_BlinkyDlg dialog
/////////////////////////////////////////////////////////////////////////////

class CHID_BlinkyDlg : public CDialog
{
// Construction
public:
	CHID_BlinkyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HID_BLINKY_DIALOG };

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
	void InitDimmerSlider();
	void InitSelectorTimer();

	void RegisterDeviceChange();
	void UnregisterDeviceChange();

	BOOL Connect();
	BOOL Disconnect();
	BOOL GetSelectedDevice(CString& path);
	BOOL FindDevice(CString path, DWORD& deviceNum);

	void UpdateDeviceList();

	void EnableDeviceCtrls(BOOL bEnable);
	void EnableCustomPatternCtrls(BOOL bEnable);
	void ShowPatternStates(int numStates);

	void GetCustomPattern(BYTE pattern[PATTERN_MAX_BYTES]);
	void SetSelectorPattern(PatternSelect select);

// HID Report Methods
protected:
	BOOL SetBlinkPattern(const BYTE pattern[PATTERN_MAX_BYTES]);
	BOOL SetBlinkEnable(BOOL enable);
	BOOL SetBlinkRate(int rateHz);
	BOOL SetBlinkDimmer(int dimmerPercent);
	BOOL GetBlinkSelector_Interrupt(PatternSelect& select);
	BOOL GetBlinkSelector_Control(PatternSelect& select);
	BOOL GetBlinkStats(int& led1ActivePercent, int& led2ActivePercent);

// Generated message map functions
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCbnDropdownComboDeviceList();
	afx_msg void OnCbnCloseupComboDeviceList();
	afx_msg void OnBnClickedCheckConnect();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonGetStats();
	afx_msg void OnBnClickedButtonSetBlinkPattern();
	afx_msg void OnBnClickedButtonSetBlinkRate();
	afx_msg void OnBnClickedButtonEnable();
	afx_msg void OnBnClickedButtonDisable();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedRadioPatternCustom();
	afx_msg void OnBnClickedRadioPatternSelector();
	afx_msg void OnCbnSelchangeComboNumStates();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

// Control Variables
public:
	CComboBox	m_comboDeviceList;
	int			m_numPatternStates;
	int			m_blinkRate;
	int			m_led1Active;
	int			m_led2Active;
	BOOL		m_led1State[PATTERN_MAX_BYTES];
	BOOL		m_led2State[PATTERN_MAX_BYTES];
	int			m_patternSel;
	CComboBox	m_comboNumPatternStates;
	CSliderCtrl m_sliderDimmer;
};
