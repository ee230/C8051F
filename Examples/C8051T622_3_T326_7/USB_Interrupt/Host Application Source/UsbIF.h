/************************************************************************
 *
 *  Module:       UsbIF.h
 *  Description:  CUsbIF base device class definition
 *  Company:      Silicon Laboratories
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

// implementation
	void SetGUID(GUID newGUID);
	BOOL OpenUsbDevice();
	HANDLE open_file( char *filename);

protected:      
	GUID	m_GUID;
	char	m_DeviceName[256];

private:

}; // class CUsbIF


#endif // _UsbIF_h_
 
/*************************** EOF **************************************/
