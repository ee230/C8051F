	      -------------------------------------------------------
               USB Interrupt Driver Example version 1.5 Readme File
                    Copyright (C) 2005 Silicon Laboratories, Inc.
              -------------------------------------------------------                

Last updated: 21 January 2005

This document provides information that supplements the Silicon Labs Interrupt Driver, which
includes instructions for installing the driver and information for building the driver.

1.0 Interrupt Device Driver Installation
-----------------------------------------------
 
1.) Connect the USB cable between the host computer and the C8051F320 target board.

2.) At this point,Windows should notify you it has found new hardware and prompt you
    with a Found New Hardware wizard. Press the Next button.

3.) Select Search for a suitable driver for my device (recommended)and press the Next button.

4.) Select Specify a location under “Optional search locations:”and press the Next button.

5.) Press the Browse button to located the SilabsInt.inf driver installation file. 
    The default location is the “Silabs\MCU\Examples\C8051F320\USB_INT ”directory. 
    Once this file is selected press the OK button.

6.) Verify that the correct path and filename are shown and press the Next button.

7.) Press the Finish button.


2.0 Building the Interrupt Device Driver
-----------------------------------------------

In order to build the Interrupt Device Driver you must have the Microsoft Windows XP Driver
Development Kit installed. Use the build utility included with the DDK to build the driver. Please 
refer to the DDK documentation section "Using the Build Utility" under the "Driver Development Tools"
section before attempting to build the driver. Using these methods the built driver will be compatible
with Windows 2000 and Windows XP.
 
1.) Open the "Win XP Checked Build Environment"

2.) Navigate to the directory where the Silabs Interrupt Driver's source files are located.

3.) Type "Build -cZ" and press enter.

4.) The built driver will be located in the subdirectory "Driver_Source\objchk\i386".


2.1 Building a driver compatible with Windows 98 Second Edition
---------------------------------------------------------------

1.) Open the file intusb.h and uncomment the line:

	//#define WIN98DRIVER

2.) Open the sources file with an editor(e.g.Notepad) and add the file intwdm98.c to the list of sources at the end of the file. The sources section will now look like the following.

	SOURCES=intusb.c \
		intpnp.c \
		intpwr.c \
		intdev.c \
		intwmi.c \
		intrwr.c \
		intwdm98.c \
	        intusb.rc

3.) Continue by following the instructions in section 2.0 above to build the driver.


3.0 REVISION HISTORY
--------------------------------------------------------

Version 1.5
	Updated build for x64 with a Test Certificate for installation

Version 1.3
	Overflow bug fixed. The issue was only apparent when making very frequent
	read requests over a long interval using multiple threads where no data 
	ever becomes available. The resolution limits the number of outstanding 
	requests that the driver will queue.

Version 1.2
	Install bug in Windows 98 fixed.

Version 1.1
	Support for Windows 98 added. 
	Support for Dual Processor Machines added.

Version 1.0
	Initial release.
