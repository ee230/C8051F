/************************************************************************
 *
 *  Module:       UsbIF.cpp
 *  Description:  USB Interface for Silabs USB development board
 *  Company:      Silicon Laboratories Inc.
 *
 ************************************************************************/

#include "stdafx.h"
#include "UsbIF.h"

// standard constructor
CUsbIF::CUsbIF()
{
	memset(m_DeviceName, 0, sizeof(m_DeviceName));
}

// destructor
CUsbIF::~CUsbIF()
{
}

// Initialize the GUID
void CUsbIF::SetGUID(GUID newGUID)
{
	m_GUID = newGUID;
}


//------------------------------------------------------------------------
// GetNumDevices()
//
// Counts the number of Silabs USB devices listed in the registry.
//------------------------------------------------------------------------
DWORD CUsbIF::GetNumDevices()
{
	DWORD numDevices = 0;

	// Retrieve device list for GUID that has been specified.
	HDEVINFO hDevInfoList = SetupDiGetClassDevs (&m_GUID, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE)); 

	if (hDevInfoList != NULL)
	{
		SP_DEVICE_INTERFACE_DATA deviceInfoData;

		for (int index = 0; index < 127;index++)
		{
			// Clear data structure
			ZeroMemory(&deviceInfoData, sizeof(deviceInfoData));
			deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

			// retrieves a context structure for a device interface of a device information set.
			if (SetupDiEnumDeviceInterfaces (hDevInfoList, 0, &m_GUID, index, &deviceInfoData)) 
			{
				numDevices++;
			}
			else
			{
				if ( GetLastError() == ERROR_NO_MORE_ITEMS ) 
					break;
			}
		}
	}

	// SetupDiDestroyDeviceInfoList() destroys a device information set
	// and frees all associated memory.
	SetupDiDestroyDeviceInfoList (hDevInfoList);

	return numDevices;
}


//------------------------------------------------------------------------
// GetDeviceStrings()
//
// dwDeviceNum:	Position of device entry in the Windows registry list.
// dev:			container class where device description strings will be stored.
//
// Copies device description from the registry for single device.
//------------------------------------------------------------------------
void CUsbIF::GetDeviceStrings(DWORD dwDeviceNum, CDeviceListEntry& dev)
{
	// Clear the device info
	dev.m_friendlyname	= "";
	dev.m_linkname		= "";
	dev.m_serialnumber	= "";

	// Retrieve device list for GUID that has been specified.
	HDEVINFO hDevInfoList = SetupDiGetClassDevs (&m_GUID, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE)); 

	if (hDevInfoList != NULL)
	{
		SP_DEVICE_INTERFACE_DATA deviceInfoData;

		// Clear data structure
		ZeroMemory(&deviceInfoData, sizeof(deviceInfoData));
		deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		// retrieves a context structure for a device interface of a device information set.
		if (SetupDiEnumDeviceInterfaces (hDevInfoList, 0, &m_GUID, dwDeviceNum, &deviceInfoData)) 
		{
			// Must get the detailed information in two steps
			// First get the length of the detailed information and allocate the buffer
			// retrieves detailed information about a specified device interface.
			PSP_DEVICE_INTERFACE_DETAIL_DATA     functionClassDeviceData = NULL;
			ULONG  predictedLength, requiredLength;

			predictedLength = requiredLength = 0;
			SetupDiGetDeviceInterfaceDetail (	hDevInfoList,
												&deviceInfoData,
												NULL,			// Not yet allocated
												0,				// Set output buffer length to zero 
												&requiredLength,// Find out memory requirement
												NULL);			

			predictedLength = requiredLength;
			functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc (predictedLength);
			functionClassDeviceData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
			
			SP_DEVINFO_DATA did = {sizeof(SP_DEVINFO_DATA)};
	
			// Second, get the detailed information
			if ( SetupDiGetDeviceInterfaceDetail (	hDevInfoList,
													&deviceInfoData,
													functionClassDeviceData,
													predictedLength,
													&requiredLength,
													&did)) 
			{
				TCHAR fname[256];

				// Try by friendly name first.
				if (!SetupDiGetDeviceRegistryProperty(hDevInfoList, &did, SPDRP_FRIENDLYNAME, NULL, (PBYTE) fname, sizeof(fname), NULL))
				{	// Try by device description if friendly name fails.
					if (!SetupDiGetDeviceRegistryProperty(hDevInfoList, &did, SPDRP_DEVICEDESC, NULL, (PBYTE) fname, sizeof(fname), NULL))
					{	// Use the raw path information for linkname and friendlyname
						strncpy(fname, functionClassDeviceData->DevicePath, 256);
					}
				}
					dev.m_friendlyname	= fname;
					dev.m_linkname		= functionClassDeviceData->DevicePath;
					// Now get the Serial Number this is OS dependent as the registry
				// has to be manually parsed under Win98
				if(!IsWin98())
				{
					GetSerialNumber(functionClassDeviceData->DevicePath, &(dev.m_serialnumber));
				}
				else
				{
					HKEY tempkey = NULL;
					LONG lRet;
					char szDeviceInstance[256];
					DWORD dwBufLen=256;

					//KeyPath is the known location of our device using SIBULK GUID {37538c66-9584-42d3-9632-ebad0a230d13} under Win98SE
					std::string KeyPath = "System\\CurrentControlSet\\Control\\DeviceClasses\\{37538c66-9584-42d3-9632-ebad0a230d13}\\";
					//SymLinkName is the dynamically allocated path of this particular instance of the Xpress USB device
					std::string SymLinkName = functionClassDeviceData->DevicePath;
					//You cannot use '/' symbols in a registry path name so replace them with "#
					SymLinkName = SymLinkName.replace(0,4,"##.#");
					//Add the dynamic SymLinkName to the known KeyPath string and you have the key path to this device instance
					KeyPath += SymLinkName;
					//Get the Key
					lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KeyPath.c_str() , NULL, KEY_READ, &tempkey);
					if( lRet != ERROR_SUCCESS )
						tempkey = NULL;	//error retrieving key
					
					// Get the Device Instance string using the key
					lRet = RegQueryValueEx( tempkey, "DeviceInstance", NULL, NULL,
					(LPBYTE) szDeviceInstance, &dwBufLen);
					if( (lRet != ERROR_SUCCESS) || (dwBufLen > 80) )
						tempkey = NULL;	//error retrieving instance string
					std::string SerialNumber = szDeviceInstance;
					SerialNumber.erase(0,22);
					dev.m_serialnumber = SerialNumber;
					
				}
					
				free( functionClassDeviceData );
			}
		}
	}

	// SetupDiDestroyDeviceInfoList() destroys a device information set
	// and frees all associated memory.
	SetupDiDestroyDeviceInfoList (hDevInfoList);
}


//------------------------------------------------------------------------
// Open()
//
// dwDevice	: position of device description string in the registry
//
// Get the description strings for a device and copy the linkname into
// m_DeviceName.  Call OpenUSBfile() to create the file handle.
//------------------------------------------------------------------------
HANDLE CUsbIF::Open(DWORD dwDevice)
{
	CDeviceListEntry dev;

	GetDeviceStrings(dwDevice, dev);
	// first set the device by setting the devicename
	strcpy(m_DeviceName, dev.m_linkname.c_str());
	// next get a handle and return it
	return OpenUSBfile(NULL);
}



//------------------------------------------------------------------------
// GetSerialNumber()
//
// Extract the serial number from the device path string.
//------------------------------------------------------------------------
void CUsbIF::GetSerialNumber(LPCTSTR DevicePath, std::string* str)
{
	std::string temp = DevicePath;

	temp = temp.substr(temp.find_first_of('#') + 1, temp.find_last_of('#') - temp.find_first_of('#'));
	temp = temp.substr(temp.find_first_of('#') + 1, (temp.find_last_of('#') - temp.find_first_of('#')) - 1);

	if (str)
	{
		*str += temp;
	}
}


//------------------------------------------------------------------------
// OpenUSBfile()
//
// sFileName : additional string for flexibility in creating the entire
// device path string
//
// Open a handle file handle for the USB device.  Will be used for all
// subsequent communication with the USB device.
//------------------------------------------------------------------------
HANDLE CUsbIF::OpenUSBfile(char* sFileName)
{
	HANDLE hFile;

    if(sFileName)
	{
		strcat (m_DeviceName, "\\");                      
		strcat (m_DeviceName, sFileName);                                      
	}

	hFile = CreateFile(	m_DeviceName,
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_WRITE | FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						0,
						NULL);

	return hFile;
}


CDeviceListEntry::CDeviceListEntry(LPCTSTR linkname, LPCTSTR friendlyname, LPCTSTR serialnumber)
{
	m_linkname		= linkname;
	m_friendlyname	= friendlyname;
	m_serialnumber	= serialnumber;
}


bool IsWin98()
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure
	// which is supported on Windows 2000.
	//
	// It that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if( ! GetVersionEx ( (OSVERSIONINFO *)&osvi) )
		{
			return FALSE;
		}
	}

	switch (osvi.dwPlatformId)
	{

	case VER_PLATFORM_WIN32_NT:
		return FALSE;
		break;

	case VER_PLATFORM_WIN32_WINDOWS:
		if ((osvi.dwMajorVersion > 4) || 
		   ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0 )))
		{
			return TRUE; // This is Windows 98
		}
		else
			return FALSE; // This is Windows 95
		break;
	
	case VER_PLATFORM_WIN32s:
		return FALSE;
		break;
			
	}
	
	return FALSE;
}
