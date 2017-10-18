// USBTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "USBTest.h"
#include "USBIF.h"
#include "USBTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// USB driver definitions

#include <initguid.h>		// Required for GUID definition
DEFINE_GUID(GUID_INTERFACE_SILABS_INTERRUPT, 
0xdda31245, 0x1bfc, 0x4225, 0xb2, 0xb8, 0xea, 0xaa, 0xb2, 0xe3, 0x90, 0xb6);

#define SILABS_INT_WRITEPIPE	"PIPE01"
#define SILABS_INT_READPIPE	"PIPE00"

#define IOCTL_READINT			0x800	// User defined IO Control code
										// for reading the Interrupt Pipe
				

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBTestDlg dialog

CUSBTestDlg::CUSBTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUSBTestDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUSBTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUSBTestDlg)
	DDX_Control(pDX, IDC_AN1_METER, m_Analog_Meter1);
	DDX_Control(pDX, IDC_AN2_METER, m_Analog_Meter2);
	DDX_Control(pDX, IDC_BTN1_LED, m_ButtonLED1);
	DDX_Control(pDX, IDC_BTN2_LED, m_ButtonLED2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUSBTestDlg, CDialog)
	//{{AFX_MSG_MAP(CUSBTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBTestDlg message handlers

BOOL CUSBTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bReadError = FALSE;
	m_bWriteError = FALSE;

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
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Custom Initialization follows:
	// Initialize handles
	hUSBRead, hUSBWrite = INVALID_HANDLE_VALUE;

	// LED's
	CWnd *pWndLedBtn1 = (CWnd *)GetDlgItem(IDC_BTN1_LED);
	m_ButtonLED1.SetLED(pWndLedBtn1,ID_LED_GREEN,ID_SHAPE_ROUND,500);
	m_ButtonLED1.SwitchOn();
	m_ButtonLED1.LedOff();

	CWnd *pWndLedBtn2 = (CWnd *)GetDlgItem(IDC_BTN2_LED);
	m_ButtonLED2.SetLED(pWndLedBtn2,ID_LED_GREEN,ID_SHAPE_ROUND,500);
	m_ButtonLED2.SwitchOn();
	m_ButtonLED2.LedOff();

	// Analog Meters
	CString sUnitPot = "Pot";
	m_Analog_Meter1.SetUnits(sUnitPot);
	m_Analog_Meter1.SetRange(0,255);
	m_Analog_Meter1.UpdateNeedle(0);
	CString sUnitTemp = "Temp";
	m_Analog_Meter2.SetUnits(sUnitTemp);
	m_Analog_Meter2.SetRange(0,255);
	m_Analog_Meter2.UpdateNeedle(0);

	// Establish USB connection
	hUSBRead = NULL;
	hUSBWrite = NULL;
	UsbIF.SetGUID(GUID_INTERFACE_SILABS_INTERRUPT);

	if (UsbIF.OpenUsbDevice())
	{
		// Get the read handle
		hUSBRead = UsbIF.open_file(SILABS_INT_READPIPE);
		if (hUSBRead == INVALID_HANDLE_VALUE)
		{
			CString sMessage;
			sMessage.Format("Error opening Read device: %s\n\nApplication is aborting.\nReset hardware and try again.",SILABS_INT_READPIPE);
			if (AfxMessageBox(sMessage,MB_OK|MB_ICONEXCLAMATION))
			{
				OnCancel();
			}
		}

		// Get the write handle
		hUSBWrite = UsbIF.open_file(SILABS_INT_WRITEPIPE);
		if (hUSBWrite == INVALID_HANDLE_VALUE)
		{
			CString sMessage;
			sMessage.Format("Error opening Write device: %s\n\nApplication is aborting.\nReset hardware and try again.",SILABS_INT_WRITEPIPE);
			if (AfxMessageBox(sMessage,MB_OK|MB_ICONEXCLAMATION))
			{
				OnCancel();
			}
		}

		// If both handles are valid, can start the timer
		if ( ((hUSBRead  != INVALID_HANDLE_VALUE) || (hUSBRead  != NULL)) &&
			 ((hUSBWrite != INVALID_HANDLE_VALUE) || (hUSBWrite != NULL)) )
		{
			SetTimer(1,100,NULL);
		}
	}
	else
	{
		if (AfxMessageBox("Error establishing USB connection.\n\nApplication is aborting.\nReset hardware and try again.",MB_OK|MB_ICONEXCLAMATION))
		{
			OnCancel();
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUSBTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
void CUSBTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUSBTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CUSBTestDlg::OnOKExit() 
{
	KillTimer(1);	

	if ( hUSBWrite != NULL )
	{
		::CloseHandle(hUSBWrite);
		hUSBWrite = NULL;
	}
	
	if ( hUSBRead != NULL )
	{
		::CloseHandle(hUSBRead);
		hUSBRead = NULL;
	}

	

	CDialog::OnOK();
}

// Note: No cancel button exists on the dialog, but this function is used
// when dialog is terminated at customer request from message box input.
void CUSBTestDlg::OnCancel() 
{
	CloseConnection(); 
	CDialog::OnCancel();
}

void CUSBTestDlg::CloseConnection() 
{
	KillTimer(1);		

	if ( hUSBWrite != NULL )
	{
		::CloseHandle(hUSBWrite);
		hUSBWrite = NULL;
	}
	
	if ( hUSBRead != NULL )
	{
		::CloseHandle(hUSBRead);
		hUSBRead = NULL;
	}
	
}

void CUSBTestDlg::OnTimer(UINT nIDEvent) 
{
	// Confirm an error hasn't already occurred
	if ( (m_bReadError == TRUE) || (m_bWriteError == TRUE) )
	{
		// Call base timer
		CDialog::OnTimer(nIDEvent);

		// Further processing not possible
		return;		
	}

	// Obtain current values of dialog editable items
	io_buffer.led1 = ((CButton*) GetDlgItem(IDC_CHECK_LED1))->GetCheck(); 
	io_buffer.led2 = ((CButton*) GetDlgItem(IDC_CHECK_LED2))->GetCheck(); 

	unsigned char B0 = ((CButton*) GetDlgItem(IDC_P1_B0))->GetCheck();
	unsigned char B1 = ((CButton*) GetDlgItem(IDC_P1_B1))->GetCheck();
	unsigned char B2 = ((CButton*) GetDlgItem(IDC_P1_B2))->GetCheck();
	unsigned char B3 = ((CButton*) GetDlgItem(IDC_P1_B3))->GetCheck();

	unsigned char P1 = ( (B3 << 3) | (B2 << 2) | (B1 << 1) | (B0) ) & 0x0F;
	io_buffer.port = P1; 

	unsigned long ulBytesSucceed;
	unsigned long ulBytesWriteRequest = (sizeof(USB_iobuf));
	unsigned long ulBytesReadRequest = sizeof(USB_iobuf);

	// Perform USB Data Transfer
	// Write transfer packet
	WriteFile(hUSBWrite, &io_buffer, ulBytesWriteRequest, &ulBytesSucceed, NULL);
	if(ulBytesSucceed != ulBytesWriteRequest)
	{
		m_bWriteError = TRUE;	// Note: Set error flag immediately so that multiple 
								// message boxes do not queue up.
		CString sError;
		sError.Format("Error writing to USB.\nWrote %d of %d bytes.\n\nApplication is aborting.\nReset hardware and try again.", ulBytesSucceed, ulBytesWriteRequest);
		if (AfxMessageBox(sError,MB_OK|MB_ICONEXCLAMATION))
		{
			OnCancel();
			return;
		}
	}
	// Read transfer packet
	ReadFile(hUSBRead, &io_buffer, ulBytesReadRequest, &ulBytesSucceed, NULL);
	if( (ulBytesSucceed != ulBytesReadRequest) && (m_bReadError == FALSE) )
	{
		m_bReadError = TRUE;	// Note: Set error flag immediately so that multiple 
								// message boxes do not queue up.

		CString sError;
		sError.Format("Error reading from USB.\nRead %d of %d bytes.\n\nApplication is aborting.\nReset hardware and try again.", ulBytesSucceed, ulBytesReadRequest);
		if (AfxMessageBox(sError,MB_OK|MB_ICONEXCLAMATION))
		{
			OnCancel();
			return;
		}
	}
		
	// Make updates to dialog display items
	if (io_buffer.led1)
		m_ButtonLED1.LedOn();
	else
		m_ButtonLED1.LedOff();

	if (io_buffer.led2)
		m_ButtonLED2.LedOn();
	else
		m_ButtonLED2.LedOff();

// For debug purposes, can watch what comes back from driver.
//	TRACE3("L1:%02X, L2:%02X, P:%02X, ",io_buffer.led1,io_buffer.led2,io_buffer.port);
//	TRACE2("A1:%02X, A2:%02X\n",io_buffer.analog1,io_buffer.analog2);

	unsigned char P0 = io_buffer.port & 0x0F;
	((CButton*) GetDlgItem(IDC_P0_B0))->SetCheck(P0 & 0x01);  
	((CButton*) GetDlgItem(IDC_P0_B1))->SetCheck((P0 & 0x02) >> 1);  
	((CButton*) GetDlgItem(IDC_P0_B2))->SetCheck((P0 & 0x04) >> 2);  
	((CButton*) GetDlgItem(IDC_P0_B3))->SetCheck((P0 & 0x08) >> 3);  

	m_Analog_Meter1.UpdateNeedle(io_buffer.analog1);
	m_Analog_Meter2.UpdateNeedle(io_buffer.analog2);

	CDialog::OnTimer(nIDEvent);
}

