-------------------------------------------------------------------------------
 Readme.txt
-------------------------------------------------------------------------------

Copyright 2011 Silicon Laboratories, Inc.
http://www.silabs.com

Program Description:
-------------------

This program demonstrates how the Firmware Template can be used to 
create a system that communicates with a PC application.  The PC
application sends LED blinking patterns to the Target Board.

Project Note:
------------

This project is organized so that it can be compiled for six different
microcontroller families: The F320/1, F326/7, 'F34x, 'F38x, 'T620, and 'T622 families.  

There are two changes that need to be made to customize the project for
each family:

1) In the file c80513xx.h, change the include file to family being used.
2) Include the appropriate "F3xx_Blink_Control_F3xx.c" file in the build.
   This is done by right-clicking on the file in the project window and
   adding to the build.  

If this example was found on the Silicon Laboratories Development kit CD,
these changes have already been made for the respective device.

How To Test:
-----------

1) Open the project file.  Select either the 'F320, 'F326, or the 'F340
   F3xx_Blink_Control.c file for the build.
2) Open C8051f3xx.h and select either C8051F320.h, C8051F326.h,
   C8051F340.h, C8051F380_defs.h, C8051T620_defs.h, or C8051T622_defs.h
3) Download the code to any C8051F32x target board using the Silicon Labs IDE
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

FID:            3XX000015
Target:         C8051F320/1, C8051F326/7, C8051F340, C8051F380, C8051T620, C8051T622
Tool chain:     Keil / Raisonance
                Silicon Laboratories IDE version 2.6
Project Name:   F3xx_BlinkyExample


Command Line Options:
--------------------

Assembler : Default
Compiler  : Default
Linker    : Default 


File List:
---------

c8051f3xx.h
F3xx_Blink_Control.h
F3xx_USB0_Descriptor.h
F3xx_USB0_InterruptServiceRoutine.h
F3xx_USB0_Register.h
F3xx_USB0_ReportHandler.h
F3xx_Blink_Control_F320.c
F3xx_Blink_Control_F326.c
F3xx_Blink_Control_F340.c
F3xx_Blink_Control_F380.c
F3xx_Blink_Control_T620.c
F3xx_Blink_Control_T622.c
F3xx_USB0_Descriptor.c
F3xx_USB0_InterruptServiceRoutine.c
F3xx_USB0_USB0_Main.c
F3xx_USB0_ReportHandler.c
F3xx_USB0_Standard_Requests.c
ReadMe.txt (this file)


Release Information:
-------------------
	        
Version 1.0
	Initial release (PD).
        13 DEC 2005

Version 1.1
	Added support for F38x, T620, and T622 familes. (ES)
        20 JAN 2011

-------------------------------------------------------------------------------
 End Of File
-------------------------------------------------------------------------------