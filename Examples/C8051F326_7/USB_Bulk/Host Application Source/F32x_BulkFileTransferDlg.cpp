// F32x_BulkFileTransferDlg.cpp : implementation file
//

#include "stdafx.h"
#include "F32x_BulkFileTransfer.h"
#include "F32x_BulkFileTransferDlg.h"
#include "F32x_BulkFileTransferFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DEFINE_GUID(GUID_INTERFACE_SILABS_BULK, 
0x37538c66, 0x9584, 0x42d3, 0x96, 0x32, 0xeb, 0xad, 0xa, 0x23, 0xd, 0x13);

#define SILABS_BULK_WRITEPIPE	"PIPE01"
#define SILABS_BULK_READPIPE		"PIPE00"

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
// CF32x_BulkFileTransferDlg dialog

CF32x_BulkFileTransferDlg::CF32x_BulkFileTransferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CF32x_BulkFileTransferDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CF32x_BulkFileTransferDlg)
	m_sTXFileName = _T("");
	m_sRXFileName = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CF32x_BulkFileTransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CF32x_BulkFileTransferDlg)
	DDX_Text(pDX, IDC_TX_FILE_NAME, m_sTXFileName);
	DDX_Text(pDX, IDC_RX_FILE_NAME, m_sRXFileName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CF32x_BulkFileTransferDlg, CDialog)
	//{{AFX_MSG_MAP(CF32x_BulkFileTransferDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TRANSFER_DATA, OnTransferData)
	ON_BN_CLICKED(IDC_RECEIVE_DATA, OnReceiveData)
	ON_BN_CLICKED(IDC_BROWSE_TX_FILE, OnBrowseTxFile)
	ON_BN_CLICKED(IDC_BROWSE_RX_FILE, OnBrowseRxFile)
	ON_BN_CLICKED(IDC_UPDATE_DEVICE_LIST, OnUpdateDeviceList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CF32x_BulkFileTransferDlg message handlers

BOOL CF32x_BulkFileTransferDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	CComboBox*	pDevList	= (CComboBox*)GetDlgItem(IDC_DEVICE_SELECT);

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
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	sgCUsbIF.SetGUID(GUID_INTERFACE_SILABS_BULK);

	// Get devices, init. combo box with their names
	FillDeviceList();
	
	if (pDevList)
	{
		// Open selected device.
		if (pDevList->GetCurSel() >= 0)
		{
			F32x_STATUS status = F32x_Open(pDevList->GetCurSel(), &m_hUSBDevice);

			// Open selected file.
			if (status == F32x_SUCCESS)
			{
				// Write file to device in MAX_PACKET_SIZE-byte chunks.
				// Get the write handle
				m_hUSBWrite = sgCUsbIF.OpenUSBfile(SILABS_BULK_WRITEPIPE);

				if (m_hUSBWrite == INVALID_HANDLE_VALUE)
				{
					CString sMessage;
					sMessage.Format("Error opening Write device: %s\n\nApplication is aborting.\nReset hardware and try again.",SILABS_BULK_WRITEPIPE);
					AfxMessageBox(sMessage,MB_OK|MB_ICONEXCLAMATION);
				}

				// Get the read handle
				m_hUSBRead = sgCUsbIF.OpenUSBfile(SILABS_BULK_READPIPE);

				if (m_hUSBRead == INVALID_HANDLE_VALUE)
				{
					CString sMessage;
					sMessage.Format("Error opening Read device: %s\n\nApplication is aborting.\nReset hardware and try again.",SILABS_BULK_READPIPE);
					AfxMessageBox(sMessage,MB_OK|MB_ICONEXCLAMATION);
				}
			}
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CF32x_BulkFileTransferDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.

void CF32x_BulkFileTransferDlg::OnPaint() 
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
// the minimized window.
HCURSOR CF32x_BulkFileTransferDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CF32x_BulkFileTransferDlg::OnTransferData() 
{
	CComboBox*	pDevList	= (CComboBox*)GetDlgItem(IDC_DEVICE_SELECT);

	BeginWaitCursor();

	if (m_hUSBDevice != INVALID_HANDLE_VALUE)
	{
		F32x_STATUS status = F32x_SUCCESS;

		// Open selected file.
		if (status == F32x_SUCCESS)
		{
			// Write file to device in MAX_PACKET_SIZE-byte chunks.
			WriteFileData();
		}
	}

	EndWaitCursor();
}

void CF32x_BulkFileTransferDlg::OnReceiveData() 
{
	CComboBox*	pDevList	= (CComboBox*)GetDlgItem(IDC_DEVICE_SELECT);

	BeginWaitCursor();

	//if (pDevList)
	if (m_hUSBDevice != INVALID_HANDLE_VALUE)
	{
		F32x_STATUS status = F32x_SUCCESS;

		// Open temporary file based on file name of input file
		if (status == F32x_SUCCESS)
		{
			// Read file data in MAX_PACKET_SIZE-byte chunks and write to temp file.
			// Compare temp. file with original
			ReadFileData();
		}
	}

	EndWaitCursor();	
}

void CF32x_BulkFileTransferDlg::OnBrowseTxFile() 
{
	CFileDialog fileDlg(TRUE);
	
	if (fileDlg.DoModal() == IDOK)
	{
		m_sTXFileName = fileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CF32x_BulkFileTransferDlg::OnBrowseRxFile() 
{
	CFileDialog fileDlg(TRUE);
	
	if (fileDlg.DoModal() == IDOK)
	{
		m_sRXFileName = fileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CF32x_BulkFileTransferDlg::OnOK() 
{
	// Close handle if open
	if (m_hUSBDevice != INVALID_HANDLE_VALUE)
	{
		F32x_Close(m_hUSBDevice);
		m_hUSBDevice = INVALID_HANDLE_VALUE;
	}
	
	CDialog::OnOK();
}

void CF32x_BulkFileTransferDlg::OnUpdateDeviceList() 
{
	FillDeviceList();
}


void CF32x_BulkFileTransferDlg::FillDeviceList()
{
	F32x_DEVICE_STRING	devStr;
	DWORD				dwNumDevices	= 0;
	CComboBox*			pDevList		= (CComboBox*)GetDlgItem(IDC_DEVICE_SELECT);

	if (pDevList)
	{
		int numDevs = pDevList->GetCount();

		for (int i = 0; i < numDevs; i++)
		{
			pDevList->DeleteString(0);
		}

		F32x_STATUS status = F32x_GetNumDevices(&dwNumDevices);

		if (status == F32x_SUCCESS)
		{
			for (DWORD d = 0; d < dwNumDevices; d++)
			{
				status = F32x_GetProductString(d, devStr, F32x_RETURN_SERIAL_NUMBER);

				if (status == F32x_SUCCESS)
				{
					if (pDevList)				// Fill device list
						pDevList->AddString(devStr);
				}
			}
		}

		pDevList->SetCurSel(0);
	}
}


BOOL CF32x_BulkFileTransferDlg::WriteFileData()
{
	BOOL success = TRUE;

	if (m_sTXFileName.GetLength() > 0)
	{
		CFile file;

		if (file.Open(m_sTXFileName, CFile::modeRead | CFile::shareDenyNone))
		{
			DWORD		size			= file.GetLength();
			DWORD		dwBytesWritten	= 0;
			DWORD		dwBytesRead		= 0;
			BYTE		buf[MAX_PACKET_SIZE_WRITE];

			buf[0] = FT_WRITE_MSG;
			buf[1] = (char)(size & 0x000000FF);
			buf[2] = (char)((size & 0x0000FF00) >> 8);

			// Check size of file (GP)
			if (size > 4096)
			{
				AfxMessageBox("File size is too large.");
				success = FALSE;
			}

			// If file is not too big, send write file size message
			else if (DeviceWrite(buf, FT_MSG_SIZE, &dwBytesWritten))
			{
				if (dwBytesWritten == FT_MSG_SIZE)
				{
					DWORD numPkts		= (size / MAX_PACKET_SIZE_WRITE) + (((size % MAX_PACKET_SIZE_WRITE) > 0)? 1 : 0);
					DWORD numLoops		= (numPkts / MAX_WRITE_PKTS) + (((numPkts % MAX_WRITE_PKTS) > 0)? 1 : 0);
					DWORD counterPkts	= 0;
					DWORD counterLoops	= 0;
					DWORD totalWritten	= 0;

					// Now write data to board in groups of 8 packets
					while (counterLoops < numLoops && success)
					{
						int		i	= 0;

						while (i < MAX_WRITE_PKTS && counterPkts < numPkts && success)
						{
							DWORD dwWriteLength	= 0;

							if ((size - totalWritten) < MAX_PACKET_SIZE_WRITE)
							{
								dwWriteLength = size - totalWritten;
							}
							else
							{
								dwWriteLength = MAX_PACKET_SIZE_WRITE;
							}

							memset(buf, 0, dwWriteLength);
							file.Read(buf, dwWriteLength);
							dwBytesWritten = 0;

                            success = DeviceWrite(buf, dwWriteLength, &dwBytesWritten);
                            totalWritten += dwWriteLength;
							counterPkts++;
							i++;
						}

						if (success)
						{
							memset(buf, 0, MAX_PACKET_SIZE_WRITE);

							// Check for ACK packet after writing 8 pkts.
							while ((buf[0] != 0xFF) && success)
							{
								success = DeviceRead(buf, 1, &dwBytesRead);
							}
						}

						counterLoops++;
					}

					if (!success)
					{
						AfxMessageBox("Target device failure while sending file data.\nCheck file size.");
						success = FALSE;
					}
				}
				else
				{
					AfxMessageBox("Incomplete write file size message sent to device.");
					success = FALSE;
				}
			}
			else
			{
				AfxMessageBox("Target device failure while sending file size information.");
				success = FALSE;
			}

			file.Close();
		}
		else
		{
			CString err;
			err.Format("Failed opening file:\n%s", m_sTXFileName);
			AfxMessageBox(err);
			success = FALSE;
		}
	}
	else
	{
		AfxMessageBox("Error:  No file selected.");
		success = FALSE;
	}

	return success;
}


BOOL CF32x_BulkFileTransferDlg::ReadFileData()
{
	BOOL success = TRUE;
    DWORD	length;
	if ((length = m_sRXFileName.GetLength()) > 0)
	{
		CFile	file;

		// Open the temporary file for writing
		if (file.Open(m_sRXFileName, CFile::modeWrite | CFile::modeCreate |CFile::shareDenyNone))
		{
			DWORD		dwBytesRead		= 0;
			DWORD		dwBytesWritten	= 0;
			BYTE		buf[MAX_PACKET_SIZE_READ];
			BYTE		msg[FT_MSG_SIZE];

			msg[0] = (BYTE)FT_READ_MSG;
			msg[1] = (BYTE)0xFF;
			msg[2] = (BYTE)0xFF;

			if (DeviceWrite(msg, FT_MSG_SIZE, &dwBytesWritten))
			{
				DWORD size			= 0;
				DWORD counterPkts	= 0;
				DWORD numPkts		= 0;

				memset(buf, 0, FT_MSG_SIZE);

				if (DeviceRead(buf, FT_MSG_SIZE, &dwBytesRead))
				{
					size	= (buf[1] & 0x000000FF) | ((buf[2] << 8) & 0x0000FF00);
					numPkts	= (size/MAX_PACKET_SIZE_READ) + (((size % MAX_PACKET_SIZE_READ) > 0)? 1 : 0);

					// Next line added in from Test Example
					DWORD totalRead = 0;
					// Now read data from board
					while (counterPkts < numPkts && success)
					{
						DWORD dwReadLength = 0;
						dwBytesRead = 0;

						if ((size - totalRead) < MAX_PACKET_SIZE_READ)
						{
							dwReadLength = size - totalRead;
						}
						else
						{
							dwReadLength = MAX_PACKET_SIZE_READ;
						}
						
						memset(buf, 0, dwReadLength);
						if (DeviceRead(buf, dwReadLength, &dwBytesRead))
						{
							file.Write(buf, dwReadLength);
						}
						else
						{
							AfxMessageBox("Failed reading file packet from target device.");
							success = FALSE;
						}

						counterPkts++;
					}
				}
				else
				{
					AfxMessageBox("Failed reading file size message from target device.");
					success = FALSE;
				}
			}
			else
			{
				AfxMessageBox("Failed sending read file message to target device.");
				success = FALSE;
			}

			if (file.GetLength() == 0)
			{
				CString err;
				err.Format("File has 0 length:\n%s", m_sRXFileName);
				AfxMessageBox(err);
			}

			file.Close();
		}
		else
		{
			CString err;
			err.Format("Failed opening file:\n%s", m_sRXFileName);
			AfxMessageBox(err);
			success = FALSE;
		}
	}
	else
	{
		AfxMessageBox("Error:  No file selected.");
		success = FALSE;
	}

	return success;
}


BOOL CF32x_BulkFileTransferDlg::DeviceRead(BYTE* buffer, DWORD dwSize, DWORD* lpdwBytesRead, DWORD dwTimeout)
{
	F32x_STATUS	status			= F32x_SUCCESS;

	status = F32x_Read(m_hUSBRead, buffer, dwSize, lpdwBytesRead);

	return (status == F32x_SUCCESS);
}


BOOL CF32x_BulkFileTransferDlg::DeviceWrite(BYTE* buffer, DWORD dwSize, DWORD* lpdwBytesWritten, DWORD dwTimeout)
{
	F32x_STATUS	status	= F32x_SUCCESS;

	status = F32x_Write(m_hUSBWrite, buffer, dwSize, lpdwBytesWritten);

	return (status == F32x_SUCCESS);
}


//------------------------------------------------------------------------
// F32x_GetNumDevices()
//
// Determine number of Silabs devices connected to the system from the
// registry.
//------------------------------------------------------------------------
F32x_STATUS 
F32x_GetNumDevices(LPDWORD lpdwNumDevices)
{
	F32x_STATUS	status = F32x_DEVICE_NOT_FOUND;

	// Validate parameter
	if (!ValidParam(lpdwNumDevices))
	{
		return F32x_INVALID_PARAMETER;
	}

	// Must set the GUID for functions that access the registry.
	sgCUsbIF.SetGUID(GUID_INTERFACE_SILABS_BULK);
	*lpdwNumDevices = sgCUsbIF.GetNumDevices();

	if (*lpdwNumDevices > 0)
	{
		status = F32x_SUCCESS;
	}

	return status;
}


//------------------------------------------------------------------------
// F32x_GetProductString()
//
// Find the product string of a device by index in the registry.
//------------------------------------------------------------------------
F32x_STATUS 
F32x_GetProductString(DWORD dwDeviceNum, LPVOID lpvDeviceString, DWORD dwFlags)
{
	F32x_STATUS			status	= F32x_DEVICE_NOT_FOUND;
	CDeviceListEntry	dev;

	// Validate parameter
	if (!ValidParam(lpvDeviceString))
	{
		return F32x_INVALID_PARAMETER;
	}

	// Must set the GUID for functions that access the registry.
	sgCUsbIF.SetGUID(GUID_INTERFACE_SILABS_BULK);
	sgCUsbIF.GetDeviceStrings(dwDeviceNum, dev);

	switch (dwFlags)
	{
		case F32x_RETURN_SERIAL_NUMBER:
				if (dev.m_serialnumber.length() > 0)
				{
					strcpy((char*)lpvDeviceString, dev.m_serialnumber.c_str());
					status = F32x_SUCCESS;
				}
				break;
		case F32x_RETURN_DESCRIPTION:
				if (dev.m_friendlyname.length() > 0)
				{
					strcpy((char*)lpvDeviceString, dev.m_friendlyname.c_str());
					status = F32x_SUCCESS;
				}
				break;
		default:
				break;
    }

	return status;
}


//------------------------------------------------------------------------
// F32x_Open()
//
// Open a file handle to access a Silabs device by index number.  The open
// routine determines the device's full name and uses it to open the handle.
//------------------------------------------------------------------------
F32x_STATUS 
F32x_Open(DWORD dwDevice, HANDLE* cyHandle)
{
	F32x_STATUS	status = F32x_DEVICE_NOT_FOUND;

	// Validate parameter
	if (!ValidParam(cyHandle))
	{
		return F32x_INVALID_PARAMETER;
	}

	// Must set the GUID for functions that access the registry.
	sgCUsbIF.SetGUID(GUID_INTERFACE_SILABS_BULK);

	if (cyHandle)
	{
		*cyHandle = sgCUsbIF.Open(dwDevice);

		if (*cyHandle != INVALID_HANDLE_VALUE)
		{
			status = F32x_SUCCESS;
		}
	}
	else
	{
		status = F32x_INVALID_HANDLE;
	}

	return status;
}


//------------------------------------------------------------------------
// F32x_Close()
//
// Close file handle used to access a Silabs device.
//------------------------------------------------------------------------
F32x_STATUS
F32x_Close(HANDLE cyHandle)
{
	F32x_STATUS	status = F32x_INVALID_HANDLE;

	if ((cyHandle != NULL) && (cyHandle != INVALID_HANDLE_VALUE))
	{
		::CloseHandle(cyHandle);
		status = F32x_SUCCESS;
	}

	return status;
}


//------------------------------------------------------------------------
// F32x_Read()
//
// Read data from USB device.
// If read timeout value has been set, check RX queue until F32x_RX_COMPLETE
// flag bit is set.  If timeout the occurs before F32x_RX_COMPLETE, return
// error.  If no timeout has been set attempt read immediately. 
//------------------------------------------------------------------------
F32x_STATUS
F32x_Read(HANDLE cyHandle, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpdwBytesReturned)
{
	F32x_STATUS	status = F32x_SUCCESS;

	// Validate parameters
	if (!ValidParam(lpBuffer, lpdwBytesReturned))
	{
		return F32x_INVALID_PARAMETER;
	}

	// Check for a valid Handle value
	if (cyHandle != INVALID_HANDLE_VALUE)
	{
		// Check that the read length is within range
		if ((dwBytesToRead > 0) && (dwBytesToRead <= F32x_MAX_READ_SIZE))
		{
			// Read transfer packet
			while(!ReadFile(cyHandle, lpBuffer, dwBytesToRead, lpdwBytesReturned, NULL))
			{	// Device IO failed.
				status = F32x_READ_ERROR;
			} 
		}
		else
			status = F32x_INVALID_REQUEST_LENGTH;
	}
	else 
		status = F32x_INVALID_HANDLE;

	return status;
}


//------------------------------------------------------------------------
// F32x_Write()
//
// Write data to USB device.
// If write timeout value has been set, continue write attempts until
// successful or timeout occurs.
//------------------------------------------------------------------------
F32x_STATUS
F32x_Write(HANDLE cyHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpdwBytesWritten)
{
	F32x_STATUS	status = F32x_INVALID_HANDLE;

	// Validate parameters
	if (!ValidParam(lpBuffer, lpdwBytesWritten))
	{
		return F32x_INVALID_PARAMETER;
	}

	if (cyHandle != INVALID_HANDLE_VALUE)
	{
		if ((dwBytesToWrite > 0) && (dwBytesToWrite <= F32x_MAX_WRITE_SIZE))
		{
			if (!WriteFile(cyHandle, lpBuffer, dwBytesToWrite, lpdwBytesWritten, NULL))
			{
				status = F32x_WRITE_ERROR;

				if (sgdwWriteTimeout > 0)
				{
					DWORD	dwStart	= GetTickCount();
					DWORD	dwEnd	= GetTickCount();

					// Keep trying to write until success or timeout
					while((dwEnd - dwStart) < sgdwWriteTimeout && status != F32x_SUCCESS)
					{
						if (WriteFile(cyHandle, lpBuffer, dwBytesToWrite, lpdwBytesWritten, NULL))
						{
							status = F32x_SUCCESS;	// Write succeeded after > 1 attempts.
						}

						dwEnd = GetTickCount();
					}
				}
			}
			else
				status = F32x_SUCCESS;				// Write succeeded on first attempt.
		}
		else
			status = F32x_INVALID_REQUEST_LENGTH;
	}

	return status;
}

//------------------------------------------------------------------------
// ValidParam(LPDWORD)
//
// Checks validity of an LPDWORD pointer value.
//------------------------------------------------------------------------
static BOOL ValidParam(LPDWORD lpdwPointer)
{
	DWORD temp = 0;

	try 
	{
		temp = *lpdwPointer;
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}


//------------------------------------------------------------------------
// ValidParam(LPVOID)
//
// Checks validity of an LPVOID pointer value.
//------------------------------------------------------------------------
static BOOL ValidParam(LPVOID lpVoidPointer)
{
	BYTE temp = 0;

	try 
	{
		temp = *((BYTE*)lpVoidPointer);
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}


//------------------------------------------------------------------------
// ValidParam(HANDLE*)
//
// Checks validity of an HANDLE* pointer value.
//------------------------------------------------------------------------
static BOOL ValidParam(HANDLE* lpHandle)
{
	HANDLE temp = 0;

	try 
	{
		temp = *lpHandle;
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}


//------------------------------------------------------------------------
// ValidParam(LPVOID, LPDWORD)
//
// Checks validity of LPVOID, LPDWORD pair of pointer values.
//------------------------------------------------------------------------
static BOOL ValidParam(LPVOID lpVoidPointer, LPDWORD lpdwPointer)
{
	if (ValidParam(lpVoidPointer))
		if (ValidParam(lpdwPointer))
			return TRUE;

	return FALSE;
}


//------------------------------------------------------------------------
// ValidParam(LPDWORD, LPDWORD)
//
// Checks validity of LPDWORD, LPDWORD pair of pointer values.
//------------------------------------------------------------------------
static BOOL ValidParam(LPDWORD lpdwPointer1, LPDWORD lpdwPointer2)
{
	if (ValidParam(lpdwPointer1))
		if (ValidParam(lpdwPointer2))
			return TRUE;

	return FALSE;
}