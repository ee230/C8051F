$NOMOD51
;-----------------------------------------------------------------------------
; F560_Blinky.asm
;-----------------------------------------------------------------------------
; Copyright 2009 Silicon Laboratories, Inc.
; http://www.silabs.com
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
; 1) Download code to a 'F560 target board
; 2) Run the code and if the P1.3 LED blinks, the code works
;
;
; Target:         C8051F560
; Tool chain:     Raisonance / Keil / SDCC
; Command Line:   None
;
; Release 1.0 / 15 JAN 2009 (GP)
;    -Initial Revision
;

$include (C8051F560.inc)               ; Include register definition file.

;------------------------------------------------------------------------------
; EQUATES
;------------------------------------------------------------------------------

GREEN_LED   equ   P1.3                 ; Green LED: '0' is OFF	

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
            ; Set SFR Page for PCA0MD
            mov   SFRPAGE, #ACTIVE_PAGE

            ; Disable the WDT.
            anl   PCA0MD, #NOT(040h)   ; Clear Watchdog Enable bit

            ; Set SFR Page for Oscillator and Port config
            mov   SFRPAGE, #CONFIG_PAGE

						; Configure the Oscillator
            orl   OSCICN, #84h         ; System Clock = 24 Mhz / 8

            ; Enable the Port I/O Crossbar

            mov   XBR2, #40h           ; Enable Crossbar
            orl   P1MDOUT, #08h        ; Make LED pin output push-pull

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
;------------------------------------------------------------------------------

END

