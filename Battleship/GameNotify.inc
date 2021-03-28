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


game_notify_go:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show a "Go!" banner on the screen during 1.5 seconds.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Clear the screen
	RCALL screen_clear

	; Draw the banner
	draw_go

	; Sleep 1.5 seconds
	sleep_ts 15

	; Redraw the game screen
	RCALL game_redraw

	RET


game_notify_enemy_shot:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This show an "Enemy shot" banner on the screen during 1.5 seconds.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Clear the screen
	RCALL screen_clear

	; Draw the banner
	draw_enemy_shot

	; Sleep 1.5 seconds
	sleep_ts 15

	; Redraw the game screen
	RCALL game_redraw

	RET