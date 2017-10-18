-------------------------------------------------------------------------------
 Readme.txt
-------------------------------------------------------------------------------

Copyright 2005 Silicon Laboratories, Inc.
http://www.silabs.com

Program Description:
-------------------

This application note covers the implementation of a simple USB application 
using the interrupt transfer type. This includes support for device enumeration, 
control and interrupt transactions, and definitions of descriptor data. The
purpose of this software is to give a simple working example of an interrupt 
transfer application; it does not include support for multiple configurations, 
or other transfer types.

How To Test:
-----------

1) Download the code to any C8051F326 target board using the Silicon Labs IDE
2) Run the GUI application that comes with the firmware and test the features


Known Issues and Limitations:
----------------------------
	
1)  Firmware works with the Silicon Labs IDE v1.71 or later and the 
	Keil C51 tool chain. Project and code modifications will be 
	necessary for use with different tool chains.

2)  Compiler optimization emphasis is selected as "favor small code". 
	This selection is necessary for the project to be compiled with the 
	trial version of the Keil C51 Compiler (under 4k code space).

3)  If using the C8051F320TB target board, jumper J2 should be 
	installed if the board is wall-powered; 
	jumper J11 should be installed if bus-powered.

4)  Windows application and driver supports Win2K, XP, and 98SE only.
	

Target and Tool Chain Information:
---------------------------------

FID:            326000023
Target:         C8051F326
Tool chain:     Keil C51 7.50 / Keil EVAL C51
                Silicon Laboratories IDE version 2.6
Project Name:   F326_USB_Interrupt


Command Line Options:
--------------------

Assembler : Default
Compiler  : Default
Linker    : Default 


File List:
---------

F326_USB_Descriptor.c (1.0)
F326_USB_Descriptor.h (1.0)
F326_USB_ISR.c (1.0)
F326_USB_Main.c (1.0)
F326_USB_Main.h (1.0)
F326_USB_Register.h (1.0)
F326_USB_Standard_Requests.c (1.1)
ReadMe.txt (this file) (1.1)


Release Information:
-------------------

Version 1.4
	08 JAN 2008 (GP)
    Updated F326_USB_Standard_Requests.c to v1.1
	        
Version 1.0
	Initial release.

-------------------------------------------------------------------------------
 End Of File
-------------------------------------------------------------------------------