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
	RCALL screen_clear

	; Draw the banner
	draw_sunk 3

	; Sleep 1.5 seconds
	sleep_ts 15

	; Don't redraw the game screen since this notify is followed by another notify

	RET