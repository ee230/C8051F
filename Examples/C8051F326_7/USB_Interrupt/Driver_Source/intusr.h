/*++


Module Name:

    sSUsr.h

Abstract:

Environment:

    Kernel mode

Notes:

 
--*/

#ifndef _INTUSB_USER_H
#define _INTUSB_USER_H

#include <initguid.h>


// {DDA31245-1BFC-4225-B2B8-EAAAB2E390B6}
DEFINE_GUID(GUID_INTERFACE_SILABS_INTERRUPT, 
0xdda31245, 0x1bfc, 0x4225, 0xb2, 0xb8, 0xea, 0xaa, 0xb2, 0xe3, 0x90, 0xb6);

#define IOCTL_READINT					0x800

#define INTUSB_IOCTL_INDEX				0x0000



#define IOCTL_INTUSB_GET_CONFIG_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     INTUSB_IOCTL_INDEX,     \
                                                     METHOD_BUFFERED,         \
                                                     FILE_ANY_ACCESS)
                                                   
#define IOCTL_INTUSB_RESET_DEVICE          CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     INTUSB_IOCTL_INDEX + 1, \
                                                     METHOD_BUFFERED,         \
                                                     FILE_ANY_ACCESS)

#define IOCTL_INTUSB_RESET_PIPE            CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     INTUSB_IOCTL_INDEX + 2, \
                                                     METHOD_BUFFERED,         \
                                                     FILE_ANY_ACCESS)

#define PAGEDCODE code_seg("page")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("init")

#define PAGEDDATA data_seg("page")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("init")

#endif

