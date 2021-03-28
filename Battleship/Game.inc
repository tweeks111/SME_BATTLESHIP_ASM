;
; Game.inc
;
; This file is part of the Battleship project.
;
; It contains all the game management functions.
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

; Turn Shooting Status byte address in SRAM
#define SHOOT_STATUS_ADDR	0x01C4


game_init_interface:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the interface for the MAIN_GAME state.
	; Note: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set the cursor area to the right game map (rectangle at 28,2;10x10)
	LDI R16, 28
	MOV R10, R16
	LDI R16, 2
	MOV R11, R16
	LDI R16, 10
	MOV R12, R16
	MOV R13, R16
	RCALL set_cursor_area

	RET


game_fire:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called when the player want to fire the cursor's
	; position. It changes the Shooting Status byte to 1.
	; Note: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Check if the fired cell is empty
	RCALL game_check_cell

	; If the cell is not empty -> refuse firing
	SBRC R10, 0
	RJMP gf_firing_refused

	; Place the Y-pointer to the Shooting Status byte address
	LDI YH,high(SHOOT_STATUS_ADDR)
	LDI YL,low(SHOOT_STATUS_ADDR)

	; Set Shooting Status to 1
	LDI R16, 1
	ST Y, R16

	RET

	gf_firing_refused:
	; If firing was refused -> play firing refused sound
	buzzer_sound_async Sound_FiringRefused

	RET


game_check_cell:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function checks if the current cursor's position is on a non-
	; empty cell. It returns 0x01 in R10 if non-empty, else 0x00.
	; Note: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Get the current cursor's position (in X=R11, Y=R12)
	get_cursor_position

	; Compute the byte index from cursor position 
	LDI R16, 10				; Size of a row is 10
	MUL R12, R16			; Multiply the Y position by the row size
	ADD R0, R11				; Add the X position

	; Set the Y-pointer to the first ECS byte's address
	LDI YH, high(ECS_ADDR)
	LDI YL, low(ECS_ADDR)

	; Add the byte index to the Y-pointer
	Add YL, R0

	; Get the cell state from ECS
	LD R16, Y

	; Return value if cell is empty
	CLR R17

	; If the cell is not empty, overwrite the return value to 1
	SBRS R16, ECS_EMPTY
		LDI R17, 1

	; Store return value to R10
	MOV R10, R17

	RET


game_play_turn:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function runs a game turn on this board.
	; Note: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Place the cursor on the point 0x0 of the game map
	LDI R16, 1
	CLR R10					; Cursor X
	CLR R11					; Cursor Y
	MOV R12, R16			; Cursor length = 1 pixel
	CLR R13					; Cursor Orientation (0 = horizontal)
	RCALL set_cursor_properties

	; Update the game maps
	RCALL update_game_maps

	; Initialize the Turn Shooting Status to 0
	LDI YH,high(SHOOT_STATUS_ADDR)
	LDI YL,low(SHOOT_STATUS_ADDR)
	CLR R16
	ST Y, R16

	; Keyboard actions loop
	gpt_turn_loop:
		; Listen to keyboard
		RCALL keyboard_listen

		; Run game maps animation routine
		RCALL animate_game_maps
		
		; Place the Y-pointer to the Shooting Status byte address
		LDI YH,high(SHOOT_STATUS_ADDR)
		LDI YL,low(SHOOT_STATUS_ADDR)

		; Get the current Shooting Status byte
		LD R16, Y

		; Exit the loop if the Shooting Status is different from 0
		CPI R16, 0
		BREQ gpt_turn_loop

	;
	; Here, the player pressed the Fire button.
	; The coordinate of the shot is the current cursor's position.
	;

	RET


game_enemy_fired_position:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called when the enemy fired at a certain position.
	; The shot position is stored in R11 and R12 (X, Y).
	; Note: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	CBI PORTC,3

	;
	; TODO: Check if a ship is hitted/sunk, update cell state in PCS and send the I2C answer for this shot
	;

	RET