$NOMOD51
;-----------------------------------------------------------------------------
; Blink.ASM
;-----------------------------------------------------------------------------
;  Copyright (C) 2006 Silicon Laboratories, Inc.
;  All rights reserved.
;
;  FILE NAME   :  BLINK.ASM 
;  DATE        :  29 Sep 2008
;  TARGET MCU  :  C8051F52xA/53xA 
;  DESCRIPTION :  This program illustrates how to disable the watchdog timer,
;                 configure the internal oscillator, configure the Crossbar, 
;                 configure a port and write to a port I/O pin.
;
; 	     NOTES : 

; Release 1.1 / 11 MAR 2010 (GP)
;    -Tested with Raisonance
;
;-----------------------------------------------------------------------------

$include (C8051F520A.inc)              ; Include register definition file.

;-----------------------------------------------------------------------------
; EQUATES
;-----------------------------------------------------------------------------

GREEN_LED   equ   P1.3                 ; Green LED: '1' is ON	

;-----------------------------------------------------------------------------
; RESET and INTERRUPT VECTORS
;-----------------------------------------------------------------------------

            ; Reset Vector
            cseg AT 0
            ljmp Main                  ; Locate a jump to the start of
                                       ; code at the reset vector.

;-----------------------------------------------------------------------------
; CODE SEGMENT
;-----------------------------------------------------------------------------


Blink       segment  CODE

            rseg     Blink             ; Switch to this code segment.
            using    0                 ; Specify register bank for the
                                       ; following program code.

Main:
            ; Disable the WDT.
            anl   PCA0MD, #NOT(040h)   ; clear Watchdog Enable bit

            ; Set the Internal Oscillator to 24.5 MHz
            mov   OSCICN, #87h

            ; Enable the Port I/O Crossbar
            mov   XBR1, #40h
            orl   P1MDOUT, #08h        ; make LED pin output push-pull
            orl   P1MDIN, #08h         ; make LED pin input mode digital

            ; Initialize LED to OFF
            clr   GREEN_LED

            ; Simple delay loop.
Loop2:      mov   R7, #08h			
Loop1:      mov   R6, #00h
Loop0:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop0
            djnz  R7, Loop1
            cpl   GREEN_LED            ; Toggle LED.
            jmp   Loop2

;-----------------------------------------------------------------------------
; End of file.

END

