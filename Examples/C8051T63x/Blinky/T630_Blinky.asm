$NOMOD51
;-----------------------------------------------------------------------------
; T630_Blinky.asm
;-----------------------------------------------------------------------------
; Copyright 2007, Silicon Laboratories, Inc.
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
; 1) Download code to a 'T630 daughter board via the 'T630 motherboard
;    Note: The daughter board can be the 'T630 Socket Daughter Board, or
;    the 'T630 Emulation Daughter Board that contains an 'F336 MCU.
; 2) Ensure that pins 1 and 2 are shorted together on the J9 header
; 3) Run the code and if the P1.3 LED blinks, the code works
;
;
; Target:         C8051T630, 'T631, 'T632, 'T633, 'T634, 'T635
; Tool chain:     Keil C51 8.0 / Keil EVAL C51
; Command Line:   None
;
; Release 1.0
;    -Initial Revision (PKC)
;    -02 NOV 2007
;

$include (C8051T630.inc)               ; Include register definition file.

;------------------------------------------------------------------------------
; EQUATES
;------------------------------------------------------------------------------

GREEN_LED   equ   P1.3                 ; Green LED: '1' is ON

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
            anl   PCA0MD,  #NOT(040h)   ; Clear Watchdog Enable bit

            ; Initialize the Port I/O Crossbar
            orl   P1SKIP,  #08h        ; Skip the LED pin from crossbar
            orl   P1MDOUT, #08h        ; Make LED pin output push-pull
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

END

