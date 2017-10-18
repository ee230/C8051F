-------------------------------------------------------------------------------
 Readme.txt
-------------------------------------------------------------------------------

Copyright 2009 Silicon Laboratories, Inc.
http://www.silabs.com

Program Description:
-------------------

This application note covers the implementation of a simple USB application 
using the interrupt transfer type. This includes support for device  
enumeration, control and interrupt transactions, and definitions of descriptor 
data. The purpose of this software is to give a simple working example of an 
interrupt transfer application; it does not includesupport for multiple 
configurations, or other transfer types.

How To Test:
-----------

1) Download the code to any C8051T620/1 or C8051T320/1/2/3 device on a 'T62x 
   mother board using the Silicon Labs IDE
2) Run the GUI application that comes with the firmware and test the features


Known Issues and Limitations:
----------------------------
	
1)  Firmware works with the Silicon Labs IDE v1.71 or later and the 
    Keil C51 tool chain. Project and code modifications will be 
    necessary for use with different tool chains.

2)  Compiler optimization emphasis is selected as "favor small code". 
    This selection is necessary for the project to be compiled with the 
    trial version of the Keil C51 Compiler (under 4k code space).

3)  If using the C8051T620 daughter card, the J1 VREGIN-VBUS pins should be 
    shorted if bus powered.  The J1 VREGIN-VDD pins should be shorted
    if wall powered.

4)  Windows application and driver supports Win2K, XP, and 98SE only.
	

Target and Tool Chain Information:
---------------------------------

Target:         C8051T620/1 or C8051T320/1/2/3
Tool chain:     Keil / Raisonance
                Silicon Laboratories IDE version 3.4x
Project Name:   T620_USB_Interrupt


Command Line Options:
--------------------

Assembler : Default
Compiler  : Default
Linker    : Default 


File List:
---------

T620_USB_Descriptor.c
T620_USB_Descriptor.h
T620_USB_ISR.c
T620_USB_Main.c
T620_USB_Main.h
T620_USB_Register.h
T620_USB_Standard_Requests.c
ReadMe.txt (this file)


Release Information:
-------------------

Version 1.2
  - 14 AUG 2008 (TP)
  - Updated all files
  - Ported from 'F340 to 'T620/1
	
Version 1.1
  - 08 JAN 2008 (GP)
  - Updated F34x_USB_Standard_Requests.c to v1.1
        
Version 1.0
  - Initial release (GP).
  - 20 DEC 2005

-------------------------------------------------------------------------------
 End Of File
-------------------------------------------------------------------------------