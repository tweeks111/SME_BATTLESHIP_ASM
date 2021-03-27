;
; GameMaps.inc
;
; This file is part of the Battleship project.
;
; It contains functions manage the left and right game maps.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

; PCS and ECS addresses in SRAM
#define PCS_ADDR				0x0200
#define ECS_ADDR				0x0300

; PCS cells states (bitwise states)
#define PCS_EMPTY			0
#define PCS_HEALTY_SHIP		1
#define PCS_HITTED_SHIP		2
#define PCS_SUNK_SHIP		3

; ECS cells states (bitwise states)
#define ECS_EMPTY			0
#define ECS_MISSED_SHOT		1
#define ECS_HITTED_SHIP		2
#define ECS_SUNK_SHIP		3

init_map_cells:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the SRAM bytes for PCS and ECS areas
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Write the empty cell state in all PCS cell
	LDI R16, 100				; Loop over 100 cells
	LDI R17, (1 << PCS_EMPTY)	; Empty Cell state

	LDI YH,high(PCS_ADDR)
	LDI YL,low(PCS_ADDR)

	imc_PCS_loop:
		ST Y+, R17
		DEC R16
		BRNE imc_PCS_loop

	; Write the empty cell state in all ECS cell
	LDI R16, 100				; Loop over 100 cells
	LDI R17, (1 << ECS_EMPTY)	; Empty Cell state

	LDI YH,high(ECS_ADDR)
	LDI YL,low(ECS_ADDR)

	imc_ECS_loop:
		ST Y+, R17
		DEC R16
		BRNE imc_ECS_loop

	RET

screen_set_PCS:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Write the current PCS to the screen
	; Note : this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI ZH, high(PCS_ADDR)
	LDI ZL, low(PCS_ADDR)
	
	LDI R16, 1
	MOV R12, R16		; Brightness	
	MOV R13, R16		; Line length

	LDI R21, 0			; Y
	ss_Y_loop:
		LDI R20, 0		; X
		ss_X_loop:
			LD R16, Z+
			SBRC R16, 1
			RCALL ss_draw_point
			INC R20
			CPI R20, 10
			BRNE ss_X_loop
		INC R21
		CPI R21, 10
		BRNE ss_Y_loop
	RET

	ss_draw_point:
		LDI R16, 2
		MOV R10, R20
		ADD R10, R16
		MOV R11, R21
		ADD R11, R16
		RCALL screen_set_hline
		RET
		
	
set_rect_game_map:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sets a rectangle of states in a game map.
	; The Y-pointer must be pre-set at the first byte's address of map.
	; R10: X position (relative to game map)
	; R11: Y position (relative to game map)
	; R12: Rectangle width (must be at least 1)
	; R13: Rectangle height (must be at least 1)
	; R14: Cell state
	; Note : this function changes: R16, R17, R18
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Compute the fist byte index in PCS
	LDI R16, 10				; 10 cells per row
	MUL R11, R16			; Multiply Y position with row size
	ADD R0, R10				; Add the X position
	
	; Place the Y-pointer to the byte corresponding to the cursor's position
	ADD YL, R0

	; Loop over the rectangle's height
	MOV R17, R13			; Store height in R17
	srgm_height_loop:
		; Loop over the rectangle's width
		MOV R16, R12		; Store width in R16
		srgm_width_loop:
			; Store the new cell state in SRAM
			ST Y+, R14

			DEC R16
			BRNE srgm_width_loop

		; After drawing a row, place the Y-pointer to the beginning of the next row
		SUB YL, R12			; Subtract the rectangle's width from the Y-pointer
		LDI R18, 10			; Store the Game Map width
		ADD YL, R18			; Add the Game Map width to Y-pointer

		DEC R17
		BRNE srgm_height_loop

	RET
	

set_rect_pcs:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sets a rectangle of states in PCS.
	; R10: X position (relative to game map)
	; R11: Y position (relative to game map)
	; R12: Rectangle width (must be at least 1)
	; R13: Rectangle height (must be at least 1)
	; R14: Cell state
	; Note : this function changes: R16, R17, R18
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Place the Y-pointer to the PCS address
	LDI YH,high(PCS_ADDR)
	LDI YL,low(PCS_ADDR)

	RCALL set_rect_game_map

	RET

set_rect_ecs:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sets a rectangle of states in ECS.
	; R10: X position (relative to game map)
	; R11: Y position (relative to game map)
	; R12: Rectangle width (must be at least 1)
	; R13: Rectangle height (must be at least 1)
	; R14: Cell state
	; Note : this function changes: R16, R17, R18
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Place the Y-pointer to the ECS address
	LDI YH,high(ECS_ADDR)
	LDI YL,low(ECS_ADDR)

	RCALL set_rect_game_map

	RET