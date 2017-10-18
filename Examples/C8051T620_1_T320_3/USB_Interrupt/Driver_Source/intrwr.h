/*++


Module Name:

    intrwr.h

Abstract:

Environment:

    Kernel mode

Notes:

  
--*/
#ifndef _INTUSB_RWR_H
#define _INTUSB_RWR_H

typedef struct _INTUSB_RW_CONTEXT {

    PURB              Urb;
    PMDL              Mdl;
    ULONG             Length;         // remaining to xfer
    ULONG             Numxfer;        // cumulate xfer
    ULONG_PTR         VirtualAddress; // va for next segment of xfer.
    PDEVICE_EXTENSION DeviceExtension;

} INTUSB_RW_CONTEXT, * PINTUSB_RW_CONTEXT;

PINTUSB_PIPE_CONTEXT
IntUsb_PipeWithName(
    IN PDEVICE_OBJECT  DeviceObject,
    IN PUNICODE_STRING FileName
    );

NTSTATUS
IntUsb_DispatchWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
IntUsb_WriteCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    );

NTSTATUS
IntUsb_DispatchRead(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS 
CreateInterruptUrbIN(
	IN PDEVICE_OBJECT DeviceObject
	);

VOID 
DeleteInterruptUrbIN(
	IN PDEVICE_OBJECT fdo
	);

NTSTATUS 
OnInterruptIN(
	IN PDEVICE_OBJECT junk,
	IN PIRP IrpIN, 
	IN PDEVICE_EXTENSION pdx
	);

NTSTATUS 
CompleteRequest(
	IN PIRP Irp, 
	IN NTSTATUS status, 
	IN ULONG_PTR info
	);

NTSTATUS 
StartInterruptUrbIN(
	IN PDEVICE_EXTENSION deviceExtension
	);

VOID 
StopInterruptUrbIN(
	IN PDEVICE_EXTENSION deviceExtension
	);


PIRP 
UncacheReadRequest(
	IN PDEVICE_EXTENSION DeviceExtension, 
	PIRP* pIrp
	);

NTSTATUS 
CacheReadRequest(
	IN PDEVICE_EXTENSION DeviceExtension, 
	PIRP Irp, 
	PIRP* pIrp
	);

NTSTATUS OnCompleteReadRequest(
	PDEVICE_OBJECT junk, 
	PIRP Irp, 
	PDEVICE_EXTENSION DeviceExtension
	);

VOID OnCancelReadRequest(
	PDEVICE_OBJECT junk, 
	PIRP Irp
	);




#endif
