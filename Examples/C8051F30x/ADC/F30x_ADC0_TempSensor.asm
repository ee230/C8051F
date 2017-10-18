$NOMOD51
;-----------------------------------------------------------------------------
; F30x_ADC0_TempSensor.asm.ASM
;-----------------------------------------------------------------------------
; Copyright (C) 2005 Silicon Laboratories, Inc.
;
; FILE:        F30x_ADC0_TempSensor.asm.ASM
; DEVICE:      C8051F30x
; ASSEMBLER:   Keil A51
; AUTH:        BW
; DATE:        11 DEC 01
;
; This program provides an example of how to configure the on-chip
; temperature sensor with the ADC.
;
; The calibrated on-chip 24.5MHz internal oscillator is used as the system
; clock source.
;
; The ADC is configured for left-justified mode, GAIN = 2, using Timer2 
; overflows as the start-of-conversion source.  Timer2 is configured in 
; auto-reload mode to overflow every 10ms.  The ADC conversion complete 
; interrupt handler reads the ADC value and compares it with the expected 
; value for room temperature (about 25 degrees C), stored in ROOMCODE.  
; If the measured temperature is below this value, the LED is turned off.  
; If the measured value is above ROOMCODE, the LED is turned on.
;
; The LED switch point can be easily modified by changing the value of 
; ROOMCODE.
;
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; EQUATES
;-----------------------------------------------------------------------------

$INCLUDE (C8051F300.inc)

LED         EQU      P0.2                 ; LED on target board ('1' is LED 
                                          ; ON)

SYSCLK      EQU      24500                ; SYSCLK frequency in kHz

TC_10ms     EQU      (SYSCLK / 12) * 10   ; number of timer counts in 10ms

ROOMCODE    EQU      0x95                 ; ADC value for 25 degrees C.

;-----------------------------------------------------------------------------
; VARIABLES
;-----------------------------------------------------------------------------

;-------------------
; STACK

STACK       SEGMENT IDATA                 ; declare STACK segment
            RSEG  STACK
            DS    80h                     ; reserve 128 bytes for stack

;-----------------------------------------------------------------------------
; MACRO DEFINITIONS
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; RESET AND INTERRUPT VECTOR TABLE
;-----------------------------------------------------------------------------

            CSEG AT 0
            ljmp  Main

            org   43h
            ljmp  ADC0_ISR                ; ADC0 end of conversion interrupt

;-----------------------------------------------------------------------------
; MAIN PROGRAM CODE
;-----------------------------------------------------------------------------

Temp_2      SEGMENT  CODE                 ; declare CODE segment
            RSEG  Temp_2                  ; select CODE segment
            USING 0                       ; using register bank 0

Main:
            ; Disable the WDT.
            anl   PCA0MD, #NOT(040h)      ; clear Watchdog Enable bit

            mov   SP, #STACK-1            ; init stack pointer

            acall SYSCLK_Init             ; initialize system clock source

            acall PORT_Init               ; initialize crossbar and GPIO
            acall ADC0_Init               ; initialize ADC0 and temp sensor
            acall Timer2_Init             ; initialize Timer2

            setb  TR2                     ; START Timer2
            setb  AD0EN                   ; enable ADC

            setb  EA                      ; enable global interrupts

            sjmp  $                       ; spin forever

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
; C, the LED is turned off.
;
ADC0_ISR:
            push  PSW                     ; preserve registers
            push  acc

            clr   AD0INT                  ; clear ADC0 interrupt flag

            clr   C
            mov   a, ADC0                 ; read ADC value
            subb  a, #ROOMCODE            ; subtract room constant

            ; compare TEMPCODE with value expected for 25 degrees C
            ; if (TEMPCODE - ROOMDEG) < 0, then turn LED off, otherwise, 
            ; turn it on.

            setb  LED                     ; turn LED on.
            jnc   ADC0_ISR_END            ; exit if subtract result was 
                                          ; positive,
            clr   LED                     ; otherwise, turn LED off then exit

ADC0_ISR_END:
            pop   acc
            pop   PSW

            reti

;-----------------------------------------------------------------------------
; SUBROUTINES
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; SYSCLK_Init
;-----------------------------------------------------------------------------
; This routine initializes the system time base by selecting the internal
; 24.5MHz oscillator as the system clock source.  Also enables the missing
; clock detector.
;
SYSCLK_Init:
            mov   OSCICN, #07h            ; select internal oscillator at
                                          ; 24.5MHz as the system clock 
                                          ; source
            mov   RSTSRC, #04h            ; enable missing clock detector

            ret

;-----------------------------------------------------------------------------
; PORT_Init
;-----------------------------------------------------------------------------
; This routine initializes the crossbar and GPIO pins.
;
PORT_Init:
            orl   XBR0, #04h              ; skip LED pin in crossbar
                                          ; assignments
            mov   XBR2, #40h              ; Enable crossbar and weak pull-ups

            orl   P0MDOUT, #04h           ; enable P0.2 (LED on target board)
                                          ; as push-pull
            orl   P0MDIN, #04h            ; enable P0.2 (LED) as digital
                                          ; input
            ret

;-----------------------------------------------------------------------------
; Timer2_Init
;-----------------------------------------------------------------------------
; This routine initializes Timer2 in 16-bit auto-reload mode to overflow
; at 100Hz using SYSCLK/12 as its time base.  Exits with Timer2
; stopped and Timer2 interrupts disabled.
;
Timer2_Init:
            mov   TMR2CN, #00h            ; STOP Timer2, clear TF2H and TF2L;
                                          ; disable low-byte interrupt;
                                          ; disable split mode; select
                                          ; internal prescaler as timebase
            anl   CKCON, #NOT(060h)       ; Timer2 uses SYSCLK/12
            mov   TMR2RLH, #HIGH(-TC_10ms); init reload values
            mov   TMR2RLL, #LOW(-TC_10ms)
            mov   TMR2H, #0ffh            ; set to auto-reload immediately
            mov   TMR2L, #0ffh
            clr   ET2                     ; disable Timer2 interrupts

            ret

;-----------------------------------------------------------------------------
; ADC0_Init
;-----------------------------------------------------------------------------
; This routine initializes ADC0 for monitoring the on-chip temp sensor at a
; gain of '2'.  Enables the End of Converstion interrupt.  Leaves ADC in 
; disabled state.
;
ADC0_Init:
            mov   ADC0CN, #02h            ; ADC0 disabled, normal
                                          ; track mode, ADC0 conversions
														; initiated by overflow of
														; Timer2
            mov   AMX0SL, #0f8h           ; select temp sensor as ADC0 input

            ; ADC conversion clock <= 7.5 MHz
            mov   ADC0CF, #((SYSCLK / 7500) SHL 3)

            orl   ADC0CF, #02h            ; PGA gain = 2

            mov   REF0CN, #0eh            ; enable temp sensor; VREF = VDD;
                                          ; bias generator is on
            orl   EIE1, #04h              ; enable ADC0 End of Conversion
                                          ; Interrupt
            ret

;-----------------------------------------------------------------------------
; End of file.

END