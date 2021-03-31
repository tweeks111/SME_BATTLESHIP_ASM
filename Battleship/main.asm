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


; Prepend the Program Memory tables at the beginning
.INCLUDE "ProgMemTables.inc"

.INCLUDE "Timer1Mux.inc"
.INCLUDE "Sleep.inc"
.INCLUDE "CoreI2C.inc"
.INCLUDE "Buzzer.inc"
.INCLUDE "Screen.inc"
.INCLUDE "ScreenDrawings.inc"
.INCLUDE "GameState.inc"
.INCLUDE "GameMaps.inc"
.INCLUDE "Cursor.inc"
.INCLUDE "Communication.inc"
.INCLUDE "PlayerSelect.inc"
.INCLUDE "Placement.inc"
.INCLUDE "Game.inc"
.INCLUDE "Keyboard.inc"
.INCLUDE "Animations.inc"
.INCLUDE "GameNotify.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init:
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

	; Initialize the sunk enemy ships list
	RCALL game_init_enemy_ships_list

	; Clear the screen
	RCALL screen_clear
	
	; This enables ALL previously configured interrupts
	SEI					; Enable Global Interrupt Flag
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init_intro:
	; Clear the screen
	RCALL screen_clear

	; Play game intro sound (long version)
	buzzer_sound_async Sound_Intro_Long

	; Write the game's Home Screen
	draw_title 3
	RCALL anim_intro

	; After the Home Screen, show the player selector
	RCALL player_select_menu
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init_ships_placement:
	; Set current game state to SHIPS_PLACEMENT
	game_change_state GS_SHIPS_PLACEMENT
	
	; Play a game sound when both players are connected
	buzzer_sound_async Sound_PlayersConnected

	; Draw the boards
	CALL screen_clear
	CALL draw_boards

	; If player 2 is selected (SLAVE), call comm_slave_exchange_prepare to start listening I2C
	player_2_rcall comm_slave_exchange_prepare

	; Start ship placement
	RCALL start_ship_placement

	; Small delay to avoid double-press of the keyboard
	sleep_ts 4

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

	
	; Draw a text "Waiting for Player X"
	selected_player_rcall game_notify_wait_player2, game_notify_wait_player1

	; RCALL to comm_master_ship_placement_done if Player 1 selected, else RCALL comm_slave_ship_placement_done.
	selected_player_rcall comm_master_ship_placement_done, comm_slave_ship_placement_done
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
init_main_game:
	; Set current game state to GS_MAIN_GAME
	game_change_state GS_MAIN_GAME

	; Delay to avoid sounds overlapping
	sleep_ts 4

	; Play game intro sound (short version)
	buzzer_sound_async Sound_Intro
	
	; Draw the boards
	CALL screen_clear
	CALL draw_boards

	; Update the game maps & ship counter
	RCALL update_game_maps

	; Configure the interface for the game
	RCALL game_init_interface

	;
	; The first player to play is Player 1 (I2C master)
	;

	main_game_turns_loop:
		;
		; Now, it's the turn for Player 1 to play
		;
		; If player 2 is selected (SLAVE), call comm_slave_exchange_prepare to start listening I2C
		player_2_rcall comm_slave_exchange_prepare

		; Show a notify ("Your Turn" for player 1, "Enemy Turn" for player 2)
		selected_player_rcall game_notify_your_turn, game_notify_enemy_turn

		; If selected player is Player 1 (MASTER), call game_play_turn.
		player_1_rcall game_play_turn

		; If selected player is Player 1 (MASTER), send the Shooting Launch packet to slave.
		; If player 2 is selected (SLAVE), wait and receive the Shooting Launch packet from master.
		selected_player_rcall comm_master_send_shooting_launch_packet, comm_slave_receive_shooting_launch_packet
	
		; Get the game state and check if the game has been won
		game_get_state
		SBRC R10, GS_GAME_WON
			RJMP game_won
		SBRC R10, GS_GAME_LOST
			RJMP game_lost

		;
		; Now, it's the turn for Player 2 to play
		;
		; If player 2 is selected (SLAVE), call comm_slave_exchange_prepare to start listening I2C
		player_2_rcall comm_slave_exchange_prepare

		; Show a notify ("Go" for player 2, "Enemy shot" for player 1)
		selected_player_rcall game_notify_enemy_turn, game_notify_your_turn
		
		; If selected player is Player 2 (SLAVE), call game_play_turn.
		player_2_rcall game_play_turn

		; If selected player is Player 1 (MASTER), receive the Shooting Launch packet from slave. 
		; If player 2 is selected (SLAVE), wait ST and send the Shooting Launch packet to master.
		selected_player_rcall comm_master_receive_shooting_launch_packet, comm_slave_send_shooting_launch_packet
		
		; Get the game state and check if the game has been won or lost
		game_get_state
		SBRC R10, GS_GAME_WON
			RJMP game_won
		SBRC R10, GS_GAME_LOST
			RJMP game_lost

		RJMP main_game_turns_loop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
game_won:
	sleep_ts 10

	; Play winner sound
	buzzer_sound_async Sound_Winner
	
	; Clear the screen
	CALL screen_clear

	; Draw winner screen
	draw_winner 3

	RJMP main

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
game_lost:
	sleep_ts 10

	; Play looser sound
	buzzer_sound_async Sound_Looser
	
	; Clear the screen
	CALL screen_clear

	; Draw looser screen
	draw_looser 3

	RJMP main

main:
	; Listen to keyboard (if user wants to start a new game, press C)
	RCALL keyboard_listen

	RJMP main
