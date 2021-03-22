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
.INCLUDE "Animations.inc"
.INCLUDE "Sleep.inc"
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
	RCALL write_battleship

main:
	; Let's play a sound
	buzzer_sound Sound_Intro

	sleep_ts 20

	buzzer_sound Sound_Winner

	sleep_ts 20

	buzzer_sound Sound_Looser
	
	sleep_ts 50

	RJMP main

	
write_go:

	draw_rect 14, 2, 3, 12, 9
	draw_rect 15, 3, 0, 10, 7
	draw_char 16, 4, 3, _G
	draw_char 20, 4, 3, _O
	draw_char 24, 4, 3, _EXC

	RET

write_battleship:
	draw_char 1, 4, 1, _B 
	draw_char 5, 4, 2, _A
	draw_char 9, 4, 3, _T
	draw_char 13, 4, 1, _T
	draw_char 17, 4, 2, _L
	draw_char 21, 4, 3, _E
	draw_char 25, 4, 1, _S
	draw_char 29, 4, 2, _H
	draw_char 33, 4, 3, _I
	draw_char 37, 4, 3, _P
	draw_line 0, 2, 3, 40
	draw_line 0, 10, 3, 40
	RET


