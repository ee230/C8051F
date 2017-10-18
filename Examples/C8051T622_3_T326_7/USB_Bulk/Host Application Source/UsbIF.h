/************************************************************************
 *
 *  Module:       UsbIF.h
 *  Description:  CUsbIF base device class definition
 *  Company:      Silicon Laboratories Inc.
 *
 ************************************************************************/

#ifndef __UsbIF_H__
#define __UsbIF_H__

// get setup API functions (only available in Win98 and Win2K)
#include <setupapi.h>
// requires to link with setupapi.lib
// Link with SetupAPI.Lib.
#pragma comment (lib, "setupapi.lib")

#include "usb100.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////

class CDeviceListEntry
{								// class CDeviceListEntry
public:
	CDeviceListEntry(LPCTSTR linkname="", LPCTSTR friendlyname="", LPCTSTR serialnumber="");

	std::string	m_linkname;
	std::string	m_friendlyname;
	std::string	m_serialnumber;
};								// class CDeviceListEntry

bool IsWin98();

//
// CUsbIF
//
// This class implements the interface to the Silabs USB device driver.
//
class CUsbIF
{
public:
	// standard constructor
	CUsbIF();
	// destructor, should be virtual
	virtual ~CUsbIF();
//	std::vector<CDeviceListEntry> m_list;	// list of devices
	char	m_DeviceName[256];


// implementation
	void		SetGUID(GUID newGUID);
//	DWORD		ListDevices();
	DWORD		GetNumDevices();
	void		GetDeviceStrings(DWORD dwDeviceNum, CDeviceListEntry& dev);
	HANDLE		Open(DWORD dwDevice);
	HANDLE		OpenUSBfile(char* sFileName);
	void		GetSerialNumber(LPCTSTR sDevicePath, std::string* str);

protected:      
	GUID	m_GUID;
	
private:

}; // class CUsbIF

#endif // _UsbIF_h_

 
/*************************** EOF **************************************/
