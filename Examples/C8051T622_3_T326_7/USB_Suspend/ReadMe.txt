-------------------------------------------------------------------------------
 Readme.txt
-------------------------------------------------------------------------------

Copyright 2008 Silicon Laboratories, Inc.
http://www.silabs.com

Program Description:
-------------------

This program demonstrates how to create a USB Mouse using a Target Board.

How To Test:
-----------

1) Open the project file and build the project.
2) Download the code to a C8051T622 development kit using the Silicon Labs IDE.
3) Place jumpers on target board to connect the P0.x and P1.x LEDs and 
    switches.
4) Connect the development board to the PC using a USB cable.  After the device
   enumerates, it should start moving the mouse cursor.
5) Press SW2 (P1.0) to toggle whether the 'T622 is controlling the mouse 
   cursor.
6) To put the device in Suspend, place the PC into Standby.  The current
   consumption of the 'T622 can be measured using the J7 header.


Known Issues and Limitations:
----------------------------
	
1)  Firmware works with the Silicon Labs IDE v3.4x or later and the 
    Keil C51 tool chain. Project and code modifications will be necessary for 
    use with different tool chains.
    

Target and Tool Chain Information:
---------------------------------
          
Target:         C8051T622/3, 'T326/7
Tool chain:     Keil / Raisonance
                Silicon Laboratories IDE version 3.4x
Project Name:   T622_USB0_Suspend


Command Line Options:
--------------------

Assembler : Default
Compiler  : Default
Linker    : Default 


File List:
---------

T622_USB0_Descriptor.h
T622_USB0_InterruptServiceRoutine.h
T622_USB0_USB0_Mouse.h
T622_USB0_Register.h
T622_USB0_ReportHandler.h
T620_USB0_Mouse.c
T622_USB0_Mouse.c
T622_USB0_Descriptor.c
T622_USB0_InterruptServiceRoutine.c
T622_USB0_USB0_Main.c
T622_USB0_ReportHandler.c
T622_USB0_Standard_Requests.c


ReadMe.txt (this file)


Release Information:
-------------------
    
Version 1.0
	Initial release.

-------------------------------------------------------------------------------
 End Of File
-------------------------------------------------------------------------------