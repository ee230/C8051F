NAME    F96x_ActiveCodeLoops
;-----------------------------------------------------------------------------
; F96x_MeasureActiveCurrent.c
;-----------------------------------------------------------------------------
; Copyright 2010 Silicon Laboratories, Inc.
; http://www.silabs.com
;
; Program Description:
; 
; This Keil Specific assembler file provides fixed code loops located at 
; specific word boundaries to provide repeatable results.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; TypicalActiveLoop
;
; This loop is representive of a typical application loop and the same loop
; used for the data sheet typical measurements.
;
;-----------------------------------------------------------------------------

        CSEG AT 1200H
        PUBLIC   TypicalActiveLoop

TypicalActiveLoop:
        mov R1, #07fh
        nop
        nop
        nop

short1:                                 ; emulates a loop to wait a certain time
        djnz R1, short1
loop1:
        mov R1, #010h
short2:
        mov A, R2          		; just do something
        mov B, A
        nop
        mov A, R2                       ; just do something
        mov B, A
        nop
        mov B, R1
        mov A, B
        dec A
        mov R1, A
        cjne A, #000h, short2
        ljmp TypicalActiveLoop

;-----------------------------------------------------------------------------
; LowCurrentActiveLoop
;
; This loop is the best-case shortest loop. The code is aligned to middle
; of a flash page plus 4.
;
;-----------------------------------------------------------------------------
        CSEG AT 1604H
        PUBLIC   LowCurrentActiveLoop

LowCurrentActiveLoop:
        sjmp LowCurrentActiveLoop


;-----------------------------------------------------------------------------
; HighCurrentActiveLoop
;
; This loop is the worst-case shortest loop. The code is aligned to a flash
; page boundary minus one.
;
;-----------------------------------------------------------------------------
        CSEG AT 17FFH
        PUBLIC   HighCurrentActiveLoop

HighCurrentActiveLoop:
        sjmp HighCurrentActiveLoop


;-----------------------------------------------------------------------------
; LongNOPActiveLoop
;
; This loop is a long loop of NOP instructions. This appoximates the power
; running endless NOP instructions. It uses a macro to generate 18 NOPs. 
; The code is aligned to the middle of a flash page. 
;
;-----------------------------------------------------------------------------
        CSEG AT 2000H
        PUBLIC   LongNOPActiveLoop

LongNOPActiveLoop:

REPT    18
	NOP
ENDM
        sjmp LongNOPActiveLoop
;-----------------------------------------------------------------------------
; END of File
;-----------------------------------------------------------------------------

END