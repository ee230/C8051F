$NOMOD51
;-----------------------------------------------------------------------------
; F70x_Blinky.asm
;-----------------------------------------------------------------------------
; Copyright 2007 Silicon Laboratories, Inc.
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
; 1) Download code to a 'F70x target board
; 2) Ensure that pins 2 and 3 are shorted together on the J8 header
; 3) Run the code and if the P1.0 LED blinks, the code works
;
;
; Target:         C8051F70x
; Tool chain:     Keil C51 7.50 / Keil EVAL C51
; Command Line:   None
;
; Release 1.0
;    -Initial Revision (PD)
;    -13 FEB 2009
;

$include (C8051F700.inc)               ; Include register definition file.

;------------------------------------------------------------------------------
; EQUATES
;------------------------------------------------------------------------------

LED_ON       equ     0                 ; LED states
LED_OFF      equ     1

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
            mov   WDTCN,  #0DEh        ; Clear Watchdog Enable bit
            mov   WDTCN,  #0ADh        ; Clear Watchdog Enable bit

						mov   SFRPAGE, #0Fh				 ; Switch to configuration pagge
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

END

