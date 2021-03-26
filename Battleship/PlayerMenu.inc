;
; PlayerMenu.inc
;
; This file is part of the Battleship project.
;
; It contains the functions to select the player at the beginning of the game
;
; Authors : Mathieu Philippart & Th�o Lepoutte
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
		RCALL comm_master_discovery
		RET
	
	; Process discovery in slave mode if Player 2
	psm_p2_slave:
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