 /*++


Module Name:

    intrwr.c

Abstract:

    This file has routines to perform reads and writes.
    The read and writes are for int transfers.

Environment:

    Kernel mode

Notes:

    
--*/

#include "intusb.h"
#include "intpnp.h"
#include "intpwr.h"
#include "intdev.h"
#include "intrwr.h"
#include "intwmi.h"
#include "intusr.h"


///////////////////////////////////////////////////////////////////////////////

//#pragma PAGEDCODE
#pragma LOCKEDCODE

NTSTATUS 
CreateInterruptUrbIN(
	IN PDEVICE_OBJECT DeviceObject
	)
/*++
 
Routine Description:

    Create the input URB Polling Interrupt.

Arguments:

    DeviceObject - pointer to DeviceObject
    
Return Value:

    NT status value

--*/
	{							// CreateInterruptUrb
	PIRP IrpIN;
	PURB urbIN;
	PDEVICE_EXTENSION      deviceExtension;
	KdPrint( (DRIVERNAME " - CreateInterruptUrbIN - begins %8.8lX\n"));

	deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
	ASSERT(deviceExtension->PollingIrpIN == NULL);
	ASSERT(deviceExtension->PollingUrbIN == NULL);

	IrpIN = IoAllocateIrp(deviceExtension->TopOfStackDeviceObject->StackSize, FALSE);
	if (!IrpIN)
		{
		KdPrint((DRIVERNAME " - Unable to create IRP for interrupt polling\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
		}

	urbIN = (PURB) ExAllocatePool(NonPagedPool, sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));
	if (!urbIN)
		{
		KdPrint((DRIVERNAME " - Unable to allocate interrupt polling URB\n"));
		IoFreeIrp(IrpIN);
		return STATUS_INSUFFICIENT_RESOURCES;
		}
	
	deviceExtension->PollingIrpIN = IrpIN;
	deviceExtension->PollingUrbIN = urbIN;
	
	return STATUS_SUCCESS;
	}							// CreateInterruptUrb


///////////////////////////////////////////////////////////////////////////////

#pragma PAGEDCODE

VOID 
DeleteInterruptUrbIN(
	IN PDEVICE_OBJECT DeviceObject
	)
/*++
 
Routine Description:

    Delete the input URB Polling Interrupt.

Arguments:

    DeviceObject - pointer to DeviceObject
    
Return Value:

    NT status value

--*/
	{							// DeleteInterruptUrb
	PDEVICE_EXTENSION deviceExtension;

	KdPrint( (DRIVERNAME " - DeleteInterruptUrbIN - begins %8.8lX\n"));

	deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
	ASSERT(deviceExtension->PollingIrpIN != NULL);
	ASSERT(deviceExtension->PollingUrbIN != NULL);
	
	ExFreePool(deviceExtension->PollingUrbIN);
	IoFreeIrp(deviceExtension->PollingIrpIN);
	deviceExtension->PollingIrpIN = NULL;
	deviceExtension->PollingUrbIN = NULL;
	
	}							// DeleteInterruptUrb


///////////////////////////////////////////////////////////////////////////////

#pragma LOCKEDCODE

NTSTATUS 
StartInterruptUrbIN(
	IN PDEVICE_EXTENSION deviceExtension
	)
/*++
 
Routine Description:

    Submit the Polling IRP with associated URB to the bus driver.
	The URB is formatted for an MAX_TRANSFER_SIZE byte transfer on the interrupt 
	intput pipe with the USBD_SHORT_TRANSFER_OK flag set so any transfer
	from 0 to MAX_TRANSFER_SIZE bytes may be used.

Arguments:

    deviceExtension - Pointer to the device Extension

Return Value:

    NT status value

--*/
	{							// StartInterruptUrb

	NTSTATUS ReturnStatus;
	BOOLEAN startirp;
	KIRQL oldirql;
	PIRP IrpIN;
	PURB urbIN;
	NTSTATUS status;
	PIO_STACK_LOCATION stackIN;

	//KdPrint( (DRIVERNAME " - StartInterruptUrbIN - begins %8.8lX\n"));
	
	// Check to see if a Poll is already pending before submitting 
	// request to the bus.

	KeAcquireSpinLock(&deviceExtension->polllock, &oldirql);
	if (deviceExtension->pollpending)
		startirp = FALSE;
	else
		startirp = TRUE, deviceExtension->pollpending = TRUE;
	KeReleaseSpinLock(&deviceExtension->polllock, oldirql);

	if (!startirp)
		return STATUS_DEVICE_BUSY;	// already pending

	IrpIN = deviceExtension->PollingIrpIN;
	urbIN = deviceExtension->PollingUrbIN;
	ASSERT(IrpIN && urbIN);
	
	// Acquire the remove lock the device cannot be removed while the IRP
	// is active.

	status = IoAcquireRemoveLock(&deviceExtension->RemoveLock, IrpIN);
	if (!NT_SUCCESS(status))
		{
		deviceExtension->pollpending = 0;
		return status;
		}

	// Initialize the URB we use for reading the interrupt pipe

	deviceExtension->hintpipeIN = deviceExtension->UsbInterface->Pipes[0].PipeHandle;

	UsbBuildInterruptOrBulkTransferRequest(urbIN, sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
		deviceExtension->hintpipeIN, &deviceExtension->intdataIN, NULL, MAX_TRANSFER_SIZE, USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK, NULL);

	// Install "OnInterruptIN" as the completion routine for the polling IRP.
	
	IoSetCompletionRoutine(IrpIN, (PIO_COMPLETION_ROUTINE) OnInterruptIN, deviceExtension, TRUE, TRUE, TRUE);

	// Initialize the IRP for an internal control request

	stackIN = IoGetNextIrpStackLocation(IrpIN);
	stackIN->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	stackIN->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
	stackIN->Parameters.Others.Argument1 = urbIN;

	// Make sure the Cancel Flag is cleared

	IrpIN->Cancel = FALSE;

	status = IoCallDriver(deviceExtension->TopOfStackDeviceObject, IrpIN);
	
	return status;
	}							// StartInterruptUrb


///////////////////////////////////////////////////////////////////////////////

#pragma LOCKEDCODE
	
VOID 
StopInterruptUrbIN(
	IN PDEVICE_EXTENSION deviceExtension
	)
/*++
 
Routine Description:

    This routine cancels the Polling IRP

Arguments:

    deviceExtension - Pointer to the device Extension


--*/
	{							// StopInterruptUrb

	KdPrint( (DRIVERNAME " - StopInterruptUrbIN - begins %8.8lX\n"));
	
	if (deviceExtension->pollpending)
		IoCancelIrp(deviceExtension->PollingIrpIN);
	}							// StopInterruptUrb

///////////////////////////////////////////////////////////////////////////////



#pragma LOCKEDCODE

NTSTATUS 
OnInterruptIN(
	IN PDEVICE_OBJECT junk, 
	IN PIRP IrpIN, 
	IN PDEVICE_EXTENSION deviceExtension
	)
/*++
 
Routine Description:

    This routine is the completion routine for the Polling IRP submitted in
	the StartInterruptIN routine. If the IRP was successful this routine will
	transfer the input data to the Application by means of an IO Control request.
	An the IO Control request must already be submitted and outstanding before this 
	routine is called in order to be serviced.

Arguments:

    DeviceObject - pointer to DeviceObject
    IrpIN - Polling input pipe Irp to be serviced.
	deviceExtension - Pointer to the device Extension

Return Value:

    NT status value

--*/
	{							// OnInterrupt

	KIRQL oldirql;
	PVOID powercontext;
	PIRP intirpIN;
	IO_REMOVE_LOCK localRemoveLock;
	
	//KdPrint( (DRIVERNAME " - OnInterruptIN - begins %8.8lX\n"));
	
	KeAcquireSpinLock(&deviceExtension->polllock, &oldirql);
	deviceExtension->pollpending = FALSE;		// allow another poll to be started
	powercontext = deviceExtension->powercontext;
	deviceExtension->powercontext = NULL;
	KeReleaseSpinLock(&deviceExtension->polllock, oldirql);

	// If the poll completed successfully,answer the IOCTL request sent by the Windows
	// application and reissue the read. We're trying to have a read outstanding on the
	// interrupt pipe when the device is running.

	if (NT_SUCCESS(IrpIN->IoStatus.Status))
		{						// an interrupt has occurred
		//KdPrint((DRIVERNAME " - Interrupt IN!\n"));
		// Get the length of the transfer from the URB we used for this request
		InterlockedExchange(&(deviceExtension->intdataLength),deviceExtension->PollingUrbIN->UrbBulkOrInterruptTransfer.TransferBufferLength);  
		
		intirpIN = UncacheReadRequest(deviceExtension, &deviceExtension->InterruptIrp);
		if (intirpIN)
		{
			// Exchange data between the Polling URB and the Servicing IRP
			RtlCopyMemory(intirpIN->AssociatedIrp.SystemBuffer,&deviceExtension->intdataIN,deviceExtension->intdataLength);
			RtlZeroMemory(&deviceExtension->intdataIN, MAX_TRANSFER_SIZE);
			
			// Complete the request and indicate the length
			// in the information field.
			
        	CompleteRequest(intirpIN, STATUS_SUCCESS, deviceExtension->intdataLength);
		}
		else
			// Interrupt occurred but was not serviced
			InterlockedIncrement(&deviceExtension->numintsIN);

		// Reissue the polling IRP. 
		StartInterruptUrbIN(deviceExtension); // issue next polling request
		}						// device signalled an interrupt
#if DBG	
	else
		{
		KdPrint((DRIVERNAME " - Interrupt polling IRP IN %X failed - %X (USBD status %X)\n",
			IrpIN, IrpIN->IoStatus.Status, URB_STATUS(deviceExtension->PollingUrbIN)));
		}
#endif

	// Release RemoveLock which was acquired in StartInterruptUrbIN
	IoReleaseRemoveLock(&deviceExtension->RemoveLock, IrpIN); 

	return STATUS_MORE_PROCESSING_REQUIRED;
	}							// OnInterrupt



///////////////////////////////////////////////////////////////////////////////

#pragma LOCKEDCODE

NTSTATUS 
CompleteRequest(
	IN PIRP Irp, 
	IN NTSTATUS status, 
	IN ULONG_PTR info
	)
/*++
 
Routine Description:

    Complete the Request with no priority boost since we are not waiting on IO.

Arguments:

    Irp - IRP to be completed
	status - IRP completion status
	info - information associated with the IRP

Return Value:

    NT status value

--*/
{							
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}							


PINTUSB_PIPE_CONTEXT
IntUsb_PipeWithName(
    IN PDEVICE_OBJECT  DeviceObject,
    IN PUNICODE_STRING FileName
    )
/*++
 
Routine Description:

    This routine will pass the string pipe name and
    fetch the pipe number.

Arguments:

    DeviceObject - pointer to DeviceObject
    FileName - string pipe name

Return Value:

    The device extension maintains a pipe context for 
    the pipes on the board.
    This routine returns the pointer to this context in
    the device extension for the "FileName" pipe.

--*/
{
    LONG                  ix;
    ULONG                 uval; 
    ULONG                 nameLength;
    ULONG                 umultiplier;
    PDEVICE_EXTENSION     deviceExtension;
    PINTUSB_PIPE_CONTEXT pipeContext;

    //
    // initialize variables
    //
    pipeContext = NULL;
    //
    // typedef WCHAR *PWSTR;
    //
    nameLength = (FileName->Length / sizeof(WCHAR));
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    KdPrint( ("IntUsb_PipeWithName - begins\n"));

    if(nameLength != 0) {
    
        KdPrint( ("Filename = %ws nameLength = %d\n", FileName->Buffer, nameLength));

        //
        // Parse the pipe#
        //
        ix = nameLength - 1;

        // if last char isn't digit, decrement it.
        while((ix > -1) &&
              ((FileName->Buffer[ix] < (WCHAR) '0')  ||
               (FileName->Buffer[ix] > (WCHAR) '9')))             {

            ix--;
        }

        if(ix > -1) {

            uval = 0;
            umultiplier = 1;

            // traversing least to most significant digits.

            while((ix > -1) &&
                  (FileName->Buffer[ix] >= (WCHAR) '0') &&
                  (FileName->Buffer[ix] <= (WCHAR) '9'))          {
        
                uval += (umultiplier *
                         (ULONG) (FileName->Buffer[ix] - (WCHAR) '0'));

                ix--;
                umultiplier *= 10;
            }
        }

        if(uval < 6 && deviceExtension->PipeContext) {
        
            pipeContext = &deviceExtension->PipeContext[uval];
        }
    }

    KdPrint( ("IntUsb_PipeWithName - ends\n"));

    return pipeContext;
}

NTSTATUS
IntUsb_DispatchWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    Dispatch routine for write.
    This routine creates a INTUSB_RW_CONTEXT for a write.
    
Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet

Return Value:

    NT status value

--*/
{
    PURB                   urb;
    ULONG                  Length;
    ULONG                  urbFlags;
    BOOLEAN                read;
    NTSTATUS               ntStatus;
    PFILE_OBJECT           fileObject;
    PDEVICE_EXTENSION      deviceExtension;
    PIO_STACK_LOCATION     irpStack;
    PIO_STACK_LOCATION     nextStack;
    PINTUSB_RW_CONTEXT    rwContext;
    PUSBD_PIPE_INFORMATION pipeInformation;
	PIO_STACK_LOCATION		IrpStack;

    //
    // initialize variables
    //
    urb = NULL;
    rwContext = NULL;
    Length = 0;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    fileObject = irpStack->FileObject;
    read = (irpStack->MajorFunction == IRP_MJ_READ) ? TRUE : FALSE;
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
	IrpStack = IoGetCurrentIrpStackLocation(Irp);
	urbFlags = USBD_SHORT_TRANSFER_OK;
	

    KdPrint( ("IntUsb_DispatchWrite - begins\n"));
	
	

    if(deviceExtension->DeviceState != Working) {

        KdPrint( ("Invalid device state\n"));

        ntStatus = STATUS_INVALID_DEVICE_STATE;
        goto IntUsb_DispatchReadWrite_Exit;
    }

    //
    // It is true that the client driver cancelled the selective suspend
    // request in the dispatch routine for create Irps.
    // But there is no guarantee that it has indeed completed.
    // so wait on the NoIdleReqPendEvent and proceed only if this event
    // is signalled.
    //
    KdPrint( ("Waiting on the IdleReqPendEvent\n"));
    
    //
    // make sure that the selective suspend request has been completed.
    //

    if(deviceExtension->SSEnable) {

        KeWaitForSingleObject(&deviceExtension->NoIdleReqPendEvent, 
                              Executive, 
                              KernelMode, 
                              FALSE, 
                              NULL);
    }

    if(fileObject && fileObject->FsContext) {

        pipeInformation = fileObject->FsContext;

        if(UsbdPipeTypeInterrupt != pipeInformation->PipeType) {
            
            KdPrint( ("Usbd pipe type is not int\n"));

            ntStatus = STATUS_INVALID_HANDLE;
            goto IntUsb_DispatchReadWrite_Exit;
        }
    }
    else {

        KdPrint( ("Invalid handle\n"));

        ntStatus = STATUS_INVALID_HANDLE;
        goto IntUsb_DispatchReadWrite_Exit;
    }

    rwContext = (PINTUSB_RW_CONTEXT)
                ExAllocatePool(NonPagedPool,
                               sizeof(INTUSB_RW_CONTEXT));

    if(rwContext == NULL) {
        
        KdPrint( ("Failed to alloc mem for rwContext\n"));

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto IntUsb_DispatchReadWrite_Exit;
    }

	// Get the length from the current Write Request
    if(Irp->AssociatedIrp.SystemBuffer) {

        Length = IrpStack->Parameters.Write.Length;
    }

    if(Length > INTUSB_TEST_BOARD_TRANSFER_BUFFER_SIZE) {

        KdPrint( ("Transfer length > buffer\n"));

        ntStatus = STATUS_INVALID_PARAMETER;

        ExFreePool(rwContext);

        goto IntUsb_DispatchReadWrite_Exit;
    }

    urbFlags |= USBD_TRANSFER_DIRECTION_OUT;
    KdPrint( ("Write operation\n"));
    

    
    
    urb = ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));

    if(urb == NULL) {

        KdPrint( ("Failed to alloc mem for urb\n"));

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;

        ExFreePool(rwContext);
        
        goto IntUsb_DispatchReadWrite_Exit;
    }

    UsbBuildInterruptOrBulkTransferRequest(
                            urb,
                            sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                            pipeInformation->PipeHandle,
                            Irp->AssociatedIrp.SystemBuffer,
                            NULL,
                            Length,
                            urbFlags,
                            NULL);

    //
    // set INTUSB_RW_CONTEXT parameters.
    //
    
    rwContext->Urb             = urb;
    rwContext->Length          = Length;
    rwContext->Numxfer         = 0;
    rwContext->DeviceExtension = deviceExtension;

    //
    // use the original read/write irp as an internal device control irp
    //

    nextStack = IoGetNextIrpStackLocation(Irp);
    nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    nextStack->Parameters.Others.Argument1 = (PVOID) urb;
    nextStack->Parameters.DeviceIoControl.IoControlCode = 
                                             IOCTL_INTERNAL_USB_SUBMIT_URB;

    IoSetCompletionRoutine(Irp, 
                           (PIO_COMPLETION_ROUTINE)IntUsb_WriteCompletion,
                           rwContext,
                           TRUE,
                           TRUE,
                           TRUE);

    //
    // since we return STATUS_PENDING call IoMarkIrpPending.
    //

    IoMarkIrpPending(Irp);

    KdPrint( ("IntUsb_DispatchReadWrite::"));
    IntUsb_IoIncrement(deviceExtension);

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject,
                            Irp);

    if(!NT_SUCCESS(ntStatus)) {

        KdPrint( ("IoCallDriver fails with status %X\n", ntStatus));

        //
        // if the device was removed, then the pipeInformation 
        // field is invalid.
        // similarly if the request was cancelled, then we need not
        // invoked reset pipe/device.
        //
        if((ntStatus != STATUS_CANCELLED) && 
           (ntStatus != STATUS_DEVICE_NOT_CONNECTED)) {
            
            ntStatus = IntUsb_ResetPipe(DeviceObject,
                                     pipeInformation);
    
            if(!NT_SUCCESS(ntStatus)) {

                KdPrint( ("IntUsb_ResetPipe failed\n"));

                ntStatus = IntUsb_ResetDevice(DeviceObject);
            }
        }
        else {

            KdPrint( ("ntStatus is STATUS_CANCELLED or "
                                 "STATUS_DEVICE_NOT_CONNECTED\n"));
        }
    }

    //
    // we return STATUS_PENDING and not the status returned by the lower layer.
    //
    return STATUS_PENDING;

IntUsb_DispatchReadWrite_Exit:

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    KdPrint( ("IntUsb_DispatchReadWrite - ends\n"));

    return ntStatus;
}

NTSTATUS
IntUsb_WriteCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    )
/*++
 
Routine Description:

    This is the completion routine for writes
    
Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet
    Context - context passed to the completion routine.

Return Value:

    NT status value

--*/
{
    ULONG               stageLength;
    NTSTATUS            ntStatus;
    PIO_STACK_LOCATION  nextStack;
    PINTUSB_RW_CONTEXT rwContext;

    //
    // initialize variables
    //
    rwContext = (PINTUSB_RW_CONTEXT) Context;
    ntStatus = Irp->IoStatus.Status;

    UNREFERENCED_PARAMETER(DeviceObject);
    KdPrint( ("IntUsb_WriteCompletion - begins\n"));

	//
    // successfully performed a transfer.
    //
    if(NT_SUCCESS(ntStatus)) 
		Irp->IoStatus.Information = rwContext->Urb->UrbBulkOrInterruptTransfer.TransferBufferLength;
	else
		KdPrint( ("ReadWriteCompletion - failed with status = %X\n", 
							ntStatus));
        
    if(rwContext) {

        //
        // dump rwContext
        //
        KdPrint( ("rwContext->Urb             = %X\n", 
                             rwContext->Urb));
        KdPrint( ("rwContext->Mdl             = %X\n", 
                             rwContext->Mdl));
        KdPrint( ("rwContext->Length          = %d\n", 
                             rwContext->Length));
        KdPrint( ("rwContext->Numxfer         = %d\n", 
                             rwContext->Numxfer));
        KdPrint( ("rwContext->DeviceExtension = %X\n", 
                             rwContext->DeviceExtension));

        KdPrint( ("IntUsb_ReadWriteCompletion::"));
        IntUsb_IoDecrement(rwContext->DeviceExtension);

        ExFreePool(rwContext->Urb);
        ExFreePool(rwContext);
    }

    KdPrint( ("IntUsb_WriteCompletion - ends\n"));

    return ntStatus;
}

NTSTATUS
IntUsb_DispatchRead(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    Dispatch routine for read requests.
    
Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet

Return Value:

    NT status value

--*/
{
	NTSTATUS				status;
	PDEVICE_EXTENSION       deviceExtension;
	UCHAR					localBuffer[12];
	ULONG info = 0;


	KdPrint( ("IntUsb_DispatchRead - begins\n"));

	deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

	IntUsb_IoIncrement(deviceExtension);


	// If an interrupt request is already outstanding, use that interrupt to service
	// this IRP instead of waiting for another interrupt.
	// The deviceExtension variable numintsIN tracks the number of interrupts 
	// which have occurred but not been serviced.

	if (InterlockedDecrement(&deviceExtension->numintsIN) >= 0)
	{					
		// Exchange data between the Polling URB and the Servicing IRP
		RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer,&deviceExtension->intdataIN,deviceExtension->intdataLength);
		RtlZeroMemory(&deviceExtension->intdataIN, MAX_TRANSFER_SIZE);
		info  = deviceExtension->intdataLength;  // Length is always 12 bytes when successful
		       		
		status = STATUS_SUCCESS;
	}					
	else		// Pend this Read Request until an interrupt occurs
	{
		InterlockedIncrement(&deviceExtension->numintsIN);
		if(deviceExtension->NumCachedRequests <= 4)
			status = CacheReadRequest(deviceExtension, Irp, &deviceExtension->InterruptIrp);
		else
			status = STATUS_DEVICE_BUSY;
	}
	
	//IoCompleteRequest(Irp, IO_NO_INCREMENT);

    KdPrint( ("IntUsb_DispatchRead - ends\n"));
    IntUsb_IoDecrement(deviceExtension);


    return status == STATUS_PENDING ? status : CompleteRequest(Irp, status, info);;
}


PIRP UncacheReadRequest(
	PDEVICE_EXTENSION DeviceExtension, 
	PIRP* pIrp
	)
/*++
 
Routine Description:

    This routine will return a pointer to a Cached Read Request
	if such a request has been cached.

Arguments:

    deviceExtension - Pointer to the device Extension
    pIrp - Where to save pointer to uncached IRP
Return Value:

    None

--*/
{
	KIRQL oldirql;
	PIRP Irp;
	KeAcquireSpinLock(&(DeviceExtension->Cachelock), &oldirql);
	Irp = (PIRP) InterlockedExchangePointer(pIrp, NULL);
	if(Irp)
	{
		// If an IRP has been cached then a Cancel routine
		// has already been set, and this call to IoSetCancelRoutine
		// will return that Cancel routine. 
		if(IoSetCancelRoutine(Irp, NULL))
		{
			// Since we now know we have a Cached IRP we can
			// now retrieve it.
			RemoveEntryList(&Irp->Tail.Overlay.ListEntry);
		}
		else
			Irp = NULL;
	}

	DeviceExtension->NumCachedRequests--;
	KeReleaseSpinLock(&(DeviceExtension->Cachelock), oldirql);
	return Irp;
}


NTSTATUS CacheReadRequest(
	PDEVICE_EXTENSION DeviceExtension, 
	PIRP Irp, 
	PIRP* pIrp
	)
/*++
 
Routine Description:

    This routine will cache a Read Request until it can
	be serviced by an interrupt in the OnInterruptIN routine.

Arguments:

    deviceExtension - Pointer to the device Extension
	Irp - IRP to cache
    pIrp - Where to save pointer to cached IRP

Return Value:

    NTSTATUS value.
www.
--*/
{						
	KIRQL oldirql;
	NTSTATUS status;
	PIO_STACK_LOCATION stack;
	PFILE_OBJECT fileObject;


	
	ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

	// Make sure there's an extra stack location for us to use

	if (Irp->CurrentLocation <= 1)
		return STATUS_INVALID_PARAMETER;	// no stack for us

	// Acquire a lock on cache access.

	KeAcquireSpinLock(&(DeviceExtension->Cachelock), &oldirql);

	
	if (*pIrp)
		status = STATUS_UNSUCCESSFUL;	// something already cached here
	else
		{						// try to cache IRP

		// Install a cancel routine

		IoSetCancelRoutine(Irp, OnCancelReadRequest);
		
		// Check to see if this Irp has already been cancelled

		if (Irp->Cancel && IoSetCancelRoutine(Irp, NULL))
			status = STATUS_CANCELLED;	// already cancelled

		// Cache this IRP in our list of Pending Read Requests. 

		else
			{					// cache it
			IoMarkIrpPending(Irp);
			status = STATUS_PENDING;

			stack = IoGetCurrentIrpStackLocation(Irp);
			stack->Parameters.Others.Argument1 = (PVOID) pIrp;
			
			//Install a completion routine to nullify the cache pointer.

			IoSetCompletionRoutine(Irp, (PIO_COMPLETION_ROUTINE) OnCompleteReadRequest, (PVOID) DeviceExtension, TRUE, TRUE, TRUE);
			IoSetNextIrpStackLocation(Irp);	// so our completion routine will get called
			fileObject = stack->FileObject;
			stack = IoGetCurrentIrpStackLocation(Irp);
			stack->DeviceObject = DeviceExtension->FunctionalDeviceObject;	// so CancelIrp can give us right ptr
			stack->FileObject = fileObject;	// for cleanup
			*pIrp = Irp;

			// Add this request to our list of Read Requests
 			InsertTailList(&DeviceExtension->Pending_IOCTL_READINT_List, &Irp->Tail.Overlay.ListEntry);
			DeviceExtension->NumCachedRequests++;
			}					// cache it
		}						// try to cache IRP
	
	KeReleaseSpinLock(&(DeviceExtension->Cachelock), oldirql);

	return status;
}							


//////////////////////////////////////////////////////////////////////////////

#pragma LOCKEDCODE

NTSTATUS OnCompleteReadRequest(
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp, 
	PDEVICE_EXTENSION DeviceExtension
	)
/*++
 
Routine Description:

	This routine is called upon completion of an IRP associated
	with a Read Request.

Arguments:

    DeviceObject - pointer to device object
    Irp - IRP which has finished
    deviceExtension - Pointer to the device Extension
	
Return Value:

    NTSTATUS value.

--*/
{							
	KIRQL oldirql;
	PIRP* pIrp;

	KeAcquireSpinLock(&DeviceExtension->Cachelock, &oldirql);
	pIrp = (PIRP*) IoGetCurrentIrpStackLocation(Irp)->Parameters.Others.Argument1;
	if (*pIrp == Irp)
		*pIrp = NULL;
	KeReleaseSpinLock(&DeviceExtension->Cachelock, oldirql);
	return STATUS_SUCCESS;
}							



///////////////////////////////////////////////////////////////////////////////

#pragma LOCKEDCODE

VOID OnCancelReadRequest(
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp
	)
/*++
 
Routine Description:

	This routine is called upon cancellation of an IRP associated
	with a Read Request.

Arguments:

    DeviceObject - pointer to device object
    Irp - IRP which has been cancelled
    	
Return Value:

    NTSTATUS value.

--*/
{							
	PDEVICE_EXTENSION deviceExtension;
	KIRQL oldirql;
	
	oldirql= Irp->CancelIrql;
	IoReleaseCancelSpinLock(DISPATCH_LEVEL);

	deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

	// Remove this request to our list of Read Requests

	KeAcquireSpinLockAtDpcLevel(&deviceExtension->Cachelock);
	RemoveEntryList(&Irp->Tail.Overlay.ListEntry);
	KeReleaseSpinLock(&deviceExtension->Cachelock, oldirql);

	// Complete the IRP

	Irp->IoStatus.Status = STATUS_CANCELLED;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
}				