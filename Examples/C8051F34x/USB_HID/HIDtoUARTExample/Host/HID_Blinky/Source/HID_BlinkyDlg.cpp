/////////////////////////////////////////////////////////////////////////////
// HID_BlinkyDlg.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HID_Blinky.h"
#include "HID_BlinkyDlg.h"
#include <dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// Library Dependencies
/////////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "SLABHIDDevice.lib")

/////////////////////////////////////////////////////////////////////////////
// Static Global Variables
/////////////////////////////////////////////////////////////////////////////

// 50% Duty Cycle Pattern
static const BYTE PATTERN_50_DUTY_CYCLE[PATTERN_MAX_BYTES] = {
	0x00,
	0x03,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN
};

// Alternating LEDs Pattern
static const BYTE PATTERN_ALTERNATING_LEDS[PATTERN_MAX_BYTES] = {
	0x01,
	0x02,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN
};

// All Permutations Pattern
static const BYTE PATTERN_ALL_PERMUTATIONS[PATTERN_MAX_BYTES] = {
	0x00,
	0x01,
	0x02,
	0x03,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN,
	PATTERN_END_PATTERN
};

// Absolute Craziness Pattern
static const BYTE PATTERN_ABSOLUTE_CRAZINESS[PATTERN_MAX_BYTES] = {
	0x02,
	0x02,
	0x03,
	0x01,
	0x03,
	0x00,
	0x03,
	0x01
};

// Pulse Pattern
static const BYTE PATTERN_PULSE[PATTERN_MAX_BYTES] = {
	0x00,
	0x00,
	0x00,
	0x03,
	0x00,
	0x03,
	0x00,
	0x00
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
/////////////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	BYTE major;
	BYTE minor;
	BOOL release;

	if (HidDevice_GetHidLibraryVersion(&major, &minor, &release) == HID_DEVICE_SUCCESS)
	{
		CString version;
		version.Format(_T("%u.%u%s"), major, minor, (release) ? _T("") : _T(" (Debug)"));

		SetDlgItemText(IDC_STATIC_LIBRARY, version);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHID_BlinkyDlg dialog
/////////////////////////////////////////////////////////////////////////////

CHID_BlinkyDlg::CHID_BlinkyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHID_BlinkyDlg::IDD, pParent)
	, m_hid(NULL)
	, m_numPatternStates(7)
	, m_blinkRate(10)
	, m_led1Active(0)
	, m_led2Active(0)
	, m_patternSel(0)
{
	m_hIcon			= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hSmallIcon	= (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0);

	// Initialize default custom blink pattern

	m_led1State[0] = TRUE;
	m_led1State[1] = TRUE;
	m_led1State[2] = TRUE;
	m_led1State[3] = TRUE;
	m_led1State[4] = FALSE;
	m_led1State[5] = FALSE;
	m_led1State[6] = FALSE;
	m_led1State[7] = FALSE;

	m_led2State[0] = TRUE;
	m_led2State[1] = TRUE;
	m_led2State[2] = TRUE;
	m_led2State[3] = TRUE;
	m_led2State[4] = FALSE;
	m_led2State[5] = FALSE;
	m_led2State[6] = FALSE;
	m_led2State[7] = FALSE;
}

void CHID_BlinkyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE_LIST, m_comboDeviceList);
	DDX_CBIndex(pDX, IDC_COMBO_NUM_STATES, m_numPatternStates);
	DDX_Text(pDX, IDC_EDIT_RATE, m_blinkRate);
	DDV_MinMaxInt(pDX, m_blinkRate, 1, 200);
	DDX_Text(pDX, IDC_EDIT_LED1_ACTIVE, m_led1Active);
	DDX_Text(pDX, IDC_EDIT_LED2_ACTIVE, m_led2Active);
	DDX_Check(pDX, IDC_CHECK_LED1_S0, m_led1State[0]);
	DDX_Check(pDX, IDC_CHECK_LED1_S1, m_led1State[1]);
	DDX_Check(pDX, IDC_CHECK_LED1_S2, m_led1State[2]);
	DDX_Check(pDX, IDC_CHECK_LED1_S3, m_led1State[3]);
	DDX_Check(pDX, IDC_CHECK_LED1_S4, m_led1State[4]);
	DDX_Check(pDX, IDC_CHECK_LED1_S5, m_led1State[5]);
	DDX_Check(pDX, IDC_CHECK_LED1_S6, m_led1State[6]);
	DDX_Check(pDX, IDC_CHECK_LED1_S7, m_led1State[7]);
	DDX_Check(pDX, IDC_CHECK_LED2_S0, m_led2State[0]);
	DDX_Check(pDX, IDC_CHECK_LED2_S1, m_led2State[1]);
	DDX_Check(pDX, IDC_CHECK_LED2_S2, m_led2State[2]);
	DDX_Check(pDX, IDC_CHECK_LED2_S3, m_led2State[3]);
	DDX_Check(pDX, IDC_CHECK_LED2_S4, m_led2State[4]);
	DDX_Check(pDX, IDC_CHECK_LED2_S5, m_led2State[5]);
	DDX_Check(pDX, IDC_CHECK_LED2_S6, m_led2State[6]);
	DDX_Check(pDX, IDC_CHECK_LED2_S7, m_led2State[7]);
	DDX_Radio(pDX, IDC_RADIO_PATTERN_CUSTOM, m_patternSel);
	DDX_Control(pDX, IDC_COMBO_NUM_STATES, m_comboNumPatternStates);
	DDX_Control(pDX, IDC_SLIDER_DIMMER, m_sliderDimmer);
}

BEGIN_MESSAGE_MAP(CHID_BlinkyDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DEVICECHANGE()
	ON_CBN_DROPDOWN(IDC_COMBO_DEVICE_LIST, &CHID_BlinkyDlg::OnCbnDropdownComboDeviceList)
	ON_CBN_CLOSEUP(IDC_COMBO_DEVICE_LIST, &CHID_BlinkyDlg::OnCbnCloseupComboDeviceList)
	ON_BN_CLICKED(IDC_CHECK_CONNECT, &CHID_BlinkyDlg::OnBnClickedCheckConnect)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_GET_STATS, &CHID_BlinkyDlg::OnBnClickedButtonGetStats)
	ON_BN_CLICKED(IDC_BUTTON_ENABLE, &CHID_BlinkyDlg::OnBnClickedButtonEnable)
	ON_BN_CLICKED(IDC_BUTTON_DISABLE, &CHID_BlinkyDlg::OnBnClickedButtonDisable)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RADIO_PATTERN_CUSTOM, &CHID_BlinkyDlg::OnBnClickedRadioPatternCustom)
	ON_BN_CLICKED(IDC_RADIO_PATTERN_SELECTOR, &CHID_BlinkyDlg::OnBnClickedRadioPatternSelector)
	ON_CBN_SELCHANGE(IDC_COMBO_NUM_STATES, &CHID_BlinkyDlg::OnCbnSelchangeComboNumStates)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SET_BLINK_PATTERN, &CHID_BlinkyDlg::OnBnClickedButtonSetBlinkPattern)
	ON_BN_CLICKED(IDC_BUTTON_SET_BLINK_RATE, &CHID_BlinkyDlg::OnBnClickedButtonSetBlinkRate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHID_BlinkyDlg message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CHID_BlinkyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hSmallIcon, FALSE);	// Set small icon

	InitializeDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHID_BlinkyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHID_BlinkyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHID_BlinkyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Update the device list when the combobox is dropped down
void CHID_BlinkyDlg::OnCbnDropdownComboDeviceList()
{
	// Automatically update the device list when the
	// list is opened/dropped down
	UpdateDeviceList();
}

// Update the device list if the selected device has been
// removed when the combobox is closed
void CHID_BlinkyDlg::OnCbnCloseupComboDeviceList()
{
	CString		path;
	DWORD		deviceNum;

	if (GetSelectedDevice(path))
	{
		// If the selected device has been removed
		if (!FindDevice(path, deviceNum))
		{
			// Then update the device list
			UpdateDeviceList();		
		}
	}
}

// Toggle connection state
void CHID_BlinkyDlg::OnBnClickedCheckConnect()
{
	// Disconnecting
	if (IsDlgButtonChecked(IDC_CHECK_CONNECT))
	{
		Disconnect();
	}
	// Connecting
	else
	{
		Connect();
	}
}

// Handle device change messages (ie a device is added or removed)
// - If an HID device is connected, then add the device to the device list
// - If the device we were connected to was removed, then disconnect from the device
BOOL CHID_BlinkyDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	// Device has been added or removed
	if (nEventType == DBT_DEVICEREMOVECOMPLETE ||
		nEventType == DBT_DEVICEARRIVAL)
	{
		if (dwData)
		{
			PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)dwData;

			if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
			{
				PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;

				// "\\?\hid#vid_10c4&pid_81ba..."
				CString deviceStr = pDevInf->dbcc_name;

				// If the device was removed
				if (nEventType == DBT_DEVICEREMOVECOMPLETE)
				{
					char devPath[MAX_PATH_LENGTH];

					// Check if our device was removed by device path
					if (HidDevice_GetString(m_hid, HID_PATH_STRING, devPath, MAX_PATH_LENGTH) == HID_DEVICE_SUCCESS)
					{
						// Our device was removed
						if (deviceStr.CompareNoCase(CString(devPath)) == 0)
						{
							// The device handle is stale
							// Disconnect from it
							Disconnect();
						}
					}
				}

				UpdateDeviceList();
			}
		}
	}

	return TRUE;
}

void CHID_BlinkyDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// Stop the pattern selector timer
	KillTimer(SELECTOR_TIMER_ID);

	// Close the device
	if (HidDevice_IsOpened(m_hid))
	{
		HidDevice_Close(m_hid);
		m_hid = NULL;
	}

	// Disable WM_DEVICECHANGE messages
	UnregisterDeviceChange();
}

// Set the blink pattern
void CHID_BlinkyDlg::OnBnClickedButtonSetBlinkPattern()
{
	if (UpdateData(TRUE))
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			// Only apply the custom pattern when selected
			if (IsDlgButtonChecked(IDC_RADIO_PATTERN_CUSTOM))
			{
				BYTE pattern[PATTERN_MAX_BYTES];

				GetCustomPattern(pattern);
				SetBlinkPattern(pattern);
			}
		}
		else
		{
			AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
		}
	}
}

// Set the blink rate
void CHID_BlinkyDlg::OnBnClickedButtonSetBlinkRate()
{
	if (UpdateData(TRUE))
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			SetBlinkRate(m_blinkRate);
		}
		else
		{
			AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
		}
	}
}

// Update LED1 and LED2 active duty cycle percent
void CHID_BlinkyDlg::OnBnClickedButtonGetStats()
{
	if (UpdateData(TRUE))
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			// Retrieve the LED1 and LED2 active duty cycle percentages
			if (GetBlinkStats(m_led1Active, m_led2Active))
			{
				UpdateData(FALSE);
			}
			else
			{
				MessageBox(_T("Failed to retrieve blink statistics"), 0, MB_ICONWARNING);
			}
		}
		else
		{
			AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
		}
	}
}

// Enable the blink pattern
void CHID_BlinkyDlg::OnBnClickedButtonEnable()
{
	if (UpdateData(TRUE))
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			SetBlinkEnable(TRUE);
		}
		else
		{
			AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
		}
	}
}

// Disable the blink pattern
void CHID_BlinkyDlg::OnBnClickedButtonDisable()
{
	if (UpdateData(TRUE))
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			if (!SetBlinkEnable(FALSE))
			{
				MessageBox(_T("Failed to disable blink pattern"), 0, MB_ICONWARNING);
			}
		}
		else
		{
			AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
		}
	}
}

// Handle dimmer slider scroll
// Send the blink dimmer value to the device
void CHID_BlinkyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// Handle horizontal scroll messages when the slider
	// is being adjusted and when the user stop scrolling the slider
	if (nSBCode == SB_ENDSCROLL || nSBCode == SB_THUMBTRACK)
	{
		// Make sure that the scroll bar is the dimmer slider
		if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_DIMMER)
		{
			// Get the dimmer percent value
			int dimmerPercent = m_sliderDimmer.GetPos();

			// Make sure that we are connected to a device
			if (HidDevice_IsOpened(m_hid))
			{
				// Set the blink dimmer value
				if (!SetBlinkDimmer(dimmerPercent))
				{
					// Silently ignore any errors
					TRACE(_T("SetBlinkDimmer(): Failed\r\n"));
				}
			}
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

// Show the custom pattern controls when
// the custom pattern radio button is selected
void CHID_BlinkyDlg::OnBnClickedRadioPatternCustom()
{
	EnableCustomPatternCtrls(TRUE);

	if (UpdateData(TRUE))
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			BYTE pattern[PATTERN_MAX_BYTES];

			GetCustomPattern(pattern);
			SetBlinkPattern(pattern);
		}
		else
		{
			AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
		}
	}
}

// Hide the custom pattern controls
// Show which selector pattern is being used
void CHID_BlinkyDlg::OnBnClickedRadioPatternSelector()
{
	// Hide the custom pattern controls when
	// the potentiometer pattern radio button is selected
	EnableCustomPatternCtrls(FALSE);

	if (UpdateData(TRUE))
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			PatternSelect select;

			// The device won't send a Get Blink Selector report
			// unless the potentiometer value has changed
			//
			// Since we don't know what the selector pattern
			// is, we send a request over the control endpoint
			if (GetBlinkSelector_Control(select))
			{
				// Send the appropriate selector pattern
				// to the device based on the pot value
				SetSelectorPattern(select);
			}
		}
		else
		{
			AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
		}
	}
}

// Show only the specified number of pattern states
// checkboxes for LED1/LED2
void CHID_BlinkyDlg::OnCbnSelchangeComboNumStates()
{
	int sel = m_comboNumPatternStates.GetCurSel();

	if (sel != CB_ERR)
	{
		ShowPatternStates(sel + 1);
	}
}

void CHID_BlinkyDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Periodically check for Get Blink Selector
	// input reports over the interrupt endpoint
	if (nIDEvent == SELECTOR_TIMER_ID)
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			PatternSelect select;

			// Get the most recent Get Blink Selector report
			if (GetBlinkSelector_Interrupt(select))
			{
				// Send the appropriate selector pattern
				// to the device based on the pot value
				SetSelectorPattern(select);
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CHID_BlinkyDlg Class - Protected Methods
/////////////////////////////////////////////////////////////////////////////

void CHID_BlinkyDlg::InitializeDialog()
{
	// Set slider range
	InitDimmerSlider();

	// Initially device controls are disabled
	// until connected to a device
	EnableDeviceCtrls(FALSE);

	// Populate the device combobox list
	UpdateDeviceList();

	// Register for WM_DEVICECHANGE messages
	// whenever an HID device is attached or removed
	RegisterDeviceChange();

	// Start a timer to monitor for Get Blink Selector input reports
	InitSelectorTimer();
}

// Set the dimmer slider range and default position
void CHID_BlinkyDlg::InitDimmerSlider()
{
	m_sliderDimmer.SetRange(0, 100);
	m_sliderDimmer.SetPos(100);
}

// Start a timer to periodically check for Get Blink Selector
// input reports over the interrupt endpoint
void CHID_BlinkyDlg::InitSelectorTimer()
{
	// Trigger the timer every 15 ms
	SetTimer(SELECTOR_TIMER_ID, SELECTOR_TIMER_ELAPSE, NULL);
}

// Register for device change notification for USB HID devices
// OnDeviceChange() will handle device arrival and removal
void CHID_BlinkyDlg::RegisterDeviceChange()
{
	DEV_BROADCAST_DEVICEINTERFACE devIF = {0};

	devIF.dbcc_size			= sizeof(devIF);    
	devIF.dbcc_devicetype	= DBT_DEVTYP_DEVICEINTERFACE;    
	
	HidDevice_GetHidGuid(&devIF.dbcc_classguid);
	
	m_hNotifyDevNode = RegisterDeviceNotification(GetSafeHwnd(), &devIF, DEVICE_NOTIFY_WINDOW_HANDLE);
}

// Unregister for device change notification for USB HID devices
void CHID_BlinkyDlg::UnregisterDeviceChange()
{
	if (m_hNotifyDevNode)
	{
		UnregisterDeviceNotification(m_hNotifyDevNode);
		m_hNotifyDevNode = NULL;
	}
}

// Connect to the device with the path string selected
// in the device list
// - Connect to the device specified in the device list
// - Set Connect checkbox/button caption and pressed state
// - Enable/disable device combobox
BOOL CHID_BlinkyDlg::Connect()
{
	BOOL		connected = FALSE;
	CString		path;
	DWORD		deviceNum;

	// Get selected device path string
	if (GetSelectedDevice(path))
	{
		// Find the selected device number
		if (FindDevice(path, deviceNum))
		{
			BYTE status = HidDevice_Open(&m_hid, deviceNum, VID, PID, MAX_REPORT_REQUEST_XP);

			// Attempt to open the device
			if (status == HID_DEVICE_SUCCESS)
			{
				connected = TRUE;
			}
			else
			{
				CString msg;
				msg.Format(_T("Failed to connect to %s."), path);
				MessageBox(msg, 0, MB_ICONWARNING);
			}
		}
	}

	// Connected
	if (connected)
	{
		// Set read/write timeouts
		// Read timeouts should be set very low since we are periodically
		// reading for input reports over the interrupt endpoint in the
		// selector timer
		HidDevice_SetTimeouts(m_hid, HID_READ_TIMEOUT, HID_WRITE_TIMEOUT);

		// Check/press the connect button
		CheckDlgButton(IDC_CHECK_CONNECT, TRUE);

		// Update Connect/Disconnect button caption
		SetDlgItemText(IDC_CHECK_CONNECT, _T("&Disconnect"));

		// Disable the device combobox
		m_comboDeviceList.EnableWindow(FALSE);

		// Enable the device controls
		EnableDeviceCtrls(TRUE);
	}
	// Disconnected
	else
	{
		// Uncheck/unpress the connect button
		CheckDlgButton(IDC_CHECK_CONNECT, FALSE);

		// Update Connect/Disconnect button caption
		SetDlgItemText(IDC_CHECK_CONNECT, _T("&Connect"));

		// Enable the device combobox
		m_comboDeviceList.EnableWindow(TRUE);

		// Disable the device controls
		EnableDeviceCtrls(FALSE);
	}

	return connected;
}

// Disconnect from the currently connected device
// - Disconnect from the current device
// - Output any error messages
// - Set Connect checkbox/button caption and pressed state
// - Enable/disable device combobox
BOOL CHID_BlinkyDlg::Disconnect()
{
	BOOL disconnected = FALSE;

	// Disconnect from the current device
	BYTE status = HidDevice_Close(m_hid);
	m_hid = NULL;

	// Output an error message if the close failed
	if (status != HID_DEVICE_SUCCESS)
	{
		CString msg;
		msg.Format(_T("Failed to disconnect."));
		MessageBox(msg, 0, MB_ICONWARNING);
	}
	else
	{
		disconnected = TRUE;
	}

	// Uncheck/unpress the connect button
	CheckDlgButton(IDC_CHECK_CONNECT, FALSE);

	// Update Connect/Disconnect button caption
	SetDlgItemText(IDC_CHECK_CONNECT, _T("&Connect"));

	// Enable the device combobox
	m_comboDeviceList.EnableWindow(TRUE);

	// Disable the device controls
	EnableDeviceCtrls(FALSE);

	return disconnected;
}

// Get the combobox device selection
// If a device is selected, return TRUE and return the path string
// Otherwise, return FALSE
BOOL CHID_BlinkyDlg::GetSelectedDevice(CString& path)
{
	BOOL selected = FALSE;

	int			sel;
	CString		selText;

	// Get current selection index or CB_ERR(-1)
	// if no device is selected
	sel = m_comboDeviceList.GetCurSel();

	if (sel != CB_ERR)
	{
		// Get the selected device string
		m_comboDeviceList.GetLBText(sel, selText);
		selected	= TRUE;
		path		= selText;
	}

	return selected;
}

// Search for an HID device with a matching device path string
// If the device was found return TRUE and return the device number
// in deviceNumber
// Otherwise return FALSE
BOOL CHID_BlinkyDlg::FindDevice(CString path, DWORD& deviceNum)
{
	BOOL found = FALSE;
	char deviceString[MAX_PATH_LENGTH];

	// Iterate through all connected HID devices
	for (DWORD i = 0; i < HidDevice_GetNumHidDevices(VID, PID); i++)
	{
		// Get the device path string
		if (HidDevice_GetHidString(i, VID, PID, HID_PATH_STRING, deviceString, MAX_PATH_LENGTH) == HID_DEVICE_SUCCESS)
		{
			if (path.CompareNoCase(CString(deviceString)) == 0)
			{
				found		= TRUE;
				deviceNum	= i;
				break;
			}
		}
	}

	return found;
}

// Populate the device list combo box with connected device path strings
// - Save previous device path string selection
// - Fill the device list with connected device path strings
// - Restore previous device selection
void CHID_BlinkyDlg::UpdateDeviceList()
{
	// Only update the combo list when the drop down list is closed
	if (!m_comboDeviceList.GetDroppedState())
	{
		int			sel;
		CString		path;
		char		deviceString[MAX_PATH_LENGTH];

		// Get previous combobox string selection
		GetSelectedDevice(path);

		// Remove all strings from the combobox
		m_comboDeviceList.ResetContent();

		// Iterate through each HID device with matching VID/PID
		for (DWORD i = 0; i < HidDevice_GetNumHidDevices(VID, PID); i++)
		{
			// Add path strings to the combobox
			if (HidDevice_GetHidString(i, VID, PID, HID_PATH_STRING, deviceString, MAX_PATH_LENGTH) == HID_DEVICE_SUCCESS)
			{
				m_comboDeviceList.AddString(CString(deviceString));
			}
		}

		sel = m_comboDeviceList.FindStringExact(-1, path);

		// Select first combobox string
		if (sel == CB_ERR)
		{
			m_comboDeviceList.SetCurSel(0);
		}
		// Restore previous combobox selection
		else
		{
			m_comboDeviceList.SetCurSel(sel);
		}
	}
}

// Enable/disable "Enable Blink Pattern", "Disable Blink Pattern",
// and "Get Blink Statistics" buttons and "Blink Dimmer" slider
void CHID_BlinkyDlg::EnableDeviceCtrls(BOOL bEnable)
{
	int nIDs[] = 
	{
		IDC_RADIO_PATTERN_CUSTOM,
		IDC_RADIO_PATTERN_SELECTOR,
		IDC_SLIDER_DIMMER,
		IDC_BUTTON_GET_STATS,
		IDC_BUTTON_SET_BLINK_PATTERN,
		IDC_BUTTON_SET_BLINK_RATE,
		IDC_BUTTON_ENABLE,
		IDC_BUTTON_DISABLE
	};

	for (int i = 0; i < sizeof(nIDs) / sizeof(nIDs[0]); i++)
	{
		GetDlgItem(nIDs[i])->EnableWindow(bEnable);
	}
}

// Enable/disable all Custom Pattern radio button controls
void CHID_BlinkyDlg::EnableCustomPatternCtrls(BOOL bEnable)
{
	int nIDs[] = {
		IDC_COMBO_NUM_STATES,
		IDC_CHECK_LED1_S0,
		IDC_CHECK_LED1_S1,
		IDC_CHECK_LED1_S2,
		IDC_CHECK_LED1_S3,
		IDC_CHECK_LED1_S4,
		IDC_CHECK_LED1_S5,
		IDC_CHECK_LED1_S6,
		IDC_CHECK_LED1_S7,
		IDC_CHECK_LED2_S0,
		IDC_CHECK_LED2_S1,
		IDC_CHECK_LED2_S2,
		IDC_CHECK_LED2_S3,
		IDC_CHECK_LED2_S4,
		IDC_CHECK_LED2_S5,
		IDC_CHECK_LED2_S6,
		IDC_CHECK_LED2_S7
	};

	for (int i = 0; i < sizeof(nIDs) / sizeof(nIDs[0]); i++)
	{
		GetDlgItem(nIDs[i])->EnableWindow(bEnable);
	}
}

// Show only the specified number of pattern states
// checkboxes for LED1/LED2
void CHID_BlinkyDlg::ShowPatternStates(int numStates)
{
	// LED1 pattern state checkbox IDs
	int nLed1IDs[PATTERN_MAX_BYTES] = 
	{
		IDC_CHECK_LED1_S0,
		IDC_CHECK_LED1_S1,
		IDC_CHECK_LED1_S2,
		IDC_CHECK_LED1_S3,
		IDC_CHECK_LED1_S4,
		IDC_CHECK_LED1_S5,
		IDC_CHECK_LED1_S6,
		IDC_CHECK_LED1_S7
	};

	// LED2 pattern state checkbox IDs
	int nLed2IDs[PATTERN_MAX_BYTES] = 
	{
		IDC_CHECK_LED2_S0,
		IDC_CHECK_LED2_S1,
		IDC_CHECK_LED2_S2,
		IDC_CHECK_LED2_S3,
		IDC_CHECK_LED2_S4,
		IDC_CHECK_LED2_S5,
		IDC_CHECK_LED2_S6,
		IDC_CHECK_LED2_S7
	};

	// Pattern state static caption IDs
	int nStaticIDs[PATTERN_MAX_BYTES] = 
	{
		IDC_STATIC_S0,
		IDC_STATIC_S1,
		IDC_STATIC_S2,
		IDC_STATIC_S3,
		IDC_STATIC_S4,
		IDC_STATIC_S5,
		IDC_STATIC_S6,
		IDC_STATIC_S7
	};

	// Show only the specified number of controls
	for (int i = 0; i < PATTERN_MAX_BYTES; i++)
	{
		BOOL show = FALSE;

		if (i < numStates)
		{
			show = TRUE;
		}

		GetDlgItem(nLed1IDs[i])->ShowWindow(show);
		GetDlgItem(nLed2IDs[i])->ShowWindow(show);
		GetDlgItem(nStaticIDs[i])->ShowWindow(show);
	}
}

// Return the pattern based on the custom pattern
// control states
void CHID_BlinkyDlg::GetCustomPattern(BYTE pattern[PATTERN_MAX_BYTES])
{
	int numPatternBytes = m_numPatternStates + 1;

	memset(pattern, 0x00, PATTERN_MAX_BYTES);

	// Iterate through all pattern bytes
	for (int i = 0; i < PATTERN_MAX_BYTES; i++)
	{
		// Only set the specified number of pattern bytes
		if (i < numPatternBytes)
		{
			if (m_led1State[i])
			{
				pattern[i] |= PATTERN_LED1_ON;
			}
			if (m_led2State[i])
			{
				pattern[i] |= PATTERN_LED2_ON;
			}
		}
		// Reached the end of the pattern
		else
		{
			pattern[i] = PATTERN_END_PATTERN;
		}
	}
}

// Set the selector pattern to the specified pattern
void CHID_BlinkyDlg::SetSelectorPattern(PatternSelect select)
{
	// If the potentiometer selector is choosen
	if (IsDlgButtonChecked(IDC_RADIO_PATTERN_SELECTOR))
	{
		// Then set the blink pattern based on the Get Blink Selector report
		switch (select)
		{
		case PS_50_DUTY_CYCLE:
			SetBlinkPattern(PATTERN_50_DUTY_CYCLE);
			CheckRadioButton(IDC_RADIO_SELECTOR_0, IDC_RADIO_SELECTOR_4, IDC_RADIO_SELECTOR_0);
			break;

		case PS_ALTERNATING_LEDS:
			SetBlinkPattern(PATTERN_ALTERNATING_LEDS);
			CheckRadioButton(IDC_RADIO_SELECTOR_0, IDC_RADIO_SELECTOR_4, IDC_RADIO_SELECTOR_1);
			break;

		case PS_ALL_PERMUTATIONS:
			SetBlinkPattern(PATTERN_ALL_PERMUTATIONS);
			CheckRadioButton(IDC_RADIO_SELECTOR_0, IDC_RADIO_SELECTOR_4, IDC_RADIO_SELECTOR_2);
			break;

		case PS_ABSOLUTE_CRAZINESS:
			SetBlinkPattern(PATTERN_ABSOLUTE_CRAZINESS);
			CheckRadioButton(IDC_RADIO_SELECTOR_0, IDC_RADIO_SELECTOR_4, IDC_RADIO_SELECTOR_3);
			break;

		case PS_PULSE:
			SetBlinkPattern(PATTERN_PULSE);
			CheckRadioButton(IDC_RADIO_SELECTOR_0, IDC_RADIO_SELECTOR_4, IDC_RADIO_SELECTOR_4);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHID_BlinkyDlg Class - HID Report Methods
/////////////////////////////////////////////////////////////////////////////

// Set the blink pattern used when blink is enabled
BOOL CHID_BlinkyDlg::SetBlinkPattern(const BYTE pattern[PATTERN_MAX_BYTES])
{
	BOOL	success		= FALSE;
	WORD	reportSize	= HidDevice_GetOutputReportBufferLength(m_hid);
	BYTE*	report		= new BYTE[reportSize];

	report[0] = ID_OUT_BLINK_PATTERN;					// Report ID
	memcpy(&report[1], pattern, PATTERN_MAX_BYTES);		// Blink Pattern

	// Send an output report over the interrupt endpoint
	if (HidDevice_SetOutputReport_Interrupt(m_hid, report, reportSize) == HID_DEVICE_SUCCESS)
	{
		success = TRUE;
	}

	delete [] report;

	return success;
}

// Enable or disable the blink pattern
BOOL CHID_BlinkyDlg::SetBlinkEnable(BOOL enable)
{
	BOOL	success		= FALSE;
	WORD	reportSize	= HidDevice_GetOutputReportBufferLength(m_hid);
	BYTE*	report		= new BYTE[reportSize];
	BYTE	status;

	report[0] = ID_OUT_BLINK_ENABLE;		// Report ID
	report[1] = (enable) ? 0x01 : 0x00;		// 0x00 - Disable blink pattern
											// 0x01 - Enable blink pattern

	// Send an output report over the interrupt endpoint
	status = HidDevice_SetOutputReport_Interrupt(m_hid, report, reportSize);

	if (status == HID_DEVICE_SUCCESS)
	{
		success = TRUE;
	}

	delete [] report;

	return success;
}

// Set pattern blink rate frequency in Hz
// This is the rate at which the device cycles
// between each state in the blink pattern
BOOL CHID_BlinkyDlg::SetBlinkRate(int rateHz)
{
	BOOL	success		= FALSE;
	WORD	reportSize	= HidDevice_GetOutputReportBufferLength(m_hid);
	BYTE*	report		= new BYTE[reportSize];
	WORD	blinkRate;

	// Convert from pattern cycle rate in Hz to
	// blink rate in 0.5 ms units of time
	blinkRate = 2000 / rateHz;

	report[0] = ID_OUT_BLINK_RATE;	// Report ID
	report[1] = HIBYTE(blinkRate);	// Blink rate in 0.5 ms units
	report[2] = LOBYTE(blinkRate);

	// Send an output report over the interrupt endpoint
	if (HidDevice_SetOutputReport_Interrupt(m_hid, report, reportSize) == HID_DEVICE_SUCCESS)
	{
		success = TRUE;
	}

	delete [] report;

	return success;
}

// Set the LED1/LED2 brightness as a percent
BOOL CHID_BlinkyDlg::SetBlinkDimmer(int dimmerPercent)
{
	BOOL	success			= FALSE;
	WORD	reportSize		= HidDevice_GetFeatureReportBufferLength(m_hid);
	BYTE*	report			= new BYTE[reportSize];
	WORD	blinkDimmer;

	// Calculate blinkDimmer value using the formula:
	// Duty_Cycle = (256 - blinkDimmer) / 256
	// where Duty-Cycle is between 0 and 1.
	blinkDimmer = (256 - ((256 * dimmerPercent) / 100));

	// Blink dimmer must be between 0 and 255
	// We can't quite generate 0% duty cycle
	blinkDimmer = min(blinkDimmer, 255);

	report[0] = ID_FEATURE_BLINK_DIMMER;	// Report ID
	report[1] = (BYTE)blinkDimmer;			// Blink Dimmer value

	// Send a feature report over the control endpoint
	if (HidDevice_SetFeatureReport_Control(m_hid, report, reportSize) == HID_DEVICE_SUCCESS)
	{
		// Get a feature report over the control endpoint
		if (HidDevice_GetFeatureReport_Control(m_hid, report, reportSize) == HID_DEVICE_SUCCESS)
		{
			// Check to see if the feature report was processed successfuly
			if (report[1] == 0x01)
			{
				success = TRUE;
			}
		}
	}

	delete [] report;

	return success;
}

// Parse through all received input reports over the interrupt
// endpoint and find the most recent pattern selector report
BOOL CHID_BlinkyDlg::GetBlinkSelector_Interrupt(PatternSelect& select)
{
	BOOL	success				= FALSE;
	WORD	inputReportSize		= HidDevice_GetInputReportBufferLength(m_hid);	// Size of the biggest input interrupt report
	DWORD	numReports			= HidDevice_GetMaxReportRequest(m_hid);			// The maximum number of reports that can be read in a single read
	DWORD	bufferSize			= inputReportSize * numReports;
	BYTE*	buffer				= new BYTE[bufferSize];
	DWORD	bytesReturned		= 0;
	BYTE	status;

	memset(buffer, 0x00, bufferSize);

	// Get the max number of input reports via the interupt endpoint
	status = HidDevice_GetInputReport_Interrupt(m_hid, buffer, bufferSize, numReports, &bytesReturned);

	// If the function times out, we still have to check for returned data
	if (status == HID_DEVICE_SUCCESS || status == HID_DEVICE_TRANSFER_TIMEOUT)
	{
		BYTE blinkSelector;
		BOOL blinkSelectorValid = FALSE;

		// Go through each input report one at a time
		for (DWORD i = 0; i < bytesReturned; i += inputReportSize)
		{
			// If the current input report is a Get Blink Selector report
			if (buffer[i] == ID_IN_BLINK_SELECTOR)
			{
				// Then get the blink selector value
				//
				// Use the last blink selector value because
				// we only want to update the selector
				// radio button once
				blinkSelector		= buffer[i + 1];
				blinkSelectorValid	= TRUE;
			}

			// Note: Ignore any other type of input report
		}

		// If we found a valid Get Blink Selector report
		if (blinkSelectorValid)
		{
			// Return the pattern selector number (0 - 4)
			select	= (PatternSelect)((blinkSelector * PATTERN_NUM_SELECTOR_PATTERNS) / 256);
			success = TRUE;
		}
	}

	delete [] buffer;

	return success;
}

// Get the blink selector over the control endpoint
BOOL CHID_BlinkyDlg::GetBlinkSelector_Control(PatternSelect& select)
{
	BOOL	success		= FALSE;
	WORD	reportSize	= HidDevice_GetInputReportBufferLength(m_hid);
	BYTE*	report		= new BYTE[reportSize];

	report[0] = ID_IN_BLINK_SELECTOR;	// Report ID

	// Get an input report over the control endpoint
	// (Only supported in Windows XP and later)
	if (HidDevice_GetInputReport_Control(m_hid, report, reportSize) == HID_DEVICE_SUCCESS)
	{
		select		= (PatternSelect)((report[1] * PATTERN_NUM_SELECTOR_PATTERNS) / 256);
		success		= TRUE;
	}

	delete [] report;

	return success;
}

// Get the LED1/LED2 active percentages
BOOL CHID_BlinkyDlg::GetBlinkStats(int& led1ActivePercent, int& led2ActivePercent)
{
	BOOL	success		= FALSE;
	WORD	reportSize	= HidDevice_GetInputReportBufferLength(m_hid);
	BYTE*	report		= new BYTE[reportSize];

	memset(report, 0x00, reportSize);

	report[0] = ID_IN_BLINK_STATS;	// Report ID

	// Get an input report over the control endpoint
	// (Only supported in Windows XP and later)
	if (HidDevice_GetInputReport_Control(m_hid, report, reportSize) == HID_DEVICE_SUCCESS)
	{
		led1ActivePercent = report[1];
		led2ActivePercent = report[2];

		success = TRUE;
	}

	delete [] report;

	return success;
}
