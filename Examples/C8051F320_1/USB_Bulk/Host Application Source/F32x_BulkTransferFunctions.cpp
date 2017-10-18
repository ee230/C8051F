
#include "stdafx.h" 
#include "F32x_BulkFileTransferFunctions.h"

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
	F32x_STATUS	status = F32x_INVALID_HANDLE;

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
			//DWORD	dwNumBytesInQueue	= 0;
			//DWORD	dwQueueStatus		= F32x_RX_NO_OVERRUN;

			// Init. for no timeout case.
			/*status = F32x_SUCCESS;

			// If timeout set, wait n milliseconds for RX queue to fill.
			// If it does not fill before the timeout, return failure.
			if (sgdwReadTimeout > 0)
			{
				DWORD	dwStart	= GetTickCount();
				DWORD	dwEnd	= GetTickCount();

				status = F32x_CheckRXQueue(cyHandle, &dwNumBytesInQueue, &dwQueueStatus);

				while(((dwEnd - dwStart) < sgdwReadTimeout) && (!(dwQueueStatus & F32x_RX_READY)) && (status == F32x_SUCCESS))
				{
					status = F32x_CheckRXQueue(cyHandle, &dwNumBytesInQueue, &dwQueueStatus);
					dwEnd = GetTickCount();
				}

				if ((status == F32x_SUCCESS) && (!(dwQueueStatus & F32x_RX_READY)))
				{
					status = F32x_RX_QUEUE_NOT_READY;
				}
			}*/

			if (status == F32x_SUCCESS)
			{
				// Read transfer packet
				if(!ReadFile(cyHandle, lpBuffer, dwBytesToRead, lpdwBytesReturned, NULL))
				{	// Device IO failed.
					status = F32x_READ_ERROR;
				}
			}
		}
		else
			status = F32x_INVALID_REQUEST_LENGTH;
	}

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