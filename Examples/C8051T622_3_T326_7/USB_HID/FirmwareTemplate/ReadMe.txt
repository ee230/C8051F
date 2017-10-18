-------------------------------------------------------------------------------
 Readme.txt
-------------------------------------------------------------------------------

Copyright 2011 Silicon Laboratories, Inc.
http://www.silabs.com

Program Description:
-------------------

This project contains the Firmware Template that can be used as a starting
point when creating custom USB-driven systems. 

How To Test:
-----------

1) Open the project file.  Add custom application files.
2) Open C8051f3xx.h and select either C8051F320.h, C8051F326.h,
   C8051F340.h, C8051F340.h, C8051F380_defs.h, C8051T620_defs.h, or C8051T622_defs.h
3) Download the code to any C8051F3xx target board using the Silicon Labs IDE


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
Project Name:   


Command Line Options:
--------------------

Assembler : Default
Compiler  : Default
Linker    : Default 


File List:
---------

c8051f3xx.h
DEFAULT_CustomApp.h
F3xx_USB0_Descriptor.h
F3xx_USB0_InterruptServiceRoutine.h
F3xx_USB0_Register.h
F3xx_USB0_ReportHandler.h
DEFAULT_CustomApp.c
F3xx_USB0_Descriptor.c
F3xx_USB0_InterruptServiceRoutine.c
F3xx_USB0_Main.c
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