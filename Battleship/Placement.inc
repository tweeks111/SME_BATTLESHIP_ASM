;
; Placement.inc
;
; This file is part of the Battleship project.
;
; It contains functions to place the ships on the left game map.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

; Ship Placement Status
#define SPS_ADDR			0x019A
#define SPS_CARRIER			0
#define SPS_BATTLESHIP		1
#define SPS_CRUISER			2
#define SPS_SUBMARINE		3
#define SPS_DESTROYER		4
#define SPS_DONE_BIT		5

; Placed ships list
#define PLAYER_SHIPS_ADDR	0x01A0


init_ships_list:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the SRAM content of the player's ships
	; list to 0x00 (all bytes).
	; It also initilizes the SPS byte in SRAM.
	; NOTE: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Place Y-pointer to the placed ships list
	LDI YH,high(PLAYER_SHIPS_ADDR)
	LDI YL,low(PLAYER_SHIPS_ADDR)

	; Fill the SRAM with 0x00
	CLR R17

	; Total size of this list: 5*4 bytes = 20 bytes
	LDI R16, 20

	; Loop over each byte and write 0x00
	isl_list_loop:
		ST Y+, R17

		DEC R16
		BRNE isl_list_loop

	; Place Y-pointer to the Ship Placement Status byte
	LDI YH,high(SPS_ADDR)
	LDI YL,low(SPS_ADDR)

	; Write 0x00 into SPS
	ST Y, R17

	RET

start_ship_placement:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function prepares the cursor and the SRAM to place the first
	; ship on the game map.
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Place Y-pointer to the Ship Placement Status byte
	LDI YH,high(SPS_ADDR)
	LDI YL,low(SPS_ADDR)

	; The first ship is represented by Bit0
	LDI R16, (1 << 0)

	; Store the ship ID into STS byte
	ST Y, R16

	;
	; Prepare the cursor for the first ship
	;

	; Get the ship size for this ship ID (returned in R11)
	MOV R10, R16
	RCALL get_ship_size_id
	MOV R12, R11			; Ship length is the new cursor's length

	; Place the cursor on the point 0x0 of the left game map
	LDI R16, 2
	MOV R10, R16			; Cursor X
	MOV R11, R16			; Cursor Y
	CLR R13					; Cursor Orientation (0 = horizontal)
	RCALL set_cursor_properties
	
	; Update the game maps
	RCALL update_game_maps

	RET

place_next_ship:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function prepares the cursor and the SRAM to place the next 
	; ship on the game map. If the last ship has been placed, it sets the
	; placed bit in SPS to 1.
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Place Y-pointer to the Ship Placement Status byte
	LDI YH,high(SPS_ADDR)
	LDI YL,low(SPS_ADDR)

	; Get the Ship Placement Status byte
	LD R16, Y

	; Nothing to do if ship placement is already done
	SBRC R16, SPS_DONE_BIT
	RJMP pns_skip

	; Shift the Ship Identifier bit to the left. It will represent the next ship 
	; to place, or set the Placement Finished bit if the last ship has been placed.
	LSL R16
	ST Y, R16		; Store the modified STS into SRAM
	
	;
	; Prepare the cursor for the next ship
	;

	; Get the ship size for this ship ID (returned in R11)
	MOV R10, R16
	RCALL get_ship_size_id
	MOV R12, R11			; Ship length is the new cursor's length

	; Place the cursor on the point 0x0 of the left game map
	LDI R16, 2
	MOV R10, R16			; Cursor X
	MOV R11, R16			; Cursor Y
	CLR R13					; Cursor Orientation (0 = horizontal)
	RCALL set_cursor_properties
	
	; Update the game maps
	RCALL update_game_maps

	pns_skip:
	RET

place_ship:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function place a ship on the left game map at the current
	; cursor position.
	; NOTE: this function changes: R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Check if the cursor overlaps a placed ship
	RCALL check_ship_overlap

	; Skip next jump if there is no overlap
	SBRC R10, 0
	RJMP ps_placement_refused

	; Place Y-pointer to the cursor's data
	LDI YH,high(CURSOR_ADDR)
	LDI YL,low(CURSOR_ADDR)

	; Get cursor's data from SRAM
	LD R10, Y+				; Cursor X
	LD R11, Y+				; Cursor Y
	LD R12, Y+				; Cursor Length
	LD R13, Y				; Cursor Orientation

	; Compute ship position relative to game map
	LDI R16, 2
	SUB R10, R16
	SUB R11, R16

	; Place Y-pointer to the SPS byte in SRAM (Ship Placement Status)
	LDI YH,high(SPS_ADDR)
	LDI YL,low(SPS_ADDR)

	; Get the SPS byte
	LD R14, Y

	; Mask 5 last bits of SPS
	LDI R16, 0b00011111
	AND R14, R16

	; Place Y-pointer to the placed ships list
	LDI YH,high(PLAYER_SHIPS_ADDR)
	LDI YL,low(PLAYER_SHIPS_ADDR)

	; Loop over the 5 ships IDs to find a free list item
	LDI R16, 5
	ps_ships_loop:
		; Get this ship ID
		LD R17, Y
		
		; Exit the loop if this Ship ID is 0x00
		CPI R17,0
		BREQ ps_store_ship_sram

		; Jump of 4 bytes with the Y-pointer (next ship ID)
		LDI R17, 4
		ADD YL, R17

		DEC R16
		BRNE ps_ships_loop

	; If there is no free list item for this ship
	RJMP ps_placement_refused

	ps_store_ship_sram:
	; The current position of the Y-pointer is the first byte for this ship in SRAM
	ST Y+, R14		; Store Ship ID
	ST Y+, R10		; Store Position X (relative to game map)
	ST Y+, R11		; Store Position Y (relative to game map)
	ST Y+, R13		; Store Orientation

	;
	; Set the PCS cells according to the newly placed ship
	;
	; R10: Cursor's position X => Rectangle position X
	; R11: Cursor's position Y => Rectangle position Y
	
	LDI R17, 1		; Store a 1 in R17

	; Check the cursor's orientation
	CLR R16
	CP R13, R16
	BRNE ps_place_vert
		; If new ship is horizontal
		; R12: Cursor's length => Rectangle width
		MOV R13, R17		; Rectangle height = 1 (horizontal)
		RJMP ps_set_rect
	
	ps_place_vert:
		; If new ship is vertical
		MOV R13, R12		; Cursor's length => Rectangle height
		MOV R12, R17		; Rectangle width = 1 (vertical)

	ps_set_rect:
	; The cells state for this new ship
	LDI R17, (1 << PCS_HEALTY_SHIP)
	MOV R14, R17

	; Set the rectangle representing the new ship in PCS
	RCALL set_rect_pcs

	; Prepare cursor and SRAM to place the next ship
	RCALL place_next_ship
	
	; Play placement accepted sound
	buzzer_sound_async Sound_PlacementAccepted

	; Wait 0.4s to filter keyboard glitches
	sleep_ts 4

	RET

	ps_placement_refused:
	
	; Play placement refused sound if refused (and wait 0.4s to filter keyboard glitches)
	buzzer_sound_async Sound_PlacementRefused
	sleep_ts 4

	RET

check_ship_overlap:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function checks if a the cursor currently overlaps a non-empty
	; cell in PCS. If this is the case, 0x01 will be returned in R10.
	; Else, 0x00 is returned.
	; NOTE: this function changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	LDI YH,high(CURSOR_ADDR)
	LDI YL,low(CURSOR_ADDR)

	; Get cursor's data from SRAM
	LD R10, Y+				; Cursor X
	LD R11, Y+				; Cursor Y
	LD R12, Y+				; Cursor Length
	LD R16,	Y				; Cursor Orientation

	; Subtract 2 to X and Y to get the cursor's position relative to the left game map
	LDI R17, 2
	SUB R10, R17
	SUB R11, R17

	; Compute the fist byte index in PCS
	LDI R17, 10				; 10 cells per row
	MUL R11, R17			; Multiply Y position with row size
	ADD R0, R10				; Add the X position
	
	; Place the Y-pointer to the PCS address
	LDI YH,high(PCS_ADDR)
	LDI YL,low(PCS_ADDR)

	; Place the Y-pointer to the byte corresponding to the cursor's position
	ADD YL, R0

	; Check the cursor's orientation
	CPI R16, 0
	BRNE cso_vert
		; If the orientation is horizontal -> loop inside a row
		cso_horiz_loop:
			; Get the next cell's state from the PCS
			LD R16, Y+

			SBRS R16, PCS_EMPTY		; Skip the next jump if this cell is empty
			RJMP cso_overlap

			DEC R12					; Decrement the cursor's length
			BRNE cso_horiz_loop

		; If we exit the previous loop -> there is no overlap
		RJMP cso_no_overlap

	cso_vert:
		LDI R17, 10					; R17 stores the size of a row

		; If the orientation is vertical -> loop over the rows
		cso_vert_loop:
			; Get the next cell's state from the PCS
			LD R16, Y

			; Increment the Y-pointer to the next cell to check
			ADD YL, R17

			SBRS R16, PCS_EMPTY		; Skip the next jump if this cell is empty
			RJMP cso_overlap

			DEC R12					; Decrement the cursor's length
			BRNE cso_vert_loop


	cso_no_overlap:
	CLR R10

	RET

	cso_overlap:
	LDI R16, 1
	MOV R10, R16

	RET


get_ship_size_id:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function returns the ship size in R11 corresponding to the 
	; ship ID given in R10
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Default return value = 0
	LDI R16, 0

	; Switch-select the corresponding size
	SBRC R10, SPS_CARRIER
		LDI R16, 5			; The Carrier ship has size 5
	SBRC R10, SPS_BATTLESHIP
		LDI R16, 4			; The Battleship ship has size 4
	SBRC R10, SPS_CRUISER
		LDI R16, 3			; The Cruiser ship has size 3
	SBRC R10, SPS_SUBMARINE
		LDI R16, 3			; The Submarine ship has size 3
	SBRC R10, SPS_DESTROYER
		LDI R16, 2			; The Destroyer ship has size 2

	; Move the selected size to the return register
	MOV R11, R16

	RET
