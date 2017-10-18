-------------------------------------------------------------------------------
 Readme.txt
-------------------------------------------------------------------------------

Copyright 2011 Silicon Laboratories, Inc.
http://www.silabs.com

Program Description:
-------------------

This program demonstrates how to create a USB Mouse using a Target Board.

How To Test:
-----------

1) Open the project file.  Select either the 'F320, 'F326, 'F340, 'F380, 'T620,
   or 'T622 F3xx_USB0_Mouse.c file for the build.
2) Open C8051f3xx.h and select either C8051F320.h, C8051F326.h,
   C8051F340.h, C8051F380_defs.h, C8051T620_defs.h, or C8051T622_defs.h
3) Download the code to any C8051F3xx target board using the Silicon Labs IDE
4) Run the GUI application that comes with the firmware and test the features


Known Issues and Limitations:
----------------------------
	
1)  Firmware works with the Silicon Labs IDE v1.71 or later and the 
	Keil C51 tool chain. Project and code modifications will be 
	necessary for use with different tool chains.

2)  Place jumpers on target board to connect the LEDs, switches, and
    the Pot to the microcontroller.

4)  Windows application and driver supports Win2K, XP, and 98SE only.
	

Target and Tool Chain Information:
---------------------------------

FID:            
Target:         C8051F320/1, C8051F326/7, C8051F340, C8051F380, C8051T620, C8051T622
Tool chain:     Keil / Raisonance
                Silicon Laboratories IDE version 2.6
Project Name:   F3xx_MouseExample


Command Line Options:
--------------------

Assembler : Default
Compiler  : Default
Linker    : Default 


File List:
---------

c8051f3xx.h
F3xx_USB0_Descriptor.h
F3xx_USB0_InterruptServiceRoutine.h
F3xx_USB0_USB0_Mouse.h
F3xx_USB0_Register.h
F3xx_USB0_ReportHandler.h
F320_USB0_Mouse.c
F326_USB0_Mouse.c
F340_USB0_Mouse.c
F380_USB0_Mouse.c
T620_USB0_Mouse.c
T622_USB0_Mouse.c
F3xx_USB0_Descriptor.c
F3xx_USB0_InterruptServiceRoutine.c
F3xx_USB0_USB0_Main.c
F3xx_USB0_ReportHandler.c
F3xx_USB0_Standard_Requests.c


ReadMe.txt (this file)


Release Information:
-------------------
	        
Version 1.0
	Initial release.

Version 1.1
	Added support for F38x, T620, and T622 familes. (ES)
        20 JAN 2011

-------------------------------------------------------------------------------
 End Of File
-------------------------------------------------------------------------------