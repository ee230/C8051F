$NOMOD51
;-----------------------------------------------------------------------------
; F00x_ADC0_TempSensor.ASM
;-----------------------------------------------------------------------------
; Copyright 2003, Copyright (C) 2005 Silicon Laboratories, Inc.
;
; FILE:        F00x_ADC0_TempSensor.ASM
; DEVICE:      C8051F00x, C8051F01x
; ASSEMBLER:   Keil A51
; AUTH:        BW
; DATE:        17 JUN 03
;
; This program provides an example of how to configure the on-chip temperature
; sensor with the ADC.  The ADC is configured for left-justified mode, so this
; code will work as-is on devices which have 10 or 12-bit ADCs.
;
; An external 18.432MHz crystal is used as the system clock source.
;
; The ADC is configured for left-justified mode, GAIN = 2, using Timer3 overflows 
; as the start-of-conversion source.  Timer3 is configured in auto-reload mode
; to overflow every 10ms.  The ADC conversion complete interrupt handler
; reads the ADC value and compares it with the expected value for room
; temperature (about 25 degrees C), stored in ROOMCODE.  If the measured
; temperature is below this value, the LED is turned off.  If the measured
; value is above ROOMCODE, the LED is turned on.
;
; The LED switch point can be easily modified by changing the value of ROOMCODE.
;
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; EQUATES
;-----------------------------------------------------------------------------

$INCLUDE (C8051F000.inc)

LED         EQU      P1.6        ; LED on target board ('1' is LED ON)

SYSCLK      EQU      18432       ; SYSCLK frequency in kHz

TC_10ms     EQU      (SYSCLK / 12) * 10; number of timer counts in 10ms

ROOMCODE    EQU      0xbaa0      ; left-justified ADC value for 25 degrees C.

;-----------------------------------------------------------------------------
; VARIABLES
;-----------------------------------------------------------------------------

MYDATA      SEGMENT DATA         ; declare DATA segment
            RSEG  MYDATA         ; select DATA segment

; ADC data variables
TEMPCODE:   DS    2              ; holding register for temp code (16-bit)
                                 ; stored MSB-first (like in 'C' code)

;-------------------
; STACK

STACK       SEGMENT IDATA        ; declare STACK segment
            RSEG  STACK
            DS    80h            ; reserve 128 bytes for stack

;-----------------------------------------------------------------------------
; MACRO DEFINITIONS
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; RESET AND INTERRUPT VECTOR TABLE
;-----------------------------------------------------------------------------

CSEG AT 0
            ljmp  Main

            org   7bh
            ljmp  ADC0_ISR       ; ADC0 end of conversion interrupt

;-----------------------------------------------------------------------------
; MAIN PROGRAM CODE
;-----------------------------------------------------------------------------

Temp_2      SEGMENT  CODE                 ; declare CODE segment
            RSEG  Temp_2                  ; select CODE segment
            USING 0                       ; using register bank 0

Main:
            mov   WDTCN, #0deh            ; disable watchdog timer
            mov   WDTCN, #0adh

            mov   SP, #STACK-1            ; init stack pointer

            mov   OSCXCN, #67h            ; enable external crystal oscillator
                                          ; at 22.1184MHz

            clr   A                       ; wait at least 1ms
            djnz  acc, $                  ;  wait 512us
            djnz  acc, $                  ;  wait 512us

osc_wait:                                 ; poll for XTLVLD-->1
            mov   a, OSCXCN
            jnb   acc.7, osc_wait

            orl   OSCICN, #08h            ; select external oscillator as 
                                          ; system clock source
            orl   OSCICN, #80h            ; enable missing clock detector

            mov   XBR2, #40h              ; Enable crossbar and weak pull-ups

            orl   PRT1CF, #40h            ; enable P1.6 (LED on target board) as 
                                          ; push-pull

            acall ADC0_Init               ; initialize ADC0 and temp sensor
            acall Timer3_Init             ; initialize Timer3

            acall Timer3_Start            ; enable Timer3
            acall ADC0_Enable             ; enable ADC

            setb  EA                      ; enable global interrupts

            sjmp  $                       ; spin forever

;-----------------------------------------------------------------------------
; MAIN SUBROUTINES
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; INTERRUPT VECTORS
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ADC0_ISR
;
; This ISR is activated on the completion of an ADC sample.  When this event
; occurs, the ADC value is copied to the holding variable TEMPCODE, and is
; compared with the code for 25 degrees C.  If the temperature is above
; 25 degrees C, the LED is turned on.  If the temperature is below 25 degrees
; C, the LED is turned off.  There is no correction here for self-heating.
;
ADC0_ISR:
            push  PSW                     ; preserve registers
            push  acc

            clr   ADCINT                  ; clear ADC0 interrupt flag

            mov   TEMPCODE, ADC0H         ; copy MSB of ADC0 result into 
                                          ; TEMPCODE
            mov   TEMPCODE+1, ADC0L       ; copy LSB of ADC result into
                                          ; TEMPCODE

            ; compare TEMPCODE with value expected for 25 degrees C

            ; if (TEMPCODE - ROOMDEG) < 0, then turn LED off, otherwise, turn it on.
            ; calculate TEMPCODE - ROOMREG and store in TEMPCODE (16-bit subtract)

            clr   C
            mov   a, TEMPCODE+1           ; subtract LSBs
            subb  a, #LOW(ROOMCODE)
            mov   TEMPCODE+1, a           ; store new LSB
            mov   a, TEMPCODE             ; subtract MSBs (and carry)
            subb  a, #HIGH(ROOMCODE)
            mov   TEMPCODE,a              ; store new MSB

            setb  LED                     ; turn LED on.
            jnc   ADC0_ISR_END            ; exit if subtract result was positive,
            clr   LED                     ; otherwise, turn LED off then exit

ADC0_ISR_END:
            pop   acc
            pop   PSW

            reti

;-----------------------------------------------------------------------------
; SUBROUTINES
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; Timer3_Init
;-----------------------------------------------------------------------------
; This routine initializes Timer3 in 16-bit auto-reload mode to overflow
; at 100Hz using SYSCLK/12 as its time base.  Exits with Timer3
; stopped and Timer3 interrupts disabled.
;
Timer3_Init:
            mov   TMR3CN, #00h            ; stop Timer3, clear TF3, use
                                          ; SYSCLK/12 as timebase
            mov   TMR3RLH, #HIGH(-TC_10ms); init reload values
            mov   TMR3RLL, #LOW(-TC_10ms)
            mov   TMR3H, #0ffh            ; set to auto-reload immediately
            mov   TMR3L, #0ffh
            anl   EIE2, #NOT(01h)         ; disable Timer3 interrupts

            ret

;-----------------------------------------------------------------------------
; TIMER3_Start
;-----------------------------------------------------------------------------
; This routine starts Timer3
;
Timer3_Start:
            orl   TMR3CN, #04h            ; set TR3
            ret

;-----------------------------------------------------------------------------
; ADC0_Init
;-----------------------------------------------------------------------------
; This routine initializes ADC0 for left-justified mode monitoring the
; on-chip temp sensor at a gain of '2'.  Leaves ADC in disabled state.
;
ADC0_Init:
            clr   ADCEN                   ; disable ADC
            mov   REF0CN, #07h            ; enable temp sensor, bias
                                          ; generator, and output
                                          ; buffer
            mov   AMX0SL, #0fh            ; select TEMP sensor as ADC0
                                          ; input
            mov   ADC0CF, #80h            ; set SAR clock to SYSCLK/16
            orl   ADC0CF, #01h            ; PGA Gain = 2
            mov   ADC0CN, #45h            ; ADC disabled, low power
                                          ; track mode, ADC0 conversions
                                          ; initiated by overflow on
                                          ; Timer3, left-justified data
            ret

;-----------------------------------------------------------------------------
; ADC0_Enable
;-----------------------------------------------------------------------------
; This routine enables the ADC and ADC interrupts.
;
ADC0_Enable:
            setb  ADCEN                   ; enable ADC
            orl   EIE2, #02h              ; enable ADC EOC interrupt
            ret

;-----------------------------------------------------------------------------
; End of file.

END