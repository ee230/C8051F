$NOMOD51
;-----------------------------------------------------------------------------
;	Copyright (C) 2007 Silicon Laboratories, Inc.
; 	All rights reserved.
;
;
; 	FILE NAME  	:	F00x_BLINKY.ASM 
; 	TARGET MCU	:	C8051F000 
; 	DESCRIPTION	:	This program illustrates how to disable the watchdog timer,
;                 configure the Crossbar, configure a port and write to a port
;                 I/O pin.
;
;
; 	NOTES: 
;
; 	(1) This note intentionally left blank.
;
;
;-----------------------------------------------------------------------------


;-----------------------------------------------------------------------------
; EQUATES
;-----------------------------------------------------------------------------

$include (c8051f000.inc)					; Include register definition file.

GREEN_LED      EQU      P1.6           ; Port I/O pin connected to Green LED.	


;-----------------------------------------------------------------------------
; VARIABLES
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; RESET and INTERRUPT VECTORS
;-----------------------------------------------------------------------------


           ; Reset Vector
           cseg AT 0
           ljmp Main                 ; Locate a jump to the start of code at 
                                     ; the reset vector.

;-----------------------------------------------------------------------------
; CODE SEGMENT
;-----------------------------------------------------------------------------

Code_Seg  segment  CODE

          rseg     Code_Seg          ; Switch to this code segment.
          using    0                 ; Specify register bank for the following
                                     ; program code.

Main: 			; Disable the WDT. (IRQs not enabled at this point.)
					; If interrupts were enabled, we would need to explicitly disable
					; them so that the 2nd move to WDTCN occurs no more than four clock 
					; cycles after the first move to WDTCN.
					mov	WDTCN, #0DEh
					mov	WDTCN, #0ADh

					; Enable the Port I/O Crossbar
					mov		XBR2, #40h

					; Set P1.6 (LED) as push-pull output.  All others default to open-drain.	 
					orl 	PRT1CF,#01000000b 

					; Initialize LED to OFF
					clr		GREEN_LED

					; Simple delay loop.
Blink:			mov	R7, #03h			
Loop0:			mov	R6, #00h
Loop1:			mov	R5, #00h
					djnz	R5, $
					djnz	R6, Loop1
					djnz	R7, Loop0
					cpl	GREEN_LED			; Toggle LED.
					jmp	Blink


;-----------------------------------------------------------------------------
; End of file.

END