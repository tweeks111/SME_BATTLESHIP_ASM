;
; Animations.inc
;
; This file is part of the Battleship project.
;
; It contains functions to show notification banners on the screen during the game.
;
; Authors: Mathieu Philippart & Théo Lepoutte
; 
 

game_redraw:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This redraws the game screen after a notification.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Draw the boards
	RCALL screen_clear
	RCALL draw_boards
	; Draw the number of ships left
	RCALL game_notify_ship_counter
	; Draw the game maps content
	RCALL update_game_maps
	
	RET


game_notify_your_turn:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show a "Your Turn" banner on the screen during 2 seconds.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Clear the screen
	RCALL screen_clear

	; Draw the banner
	draw_your_turn

	; Sleep 2 seconds
	sleep_ts 20

	; Redraw the game screen
	RCALL game_redraw

	RET


game_notify_enemy_turn:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show an "Enemy turn" banner on the screen during 2 seconds.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Clear the screen
	RCALL screen_clear

	; Draw the banner
	draw_enemy_turn

	; Sleep 1.5 seconds
	sleep_ts 20

	; Redraw the game screen
	RCALL game_redraw

	RET
	

game_notify_shot_missed:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show a "Missed" banner on the screen during 1.5 seconds.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Clear the screen
	RCALL screen_clear

	; Draw the banner
	draw_missed 3

	; Sleep 1.5 seconds
	sleep_ts 15

	; Don't redraw the game screen since this notify is followed by another notify

	RET
	

game_notify_shot_hit:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show a "Hit" banner on the screen during 1.5 seconds.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Clear the screen
	RCALL screen_clear

	; Draw the banner
	draw_hit 3

	; Sleep 1.5 seconds
	sleep_ts 15

	; Don't redraw the game screen since this notify is followed by another notify

	RET
	

game_notify_shot_sunk:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show a "Sunk" banner on the screen during 1.5 seconds.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Clear the screen
	CALL screen_clear

	; Draw the banner
	draw_sunk 3

	; Sleep 1.5 seconds
	sleep_ts 15

	; Don't redraw the game screen since this notify is followed by another notify

	RET

game_notify_wait_player1:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show "Waiting for Player 1" on the screen.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Draw a text "Waiting for Player 1"
	CALL screen_clear
	draw_word 6, 1, 3, WordWaiting
	draw_word 5, 8, 3, WordPlayer1
	
	RET

game_notify_wait_player2:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show "Waiting for Player 2" on the screen.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Draw a text "Waiting for Player 2"
	CALL screen_clear
	draw_word 6, 1, 3, WordWaiting
	draw_word 5, 8, 3, WordPlayer2
	
	RET

game_notify_ship_counter:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show the ships left for every player on the screen.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI R16, 15		; X
	MOV R10, R16	
	LDI R16, 8		; Y
	MOV R11, R16	
	LDI R16, 3		; Brightness
	MOV R12, R16	

	; Show the number of player's ships left
	LDI R16, 5
	LDS R17, SKSCNT
	SUB R16, R17
	MOV R13, R16	
	CALL screen_set_char

	; Show the number of enemy's ships left
	LDI R16, 22		; X
	MOV R10, R16	
	LDI R16, 5
	LDS R17, SKESCNT
	SUB R16, R17
	MOV R13, R16	
	CALL screen_set_char

	; Draw a separation line between the numbers
	draw_line 19, 10, 3, 2

	RET 