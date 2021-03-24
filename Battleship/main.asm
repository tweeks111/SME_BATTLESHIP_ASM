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
;   Keyboard Reserved    ;
;------------------------;
; R24 | Kbd action sel.  ;
;------------------------;

;------------------------;
; Interrupts Temp. Regs. ;
;------------------------;
; R25 | Temp. register   ;
;------------------------;


.INCLUDE "m328pdef.inc"
.ORG 0x0000
JMP init						; Not RJMP because our program is... too fat -_-
.ORG 0x0012
RJMP Timer2OverflowInterrupt
.ORG 0x001A
RJMP Timer1OverflowInterrupt
.ORG 0x0020
RJMP Timer0OverflowInterrupt


.INCLUDE "Timer1Mux.inc"
.INCLUDE "Sleep.inc"
.INCLUDE "CoreI2C.inc"
.INCLUDE "Communication.inc"
.INCLUDE "Buzzer.inc"
.INCLUDE "Screen.inc"
.INCLUDE "ScreenDrawings.inc"
.INCLUDE "Game.inc"
.INCLUDE "Keyboard.inc"
.INCLUDE "Animations.inc"
.INCLUDE "PlayerMenu.inc"

init:
	; Configure output pin PC3 (LED BOTTOM)
	SBI DDRC,3			; Pin PC3 is an output
	SBI PORTC,3			; Output Vcc => Off

	; Initialize components
    CALL screen_init
	CALL timer1_mux_init
	CALL buzzer_init
	CALL i2c_init
	
	; This enables ALL previously configured interrupts
	SEI					; Enable Global Interrupt Flag

init_game:
	; Clear the screen
	RCALL screen_clear

	; Write the game's Home Screen
	buzzer_sound_async Sound_Intro_Long
	draw_title 3
	RCALL anim_intro

	; After the Home Screen, show the player selector
	RCALL player_select_menu
	
init_ships_placement:
	; Set current game state to SHIPS_PLACEMENT
	game_change_state GS_SHIPS_PLACEMENT
	
	; Draw the boards
	RCALL screen_clear
	RCALL draw_boards
	
	LDI YH,0x01
	LDI YL,0x8E
	LDI R16, 0x03	; X cursor
	ST Y+, R16
	LDI R16, 0x03	; Y cursor
	ST Y, R16
	RCALL draw_cursor

	;RCALL comm_master_discovery
	;RCALL comm_slave_discovery
	
main:
	RCALL keyboard_listen

	RJMP main
