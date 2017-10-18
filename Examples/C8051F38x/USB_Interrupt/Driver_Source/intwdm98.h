/*++


Module Name:

    intwdm98.h

Abstract:

Environment:

    Kernel mode

Notes:

 
--*/

typedef VOID (NTAPI *PIO_WORKITEM_ROUTINE)(PDEVICE_OBJECT DeviceObject, PVOID Context);

struct _IO_WORKITEM {
	WORK_QUEUE_ITEM super;			// parent structure from which we inherit the WORK_QUEUE_ITEM structure
	PDEVICE_OBJECT DeviceObject;
	PIO_WORKITEM_ROUTINE Routine;
	PVOID Context;
	};


#undef PIO_WORKITEM

typedef struct _IO_WORKITEM IO_WORKITEM, *PIO_WORKITEM;

PIO_WORKITEM 
AllocateWorkItem98(
	IN PDEVICE_OBJECT DeviceObject
	);

VOID 
FreeWorkItem98(
	IN PIO_WORKITEM item
	);

VOID 
QueueWorkItem98(
	IN PIO_WORKITEM pIOWorkItem,
	IN PIO_WORKITEM_ROUTINE Routine, 
	IN WORK_QUEUE_TYPE QueueType, 
	IN PVOID Context
	);

VOID 
CallWorkItemProcessingRoutine(
	IN PIO_WORKITEM item
	);