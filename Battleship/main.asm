;
; main.asm
;
; This file is part of the Battleship project.
;
; This is the main file. It includes all the component's files and 
; declare the ISRs for all interrupt addresses. 
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

; NOTE: R0 and R1 are used to store the result of some operations (e.g. MUL)

; NOTE: If two interrupts occur at the same time, the 2nd waits for the first to finish.
;		Hence we can use the same non-persistent register in all interrupts without interferences.

; NOTE: The registers R26-R31 are used by the X, Y and Z pointers.
;		We reserve those registers for the X,Y,Z-pointers

;------------------------;
;    Screen Interrupt	 ;
;  Persistent Registers  ;
;------------------------;
; R2  | LED State mask   ;
;------------------------;
; R3  | PWM counter		 ;
;------------------------;
; R4  | Shift counter    ;
;------------------------;
; R5  | SRAM counter     ;
;------------------------;
; R6  | Row select       ;
;------------------------;

;------------------------;
;    Sleep Interrupt	 ;
;  Persistent Registers  ;
;------------------------;
; R7  | Timer1 sleep cnt ;
;------------------------;

;------------------------;
;    Buzzer Interrupt	 ;
;  Persistent Registers  ;
;------------------------;
; R8  | Timer2 cnt init  ;
;------------------------;

;------------------------;
; Interrupts Temp. Regs. ;
;------------------------;
; R25  | Temp. register  ;
;------------------------;

.INCLUDE "m328pdef.inc"
.ORG 0x0000
RJMP init
.ORG 0x0012
RJMP Timer2OverflowInterrupt
.ORG 0x001A
RJMP Timer1OverflowInterrupt
.ORG 0x0020
RJMP Timer0OverflowInterrupt

.INCLUDE "Screen.inc"
.INCLUDE "Keyboard.inc"
.INCLUDE "Timer1Mux.inc"
.INCLUDE "Sleep.inc"
.INCLUDE "Animations.inc"
.INCLUDE "Buzzer.inc"


init:
	; Configure output pin PC3 (LED BOTTOM)
	SBI DDRC,3			; Pin PC3 is an output
	SBI PORTC,3			; Output Vcc => Off

	; Initialize components
    RCALL screen_init
	RCALL timer1_mux_init
	RCALL buzzer_init
	
	; This enables ALL previously configured interrupts
	SEI					; Enable Global Interrupt Flag
	
	; Clear the screen
	CLR R10
	RCALL screen_fill

	; Write the game's home screen
	draw_title 3
	buzzer_sound Sound_Intro
	RCALL anim_intro

main:
	RJMP main


