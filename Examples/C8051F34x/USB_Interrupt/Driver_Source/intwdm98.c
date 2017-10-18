/*++


Module Name:

    intwdm98.c

Abstract:

	This file contains routines to handle functions not
	implemented in the Windows 98 version of the WDM. 
	This file only needs to be included in the build if 
	you are building a Windows 98 driver.

Environment:

    Kernel mode

Notes:


--*/#undef POOLTAGGING
#include <wdm.h>
#include "intwdm98.h"


///////////////////////////////////////////////////////////////////////////////

VOID
CallWorkItemProcessingRoutine(
	IN PIO_WORKITEM item
	)
/*++
 
Routine Description:

    This routine is called in Windows 98 only to call the specified routine
	to process this work item.

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet

Return Value:

    Void

--*/
{							
	(*item->Routine)(item->DeviceObject, item->Context);
	ObDereferenceObject(item->DeviceObject);
}	
///////////////////////////////////////////////////////////////////////////////

PIO_WORKITEM 
AllocateWorkItem98(
	PDEVICE_OBJECT DeviceObject
	)
/*++
 
Routine Description:

    This routine is called in Windows 98 only to Allocate a work item.

Arguments:

    DeviceObject - pointer to device object
  
Return Value:

    Pointer to a newly allocated work item.

--*/
{							// AllocateWorkItem
	PIO_WORKITEM item = (PIO_WORKITEM) ExAllocatePool(NonPagedPool, sizeof(IO_WORKITEM));
	item->DeviceObject = DeviceObject;
	return item;
}							// AllocateWorkItem


///////////////////////////////////////////////////////////////////////////////

VOID
FreeWorkItem98(
	PIO_WORKITEM item
	)
/*++
 
Routine Description:

    This routine is called in Windows 98 only to free a work item.

Arguments:

    item - pointer to work item to be freed
    

Return Value:

    VOID

--*/
{							// FreeWorkItem
	ExFreePool(item);
}							// FreeWorkItem

///////////////////////////////////////////////////////////////////////////////

VOID
QueueWorkItem98(
	PIO_WORKITEM item, 
	PIO_WORKITEM_ROUTINE Routine, 
	WORK_QUEUE_TYPE QueueType, 
	PVOID Context
	)
/*++
 
Routine Description:

    This routine is called in Windows 98 only to queue a work item.

Arguments:

    DeviceObject - pointer to device object
    item - pointer to work item to be queued
	Routine - pointer to routine which processes the work item
	QueueType - work queue type specifier
	Context - work item context
	
Return Value:

    VOID

--*/
{							// QueueWorkItem
	ExInitializeWorkItem(&(item->super), (PWORKER_THREAD_ROUTINE) CallWorkItemProcessingRoutine, &(item->super));
	item->Context = Context;
	item->Routine = Routine;
	ObReferenceObject(item->DeviceObject);
	ExQueueWorkItem(&(item->super), QueueType);
}							// QueueWorkItem

 
