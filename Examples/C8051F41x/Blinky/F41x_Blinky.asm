$NOMOD51
;-----------------------------------------------------------------------------
; F41x_Blinky.asm
;-----------------------------------------------------------------------------
; Copyright 2006 Silicon Laboratories, Inc.
; http:;www.silabs.com
;
; Program Description:
;
; This program illustrates how to disable the watchdog timer,
; configure the Crossbar, configure a port and write to a port
; I/O pin.
;
;
; How To Test:
;
; 1) Download code to a 'F41x target board
; 2) Run the code and if the P2.1 LED blinks, the code works
;
;
; FID:            41X000003
; Target:         C8051F41x
; Tool chain:     Raisonance / Keil
; Command Line:   None
;
; Release 1.1 / 11 MAR 2010 (GP)
;    -Tested with Raisonance
;
; Release 1.0
;    -Initial Revision (GP)
;    -03 JAN 2006
;

$include (c8051f410.inc)               ; Include register definition file.

;------------------------------------------------------------------------------
; EQUATES
;------------------------------------------------------------------------------

GREEN_LED   equ   P2.1                 ; Green LED: '1' is ON	

;------------------------------------------------------------------------------
; RESET and INTERRUPT VECTORS
;------------------------------------------------------------------------------

            ; Reset Vector
            cseg AT 0
            ljmp Main                  ; Locate a jump to the start of
                                       ; code at the reset vector.

;------------------------------------------------------------------------------
; CODE SEGMENT
;------------------------------------------------------------------------------


Blink       segment  CODE

            rseg     Blink             ; Switch to this code segment.
            using    0                 ; Specify register bank for the
                                       ; following program code.

Main:
            ; Disable the WDT.
            anl   PCA0MD, #NOT(040h)   ; Clear Watchdog Enable bit

						; Configure the Oscillator
            orl   OSCICN, #04h         ; sysclk = 24.5 Mhz / 8

            ; Enable the Port I/O Crossbar

            mov   XBR1, #40h           ; Enable Crossbar
            orl   P2MDOUT, #02h        ; Make LED pin output push-pull

            ; Initialize LED to OFF
            clr   GREEN_LED

            ; Simple delay loop.
Loop2:      mov   R7, #03h			
Loop1:      mov   R6, #00h
Loop0:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop0
            djnz  R7, Loop1
            cpl   GREEN_LED            ; Toggle LED.
            jmp   Loop2

;------------------------------------------------------------------------------
; End of file.

END

