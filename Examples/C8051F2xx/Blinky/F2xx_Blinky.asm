$NOMOD51
;-----------------------------------------------------------------------------
;  Copyright (C) 2007 Silicon Laboratories, Inc.
;  All rights reserved.
;
;
;
;  FILE NAME   :  F2xx_BLINKY.ASM 
;  TARGET MCU  :  C8051F2xx 
;  DESCRIPTION :  This program illustrates how to disable the watchdog timer,
;                 configure a port and write to a port I/O pin.
;
; 	NOTES: 
;
;-----------------------------------------------------------------------------

$include (c8051f200.inc)               ; Include register definition file.

;-----------------------------------------------------------------------------
; EQUATES
;-----------------------------------------------------------------------------

GREEN_LED      equ   P2.4              ; Port I/O pin connected to Green LED.	

;-----------------------------------------------------------------------------
; RESET and INTERRUPT VECTORS
;-----------------------------------------------------------------------------

               ; Reset Vector
               cseg AT 0
               ljmp Main               ; Locate a jump to the start of code at 
                                       ; the reset vector.

;-----------------------------------------------------------------------------
; CODE SEGMENT
;-----------------------------------------------------------------------------


Blink          segment  CODE

               rseg     Blink          ; Switch to this code segment.
               using    0              ; Specify register bank for the
                                       ; following program code.

Main:          ; Disable the WDT. (IRQs not enabled at this point.)
               ; If interrupts were enabled, we would need to explicitly 
               ; disable them so that the 2nd move to WDTCN occurs no more 
               ; than four clock cycles after the first move to WDTCN.

               mov   WDTCN, #0DEh
               mov   WDTCN, #0ADh

               ; Set P2.4 (LED) as digital output in push-pull mode.  
               orl   PRT2CF, #10h	 
               orl   P2MODE, #10h 

               ; Initialize LED to OFF
               clr   GREEN_LED

               ; Simple delay loop.
Loop2:         mov   R7, #03h			
Loop1:         mov   R6, #00h
Loop0:         mov   R5, #00h
               djnz  R5, $
               djnz  R6, Loop0
               djnz  R7, Loop1
               cpl   GREEN_LED         ; Toggle LED.
               jmp   Loop2


;-----------------------------------------------------------------------------
; End of file.

END
