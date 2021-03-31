;
; Game.inc
;
; This file is part of the Battleship project.
;
; It contains functions to manage and show a cursor on the screen.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

; Cursor shape bytes in SRAM
#define CURSHAPE_X			0x0196	; Cursor X position
#define CURSHAPE_Y			0x0197	; Cursor Y position
#define CURSHAPE_LEN		0x0198	; Cursor Length
#define CURSHAPE_ORT		0x0199	; Cursor Orientation

; Cursor area data address in SRAM
#define CURAREA_X			0x019A	; Area X position
#define CURAREA_Y			0x019B	; Area Y position
#define CURAREA_W			0x019C	; Area Width
#define CURAREA_H			0x019D	; Area Height


.macro get_cursor_position
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro returns the cursor's position in R11 and R12 (X, Y).
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	LDS R11, CURSHAPE_X		; Position X
	LDS R12, CURSHAPE_Y		; Position Y
.endmacro

set_cursor_area:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sets the cursor's area.
	; R10: Area start position X
	; R11: Area start position  Y
	; R12: Area Width 
	; R13: Area Height
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	STS CURAREA_X, R10		; Area offset position X
	STS CURAREA_Y, R11		; Area offset position  Y
	STS CURAREA_W, R12		; Area Width 
	STS CURAREA_H, R13		; Area Height

	RET

set_cursor_properties:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sets the cursor's properties.
	; R10: Cursor's X position
	; R11: Cursor's Y position
	; R12: Cursor's Length
	; R13: Cursor's Orientation (0 = Horizontal, 1 = Vertical)
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	STS CURSHAPE_X, R10		; Position X
	STS CURSHAPE_Y, R11		; Position Y
	STS CURSHAPE_LEN, R12	; Length
	STS CURSHAPE_ORT, R13	; Orientation

	RET

cursor_move_up:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function moves the cursor up of one pixel if it is possible.
	; NOTE: this function uses: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Get the cursor's Y position
	LDS R16, CURSHAPE_Y		; Cursor Y

	CPI R16, 0				; Don't move up if the cursor's position is already the maximum
	BREQ cmu_skip_move_up

		; Decrement and store the cursor's Y position
		DEC R16
		STS CURSHAPE_Y, R16

		; Update the game maps
		RCALL update_game_maps

	cmu_skip_move_up:
	RET

cursor_move_down:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function moves the cursor down of one pixel if it is possible.
	; NOTE: this function uses: R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Get the cursor's data
	LDS R16, CURSHAPE_Y		; Cursor Y
	LDS R17, CURSHAPE_LEN	; Cursor Length
	LDS R18, CURSHAPE_ORT	; Cursor Orientation (0 = horizontal, 1 = vertical)
	
	; Get the cursor area height
	LDS R19, CURAREA_H
	DEC R19				; Decrement by 1 to use R19 with BRGE (greater or *equal*)

	; Decrement the length before SUB (since the first pixel is at relative position 0)
	DEC R17

	SBRC R18,0			; If orientation is horizontal, skip the next instruction
	SUB R19, R17		; If the cursor's orientation is vertical, subtract the cursor's length to the Y position limit value

	CP R16, R19			; Don't move if the cursor's position is already the maximum
	BRGE cmd_skip_move_down

		; Increment and store the cursor's Y position
		INC R16
		STS CURSHAPE_Y, R16

		; Update the game maps
		RCALL update_game_maps

	cmd_skip_move_down:
	RET

cursor_move_left:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function moves the cursor left of one pixel if it is possible.
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		
	; Get the cursor's data
	LDS R16, CURSHAPE_X		; Cursor X

	CPI R16, 0				; Don't move if the cursor's position is already the maximum
	BREQ cml_skip_move_left

		; Decrement and store the cursor's X position
		DEC R16
		STS CURSHAPE_X, R16
		
		; Update the game maps
		RCALL update_game_maps

	cml_skip_move_left:
	RET
	
cursor_move_right:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function moves the cursor right of one pixel if it is possible.
	; NOTE: this function uses: R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		
	; Get the cursor's data
	LDS R16, CURSHAPE_X		; Cursor X
	LDS R17, CURSHAPE_Y		; Cursor Y (unused -> discarded at next instruction)
	LDS R17, CURSHAPE_LEN	; Cursor Length
	LDS R18, CURSHAPE_ORT	; Cursor Orientation (0 = horizontal, 1 = vertical)
		
	; Get the cursor area width
	LDS R19, CURAREA_W
	DEC R19				; Decrement by 1 to use R19 with BRGE (greater or *equal*)

	; Decrement the length before SUB (since the first pixel is at relative position 0)
	DEC R17

	SBRS R18,0			; If orientation is vertical, skip the next instruction
	SUB R19, R17		; If the cursor's orientation is horizontal, subtract the cursor's length to the X position limit value

	CP R16, R19			; Don't move if the cursor's position is already the maximum
	BRGE cmr_skip_move_right

		; Increment and store the cursor's X position
		INC R16
		STS CURSHAPE_X, R16
		
		; Update the game maps
		RCALL update_game_maps

	cmr_skip_move_right:
	RET


cursor_rotate:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function rotates the cursor according to the map's boundaries.
	; NOTE: this function uses: R16, R17, R18, R19, R20
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Get the cursor's data
	LDS R16, CURSHAPE_X		; Cursor X
	LDS R17, CURSHAPE_Y		; Cursor Y
	LDS R18, CURSHAPE_LEN	; Cursor Length
	LDS R19, CURSHAPE_ORT	; Cursor Orientation (0 = horizontal, 1 = vertical)
		
	; Get the cursor area width
	LDS R20, CURAREA_W

	; Compute the maximum X position of cursor after rotation
	SBRC R19, 0			; Skip next if ship was horizontal before rotate
	SUB R20, R18		; Subtract the cursor's length if it is horizontal now

	; Check if cursor out of X limits
	CP R16, R20
	BRLT cr_skip_x_fix		; Skip next part if X is not out of limits
		MOV R16, R20		; If out of limits -> the X limit is the new X position

	cr_skip_x_fix:
	; Get the cursor area height
	LDS R20, CURAREA_H

	; Compute the maximum Y position of cursor after rotation
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
	
	; Store the new cursor's data
	STS CURSHAPE_X, R16		; Cursor X
	STS CURSHAPE_Y, R17		; Cursor Y
	STS CURSHAPE_ORT, R19	; Cursor Orientation (0 = horizontal, 1 = vertical)
		
	; Update the game maps
	RCALL update_game_maps

	RET
	

screen_set_cursor:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Write the current cursor to the screen.
	; If the cursor's length is 0, the cursor is not drawn
	; Note : this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	LDS R10, CURSHAPE_X		; Cursor X
	LDS R11, CURSHAPE_Y		; Cursor Y
	LDS R16, CURSHAPE_LEN	; Cursor Length
	LDS R17, CURSHAPE_ORT	; Cursor Orientation (0 = horizontal, 1 = vertical)
	
	; Don't draw the cursor if its length is 0
	CPI R16, 0
	BREQ ssc_no_cur
	
	LDS R18, CURAREA_X	; Cursor area offset position X
	LDS R19, CURAREA_Y	; Cursor area offset position Y

	; Add the area start position to the cursor's position
	ADD R10, R18		; Add X offset
	ADD R11, R19		; Add Y offset

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
