/************************************************************************
 *
 *  Module:       UsbIF.cpp
 *  Description:  USB Interface for Silabs USB development board
 *  Company:      Silicon Laboratories
 *
 ************************************************************************/

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
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


BOOL CUsbIF::OpenUsbDevice()
{
	BOOL bRet = FALSE;

	// Retrieve device list for GUID that has been specified.
	HDEVINFO hDevInfoList = SetupDiGetClassDevs (&m_GUID, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE)); 

	if (hDevInfoList != NULL)
	{
		SP_DEVICE_INTERFACE_DATA deviceInfoData;

		for (int MemberIndex = 0; MemberIndex < 127; MemberIndex++)
		{
			// Clear data structure
			ZeroMemory(&deviceInfoData, sizeof(deviceInfoData));
			deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

			// retrieves a context structure for a device interface of a device information set.
			if (SetupDiEnumDeviceInterfaces (hDevInfoList, 0, &m_GUID, MemberIndex, &deviceInfoData)) 
			{
				// Must get the detailed information in two steps
				// First get the length of the detailed information and allocate the buffer
				// retrieves detailed information about a specified device interface.
				PSP_DEVICE_INTERFACE_DETAIL_DATA     functionClassDeviceData = NULL;
				ULONG  predictedLength, requiredLength;
				predictedLength = requiredLength = 0;
				SetupDiGetDeviceInterfaceDetail (
						hDevInfoList,
						&deviceInfoData,
						NULL,			// Not yet allocated
						0,				// Set output buffer length to zero 
						&requiredLength,// Find out memory requirement
						NULL);			

				predictedLength = requiredLength;
				functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc (predictedLength);
				functionClassDeviceData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);

				// Second, get the detailed information
				if ( SetupDiGetDeviceInterfaceDetail (
							hDevInfoList,
							&deviceInfoData,
							functionClassDeviceData,
							predictedLength,
							&requiredLength,
							NULL)) 
				{
					// Save the device name for subsequent pipe open calls
					strcpy( m_DeviceName, functionClassDeviceData->DevicePath);
					free( functionClassDeviceData );
					bRet = TRUE;
					break;
				}

				free( functionClassDeviceData );

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

	return bRet;
}

HANDLE CUsbIF::open_file( char *filename)
{
	HANDLE hFile;

    if(filename)
	{
		strcat (m_DeviceName, "\\");                      
		strcat (m_DeviceName, filename);                                      
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

/*************************** EOF **************************************/
