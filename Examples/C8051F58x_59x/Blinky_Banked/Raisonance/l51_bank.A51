;=============================================================================
; File:  L51_BANK.A51
; Date:  Sep 1994
;   By:  FL
; Modif:  20.01.96 - SWB
;         15.07.03 - FL
;=============================================================================
; This module is used by LX51 to execute CODE bank switches, and comes in two 
; parts.  The first part is the common bridge to user's routines located in
; extended (banked) address areas. This section may be altered to suit your 
; particular hardware needs.
;
; NOTE:  Only the macro named SWITCH_BANK is to be modified by the user!
;-----------------------------------------------------------------------------
;                  * * * Important * * *
; The bank switching mechanism can be used in two modes:
;    * The "standard mode" for bank switching uses the external stack 
;      of the COMPACT and LARGE models to store the 3 additional 
;      bytes needed for each inter-bank call.
;    * The PL/M mode pushes those 3 bytes on the internal stack
;
; Although the PL/M mode provides a faster bank switch time, most 
; applications running in banked mode are short of internal RAM. 
; Consequently the use of the standard mode is usually the best
; solution and is assumed in this module.
;
; To use the PL/M mode:
;    * "PLM" has to be defined (using the set/equ commands), or in the 
;      command line when assembling this file.  For example:
;           MA51  L51_BANK.A51 SET(PLM)
;    * Then "XOPT(PLM)" has to be in the command line when invoking
;      LX51.  For example:
;           LX51  func1.obj, func2.obj,... XOPT(PLM) ...
;
;=============================================================================
; NOTE :
;   if you do not use the PL/M mode and there is in your project some functions
; that have two generic pointers as first parameters, those parameters will be
; passed in R1-R2-R3 for the first one and R0-R4-R5 for the second one. Then if
; at least one call of those functions needs a bank switch, you have to
; uncomment the following line:
;               $SET(TWO_PARAMS_SIZE_3_INREG=1)
;   This will change the bank switching procedure to save R0 register.
;=============================================================================
$nodb
$nolines
$MOD51
NAME ?BANK?SWITCHING

;=============================================================================
; NOTE:  DO NOT change the name of these two segments.  They are required by
; the L51 bank mode management facility.
;-----------------------------------------------------------------------------

?BANK?SWITCH		SEGMENT 	CODE
?DT?BANK?SWITCH 	SEGMENT		DATA

;-----------------------------------------------------------------------------

EXTRN 		DATA(SPX)	;external stack pointer

;-----------------------------------------------------------------------------
;
; The following two public declarations are not required, but are useful
; for the simulator/debugger.
;
;-----------------------------------------------------------------------------
PUBLIC		__SWITCH__0
PUBLIC  		__SWITCH__1
PUBLIC		?B_CURRENTBANK


;-----------------------------------------------------------------------------


;=============================================================================
;
; This is the switching-macro.  This is normally the only place where 
; changes need to be made to permit adaptation to the switch logic of 
; the target system.
;
;-----------------------------------------------------------------------------
	SWITCH_BANK	macro
$INCLUDE  (BNK_SWTC.MAC)
	endm
;-----------------------------------------------------------------------------
;
;
;=============================================================================
;
;
;=============================================================================

rseg ?DT?BANK?SWITCH
	?B_CURRENTBANK:	ds	 1

;=============================================================================
rseg ?BANK?SWITCH

	XCALL:		; common code to switch to destination bank.
$if (PLM)
		;=======================================================================
		; -------------------------- PLM mode ----------------------------------
		;=======================================================================
		; The RETURN address is pushed onto the hardware (internal) stack
		; Historically, this mode is called "PLM" because it has been developed
		; in 1989 for some programmers using the PLM programming language.  
		; It can be used with RC51 while the parameters of the destination 
		; function are NOT PUSHED! Indeed, this mode allows faster switches, but 
		; it modifies the stack frame.  
		; Note that the RC51 "printf" function cannot be called in PLM mode.  
		;=======================================================================
			; Save the current bank ID to restore it after returning from dest
			push	?B_CURRENTBANK
			xch   A,B
			mov	?B_CURRENTBANK,A	
			; Change the return address for RET_ADDR
			mov	A,#low(RET_ADDR)
			push	ACC
			mov	A,#high(RET_ADDR)
			push	ACC
			; Prepare the indirect JUMP (push lowPC, push HighPC, RET)
			push	DPL
			push	DPH
			;**************************************
			SWITCH_BANK		; switch to the new bank
			;**************************************
			mov   A,B		; original ACC content is restored. 
$else
		;=======================================================================
		; -------------------------- Standard mode -----------------------------
		;=======================================================================
		; The RETURN address is pushed onto the EXTERNAL stack. This mode keeps 
		; the integrity of the stack frame. However, it makes the switch slower. 
		;=======================================================================
		$if (TWO_PARAMS_SIZE_3_INREG)
				;-----------------------------------------------------------------
				; A very special case occurs when at least one destination function
				; has two 3-byte parameters in register (e.g. R1R2R3 and R0R4R5).
				; In such a situation, R0 must be saved temporarily in the external
				; stack.
				;-----------------------------------------------------------------
								; prepare the external stack to receive 4 bytes:
								; return address (2 bytes), current bank (1 byte)
								; and temporarily R0
                        inc     SPX
                        inc     SPX
                        inc     SPX
                        inc     SPX
                        mov     A, SPX
                        xch     A, R0
                        movx    @R0, A
                        dec     R0
                        dec     R0
                        dec     R0
                        ; Move the return address onto the external stack
                        pop     ACC
                        movx    @R0, A
                        inc     R0
                        ; Move the return address onto the external stack
                        pop     ACC
                        movx    @R0,A
                        mov     A,?B_CURRENTBANK
                        inc     R0
                        ; Save the current bank onto the external stack
                        movx    @R0,A
                        mov     ?B_CURRENTBANK,B

								; Change the return address for RET_ADDR
                        mov     A,#low(RET_ADDR)
                        push    ACC
                        mov     A,#high(RET_ADDR)
                        push    ACC
								; Prepare the indirect JUMP (push lowPC, push HighPC, RET)
								push	DPL
								push	DPH
								;**************************************
								SWITCH_BANK		; switch to the new bank
								;**************************************	
                        inc     R0
                        movx    A, @R0
                        ; restore R0 (possible parameter for dest)
                        mov     R0, A
                        dec     SPX
		; ===================================================================
		$else 	;Standard case using external stack
		; ===================================================================
								inc	SPX
								mov	R0,SPX
                        ; Move the return address onto the external stack
                        pop     ACC
								movx 	@R0,A			
								inc	R0
								inc	SPX
                        ; Move the return address onto the external stack
								pop	ACC
								movx 	@R0,A
                        ; Save the current bank onto the external stack
								mov	A,?B_CURRENTBANK
								inc	R0
								inc	SPX
								movx 	@R0,A
								; Set the NEW current bank
								mov	?B_CURRENTBANK,B
								; Change the return address for RET_ADDR
								mov	A,#low(RET_ADDR)
								push	ACC
								mov	A,#high(RET_ADDR)
								push	ACC
								; Prepare the indirect JUMP (push lowPC, push HighPC, RET)
								push	DPL
								push	DPH
								;**************************************
								SWITCH_BANK		; switch to the new bank
								;**************************************	
		$endif
$endif			
		; ===================================================================
		__SWITCH__0:		;INDIRECT JUMP to the DESTINATION
						ret	
		; ===================================================================
	RET_ADDR:		
		; we return HERE from the destination
		; We have to restore the original (source) bank before returning to
		; the original return address
		; ===================================================================
$if (PLM)
		; simply pop the source bank. 
		; Return address is already on the internal stack
			pop	?B_CURRENTBANK	; old bank is restored
		; ===================================================================
$else	; Move from the external stack bothe the current bank and the address.
		; ===================================================================
			mov	R0,SPX
			movx	A,@R0		; old bank is restored
			mov	?B_CURRENTBANK,A
			dec	R0
			movx	A,@R0
			push	ACC
			dec	R0
			movx	A,@R0
			push	ACC
			dec	R0
			mov	SPX,R0		
$endif
			;**************************************	
			SWITCH_BANK		; restore old area
			;**************************************
	__SWITCH__1 :	ret
		
;=============================================================================
;*;
;*; The following section is put here for reference and to satisfy the 
;*; curiosity of those who feel the need to "know".  The section below 
;*; describes the code generated by L51 for each bank ( XBANK# ) area 
;*; declared, and one FCTk$i for each public declaration in that area.
;*;
;=============================================================================
;*;     ;Bank 1		----------------
;*;     XBANK1 :        mov     B,#1            ;HC980702 no modification for PL/M ; Note: use MOV R0,#1 ; if PL/M
;*;			ajmp	XCALL
;*;			
;*;	FCT1$1:		mov	DPTR,#PROG1$1
;*;			ajmp 	XBANK1
;*;
;*;	FCT1$2:		mov	DPTR,#PROG1$2
;*;			ajmp 	XBANK1
;*;     .....
;*;
;*;     ;Bank 2		----------------
;*;     XBANK2 :        mov     B,#2            ;HC980702 no modification for PL/M ; Note: use MOV R0,#2 ; if PL/M
;*;			ajmp	XCALL
;*;			
;*;	FCT2$1:		mov	DPTR,#PROG2$1
;*;			ajmp 	XBANK2
;*;	......
;=============================================================================
;************************ Configuration Section *******************************
; If your debugger does not support the ".LIS" script file that describes the *
; list of the banks, you should fill the following section. If you are using  *
; the Raisonance RIDE debugger, you do not need to fill this section.         *
;******************************************************************************
?B_NBANKS       EQU     8        ; Define max. Number of Banks (8-16-32)      *
?B_MODE         EQU     0        ; 0 for Bank-Switching via 8051 Port         *
?B_CURB         EQU     P1       ; where is defined the current bank          *
?B_MASK         EQU     7        ; define the significant bits                *
?B_PORT         EQU     P1       ; default is P1                              *
?B_FIRSTBIT     EQU     0        ; for ex. if P1.2 is the first signific. bit *
?B_XDATAPORT    EQU     00H      ; default is XDATA Port Address 0FFFFH       *
?BANKSTART      EQU     0000H    ; where the banks start (default 00)         *
?BANKEND        EQU     0FFFFH   ; where the banks end  (default 0ffffh)      *
;******************************************************************************
PUBLIC  ?B_NBANKS, ?B_MODE, ?B_MASK , ?B_XDATAPORT 
PUBLIC  ?BANKSTART, ?BANKEND, ?B_CURB, ?B_FIRSTBIT, ?B_PORT
;******************************************************************************


end
