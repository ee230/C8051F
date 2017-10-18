/*++


Module Name:

    bulkrwr.h

Abstract:

Environment:

    Kernel mode

Notes:

  
--*/
#ifndef _INTUSB_RWR_H
#define _INTUSB_RWR_H

typedef struct _BULKUSB_RW_CONTEXT {

    PURB              Urb;
    PMDL              Mdl;
    ULONG             Length;         // remaining to xfer
    ULONG             Numxfer;        // cumulate xfer
    ULONG_PTR         VirtualAddress; // va for next segment of xfer.
    PDEVICE_EXTENSION DeviceExtension;

} BULKUSB_RW_CONTEXT, * PBULKUSB_RW_CONTEXT;

PINTUSB_PIPE_CONTEXT
BulkUsb_PipeWithName(
    IN PDEVICE_OBJECT  DeviceObject,
    IN PUNICODE_STRING FileName
    );

NTSTATUS
BulkUsb_DispatchReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
BulkUsb_ReadWriteCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    );

#endif
