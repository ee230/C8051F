;-------------------------------------------------------------------------------
; Copyright (c) Raisonance S.A. 1989 - 2003
; This is the 'C' language "STARTUP" routine for RC51
;
;      File name:  STARTUP.A51
;
;        Purpose:  To provide the initial starting code for the 8051 from
;                  power-up to the initial execution of the main() routine.
;
;          Scope:  This file contains all the code and information required
;                  to accomplish "Purpose".  It is applicable to all memory
;                  models and initialization concepts EXCEPT the "Tiny"
;                  model. This file, assembled in the appropriate memory model,
;                  is automatically linked in by the linker when it encounters
;                  a "main" symbol in the object modules to be linked.
;
;                  The routine will be executed each time the controller
;                  experiences a "power-on" condition.
;
;                  If you wish to use this startup routine with your assembly
;                  code too, you should add a "main" label in your code.
;
; Considerations:  The values selected are intended for a standard 8051 controller.
;                  In most cases these will work well for must other processors
;                  under most conditions.  The ranges for initialization, or the
;                  initialization values can be changed or modified as needed.
;
;   Other micros:  The most common change to this module will be to accommodate
;                  the Philips '751 chips with their restricted internal memory
;                  space, and the lack of LJMP or LCALL instructions. The
;                  locations in this module that may need changing are marked
;                  with: (** For 751 use **)
;
;        Caveats:  It is possible to modify this file and effect the execution
;                  the target processor in many ways.  Often settings and
;                  selections have effects on each other.  Some many not be
;                  compatible.  This startup routine is provided "as is".  If
;                  you change it to meet some design objective you are responsible
;                  to know the outcome of your selections, and their effect upon
;                  the target system.
;
;-------------------------------------------------------------------------------
;  NOTE:  YOU SHOULD ALTER THIS FILE ONLY AFTER YOU HAVE BUILT AND TESTED YOUR
;  CODE, AND YOU'RE CONVINCED THAT YOU CAN MAKE IT RUN OR WORK BETTER BY DOING
;  SO.  IF YOU ONLY NEED SOME SPECIFIC INITIALIZATIONS, IT IS RECOMMENDED THAT
;  YOU DO THEM IN THE FIRST FEW LINES OF YOUR MAIN() ROUTINE.  MODIFY THIS FILE
;  ONLY AS A LAST RESORT.
;-------------------------------------------------------------------------------

;----------------------------------------------
; Register Declarations for the 8051 Processor
; Copyright (c) Raisonance SA, 1987-2003
;----------------------------------------------

; BYTE Register
P2      DATA    0A0H
SP      DATA    81H
TCON    DATA    88H
TMOD    DATA    89H
TL0     DATA    8AH
TL1     DATA    8BH
TH0     DATA    8CH
TH1     DATA    8DH
SCON    DATA    98H

; BIT Register
; PSW
CY      BIT     0D7H
AC      BIT     0D6H
F0      BIT     0D5H
RS1     BIT     0D4H
RS0     BIT     0D3H
OV      BIT     0D2H
P       BIT     0D0H

EXTRN    IDATA ( _STACK )
EXTRN    CODE  ( main )       ; main routine called at the end of this startup.
EXTRN    DATA  ( HPD_ADDR )   ; HPD_ADDR is supposed to be P2 address...
EXTRN    NUMBER ( _INITP2? )  ; Initial value for P2

;-------------------------------------------------------------------------------
; The next section contains the starting address and length of each of the data
; segments.  Note that you must use these labels given below, as Raisonance 51
; toolchain understands them and uses them to modify actual values in the module.
;
; The IDATA information and setup:
; Initialize the values for the IDATA space.  By default IDATA starts at
; location 0, and goes to top of internal memory.  On the 8051 that's 128 bytes
; (0x7F), on other derivatives it may be up to 256 bytes (0xFF).  On some it can
; be as little as 64 bytes.  Since it will always start at location 0x0000, we
; will permit the linker to manage this variable.
;   NOTE:  IDATA space physically overlaps both the DATA, REGISTER, and BIT spaces.
;   At a very minimum, the user is admonished to initialize at least the first 8
;   bytes (Register bank 0) and at least one additional byte for the stack.

IDATASTART  EQU   000h  ; a default value and is added only for completeness.
IDATALEN    EQU   080h  ; typical length is 128 bytes.
;
;   (** For 751 use **) you may need to adjust the IDATALEN value
;
; Note:  The entire internal address space is IDATA space!  The lower
;        128 bytes is also DATA space.  Both DATA and IDATA physically
;        overlap the BIT area.  So initializing either of these spaces
;        will initialize the BIT area as well.  Given the nature of the
;        8051 DATA space initializations are best done as IDATA accesses.
;
; The PDATA information and setup:
; Since PDATA resides in XDATA space it is included for reference only.  Note that
; you must use (and correctly initialize) P2 in order to succeed with PDATA accesses.
; The value for P2 is in PAGES of 256 bytes, and is dependent upon the address
; selected for that page.

PDATASTART  EQU   000h  ; this is the default value.
PDATALEN    EQU   000h  ; typical length is 256 bytes.

;
; The XDATA information and setup:
; If your application has non-standard address ranges for XDATA this is where you
; will need to alter the values.  The length and location of XDATA space can vary
; according to the chip and/or hardware design.  If reentrant functions are used,
; this space should be initialized.  But this is entirely dependent upon system
; and program design requirements.  By default, XDATA space IS NOT initialized.
; To force initialization you should make the XDATA length value non-zero.

XDATASTART  EQU   000h  ; the start address of XDATA space
XDATALEN    EQU   000h  ; typically we don't need to initialize xdata space

; ANSI 'C' language specification requires initialization of global and static
; variables. This is done at the end of this startup. However, if for any reason
; you do not want this initialization, turn INIDATA to zero.
;-------------------------------------------------------------------------------
;
; Advanced details:
; To initialize static and global variables, RC51 generates a partial segment
; with the name ?C_INITSEGSTART.  The contents of this segment are:
; (nb = number of bytes)
; 0 - nb * 4 + bit 0 if in data memory
;            + bit 1 if in xdata memory
;            + bit 2 if zero initialization
;     Note:  if bit 0 and bit 1 of nb are zero, it is a bit initialization
;            else we are in a data, idata or xdata initialization
; 1 - Low byte of address to initialize
; 2 - High byte of address to initialize
; 3 - data (limited to 127 bytes)
;
; LX51 chains the partial segments into the final segment, and adds a NULL
; termination.
;
;-------------------------------------------------------------------------------

INIDATA     EQU   001h  ; allow global and static variable initialization.

;
; This selection permits or denies the initialization of chip I/O registers.  By
; default, I/O initialization is OFF.  On critical programs eliminating this step
; can save a few bytes of code.  Typically chip I/O is initialized early in the
; MAIN() routine.  To do this initialization in this startup routine, make the
; variable non-zero.  You may insert additional initialization requirements into
; the code section controlled by this directive.  For instance, you could put
; initialization of a watchdog timer into this section.

INITIO      EQU   001h  ; make non-zero to permit I/O initialization
TIM1_INIT   EQU   0E8h  ; timer 1 initialization value for 9600 baud

;-------------------------------------------------------------------------------
; Watch dog timer macro:
; In some instances when many initializations are performed it may be necessary
; to service the watch dog timer.  The actual code needed to accomplish this
; will be dependent upon the actual device in use, and it's operating
; characteristics.  This is a template for such a watch dog timer function.

PET_THE_DOG MACRO
                        ; Enter your watchdog service code here.
ENDM

;
; To use this macro, place it into the code stream at the location where it may
; need to be called.

;-------------------------------------------------------------------------------
; The next section contains the EXTERNAL stack initialization directive.

XSTACK      EQU   000h  ; make non-zero to use external stack
XSTACKLEN   SET   000h  ; an initial length for the external stack

;-------------------------------------------------------------------------------
; Miscellaneous directives.

INTERNAL_XRAM_LIKE_8xC592  EQU   000h  ; make non-zero to use internal XRAM.

;===============================================================================
; Startup relocatable CODE segment name is set here.
?PR?C51_STARTUP?  SEGMENT  CODE

   NAME STARTUP

;-------------------------------------------------------------------------------
; By design, all code for the 8051 MUST start at location 0000h.  In special cases
; it is desirable to change this location.  For instance, to make a monitor
; resident at location 8000h.  When you do this other considerations are required.
; For instance, the location of interrupts must be considered.
;
; 1.  Set the CODE segment defined above to begin at location 0x0000.  To change
;     the location of where you want your startup vector to be located you would
;     modify this value.  Note:  That there are considerations beyond just simple
;     code locations if you do this.  Consequently this action is NOT RECOMMENDED!

   CSEG    AT  02600H

; 2.  Make the label public and jump to it.  This permits the vector to live at
;     the CSEG location defined above, and the rest of the code to be relocated
;     as required by the linker to resolve it's instructions and directives.

   PUBLIC   ?C_START

   LJMP  ?C_START  ;(** For 751 use **) you must change the LJMP to an AJMP

; 3.  Set the C51_STARTUP segment to be relocatable

   RSEG  ?PR?C51_STARTUP?

?C_START:

;>>>>>>LX51_STARTUP_MICRO_SPECIFIC
;NOTE - Ride will add and remove initialization code here.
;DO NOT EDIT this block of code!
;<<<<<<LX51_STARTUP_MICRO_SPECIFIC


;-------------------------------------------------------------------------------
; 4.  Stack definition and initialization:
; The "STACK" is not actually defined anywhere.  It is always located in IDATA
; space.  It is always only 1 byte long.  And it always GROWS UP.  The stack has
; been given the name, "_STACK".  This location, by name, will be adjusted by the
; linker to be located on top of all internal memory (registers+DATA+BITS+IDATA)
; consumed.

   MOV   SP, #_STACK

;-------------------------------------------------------------------------------
; 5.  Initialize the INTERNAL data memory spaces.
; First we do the internal memory space.  It must always be initialized, for
; all chips, and all memory models.  Internal memory will be initialized with
; whatever value is set into A.  Zero is recommended, although other values
; could be used for special debugging reasons.
;
; Note:  This initialization counts DOWN.

IF IDATALEN <> 0
   CLR   A
   MOV   R0, #LOW(IDATALEN - 1)

__INTERNAL_MEM_INIT__:
   MOV   @R0, A
   DJNZ  R0, __INTERNAL_MEM_INIT__
ENDIF

;-------------------------------------------------------------------------------
; 6.  Initialize the EXTERNAL data memory spaces.
; We initialize the XDATA memory counting up from XDATASTART.  Note that
; PDATA is the same location as XDATA.
;
; (** For 751 use **) CUT FROM HERE ---------- vvvvvvv  ----------
; Either EXPLICITLY do not use XDATA in your code, or comment out or remove
; the entire XDATA initialization section to make absolutely sure it is NOT used.
;
; Note:  This initialization counts UP.
;
IF XDATALEN <> 0
   MOV   DPTR, #XDATASTART    ; set to lowest address, start there
   MOV   R1, #LOW(XDATALEN)   ; get the low byte of the length

 IF (LOW (XDATALEN)) <> 0
   MOV   R0, #(HIGH XDATALEN) + 1   ; add one for physical -> logical value
 ELSE
   MOV   R0, #HIGH (XDATALEN)       ; if 0, then it doesn't matter
 ENDIF

   CLR   A                 ; or use a special value for debugging purposes

FILLXD:
   MOVX  @DPTR, A          ; write the value contained in A through DPTR
   INC   DPTR              ; point to the next higher byte location
   DJNZ  R1, FILLXD        ; loop until filled, fall through to outer loop on 0
   PET_THE_DOG             ; if needed, put the watch dog service routine here
   DJNZ  R0,FILLXD         ;
ENDIF


;-------------------------------------------------------------------------------
; 7.  Initialization of all global and static variables.

IF INIDATA <> 0
EXTRN   CODE (?C_INITSEGSTART)      ; Data used to initialize.

 IF XSTACK <> 0
  EXTRN   XDATA (_XSTK0)
 ENDIF

F1 BIT   0D1h                       ; F1 bit of PSW definition.

   MOV   DPTR, #?C_INITSEGSTART     ; Base address.

__PAQ__:
   ;------------------------------
   ; Read identification byte (nb)
   ;------------------------------
   CLR   A
   MOVC  A, @A+DPTR
   JNZ   __INIDATA_LABEL__          ; Zero is the end of the initialization.

$IF( INTERNAL_XRAM_LIKE_8xC592 )
   MOV   P2, #0FFH
$ELSE
 $IF( XSTACK )
   MOV   P2, #HIGH(_XSTK0)
 $ENDIF
$ENDIF

   SJMP  __END_OF_INIT__

__INIDATA_LABEL__:
   ;---------------------------------------------------------
   ; LSB indicates internal memory (0) or external memory (1)
   ;---------------------------------------------------------
   CLR   C
   RRC   A
   MOV   F0, C
   ;---------------------------------------------------------------------
   ; Bit 1 indicates full zero init (1) or init with different values (0)
   ;---------------------------------------------------------------------
   CLR   C
   RRC   A
   MOV   F1, C
   ;------------------------------------------------------
   ; The others bits indicate the number of bytes to read
   ;------------------------------------------------------
   MOV   R7, A
   INC   DPTR
   CLR   A
   MOVC  A, @A+DPTR        ; Read low byte of address to initialize.
   MOV   R0, A
   ANL   C, /F0            ; Verify if it is not a bit initialization.
   JC    __BIT_INIT__
   JNB   F0, __INIT_LOOP__
   CLR   A
   INC   DPTR
   MOVC  A, @A+DPTR        ; Read high byte of address to initialize.

$IF( INTERNAL_XRAM_LIKE_8xC592 )
   MOV   B, A
$ELSE
   MOV   P2, A
$ENDIF

__INIT_LOOP__:
   ;------------------------------
   ; Initialization of memory area
   ;------------------------------
   CLR   A
   JB    F1, __ZERO_INIT__
   INC   DPTR
   MOVC  A, @A+DPTR           ; Read initialization value

__ZERO_INIT__:
   JB    F0, __XDATA_INIT__

   MOV   @R0, A
   INC   R0

__SIZE_LOOP__:
   DJNZ  R7, __INIT_LOOP__

__SIZE_LOOP_1__:
   INC   DPTR
   SJMP  __PAQ__

__XDATA_INIT__:
$IF( INTERNAL_XRAM_LIKE_8xC592 )
   XCH   A, DPH
   XCH   A, B
   XCH   A, DPH
   XCH   A, DPL
   XCH   A, R0
   XCH   A, DPL
   MOVX  @DPTR, A
   INC   DPTR
   XCH   A, DPH
   XCH   A, B
   XCH   A, DPH
   XCH   A, DPL
   XCH   A, R0
   XCH   A, DPL
$ELSE
   MOVX  @R0, A
   INC   R0
   CJNE  R0, #0, __SIZE_LOOP__
   INC   P2
$ENDIF
   SJMP  __SIZE_LOOP__

__BIT_INIT__:
   MOV   A, R0
   RR    A
   RR    A
   RR    A
   ANL   A, #1Fh
   ADD   A, #20h
   MOV   R1, A           ; Byte address
   MOV   A, R0
   ANL   A, #7h          ; Bit offset
   MOV   R0, A
   CLR   A
   SETB  C
   RLC   A
   CJNE  R0, #00, __MASK_GEN__
   SJMP  __MASK_APPLY__

   ;---------------------------------------------
   ; Generate mask to initialize the correct bit
   ;---------------------------------------------
__MASK_GEN__:
   RLC   A
   DJNZ  R0, __MASK_GEN__

   ;------------------------------------------
   ; Initialize the correct bit with the mask
   ;------------------------------------------
__MASK_APPLY__:
   ORL   A, @R1
   MOV   @R1, A
   SJMP  __SIZE_LOOP_1__

__END_OF_INIT__:

ENDIF


;-------------------------------------------------------------------------------
; 8.  If selected, initialize the chips I/O registers.  This applies to all
; chips and all memory models.  You can save a few bytes by eliminating
; this code for some applications.

IF INITIO <> 0
   MOV   TMOD, #20H
   MOV   TCON, #40H
   MOV   SCON, #52H
   MOV   TH1, #low(TIM1_INIT)
ENDIF


;-------------------------------------------------------------------------------
; 9.  If selected, initialize the external stack.
; Note:  (** For 751 use **) Do not use this section the 751 processors.
IF XSTACK <> 0
EXTRN DATA  ( SPX )
EXTRN XDATA (_XSTK0)
XSTACKLEN   SET   0100h    ; an initial length for the external stack
   MOV   SPX, #low(_XSTK0)-1
ENDIF
	MOV     HPD_ADDR, #_INITP2?      ;SC030313 MOV     P2, #_INITP2?

; (** For 751 use **) CUT TO HERE ---------- ^^^^^^^ ----------


   LJMP  main     ;  (** For 751 use **) changed the LJMP to an AJMP

   END            ; End of module

;===============================================================================
;                               STARTUP.A51
;===============================================================================

