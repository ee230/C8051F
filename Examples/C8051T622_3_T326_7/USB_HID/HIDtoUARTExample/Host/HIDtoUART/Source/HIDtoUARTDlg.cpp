/////////////////////////////////////////////////////////////////////////////
// HIDtoUARTDlg.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HIDtoUART.h"
#include "HIDtoUARTDlg.h"
#include <dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// Library Dependencies
/////////////////////////////////////////////////////////////////////////////

#pragma comment (lib, "SLABHIDDevice.lib")

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
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	BYTE major;
	BYTE minor;
	BOOL release;

	// Display the SLABHIDDevice.dll version on the about dialog
	if (HidDevice_GetHidLibraryVersion(&major, &minor, &release) == HID_DEVICE_SUCCESS)
	{
		CString version;
		version.Format(_T("%u.%u%s"), major, minor, (release) ? _T("") : _T(" (Debug)"));
		SetDlgItemText(IDC_STATIC_LIBRARY_VERSION, version);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CHIDtoUARTDlg dialog
/////////////////////////////////////////////////////////////////////////////

CHIDtoUARTDlg::CHIDtoUARTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHIDtoUARTDlg::IDD, pParent)
	, m_baudRate(115200)
{
	m_hIcon			= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hSmallIcon	= (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0);
}

void CHIDtoUARTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE_LIST, m_comboDeviceList);
	DDX_Text(pDX, IDC_EDIT_BAUD_RATE, m_baudRate);
	DDV_MinMaxUInt(pDX, m_baudRate, UART_MIN_BAUD_RATE, UART_MAX_BAUD_RATE);
	DDX_Control(pDX, IDC_RICHEDIT2_RECEIVE, m_richReceive);
}

BEGIN_MESSAGE_MAP(CHIDtoUARTDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DEVICECHANGE()
	ON_BN_CLICKED(IDC_CHECK_CONNECT, &CHIDtoUARTDlg::OnBnClickedCheckConnect)
	ON_CBN_DROPDOWN(IDC_COMBO_DEVICE_LIST, &CHIDtoUARTDlg::OnCbnDropdownComboDeviceList)
	ON_CBN_CLOSEUP(IDC_COMBO_DEVICE_LIST, &CHIDtoUARTDlg::OnCbnCloseupComboDeviceList)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_GET_BAUD_RATE, &CHIDtoUARTDlg::OnBnClickedButtonGetBaudRate)
	ON_BN_CLICKED(IDC_BUTTON_SET_BAUD_RATE, &CHIDtoUARTDlg::OnBnClickedButtonSetBaudRate)
	ON_BN_CLICKED(IDC_BUTTON_TRANSMIT, &CHIDtoUARTDlg::OnBnClickedButtonTransmit)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CHIDtoUARTDlg::OnBnClickedButtonClear)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHIDtoUARTDlg message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CHIDtoUARTDlg::OnInitDialog()
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

void CHIDtoUARTDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHIDtoUARTDlg::OnPaint()
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
HCURSOR CHIDtoUARTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Handle device change messages (ie a device is added or removed)
// - If an HID device is connected, then add the device to the device list
// - If the device we were connected to was removed, then disconnect from the device
BOOL CHIDtoUARTDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
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

// Update the device list when the combobox is dropped down
void CHIDtoUARTDlg::OnCbnDropdownComboDeviceList()
{
	// Automatically update the device list when the
	// list is opened/dropped down
	UpdateDeviceList();
}

// Update the device list if the selected device has been
// removed when the combobox is closed
void CHIDtoUARTDlg::OnCbnCloseupComboDeviceList()
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
void CHIDtoUARTDlg::OnBnClickedCheckConnect()
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

// Retrieve the UART baud rate configuration
void CHIDtoUARTDlg::OnBnClickedButtonGetBaudRate()
{
	if (UpdateData(TRUE))
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			if (GetBaudRate(m_baudRate))
			{
				UpdateData(FALSE);
			}
			else
			{
				MessageBox(_T("Failed to get baud rate"), 0, MB_ICONWARNING);
			}
		}
		else
		{
			AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
		}
	}
}

// Set the UART baud rate
void CHIDtoUARTDlg::OnBnClickedButtonSetBaudRate()
{
	if (UpdateData(TRUE))
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			if (!SetBaudRate(m_baudRate))
			{
				MessageBox(_T("Failed to set baud rate"), 0, MB_ICONWARNING);
			}
		}
		else
		{
			AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
		}
	}
}

// Transmit the data in the transmit edit box
// to the device
void CHIDtoUARTDlg::OnBnClickedButtonTransmit()
{
	// Make sure that we are connected to a device
	if (HidDevice_IsOpened(m_hid))
	{
		CString transmitStr;

		// Get a string of ASCII characters to send to the device
		GetDlgItemText(IDC_EDIT_TRANSMIT, transmitStr);
		DWORD transmitBufferSize = transmitStr.GetLength();

		if (transmitBufferSize > 0)
		{
			BYTE* transmitBuffer = new BYTE[transmitBufferSize];

			// Convert a string of ASCII characters into
			// a BYTE array
			for (int i = 0; i < (int)transmitBufferSize; i++)
			{
				transmitBuffer[i] = (BYTE)transmitStr[i];
			}

			// Transmit the data to the device which will
			// then transmit the data via the UART
			if (!TransmitData(transmitBuffer, transmitBufferSize))
			{
				MessageBox(_T("Failed to transmit data"), 0, MB_ICONWARNING);
			}

			delete [] transmitBuffer;
		}
	}
	else
	{
		AfxMessageBox(IDS_STRING_NOT_CONNECTED, MB_ICONWARNING);
	}	
}

// Clear the receive rich edit control
void CHIDtoUARTDlg::OnBnClickedButtonClear()
{
	m_richReceive.SetWindowText(_T(""));
}

// Handle timer messages
void CHIDtoUARTDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Periodically check for input reports over
	// the interrupt endpoint
	if (nIDEvent == TIMER_READ_ID)
	{
		// Make sure that we are connected to a device
		if (HidDevice_IsOpened(m_hid))
		{
			DWORD bufferSize	= HidDevice_GetMaxReportRequest(m_hid) * SIZE_MAX_READ;
			BYTE* buffer		= new BYTE[bufferSize];
			DWORD bytesRead;

			// Receive the max number of UART input data bytes
			if (ReceiveData(buffer, bufferSize, bytesRead))
			{
				if (bytesRead > 0)
				{
					CString receiveStr;

					// Convert the receive buffer into a string of
					// ASCII characters
					for (int i = 0; i < (int)bytesRead; i++)
					{
						// Output standard ASCII and a few special characters (CR, LF, Tab)
						if ((buffer[i] >= 0x20 && buffer[i] <= 0x7E) ||
							(buffer[i] == '\r') ||
							(buffer[i] == '\n') ||
							(buffer[i] == '\t'))
						{
							receiveStr += buffer[i];
						}
						else
						{
							// Display the invalid character placeholder (square)
							receiveStr += (char)0x7F;
						}
					}

					// Add received data to the rich edit control
					AppendReceiveText(receiveStr);
				}
			}

			delete [] buffer;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CHIDtoUARTDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// Close the device
	if (HidDevice_IsOpened(m_hid))
	{
		// Disable the read timer before we disconnect
		// from the device
		StopReadTimer();

		HidDevice_Close(m_hid);
		m_hid = NULL;
	}

	// Disable WM_DEVICECHANGE messages
	UnregisterDeviceChange();
}

/////////////////////////////////////////////////////////////////////////////
// CHIDtoUARTDlg Class - Protected Methods
/////////////////////////////////////////////////////////////////////////////

void CHIDtoUARTDlg::InitializeDialog()
{
	// Initially device controls are disabled
	// until connected to a device
	EnableDeviceCtrls(FALSE);

	// Populate the device combobox list
	UpdateDeviceList();

	// Register for WM_DEVICECHANGE messages
	// whenever an HID device is attached or removed
	RegisterDeviceChange();
}

// Register for device change notification for USB HID devices
// OnDeviceChange() will handle device arrival and removal
void CHIDtoUARTDlg::RegisterDeviceChange()
{
	DEV_BROADCAST_DEVICEINTERFACE devIF = {0};

	devIF.dbcc_size			= sizeof(devIF);    
	devIF.dbcc_devicetype	= DBT_DEVTYP_DEVICEINTERFACE;    
	
	HidDevice_GetHidGuid(&devIF.dbcc_classguid);
	
	m_hNotifyDevNode = RegisterDeviceNotification(GetSafeHwnd(), &devIF, DEVICE_NOTIFY_WINDOW_HANDLE);
}

// Unregister for device change notification for USB HID devices
void CHIDtoUARTDlg::UnregisterDeviceChange()
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
BOOL CHIDtoUARTDlg::Connect()
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

		// Enable the read timer to check for input reports
		// over the interrupt endpoint
		StartReadTimer();
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
BOOL CHIDtoUARTDlg::Disconnect()
{
	BOOL disconnected = FALSE;

	// Disable the read timer before we disconnect
	// from the device
	StopReadTimer();

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
BOOL CHIDtoUARTDlg::GetSelectedDevice(CString& path)
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
BOOL CHIDtoUARTDlg::FindDevice(CString path, DWORD& deviceNum)
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
void CHIDtoUARTDlg::UpdateDeviceList()
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

// Enable/disable controls that should only be enabled
// when connected to a device
void CHIDtoUARTDlg::EnableDeviceCtrls(BOOL bEnable)
{
	int nIDs[] = 
	{
		IDC_BUTTON_GET_BAUD_RATE,
		IDC_BUTTON_SET_BAUD_RATE,
		IDC_BUTTON_TRANSMIT
	};

	for (int i = 0; i < sizeof(nIDs) / sizeof(nIDs[0]); i++)
	{
		GetDlgItem(nIDs[i])->EnableWindow(bEnable);
	}
}

// Start a timer to periodically check for input reports
// over the interrupt endpoint
void CHIDtoUARTDlg::StartReadTimer()
{
	SetTimer(TIMER_READ_ID, TIMER_READ_ELAPSE, NULL);
}

// Stop the read timer
void CHIDtoUARTDlg::StopReadTimer()
{
	KillTimer(TIMER_READ_ID);
}

// Append text to the end of the receive rich edit control
void CHIDtoUARTDlg::AppendReceiveText(const CString& text)
{
	long len = m_richReceive.GetTextLength();

	m_richReceive.SetSel(len, len);
	m_richReceive.ReplaceSel(text);
}

/////////////////////////////////////////////////////////////////////////////
// CHIDtoUARTDlg Class - HID Report Methods
/////////////////////////////////////////////////////////////////////////////

// Send Set Baud Rate output report over the control endpoint
BOOL CHIDtoUARTDlg::SetBaudRate(DWORD baudRate)
{
	BOOL	success		= FALSE;
	WORD	reportSize	= HidDevice_GetOutputReportBufferLength(m_hid);
	BYTE*	report		= new BYTE[reportSize];

	// Make sure that the device report size is adequate
	if (reportSize >= SIZE_OUT_CONTROL)
	{
		report[0] = ID_OUT_CONTROL;				// Report ID
		report[1] = (BYTE)(baudRate >> 24);		// Baud Rate (32-bit, Big Endian)
		report[2] = (BYTE)(baudRate >> 16);
		report[3] = (BYTE)(baudRate >> 8);
		report[4] = (BYTE)(baudRate);

		// Send an output report over the control endpoint
		// (Only supported in Windows XP and later)
		if (HidDevice_SetOutputReport_Control(m_hid, report, reportSize) == HID_DEVICE_SUCCESS)
		{
			success = TRUE;
		}
	}

	delete [] report;

	return success;
}

// Receive Get Baud Rate input report over the control endpoint
BOOL CHIDtoUARTDlg::GetBaudRate(DWORD& baudRate)
{
	BOOL	success		= FALSE;
	WORD	reportSize	= HidDevice_GetInputReportBufferLength(m_hid);
	BYTE*	report		= new BYTE[reportSize];
	BYTE	status;

	// Make sure that the device report size is adequate
	if (reportSize >= SIZE_IN_CONTROL)
	{
		report[0] = ID_IN_CONTROL;	// Report ID

		// Receive an input report over the control endpoint
		// (Only supported in Windows XP and later)
		status = HidDevice_GetInputReport_Control(m_hid, report, reportSize);

		// The firmware should return successfully, but there is
		// a bug in the report size in the report handler.
		// To be fixed in the future.
		//if (status == HID_DEVICE_SUCCESS)
		{
			baudRate = (((DWORD)report[1]) << 24) |
					   (((DWORD)report[2]) << 16) |
					   (((DWORD)report[3]) << 8) |
					   (((DWORD)report[4]));

			success = TRUE;
		}
	}

	delete [] report;

	return success;
}

// Fragment and transmit UART data by sending output reports over
// the interrupt endpoint
BOOL CHIDtoUARTDlg::TransmitData(const BYTE* buffer, DWORD bufferSize)
{
	BOOL	success		= FALSE;
	WORD	reportSize	= HidDevice_GetOutputReportBufferLength(m_hid);
	BYTE*	report		= new BYTE[reportSize];

	// Make sure that the device report size is adequate
	if (reportSize >= SIZE_OUT_DATA)
	{
		DWORD bytesWritten = 0;
		DWORD bytesToWrite = bufferSize;

		// Fragment the buffer into several writes of up to SIZE_MAX_WRITE(61)
		// bytes
		while (bytesWritten < bytesToWrite)
		{
			DWORD transferSize = min(bytesToWrite - bytesWritten, SIZE_MAX_WRITE);

			report[0] = ID_OUT_DATA;
			report[1] = (BYTE)transferSize;
			memcpy(&report[2], &buffer[bytesWritten], transferSize);

			// Send an output report over the interrupt endpoint
			if (HidDevice_SetOutputReport_Interrupt(m_hid, report, reportSize) != HID_DEVICE_SUCCESS)
			{
				// Stop transmitting if there was an error
				break;
			}

			bytesWritten += transferSize;
		}

		// Write completed successfully
		if (bytesWritten == bytesToWrite)
		{
			success = TRUE;
		}
	}

	delete [] report;

	return success;
}

// Receive several UART data input reports over the interrupt endpoint
BOOL CHIDtoUARTDlg::ReceiveData(BYTE* buffer, DWORD bufferSize, DWORD& bytesRead)
{
	BOOL success		= FALSE;
	WORD reportSize		= HidDevice_GetOutputReportBufferLength(m_hid);

	// Make sure that the device report size is adequate
	if (reportSize >= SIZE_IN_DATA)
	{
		// Make sure that the buffer is at least big enough to hold the maximum
		// number of input data bytes from a single report
		if (bufferSize >= SIZE_MAX_READ)
		{
			// Determine the worst-case number of reports that will fit in the
			// user buffer
			DWORD	numReports			= bufferSize / SIZE_MAX_READ;
			DWORD	reportBufferSize	= numReports * reportSize;
			BYTE*	reportBuffer		= new BYTE[reportBufferSize];
			DWORD	reportBufferRead	= 0;
			BYTE	status;

			// Receive as many input reports as possible
			// (resulting data bytes must be able to fit in the user buffer)
			status = HidDevice_GetInputReport_Interrupt(m_hid, reportBuffer, reportBufferSize, numReports, &reportBufferRead);

			// Success indicates that numReports were read
			// Transfer timeout may have returned less data
			if (status == HID_DEVICE_SUCCESS ||
				status == HID_DEVICE_TRANSFER_TIMEOUT)
			{
				bytesRead = 0;

				// Iterate through each report in the report buffer
				for (DWORD i = 0; i < reportBufferRead; i += reportSize)
				{
					// Determine the number of valid data bytes in the current report
					BYTE bytesInReport = reportBuffer[i + 1];

					// Copy the data bytes into the user buffer
					memcpy(&buffer[bytesRead], &reportBuffer[i + 2], bytesInReport);

					// Keep track of how many valid bytes are being returned in the user buffer
					bytesRead += bytesInReport;
				}

				success = TRUE;
			}

			delete [] reportBuffer;
		}
	}

	return success;
}
