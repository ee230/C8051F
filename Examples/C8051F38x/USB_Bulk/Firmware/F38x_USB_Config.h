//-----------------------------------------------------------------------------
// F38x_USB_Config.h
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Header file for usb firmware. Includes device configuration details.
// Constants defined in this file should be modified when changes are
// made to the firmware, including changes to endpoints, interfaces,
// packet sizes, or feature selections (remote wakeup, suspend, etc.).
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051F38x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F38x_USB_Bulk
//
//
// Release 1.3
//    -All changes by GP
//    -21 NOV 2005
//    -Changed revision number to match project revision
//     No content changes to this file
//    -Modified file to fit new formatting guidelines
//    -Changed file name from usb_config.h
//
//
// Release 1.2
//    -Initial Revision (JS/JM)
//    -XX OCT 2003
//

#ifndef  F38x_USB_CONFIG_H
#define  F38x_USB_CONFIG_H

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//--------------------------
// Begin device details
//--------------------------
// These constants should be modified to match the current implementation
//
#define NUM_CFG 1                         // Total number of defined
                                          // configurations

#define MAX_IF_DSC 1                      // Maximum number of interface
                                          // descriptors for any defined
                                          // configuration

#define MAX_IF 1                          // Maximum number of interfaces
                                          // for any defined configuration

#define CFG1_IF_DSC 1                     // Total number of interface
                                          // descriptors for configuration1

#define CFG1_EP_DSC 2                     // Total number of endpoint
                                          // descriptors for configuration1

#define REMOTE_WAKEUP_SUPPORT OFF         // This should be "ON" if the
                                          // device is capable of remote
                                          // wakeup (this does not mean that
                                          // remote wakeup is enabled)
                                          // Otherwise "OFF"

#define SELF_POWERED_SUPPORT OFF          // This should be "ON" if the
                                          // device is self-powered;
                                          // "OFF" if the device
                                          // is bus-powered.

#define SUSPEND_ENABLE OFF                // This should be "ON" if the
                                          // device should respond to suspend
                                          // signaling on the bus.
                                          // Otherwise "OFF"

#define ISO_UPDATE_ENABLE OFF             // This should be "ON" if the ISO
                                          // Update feature should be turned
                                          // on for all ISO endpoints. It
                                          // should be "OFF" if the ISO
                                          // update feature should not be
                                          // enabled, or if no ISO endpoints
                                          // will be configured

// Endpoint buffer / packet sizes
// These constants should match the desired maximum packet size for each
// endpoint. Note that the size must not exceed the size of the FIFO
// allocated to the target endpoint. This size will depend on the configuration
// of the endpoint FIFO (split mode and double buffer options), as described
// in the device datasheet.

#define EP0_MAXP       64                 // Endpoint0 maximum packet size
#define EP1_IN_MAXP    64                 // Endpoint1 IN maximum packet size
#define EP2_IN_MAXP    128                // Endpoint2 IN maximum packet size
#define EP3_IN_MAXP    256                // Endpoint3 IN maximum packet size
#define EP1_OUT_MAXP   128                // Endpoint1 OUT maximum packet size
#define EP2_OUT_MAXP   64                 // Endpoint2 OUT maximum packet size
#define EP3_OUT_MAXP   256                // Endpoint3 OUT maximum packet size

//--------------------------
// End device details
//--------------------------

#endif                                 // F38x_USB_CONFIG_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------