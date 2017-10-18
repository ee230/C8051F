-------------------------------------------------------------------------------
 Readme.txt
-------------------------------------------------------------------------------

Copyright 2005 Silicon Laboratories, Inc.
http://www.silabs.com

Program Description:
-------------------

This firmware is intended to work with the Silabs USB Bulk File Transfer 
example, implementing two Bulk pipes with 64-byte Maximum transfers. The 
endpoints used are as follows:

Endpoint1 IN - BULK IN
Endpoint2 OUT - BULK OUT

How To Test:
-----------

1) Download the code to any C8051F38x target board using the Silicon Labs IDE
2) Run the GUI application that comes with the firmware and test the features


Known Issues and Limitations:
----------------------------
	
1)  Firmware works with the Silicon Labs IDE v1.71 or later and the 
    Keil C51 tool chain. Project and code modifications will be 
    necessary for use with different tool chains.

2)  Compiler optimization emphasis is selected as "favor small code". 
    This selection is necessary for the project to be compiled with the 
    trial version of the Keil C51 Compiler (under 4k code space).

3)  If using the C8051F380TB target board, the VREGIN-VBUS pins should be 
    shorted if bus powered.  The VREGIN-VDD pins should be shorted
    if wall powered.

4)  Windows application and driver supports Win2K, XP, and 98SE only.
	

Target and Tool Chain Information:
---------------------------------

Target:         C8051F38x
Tool chain:     Keil C51 7.50 / Keil EVAL C51
                Silicon Laboratories IDE version 2.6
Project Name:   F38x_USB_Bulk


Command Line Options:
--------------------

Assembler : Default
Compiler  : Default
Linker    : Default 


File List:
---------

F38x_USB_Bulk.wsp
F38x_USB_Config.h
F38x_USB_Descriptors.c
F38x_USB_Descriptors.h
F38x_USB_Main.c
F38x_USB_Main.h
F38x_USB_Registers.h
F38x_USB_Request.h
F38x_USB_Standard_Requests.c
F38x_USB_Structs.h
F38x_USB_ISR.c
F38x_USB_Utilities.c
Readme.txt (this file)


Release Information:
-------------------

Release 1.4 (PKC)
  -Updated all files
  -Ported from 'F340 to 'F380

Release 1.3
  -Updated all files
  -Ported from 'F320 to 'F340

Release 1.2
  -Initial revisions by JS / CS / JM
  -JM revisions were on 25 MAY 2005
  -Before the new guidelines


-------------------------------------------------------------------------------
 End Of File
-------------------------------------------------------------------------------