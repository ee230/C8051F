		-----------------------------------------------------------------
		   USB Bulk Host Application Example version 1.2 Readme File
                         Copyright (C) 2009 Silicon Laboratories, Inc.
                -----------------------------------------------------------------


Last updated: 21 January 2009

This document provides information that supplements the Silicon Labs Bulk Host Application example, which
includes instructions for running the application.


1.0 Running the Host Application on the C8051F32x target board
----------------------------------------------------------------------

1. Once firmware has been downloaded to the target board and the driver has been installed successfully, 
   run the F32x_BulkFileTransfer.exe example application located by default in the 
   “SiLabs\MCU\Examples\C8051F32x\USB_BULK” directory.

2. The F32x_BulkFileTransfer application will display the current USB devices to select from.

3. To configure the example provide a path for the Transfer file name and the Receive file name.

4. Press the "Transfer Data" and "Receive Data" button to run the example.


2.0 KNOWN ISSUES AND LIMITATIONS
---------------------------------
	
1. If using the C8051F320TB target board, jumper J2 should be installed if the board is wall-powered; 
   jumper J11 should be installed if bus-powered.

2. Windows application and driver supports Windows 98SE/2000/XP operating systems.


3.0 REVISION HISTORY
---------------------

version 1.2
	Updated to include MFC as a static library for portability

Version 1.1
	Fixed issue retrieving Serial Number under Win98 SE
        
Version 1.0
	Initial release.
