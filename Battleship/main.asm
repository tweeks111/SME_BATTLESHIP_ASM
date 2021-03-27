;
; main.asm
;
; This file is part of the Battleship project.
;
; This is the main file. It includes all the component's files and 
; declare the ISRs for all interrupt addresses. 
;
; Authors: Mathieu Philippart & Théo Lepoutte
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
;   Animation Interrupt	 ;
;  Persistent Registers  ;
;------------------------;
; R9  | Animation cnt.	 ;
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
JMP Timer2OverflowInterrupt
.ORG 0x001A
JMP Timer1OverflowInterrupt
.ORG 0x0020
JMP Timer0OverflowInterrupt


.INCLUDE "Timer1Mux.inc"
.INCLUDE "Sleep.inc"
.INCLUDE "CoreI2C.inc"
.INCLUDE "Communication.inc"
.INCLUDE "Buzzer.inc"
.INCLUDE "Screen.inc"
.INCLUDE "ScreenDrawings.inc"
.INCLUDE "Game.inc"
.INCLUDE "GameMaps.inc"
.INCLUDE "Cursor.inc"
.INCLUDE "Placement.inc"
.INCLUDE "Keyboard.inc"
.INCLUDE "Animations.inc"
.INCLUDE "PlayerSelect.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init:
	; Configure output pin PC3 (LED BOTTOM)
	SBI DDRC,3			; Pin PC3 is an output
	SBI PORTC,3			; Output Vcc => Off

	; Initialize components
    CALL screen_init
	CALL timer1_mux_init
	CALL buzzer_init
	CALL i2c_init
	CALL anim_init

	; Initialize game maps SRAM content
	RCALL init_map_cells

	; Initialize player's ships list
	RCALL init_ships_list

	; Clear the screen
	RCALL screen_clear
	
	; This enables ALL previously configured interrupts
	SEI					; Enable Global Interrupt Flag

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init_intro:
	; Clear the screen
	RCALL screen_clear

	; Write the game's Home Screen
	buzzer_sound_async Sound_Intro_Long
	draw_title 3
	RCALL anim_intro

	; After the Home Screen, show the player selector
	RCALL player_select_menu
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init_ships_placement:
	; Set current game state to SHIPS_PLACEMENT
	game_change_state GS_SHIPS_PLACEMENT

	; Draw the boards
	RCALL screen_clear
	RCALL draw_boards

	; If player 2 is selected (SLAVE), call comm_slave_ship_placement_prepare
	player_2_rcall comm_slave_ship_placement_prepare

	; Start ship placement
	RCALL start_ship_placement

	isp_ship_placement_loop:
		; Listen to keyboard
		RCALL keyboard_listen

		;
		; Check if ship placement done, else continue the loop
		;

		; Place Y-pointer to the Ship Placement Status byte
		LDI YH,high(SPS_ADDR)
		LDI YL,low(SPS_ADDR)

		; Get the Ship Placement Status byte
		LD R16, Y

		; Exit the loop if SPS_DONE_BIT is set
		SBRS R16, SPS_DONE_BIT
		RJMP isp_ship_placement_loop
		
	; RCALL to comm_master_ship_placement_done if Player 1 selected, else RCALL comm_slave_ship_placement_done.
	selected_player_rcall comm_master_ship_placement_done, comm_slave_ship_placement_done
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init_main_game:
	; Set current game state to GS_MAIN_GAME
	game_change_state GS_MAIN_GAME

	; Update the game maps
	RCALL update_game_maps

	;
	; The first player to play is Player 1 (I2C master)
	;



main:
	; Mask to uncheck the bit 4 of the animation counter
	LDI R16, 0b11101111

	; Check if "5 Hz interrupt flag" is set
	SBRC R9, 4
	RJMP refresh_test

	RJMP main

	refresh_test:
		AND R9, R16

		; Update the game maps (without clearing the whole maps)
		RCALL screen_set_PCS_ships

		RJMP main
