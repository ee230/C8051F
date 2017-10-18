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

// {37538C66-9584-42d3-9632-EBAD0A230D13}
DEFINE_GUID(GUID_INTERFACE_SILABS_BULK, 
0x37538c66, 0x9584, 0x42d3, 0x96, 0x32, 0xeb, 0xad, 0xa, 0x23, 0xd, 0x13);


#define IOCTL_READINT					0x800

#define INTUSB_IOCTL_INDEX             0x0000


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


#endif

