
#include "wdm.h"
#include "intwdm98.h"


typedef VOID (NTAPI *PIO_WORKITEM_ROUTINE)(PDEVICE_OBJECT DeviceObject, PVOID Context);

struct _IO_WORKITEM : public _WORK_QUEUE_ITEM {
	PDEVICE_OBJECT DeviceObject;
	PIO_WORKITEM_ROUTINE Routine;
	PVOID Context;
	};
typedef struct _IO_WORKITEM IO_WORKITEM, *PIO_WORKITEM;

///////////////////////////////////////////////////////////////////////////////

VOID CallbackWrapper(PIO_WORKITEM item)
	{							// CallbackWrapper
	(*item->Routine)(item->DeviceObject, item->Context);
	ObDereferenceObject(item->DeviceObject);
	}	
///////////////////////////////////////////////////////////////////////////////

PIO_WORKITEM AllocateWorkItem(PDEVICE_OBJECT DeviceObject)
	{							// AllocateWorkItem
	PIO_WORKITEM item = (PIO_WORKITEM) ExAllocatePool(NonPagedPool, sizeof(_IO_WORKITEM));
	item->DeviceObject = DeviceObject;
	return item;
	}							// AllocateWorkItem


///////////////////////////////////////////////////////////////////////////////

VOID FreeWorkItem(PIO_WORKITEM item)
	{							// FreeWorkItem
	ExFreePool(item);
	}							// FreeWorkItem

///////////////////////////////////////////////////////////////////////////////

VOID QueueWorkItem(PIO_WORKITEM item, PIO_WORKITEM_ROUTINE Routine, WORK_QUEUE_TYPE QueueType, PVOID Context)
	{							// QueueWorkItem
	ExInitializeWorkItem(item, (PWORKER_THREAD_ROUTINE) CallbackWrapper, item);
	item->Context = Context;
	item->Routine = Routine;
	ObReferenceObject(item->DeviceObject);
	ExQueueWorkItem(item, QueueType);
	}							// QueueWorkItem
