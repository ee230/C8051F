		-----------------------------------------------------------------
		 USB Interrupt Host Application Example version 1.1 Readme File
                         Copyright (C) 2005 Silicon Laboratories, Inc.
                -----------------------------------------------------------------

Last updated: 25 MAY 2005

This document provides information that supplements the Silicon Labs Interrupt Host Application 
example, which includes instructions for running the application.


1.0 Running the Host Application on the C8051F32x target board
----------------------------------------------------------------------

1. Once firmware has been downloaded to the target board and the driver has been installed 
successfully, run the USBTest.exe example application located by default in the 
“SiLabs\MCU\Examples\C8051F320\USB_INT” directory.

2. The USBTest application will display the status of the potentiometer, the temperature sensor, 
the two switches, and the lower four bits of Port 0.

3. The application also includes check boxes for turning on the two LEDs and changing the logic 
levels on the lower four bits of Port 1.

4. If any of the application functions do not operate correctly, verify jumper shorting blocks are
installed at the following positions: J3[1-2] (P2.0 Switch), J3[3-4] (P2.1 Switch), J3[5-6] 
(P2.2 LED), J3[7-8] (P2.3 LED), J9 (P0.2), J10 (P0.3) and J13 (P1.7 Potentiometer).


2.0 KNOWN ISSUES AND LIMITATIONS
---------------------------------
	
1. If using the C8051F320TB target board, jumper J2 should be installed if the board is 
wall-powered; jumper J11 should be installed if bus-powered.

2. Windows application and driver supports Windows 98SE/2000/XP operating systems.


3.0 REVISION HISTORY
---------------------
        
Version 1.1
	Modified to support 64 byte transfers

Version 1.0
	Initial release.
