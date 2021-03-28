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
#define SHOOTING_STATUS_ADDR	0x01C4

; Sunk Enemy Ships list address in SRAM
#define SUNK_ENEMY_SHIPS_ADDR	0x02A0

; Player's Sunk Ships Counter address in SRAM
#define SUNK_SHIPS_CNT_ADDR		0x01BA


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
	
	; Place Y-pointer to the Sunk Ships Counter
	LDI YH,high(SUNK_SHIPS_CNT_ADDR)
	LDI YL,low(SUNK_SHIPS_CNT_ADDR)

	; Initialize the Sunk Ships Counter to 0
	CLR R16
	ST Y, R16

	RET

game_init_enemy_ships_list:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the SRAM content of the sunk enemy ships
	; list to 0x00 (all bytes).
	; NOTE: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Place Y-pointer to the sunk enemy ships list
	LDI YH,high(SUNK_ENEMY_SHIPS_ADDR)
	LDI YL,low(SUNK_ENEMY_SHIPS_ADDR)

	; Fill the SRAM with 0x00
	CLR R17

	; Total size of this list: 5*4 bytes = 20 bytes
	LDI R16, 20

	; Loop over each byte and write 0x00
	giesl_list_loop:
		ST Y+, R17

		DEC R16
		BRNE giesl_list_loop

	RET


game_reset_cursor:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function set the cursor as visible.
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Place the cursor on the point 0x0 of the game map
	LDI R16, 1
	CLR R10					; Cursor X
	CLR R11					; Cursor Y
	MOV R12, R16			; Cursor length = 1 pixel
	CLR R13					; Cursor Orientation (0 = horizontal)
	RCALL set_cursor_properties

	RET
	
game_show_cursor:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function set the cursor as hidden.
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set Y-pointer to the cursor's length byte in SRAM
	LDI YH,high(CURSOR_ADDR+2)
	LDI YL,low(CURSOR_ADDR+2)

	LDI R16, 1
	ST Y, R16

	RET

game_hide_cursor:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function set the cursor as hidden.
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set Y-pointer to the cursor's length byte in SRAM
	LDI YH,high(CURSOR_ADDR+2)
	LDI YL,low(CURSOR_ADDR+2)

	CLR R16
	ST Y, R16

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
	LDI YH,high(SHOOTING_STATUS_ADDR)
	LDI YL,low(SHOOTING_STATUS_ADDR)

	; Set Shooting Status to 1
	LDI R16, 1
	ST Y, R16

	sleep_ts 4

	RET

	gf_firing_refused:
	; If firing was refused -> play firing refused sound
	buzzer_sound_async Sound_FiringRefused

	sleep_ts 4

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

	; Show the cursor at position 0x0
	RCALL game_reset_cursor

	; Update the game maps
	RCALL update_game_maps

	; Initialize the Turn Shooting Status to 0
	LDI YH,high(SHOOTING_STATUS_ADDR)
	LDI YL,low(SHOOTING_STATUS_ADDR)
	CLR R16
	ST Y, R16

	; Keyboard actions loop
	gpt_turn_loop:
		; Listen to keyboard
		RCALL keyboard_listen

		; Run game maps animation routine
		RCALL animate_game_maps
		
		; Place the Y-pointer to the Shooting Status byte address
		LDI YH,high(SHOOTING_STATUS_ADDR)
		LDI YL,low(SHOOTING_STATUS_ADDR)

		; Get the current Shooting Status byte
		LD R16, Y

		; Exit the loop if the Shooting Status is different from 0
		CPI R16, 0
		BREQ gpt_turn_loop

	;
	; Here, the player pressed the Fire button.
	; The coordinate of the shot is the current cursor's position.
	;

	; Hide the cursor
	RCALL game_hide_cursor

	RET


game_enemy_fired_position:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called when the enemy fired at a certain position.
	; It checks the consequences of the enemy shot and execute actions 
	; corresponding to these consequences.
	; The shot position is given via R11 and R12 (X, Y).
	; Note: this function changes: R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Move the arguments from R11, R12 to R14, R15
	MOV R15, R12			; Shot position Y
	MOV R14, R11			; Shot position X

	; NOTE: To check which ship has been hitted/sunk, we must loop over the
	;		player's Placed Ships list (we cannot simply check the PCS map).

	; Place Y-pointer to the placed ships list
	LDI YH,high(PLAYER_SHIPS_ADDR)
	LDI YL,low(PLAYER_SHIPS_ADDR)

	; Store 5 in R18 (5 is the number of placed ships in the list)
	LDI R18, 5
	
	; Loop across the placed ships
	gces_ships_loop:
		LD R10, Y+				; Ship ID
		LD R12, Y+				; Ship position X
		LD R13, Y+				; Ship position Y
		LD R19, Y+				; Ship orientation
			
		; Get the ship size corresponding to this ID (ID given in R10)
		RCALL get_ship_size_id
		MOV R17, R11			; Store the ship size in R17

		; Check ship orientation
		SBRC R19, 0
		RJMP gces_ship_vert
			; If the ship's orientation is horizontal

			; Check if shot and ship are on the same Y position
			CP R13, R15
			BRNE gces_ships_loop_end		; If not on the same Y position -> check next ship
			
			; Copy ship position X to temporary register
			MOV R16, R12			; Ship position X

			; Loop over the ship size
			gces_horiz_loop:
				CP R16, R14					; Compare shot and ship X position
				BREQ gces_shot_hits_ship	; If X (and Y) are the same -> shot hitted a ship
			
				INC R16						; Increment the X position
				DEC R17						; Decrement the ship size
				BRNE gces_horiz_loop
			
			; Check next ship if this one is not hitted by this shot
			RJMP gces_ships_loop_end
		
		gces_ship_vert:
			; If the ship's orientation is vertical

			; Check if shot and ship are on the same X position
			CP R12, R14
			BRNE gces_ships_loop_end		; If not on the same X position -> check next ship
			
			; Copy ship position Y to temporary register
			MOV R16, R13			; Ship position Y

			; Loop over the ship size
			gces_vert_loop:
				CP R16, R15					; Compare shot and ship Y position
				BREQ gces_shot_hits_ship	; If Y (and X) are the same -> shot hitted a ship
			
				INC R16						; Increment the Y position
				DEC R17						; Decrement the ship size
				BRNE gces_vert_loop
	
	
		; Check next ship if this one is not hitted by this shot
		gces_ships_loop_end:
			; Place the Y-pointer on the next placed ship ID's address
			INC YL

			; Decrease the ship counter (and check the next ship if this is not the last one)
			DEC R18
			BRNE gces_ships_loop

	; If we fall here, this means that no ship has been hitted by the shot.
	; The original shot position is in R14, R15 (X, Y).
	gces_shot_missed:
		; Prepare argument registers
		MOV R11, R14					; Original shot position X
		MOV R12, R15					; Original shot position Y

		; Execute corresponding action
		RCALL game_enemy_shot_missed

		RET

	; If we fall here, this means that a ship has been hitted by the shot.
	; The hitted ship properties are in R10, R12, R13, R19, R11 (ID, X, Y, Orientation, Size) and Y-pointer is at the #Hits address of this ship.
	; The original shot position is in R14, R15 (X, Y).
	gces_shot_hits_ship:
		; Get the Ship Hits counter
		LD R16, Y

		; Increase the Hits counter for this ship
		INC R16

		; Store the increased Hits counter
		ST Y, R16

		; Check if the number of hits is equal to the ship's size
		CP R16, R11
		BREQ gces_shot_sunk_ship		; If the ship has been sunk by this shot -> go to the ship_sunk section

		; If we fall here, the ship has been hit but not sunk

		; Prepare argument registers
		MOV R11, R14					; Original shot position X
		MOV R12, R15					; Original shot position Y

		; Execute corresponding action
		RCALL game_enemy_shot_hitted

		RET
		
	; If we fall here, this means that a ship has been hitted and sunk by the shot.
	; The hitted ship properties are in R10, R12, R13, R19, R11 (ID, X, Y, Orientation, Size).
	; The original shot position is in R14, R15 (X, Y).
	gces_shot_sunk_ship:
		; Prepare argument registers
		MOV R15, R11					; Sunk ship Size
		MOV R11, R10					; Sunk ship ID
		; R12							; Sunk ship position X
		; R13							; Sunk ship position Y
		MOV R14, R19					; Sunk ship orientation

		; Execute corresponding action
		RCALL game_enemy_shot_sunk

		RET


game_enemy_shot_missed:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called when the enemy missed its shot.
	; The shot position is given via R11 and R12 (X, Y).
	; Note: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; When the enemy missed its shot, there is nothing to draw into PCS

	; Send the Shot Splash answer to the enemy
	; Call master send for Player 1, slave send for Player 2
	selected_player_rcall comm_master_send_shot_splash_packet, comm_slave_send_shot_splash_packet

	RET


game_enemy_shot_hitted:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called when the enemy shot hitted a ship.
	; The shot position is given via R11 and R12 (X, Y).
	; Note: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		
	; When the enemy hitted a ship, send an answer to enemy then change the cell state in PCS to hitted

	; Send the Shot Hit answer to the enemy
	; Call master send for Player 1, slave send for Player 2
	selected_player_rcall comm_master_send_shot_hit_packet, comm_slave_send_shot_hit_packet

	; Set the new cell state in PCS
	MOV R10, R11				; Cell position X
	MOV R11, R12				; Cell position Y
	LDI R16, 1					; Store 1 in R16
	MOV R12, R16				; Rectangle width = 1
	MOV R13, R16				; Rectangle height = 1
	LDI R16, (1 << PCS_HITTED_SHIP)
	MOV R14, R16				; Cell state = HITTED_SHIP
	RCALL set_rect_pcs

	RET


game_enemy_shot_sunk:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called when the enemy shot has sunk a ship.
	; The sunk ship properties are given via R11, R12, R13, R14, R15 (ID,
	; X, Y, Orientation, Size).
	; Note: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	;
	; When the enemy has sunk a ship, send an answer to enemy then change the cells state in PCS to sunk
	;
	
	; Move the ship size to R10 (since R15 will be overwritten by comm_master_send_shot_sank_packet or comm_slave_send_shot_sank_packet
	MOV R20, R15

	;----------------------------------------------------
	; Send the Shot Sank answer to the enemy
	; Call master send for Player 1, slave send for Player 2
	selected_player_rcall comm_master_send_shot_sank_packet, comm_slave_send_shot_sank_packet

	;----------------------------------------------------
	; Set the new cells state in PCS (for the whole ship)
	MOV R10, R12				; Ship position X
	MOV R11, R13				; Ship position Y
	LDI R16, 1					; Store 1 in R16

	; Check the ship's orientation
	SBRC R14, 0
	RJMP gess_ship_vert
		; If ship is horizontal
		MOV R12, R20			; Rectangle width = Ship size
		MOV R13, R16			; Rectangle height = 1
		RJMP gess_set_cells

	gess_ship_vert:
		; If ship is vertical
		MOV R12, R16			; Rectangle width = 1
		MOV R13, R20			; Rectangle height = Ship size

	gess_set_cells:
	LDI R16, (1 << PCS_SUNK_SHIP)
	MOV R14, R16				; Cells state = SUNK_SHIP
	RCALL set_rect_pcs
	

	;----------------------------------------------------
	; Finally, increment the player's sunk ships counter
	; Place Y-pointer to the Sunk Ships Counter
	LDI YH,high(SUNK_SHIPS_CNT_ADDR)
	LDI YL,low(SUNK_SHIPS_CNT_ADDR)

	; Get, increment and store the current sunk ships counter
	LD R16, Y
	INC R16
	ST Y, R16

	; If all (5) ships have been sunk -> game lost
	CPI R16, 5
	BRNE gess_game_not_lost
		; Change game state to GAME_LOST
		game_change_state GS_GAME_LOST
	
	gess_game_not_lost:

	RET


game_shot_result_splash:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called when the player's shot consequence on the
	; enemy is Splash (shot in the water).
	; The shot position is given via R11, R12 (X, Y).
	; Note: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; When a shot hits the water, only change the cell state in ECS.
	
	; Set the new cell state in ECS
	MOV R10, R11				; Cell position X
	MOV R11, R12				; Cell position Y
	LDI R16, 1					; Store 1 in R16
	MOV R12, R16				; Rectangle width = 1
	MOV R13, R16				; Rectangle height = 1
	LDI R16, (1 << ECS_MISSED_SHOT)
	MOV R14, R16				; Cell state = MISSED_SHOT
	RCALL set_rect_ecs

	RET

	
game_shot_result_hitted:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called when the player's shot consequence on the
	; enemy is Ship Hitted (shot hits a ship).
	; The shot position is given via R11, R12 (X, Y).
	; Note: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; When a shot hits a ship, only change the cell state in ECS.
	
	; Set the new cell state in ECS
	MOV R10, R11				; Cell position X
	MOV R11, R12				; Cell position Y
	LDI R16, 1					; Store 1 in R16
	MOV R12, R16				; Rectangle width = 1
	MOV R13, R16				; Rectangle height = 1
	LDI R16, (1 << ECS_HITTED_SHIP)
	MOV R14, R16				; Cell state = HITTED_SHIP
	RCALL set_rect_ecs

	RET

	
game_shot_result_sunk:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called when the player's shot consequence on the
	; enemy is Ship Sunk (shot sank a ship).
	; The sunk ship properties are given via R11, R12, R13, R14, (ID, X, 
	; Y, Orientation).
	; Note: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; When a shot sinks a ship, store the ship's data in SRAM and change the cell state in ECS.

	; Add this ship to enemy sunk ships list
	RCALL game_add_enemy_sunk_ship

	; Get the ship size for this ship ID
	MOV R10, R11				; Move the ship ID in R10
	RCALL get_ship_size_id
	MOV R15, R11				; Move the ship size in R15
	
	; Set the new cells state in ECS (for the whole ship)
	MOV R10, R12				; Ship position X
	MOV R11, R13				; Ship position Y
	LDI R16, 1					; Store 1 in R16

	; Check the ship's orientation
	SBRC R14, 0
	RJMP gsrh_ship_vert
		; If ship is horizontal
		MOV R12, R15			; Rectangle width = Ship size
		MOV R13, R16			; Rectangle height = 1
		RJMP gsrh_set_cells

	gsrh_ship_vert:
		; If ship is vertical
		MOV R12, R16			; Rectangle width = 1
		MOV R13, R15			; Rectangle height = Ship size

	gsrh_set_cells:
	LDI R16, (1 << ECS_SUNK_SHIP)
	MOV R14, R16				; Cells state = SUNK_SHIP
	RCALL set_rect_ecs

	RET


game_add_enemy_sunk_ship:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function adds the given ship to the sunk enemy ships list.
	; The sunk ship properties are given via R11, R12, R13, R14, (ID, X, 
	; Y, Orientation).
	; Note: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Place Y-pointer to the sunk enemy ships list
	LDI YH,high(SUNK_ENEMY_SHIPS_ADDR)
	LDI YL,low(SUNK_ENEMY_SHIPS_ADDR)

	; Loop over max 5 ships IDs to find a free list item
	LDI R16, 5
	gaess_ships_loop:
		; Get this ship ID
		LD R17, Y
		
		; Exit the loop if this Ship ID is 0x00
		CPI R17,0
		BREQ gaess_store_ship_sram

		; Jump of 4 bytes with the Y-pointer (next ship ID)
		LDI R17, 4
		ADD YL, R17

		DEC R16
		BRNE gaess_ships_loop

	; If there is no free list item for this ship
	RJMP gaess_enemy_ships_list_full

	gaess_store_ship_sram:
		; The current position of the Y-pointer is the first byte for this ship in SRAM
		ST Y+, R11		; Store Ship ID
		ST Y+, R12		; Store Position X (relative to game map)
		ST Y+, R13		; Store Position Y (relative to game map)
		ST Y+, R14		; Store Orientation

		; Check if this sunk ship fills the sunk emeny ships list
		CPI YL, low(SUNK_ENEMY_SHIPS_ADDR+20)
		BREQ gaess_enemy_ships_list_full		; If the enemy ships list is now full -> game is won by the player

		RET

	gaess_enemy_ships_list_full:
		; If we fall to this label, this means that the enemy ships list is full.
		; In this case, the player won the game !

		; Change game state to GAME_WON
		game_change_state GS_GAME_WON

		RET