/*++


Module Name:

    intusb.h

Abstract:

Environment:

    Kernel mode

Notes:


--*/

#include <initguid.h>
#include <wdm.h>
#include <wmilib.h>
#include <wmistr.h>
#include "usbdi.h"
#include "usbdlib.h"
#include "usbioctl.h"

// Remove the comment from the following line in order to build a version of
// CygInt that works in Windows 98. 
//#define WIN98DRIVER


// Global transfer size definition
#define MAX_TRANSFER_SIZE				0x40


#ifndef _INTUSB_H
#define _INTUSB_H

#define INTTAG (ULONG) 'KluB'
#define DRIVERNAME "Silabs Int"				// for use in messages

#undef ExAllocatePool
#define ExAllocatePool(type, size) \
    ExAllocatePoolWithTag(type, size, INTTAG);

#if DBG

#define IntUsb_DbgPrint(level, _x_) \
            if((level) <= DebugLevel) { \
                DbgPrint _x_; \
            }

#else

#define IntUsb_DbgPrint(level, _x_)

#endif

typedef struct _GLOBALS {

    UNICODE_STRING IntUsb_RegistryPath;

} GLOBALS;

#define IDLE_INTERVAL 5000


typedef enum _DEVSTATE {

    NotStarted,         // not started
    Stopped,            // device stopped
    Working,            // started and working
    PendingStop,        // stop pending
    PendingRemove,      // remove pending
    SurpriseRemoved,    // removed by surprise
    Removed             // removed

} DEVSTATE;

typedef enum _QUEUE_STATE {

    HoldRequests,       // device is not started yet
    AllowRequests,      // device is ready to process
    FailRequests        // fail both existing and queued up requests

} QUEUE_STATE;

typedef enum _WDM_VERSION {

    WinXpOrBetter,
    Win2kOrBetter,
    WinMeOrBetter,
    Win98OrBetter

} WDM_VERSION;

#define INITIALIZE_PNP_STATE(_Data_)    \
        (_Data_)->DeviceState =  NotStarted;\
        (_Data_)->PrevDevState = NotStarted;

#define SET_NEW_PNP_STATE(_Data_, _state_) \
        (_Data_)->PrevDevState =  (_Data_)->DeviceState;\
        (_Data_)->DeviceState = (_state_);

#define RESTORE_PREVIOUS_PNP_STATE(_Data_)   \
        (_Data_)->DeviceState =   (_Data_)->PrevDevState;


#define INTUSB_MAX_TRANSFER_SIZE   256
#define INTUSB_TEST_BOARD_TRANSFER_BUFFER_SIZE (MAX_TRANSFER_SIZE )

//
// registry path used for parameters 
// global to all instances of the driver
//

#define INTUSB_REGISTRY_PARAMETERS_PATH  \
	L"\\REGISTRY\\Machine\\System\\CurrentControlSet\\SERVICES\\INTUSB\\Parameters"


typedef struct _INTUSB_PIPE_CONTEXT {

    BOOLEAN PipeOpen;

} INTUSB_PIPE_CONTEXT, *PINTUSB_PIPE_CONTEXT;


//
// A structure representing the instance information associated with
// this particular device.
//

typedef struct _DEVICE_EXTENSION {

    // Functional Device Object
    PDEVICE_OBJECT FunctionalDeviceObject;

    // Device object we call when submitting Urbs
    PDEVICE_OBJECT TopOfStackDeviceObject;

    // The bus driver object
    PDEVICE_OBJECT PhysicalDeviceObject;

    // Name buffer for our named Functional device object link
    // The name is generated based on the driver's class GUID
    UNICODE_STRING InterfaceName;

    // Bus drivers set the appropriate values in this structure in response
    // to an IRP_MN_QUERY_CAPABILITIES IRP. Function and filter drivers might
    // alter the capabilities set by the bus driver.
    DEVICE_CAPABILITIES DeviceCapabilities;

    // Configuration Descriptor
    PUSB_CONFIGURATION_DESCRIPTOR UsbConfigurationDescriptor;

    // Interface Information structure
    PUSBD_INTERFACE_INFORMATION UsbInterface;

    // Pipe context for the intusb driver
    PINTUSB_PIPE_CONTEXT PipeContext;

    // current state of device
    DEVSTATE DeviceState;

    // state prior to removal query
    DEVSTATE PrevDevState;

    // obtain and hold this lock while changing the device state,
    // the queue state and while processing the queue.
    KSPIN_LOCK DevStateLock;

    // current system power state
    SYSTEM_POWER_STATE SysPower;

    // current device power state
    DEVICE_POWER_STATE DevPower;

    // Pending I/O queue state
    QUEUE_STATE QueueState;

    // Pending I/O queue
    LIST_ENTRY NewRequestsQueue;

    // I/O Queue Lock
    KSPIN_LOCK QueueLock;

    KEVENT RemoveEvent;

    KEVENT StopEvent;
    
    ULONG OutStandingIO;

    KSPIN_LOCK IOCountLock;

    // selective suspend variables

    LONG SSEnable;

    LONG SSRegistryEnable;

    PUSB_IDLE_CALLBACK_INFO IdleCallbackInfo;
	
    PIRP PendingIdleIrp;
	
    LONG IdleReqPend;

    LONG FreeIdleIrpCount;

    KSPIN_LOCK IdleReqStateLock;

    KEVENT NoIdleReqPendEvent;

    // default power state to power down to on self-susped
    ULONG PowerDownLevel;
    
    // remote wakeup variables
    PIRP WaitWakeIrp;

    LONG FlagWWCancel;

    LONG FlagWWOutstanding;

    LONG WaitWakeEnable;

    // open handle count
    LONG OpenHandleCount;

    // selective suspend model uses timers, dpcs and work item.
    KTIMER Timer;

    KDPC DeferredProcCall;

    // This event is cleared when a DPC/Work Item is queued.
    // and signaled when the work-item completes.
    // This is essential to prevent the driver from unloading
    // while we have DPC or work-item queued up.
    KEVENT NoDpcWorkItemPendingEvent;

    // WMI information
    WMILIB_CONTEXT WmiLibInfo;

    // WDM version
    WDM_VERSION WdmVersion;

	PIRP InterruptIrp;						// the IOCTL that's waiting for an interrupt
	PIRP PollingIrpIN;						// IRP used to poll for interrupts
	PURB PollingUrbIN;						// URB used to poll for interrupts
	KSPIN_LOCK polllock;					// lock for managing polling IRP
	BOOLEAN pollpending;					// polling irp is pending
	IO_REMOVE_LOCK RemoveLock;				// removal control locking structure
	USBD_PIPE_HANDLE hintpipeIN;
	USBD_PIPE_HANDLE hintpipeOUT;

	LIST_ENTRY Pending_IOCTL_READINT_List;			// list of asynchronous IOCTLs
	
	UCHAR intdataIN[MAX_TRANSFER_SIZE];						// interrupt data
	LONG NumCachedRequests;
	LONG intdataLength;							// interrupt data transfer length
	LONG numintsIN;							// number of pending interrupts
	PVOID powercontext;						// context value to use in "save/restore" notification

	KSPIN_LOCK Cachelock;					// lock for managing the IO control cache
	
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


typedef struct _IRP_COMPLETION_CONTEXT {

    PDEVICE_EXTENSION DeviceExtension;

    PKEVENT Event;

} IRP_COMPLETION_CONTEXT, *PIRP_COMPLETION_CONTEXT;

extern GLOBALS Globals;
extern ULONG DebugLevel;


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING UniRegistryPath
    );

VOID
IntUsb_DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
IntUsb_AddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );

PIO_WORKITEM AllocateWorkItem(PDEVICE_OBJECT DeviceObject);
VOID FreeWorkItem(PIO_WORKITEM item);
VOID QueueWorkItem(PIO_WORKITEM pIOWorkItem, PIO_WORKITEM_ROUTINE Routine, WORK_QUEUE_TYPE QueueType, PVOID Context);

#endif
