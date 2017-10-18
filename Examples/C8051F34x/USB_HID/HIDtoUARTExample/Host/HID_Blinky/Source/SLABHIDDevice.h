/////////////////////////////////////////////////////////////////////////////
// SLABHIDDevice.h
// SLABHIDDevice.dll Version 1.6
/////////////////////////////////////////////////////////////////////////////

#ifndef SLAB_HID_DEVICE_H
#define SLAB_HID_DEVICE_H

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>

/////////////////////////////////////////////////////////////////////////////
// DLL Preprocessor Definitions
/////////////////////////////////////////////////////////////////////////////

#ifdef SLAB_HID_DEVICE_EXPORTS
#define SLAB_HID_DEVICE_API __declspec(dllexport)
#elif defined(SLAB_HID_DEVICE_BUILD_SOURCE)
#define SLAB_HID_DEVICE_API
#else
#define SLAB_HID_DEVICE_API __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// Definitions
/////////////////////////////////////////////////////////////////////////////

// Return Codes
#define HID_DEVICE_SUCCESS					0x00
#define HID_DEVICE_NOT_FOUND				0x01
#define HID_DEVICE_NOT_OPENED				0x02
#define HID_DEVICE_ALREADY_OPENED			0x03
#define	HID_DEVICE_TRANSFER_TIMEOUT			0x04
#define HID_DEVICE_TRANSFER_FAILED			0x05
#define HID_DEVICE_CANNOT_GET_HID_INFO		0x06
#define HID_DEVICE_HANDLE_ERROR				0x07
#define HID_DEVICE_INVALID_BUFFER_SIZE		0x08
#define HID_DEVICE_SYSTEM_CODE				0x09
#define HID_DEVICE_UNSUPPORTED_FUNCTION		0x0A
#define HID_DEVICE_UNKNOWN_ERROR			0xFF

// Max number of USB Devices allowed
#define MAX_USB_DEVICES					64

// Max number of reports that can be requested at time
#define MAX_REPORT_REQUEST_XP			512
#define MAX_REPORT_REQUEST_2K			200

#define DEFAULT_REPORT_INPUT_BUFFERS	0

// String Types
#define HID_VID_STRING					0x01
#define HID_PID_STRING					0x02
#define HID_PATH_STRING					0x03
#define HID_SERIAL_STRING				0x04
#define HID_MANUFACTURER_STRING			0x05
#define HID_PRODUCT_STRING				0x06

// String Lengths
#define MAX_VID_LENGTH					5
#define MAX_PID_LENGTH					5
#define MAX_PATH_LENGTH					MAX_PATH
#define MAX_SERIAL_STRING_LENGTH		256
#define MAX_MANUFACTURER_STRING_LENGTH	256
#define MAX_PRODUCT_STRING_LENGTH		256
#define MAX_INDEXED_STRING_LENGTH		256
#define MAX_STRING_LENGTH				260

/////////////////////////////////////////////////////////////////////////////
// Typedefs
/////////////////////////////////////////////////////////////////////////////

typedef void* HID_DEVICE;

/////////////////////////////////////////////////////////////////////////////
// Exported Functions
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SLAB_HID_DEVICE_API	DWORD	WINAPI	HidDevice_GetNumHidDevices(WORD vid, WORD pid);
SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_GetHidString(DWORD deviceIndex, WORD vid, WORD pid, BYTE hidStringType, char* deviceString, DWORD deviceStringLength);
SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_GetHidIndexedString(DWORD deviceIndex, WORD vid, WORD pid, DWORD stringIndex, char* deviceString, DWORD deviceStringLength);
SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_GetHidAttributes(DWORD deviceIndex, WORD vid, WORD pid, WORD* deviceVid, WORD* devicePid, WORD* deviceReleaseNumber);
SLAB_HID_DEVICE_API	void	WINAPI	HidDevice_GetHidGuid(void* hidGuid);
SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_GetHidLibraryVersion(BYTE* major, BYTE* minor, BOOL* release);

SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_Open(HID_DEVICE* device, DWORD deviceIndex, WORD vid, WORD pid, DWORD numInputBuffers);
SLAB_HID_DEVICE_API	BOOL	WINAPI	HidDevice_IsOpened(HID_DEVICE device);
SLAB_HID_DEVICE_API	HANDLE	WINAPI	HidDevice_GetHandle(HID_DEVICE device);

SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_GetString(HID_DEVICE device, BYTE hidStringType, char* deviceString, DWORD deviceStringLength);
SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_GetIndexedString(HID_DEVICE device, DWORD stringIndex, char* deviceString, DWORD deviceStringLength);
SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_GetAttributes(HID_DEVICE device, WORD* deviceVid, WORD* devicePid, WORD* deviceReleaseNumber);

SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_SetFeatureReport_Control(HID_DEVICE device, BYTE* buffer, DWORD bufferSize);
SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_GetFeatureReport_Control(HID_DEVICE device, BYTE* buffer, DWORD bufferSize);
SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_SetOutputReport_Interrupt(HID_DEVICE device, BYTE* buffer, DWORD bufferSize);
SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_GetInputReport_Interrupt(HID_DEVICE device, BYTE* buffer, DWORD bufferSize, DWORD numReports, DWORD* bytesReturned);
SLAB_HID_DEVICE_API BYTE	WINAPI	HidDevice_SetOutputReport_Control(HID_DEVICE device, BYTE* buffer, DWORD bufferSize);
SLAB_HID_DEVICE_API BYTE	WINAPI	HidDevice_GetInputReport_Control(HID_DEVICE device, BYTE* buffer, DWORD bufferSize);

SLAB_HID_DEVICE_API	WORD	WINAPI	HidDevice_GetInputReportBufferLength(HID_DEVICE device);
SLAB_HID_DEVICE_API	WORD	WINAPI	HidDevice_GetOutputReportBufferLength(HID_DEVICE device);
SLAB_HID_DEVICE_API	WORD	WINAPI	HidDevice_GetFeatureReportBufferLength(HID_DEVICE device);
SLAB_HID_DEVICE_API	DWORD	WINAPI	HidDevice_GetMaxReportRequest(HID_DEVICE device);
SLAB_HID_DEVICE_API	BOOL	WINAPI	HidDevice_FlushBuffers(HID_DEVICE device);
SLAB_HID_DEVICE_API	BOOL	WINAPI	HidDevice_CancelIo(HID_DEVICE device);

SLAB_HID_DEVICE_API	void	WINAPI	HidDevice_GetTimeouts(HID_DEVICE device, DWORD* getReportTimeout, DWORD* setReportTimeout);
SLAB_HID_DEVICE_API	void	WINAPI	HidDevice_SetTimeouts(HID_DEVICE device, DWORD getReportTimeout, DWORD setReportTimeout);

SLAB_HID_DEVICE_API	BYTE	WINAPI	HidDevice_Close(HID_DEVICE device);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SLAB_HID_DEVICE_H
