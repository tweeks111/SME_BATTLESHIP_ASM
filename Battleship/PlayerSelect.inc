;
; PlayerSelect.inc
;
; This file is part of the Battleship project.
;
; It contains the functions to select the player at the beginning of the game
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

#define PLAYER_SELECT_ADDR	0x0194

.macro player_set_selected
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro changes the selected player in SRAM by the one given in
	; argument 0.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI YH,high(PLAYER_SELECT_ADDR)
	LDI YL,low(PLAYER_SELECT_ADDR)

	LDI R16, @0
	ST Y, R16
.endmacro

.macro player_get_selected
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro gets selected player from SRAM and stores it in R10.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI YH,high(PLAYER_SELECT_ADDR)
	LDI YL,low(PLAYER_SELECT_ADDR)
	LD R10, Y
.endmacro

.macro selected_player_rcall
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro RCALLs the argument 0 if Player 1 is selected, else it 
	; RCALLs the argument 1.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Get the currently selected player
	LDI YH,high(PLAYER_SELECT_ADDR)
	LDI YL,low(PLAYER_SELECT_ADDR)
	LD R10, Y

	; If Player 1, RCALL the argument 0
	SBRS R10, 0
	RCALL @0

	; Get the currently selected player (again, since R10 an Y may change)
	LDI YH,high(PLAYER_SELECT_ADDR)
	LDI YL,low(PLAYER_SELECT_ADDR)
	LD R10, Y

	; If Player 2, RCALL the argument 1
	SBRC R10, 0
	RCALL @1
.endmacro

.macro player_1_rcall
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro RCALLs the argument 0 if Player 1 is selected
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI YH,high(PLAYER_SELECT_ADDR)
	LDI YL,low(PLAYER_SELECT_ADDR)
	
	; Get the currently selected player
	LD R10, Y

	; If Player 1, RCALL the argument 0
	SBRS R10, 0
	RCALL @0
.endmacro

.macro player_2_rcall
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro RCALLs the argument 0 if Player 2 is selected
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI YH,high(PLAYER_SELECT_ADDR)
	LDI YL,low(PLAYER_SELECT_ADDR)
	
	; Get the currently selected player
	LD R10, Y

	; If Player 2, RCALL the argument 0
	SBRC R10, 0
	RCALL @0
.endmacro


player_select_menu:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function draws the Player Selection Menu then 
	; listen to the keyboard input until a player has been
	; selected. Finally, it initializes the I2C connection.
	; NOTE: this macro modifies: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	RCALL screen_clear
	draw_word 7, 1, 3, WordPlayer1
	draw_word 7, 8, 3, WordPlayer2
	
	; Set current game state to INIT
	game_change_state GS_INIT

	; Pre-select player 1 (selection accepted if Bit7 is set)
	RCALL player_select_1

	; Loop over the keyboard until the player is selected
	psm_keyboard_loop:
		RCALL keyboard_listen

		player_get_selected			; Get current player selection (stored in R10)
		SBRS R10,7					; If Bit7 is set -> selection validated -> exit the keyboard loop
		RJMP psm_keyboard_loop

	; Here, a player has been selected
	; Let Player 1 be the master and Player 2 the slave

	; If player 1 has been selected
	SBRS R10,0
		RJMP psm_p1_master
	; Else
		RJMP psm_p2_slave

	; Process discovery in master mode if Player 1
	psm_p1_master:
		; Draw a text "Waiting for Player 2"
		RCALL game_notify_wait_player2

		RCALL comm_master_discovery
		RET
	
	; Process discovery in slave mode if Player 2
	psm_p2_slave:
		; Draw a text "Waiting for Player 1"
		RCALL game_notify_wait_player1

		RCALL comm_slave_discovery
		RET


player_select_1:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function marks the Player 1 as selected on screen
	; and in SRAM.
	; NOTE: this macro modifies: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	draw_small_right_arrow 1, 3, 3	; Draw a small right arrow in front of the Player 1 line
	draw_rect 1, 9, 0, 4, 3			; Remove the arrow from the Player 2 line
	player_set_selected 0			; Mark Player 1 as selected in SRAM
	RET


player_select_2:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function marks the Player 2 as selected on screen
	; and in SRAM.
	; NOTE: this macro modifies: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	draw_rect 1, 2, 0, 4, 3			; Remove the arrow from the Player 1 line
	draw_small_right_arrow 1, 10, 3 ; Draw a small right arrow in front of the Player 2 line
	player_set_selected 1			; Mark Player 2 as selected in SRAM
	RET


player_select_validate:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function marks currently selected player as 
	; validated (Bit7 is set in SRAM).
	; NOTE: this macro modifies: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Place Y-pointer on PLAYER_SELECT_ADDR
	LDI YH,high(PLAYER_SELECT_ADDR)
	LDI YL,low(PLAYER_SELECT_ADDR)

	LD R16, Y		; Get current player selection
	SBR R16,(1<<7)	; Set the Bit7
	ST Y, R16		; Store the new byte in SRAM

	RET