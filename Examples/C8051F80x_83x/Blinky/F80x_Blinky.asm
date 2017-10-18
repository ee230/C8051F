$NOMOD51
;-----------------------------------------------------------------------------
; F80x_Blinky.asm
;-----------------------------------------------------------------------------
; Copyright 2009, Silicon Laboratories, Inc.
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
; 1) Ensure that a shorting block is present on the J2 header of the 'F800 
;    target board.
; 2) Download code to the 'F800 target board.
; 3) Ensure that pins 1 and 2 are shorted together on the J3 header.
; 4) Run the program.  If the Green LED flashes, the program is working.
;
;
; Target:         C8051F800
; Tool chain:     Keil C51 7.50 / Keil EVAL C51
; Command Line:   None
;
; Release 1.0 - 15 JUL 2009 (PKC)
;    -Initial Revision.
;

;------------------------------------------------------------------------------
; Includes
;------------------------------------------------------------------------------

$include (C8051F800.INC)               ; Include register definition file.

;------------------------------------------------------------------------------
; EQUATES
;------------------------------------------------------------------------------

LED_ON       equ     1                 ; LED states
LED_OFF      equ     0

GREEN_LED   equ   P1.0                 ; GREEN LED: '0' is ON

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
            anl   PCA0MD, #NOT(040h)   ; clear Watchdog Enable bit

            ; Initialize the Port I/O Crossbar
            orl   P1SKIP,  #01h        ; Skip the LED pin from crossbar
            orl   P1MDOUT, #01h        ; Make LED pin output push-pull
            mov   XBR0,    #00h        ; No digital peripherals selected
            mov   XBR1,    #40h        ; Enable Crossbar

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