;
; Game.inc
;
; This file is part of the Battleship project.
;
; It contains functions to manage and show a cursor on the screen.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

; Cursor data address in SRAM
#define CURSOR_ADDR			0x0196


set_cursor_properties:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sets the cursor's properties.
	; R10: Cursor's X position
	; R11: Cursor's Y position
	; R12: Cursor's Length
	; R13: Cursor's Orientation (0 = Horizontal, 1 = Vertical)
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set Y-pointer to the cursor's data byte in SRAM
	LDI YH,high(CURSOR_ADDR)
	LDI YL,low(CURSOR_ADDR)
	
	ST Y+, R10		; Position X
	ST Y+, R11		; Position Y
	ST Y+, R12		; Length
	ST Y, R13		; Orientation

	RET

cursor_move_up:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function moves the cursor up of one pixel if it is possible.
	; NOTE: this function uses: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set Y-pointer to byte of cursor's Y position in SRAM
	LDI YH,high(CURSOR_ADDR+1)
	LDI YL,low(CURSOR_ADDR+1)
	
	; Get the cursor's Y position
	LD R16, Y			; Cursor Y

	CPI R16, 3			; Don't move up if the cursor's position is already the maximum
	BRLO cmu_skip_move_up

		; Decrement and store the cursor's Y position
		DEC R16
		ST Y, R16

		; Update the game maps
		RCALL update_game_maps

	cmu_skip_move_up:
	RET

cursor_move_down:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function moves the cursor down of one pixel if it is possible.
	; NOTE: this function uses: R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set Y-pointer to byte of cursor's Y position in SRAM
	LDI YH,high(CURSOR_ADDR+1)
	LDI YL,low(CURSOR_ADDR+1)
	
	; Get the cursor's data
	LD R16, Y+			; Cursor Y
	LD R17, Y+			; Cursor Length
	LD R18, Y			; Cursor Orientation (0 = horizontal, 1 = vertical)

	; The limit Y position for the pointer is 2 + 9 (for a point-cursor)
	LDI R19, 11

	; Decrement the length before SUB (since the first pixel is at relative position 0)
	DEC R17

	SBRC R18,0			; If orientation is horizontal, skip the next instruction
	SUB R19, R17		; If the cursor's orientation is vertical, subtract the cursor's length to the Y position limit value

	CP R16, R19			; Don't move if the cursor's position is already the maximum
	BRGE cmd_skip_move_down

		; Increment and store the cursor's Y position
		INC R16

		; Set Y-pointer to byte of cursor's Y position in SRAM
		LDI YH,high(CURSOR_ADDR+1)
		LDI YL,low(CURSOR_ADDR+1)
		ST Y, R16

		; Update the game maps
		RCALL update_game_maps

	cmd_skip_move_down:
	RET

cursor_move_left:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function moves the cursor left of one pixel if it is possible.
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set Y-pointer to byte of cursor's X position in SRAM
	LDI YH,high(CURSOR_ADDR)
	LDI YL,low(CURSOR_ADDR)
	
	; Get the cursor's data
	LD R16, Y			; Cursor X

	CPI R16, 3			; Don't move if the cursor's position is already the maximum
	BRLT cml_skip_move_left

		; Decrement and store the cursor's X position
		DEC R16
		ST Y, R16
		
		; Update the game maps
		RCALL update_game_maps

	cml_skip_move_left:
	RET
	
cursor_move_right:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function moves the cursor right of one pixel if it is possible.
	; NOTE: this function uses: R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set Y-pointer to byte of cursor's X position in SRAM
	LDI YH,high(CURSOR_ADDR)
	LDI YL,low(CURSOR_ADDR)
	
	; Get the cursor's data
	LD R16, Y+			; Cursor X
	LD R17, Y+			; Cursor Y (unused -> discarded at next instruction)
	LD R17, Y+			; Cursor Length
	LD R18, Y			; Cursor Orientation (0 = horizontal, 1 = vertical)

	; The limit X position for the pointer is 2 + 9 (for a point-cursor)
	LDI R19, 11

	; Decrement the length before SUB (since the first pixel is at relative position 0)
	DEC R17

	SBRS R18,0			; If orientation is vertical, skip the next instruction
	SUB R19, R17		; If the cursor's orientation is horizontal, subtract the cursor's length to the X position limit value

	CP R16, R19			; Don't move if the cursor's position is already the maximum
	BRGE cmr_skip_move_right

		; Increment and store the cursor's X position
		INC R16

		; Set Y-pointer to byte of cursor's X position in SRAM
		LDI YH,high(CURSOR_ADDR)
		LDI YL,low(CURSOR_ADDR)
		ST Y, R16
		
		; Update the game maps
		RCALL update_game_maps

	cmr_skip_move_right:
	RET


cursor_rotate:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function rotates the cursor according to the map's boundaries.
	; NOTE: this function uses: R16, R17, R18, R19, R20
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set Y-pointer cursor's data byte in SRAM
	LDI YH,high(CURSOR_ADDR)
	LDI YL,low(CURSOR_ADDR)
	
	; Get the cursor's data
	LD R16, Y+			; Cursor X
	LD R17, Y+			; Cursor Y
	LD R18, Y+			; Cursor Length
	LD R19, Y			; Cursor Orientation (0 = horizontal, 1 = vertical)
	
	; Decrement the length before SUB (since the first pixel is at relative position 0)
	DEC R18

	; Compute the maximum X position of cursor after rotation
	LDI R20, 11			; Initially, the most-right position is 11 (for point-cursor)
	SBRC R19, 0			; Skip next if ship was horizontal before rotate
	SUB R20, R18		; Subtract the cursor's length if it is horizontal now

	; Check if cursor out of X limits
	CP R16, R20
	BRLT cr_skip_x_fix		; Skip next part if X is not out of limits
		MOV R16, R20		; If out of limits -> the X limit is the new X position

	cr_skip_x_fix:
	; Compute the maximum Y position of cursor after rotation
	LDI R20, 11			; Initially, the most-down position is 11 (for point-cursor)
	SBRS R19, 0			; Skip next if ship was vertical before rotate
	SUB R20, R18		; Subtract the cursor's length if it is vertical now
	
	; Check if cursor out of Y limits
	CP R17, R20
	BRLT cr_skip_y_fix		; Skip next part if Y is not out of limits
		MOV R17, R20		; If out of limits -> the Y limit is the new Y position

	cr_skip_y_fix:
	; Store the new cursor's orientation
	COM R19					; Reverse all bits of R19
	ANDI R19, (1 << 0)		; Keep only the bit 0
	
	; Set Y-pointer cursor's data byte in SRAM
	LDI YH,high(CURSOR_ADDR)
	LDI YL,low(CURSOR_ADDR)

	; Store the new cursor's data
	ST Y+, R16			; Cursor X
	ST Y+, R17			; Cursor Y
	INC YL				; Cursor Length (not changed)
	ST Y, R19			; Cursor Orientation (0 = horizontal, 1 = vertical)
		
	; Update the game maps
	RCALL update_game_maps

	RET
	

screen_set_cursor:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Write the current cursor to the screen.
	; If the cursor's length is 0, the cursor is not drawn
	; Note : this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI YH, high(CURSOR_ADDR)
	LDI YL, low(CURSOR_ADDR)
	
	LD R10, Y+			; Cursor X
	LD R11, Y+			; Cursor Y
	LD R16, Y+			; Cursor Length
	LD R17, Y			; Cursor Orientation (0 = horizontal, 1 = vertical)
	
	; Don't draw the cursor if its length is 0
	CPI R16, 0
	BREQ ssc_no_cur

	LDI R18, 1			; Store a 1 on R18

	CPI R17, 0			; Check if orientation is horizontal
	BRNE ssc_cur_vert
		; If the cursor is horizontal -> W = R16, H = 1
		MOV R13, R16		; Cursor's Width
		MOV R14, R18		; Cursor's Height
		RJMP ssc_draw_cur

	ssc_cur_vert:
		; If the cursor is vertical -> W = 1, H = R16
		MOV R13, R18		; Cursor's Width
		MOV R14, R16		; Cursor's Height

	ssc_draw_cur:
	LDI R16, 3
	MOV R12, R16			; Cursor's bightness

	RCALL screen_set_rect

	ssc_no_cur:
	RET
