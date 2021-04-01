;
; GameMaps.inc
;
; This file is part of the Battleship project.
;
; It contains functions manage the left and right game maps.
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

; PCS and ECS addresses in SRAM
#define PCS_ADDR				0x0200
#define ECS_ADDR				0x0300

; PCS cells states (bitwise states)
#define PCS_EMPTY			0
#define PCS_HEALTY_SHIP		1
#define PCS_HIT_SHIP		2
#define PCS_SUNK_SHIP		3

; ECS cells states (bitwise states)
#define ECS_EMPTY			0
#define ECS_MISSED_SHOT		1
#define ECS_HIT_SHIP		2
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
	
;TODO: create a function to redraw only one map at a time
update_game_maps:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function updates the game maps
	; NOTE: this function uses: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Draw two rectangles to clear the two game maps
	draw_rect 2, 2, 0, 10, 10
	draw_rect 28, 2, 0, 10, 10

	; Re-draw the PCS
	RCALL screen_set_PCS_map
	
	; Re-draw the ECS
	RCALL screen_set_ECS_map

	; Re-draw the cursor
	RCALL screen_set_cursor
	RET

animate_game_maps:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function updates the screen according to the cells status in 
	; PCS and ECS and to the animation counter. It should be called the
	; most regularily and often as possible.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Mask to uncheck the bit 4 of the animation counter
	LDI R16, 0b11101111

	; Check if "5 Hz interrupt flag" is set
	SBRS R9, 4
	RJMP agm_end

		; This section runs at max 5 Hz

		; Apply the mask to the animation counter to unset the "5 Hz interrupt flag"
		AND R9, R16
	
		; Update the game maps (without clearing the whole maps)
		RCALL screen_set_PCS_map
		RCALL screen_set_ECS_map
		
		; Re-draw the cursor (must be drawn last to be on the foreground)
		RCALL screen_set_cursor
	agm_end:
	RET

screen_set_PCS_map:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function translates the current cells state from PCS to the 
	; screen.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI ZH, high(PCS_ADDR)
	LDI ZL, low(PCS_ADDR)

	; Store parameters for the function screen_set_hline
	LDI R16, 1
	MOV R13, R16					; Line length = 1 (point)

	; Loop over the Y position
	LDI R21, 0
	ssps_Y_loop:
		; Loop over the X position
		LDI R20, 0
		ssps_X_loop:
			; Get the cell state for this position
			LD R16, Z+

			SBRC R16, PCS_HEALTY_SHIP
			RJMP ssps_draw_healty_ship_point		; If this cell is a healty part of a ship

			SBRC R16, PCS_HIT_SHIP
			RJMP ssps_draw_hit_ship_point			; If this cell is a hit part of a ship

			SBRC R16, PCS_SUNK_SHIP
			RJMP ssps_draw_sunk_ship_point			; If this cell is part of a sunk ship

			ssps_X_loop_end:
			INC R20
			CPI R20, 10
			BRNE ssps_X_loop

		INC R21
		CPI R21, 10
		BRNE ssps_Y_loop
	RET

	ssps_draw_healty_ship_point:
		; The brightness for a PCS_HEALTY_SHIP cell depends on the current 
		; game state (low brightness when placing ships, high brightness else).
		LDI R16, 3						; Default brightness = maximum brightness
	
		; Get current game state
		game_get_state
		SBRC R10, GS_SHIPS_PLACEMENT	; Skip next instruction if game state is not GS_SHIPS_PLACEMENT
			LDI R16, 1					; Brightness in case of Ship Placement = minimum brightness
			
		MOV R12, R16					; Brightness

		; Add the map's offset of 2 pixels
		LDI R16, 2
		MOV R10, R20
		ADD R10, R16
		MOV R11, R21
		ADD R11, R16

		; Draw the point on the screen
		RCALL screen_set_hline

		RJMP ssps_X_loop_end
		
	ssps_draw_hit_ship_point:
		; Hit parts of ships are blinking at 2.5 Hz
		; We use the animation counter to determine if we are in ON or OFF state
		LDI R16, 3						; Blink from max to min brightness
		SBRC R9, 2						; Skip next instruction if Bit2 of animation counter cleared (to divide frequency by 4)
			LDI R16, 1					; Min brightness
			
		MOV R12, R16					; Brightness
		
		; Add the map's offset of 2 pixels
		LDI R16, 2
		MOV R10, R20
		ADD R10, R16
		MOV R11, R21
		ADD R11, R16

		; Draw the point on the screen
		RCALL screen_set_hline

		RJMP ssps_X_loop_end
		
	ssps_draw_sunk_ship_point:
		; Sunk of ships are blinking at 1.25 Hz
		; We use the animation counter to determine if we are in ON or OFF state
		LDI R16, 1						; Blink from min to zero brightness
		SBRC R9, 3						; Skip next instruction if Bit2 of animation counter cleared (to divide frequency by 8)
			LDI R16, 0					; LED off
			
		MOV R12, R16					; Brightness

		; Add the map's offset of 2 pixels
		LDI R16, 2
		MOV R10, R20
		ADD R10, R16
		MOV R11, R21
		ADD R11, R16

		; Draw the point on the screen
		RCALL screen_set_hline

		RJMP ssps_X_loop_end
		
		
screen_set_ECS_map:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function translates the current cells state from ECS to the 
	; screen.
	; Note: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI ZH, high(ECS_ADDR)
	LDI ZL, low(ECS_ADDR)

	; Store parameters for the function screen_set_hline
	LDI R16, 1
	MOV R13, R16					; Line length = 1 (point)

	; Loop over the Y position
	LDI R21, 0
	sses_Y_loop:
		; Loop over the X position
		LDI R20, 0
		sses_X_loop:
			; Get the cell state for this position
			LD R16, Z+

			SBRC R16, ECS_MISSED_SHOT
			RJMP sses_draw_missed_shot_point		; If this cell is a healty part of a ship

			SBRC R16, ECS_HIT_SHIP
			RJMP sses_draw_hit_ship_point			; If this cell is a hit part of a ship

			SBRC R16, ECS_SUNK_SHIP
			RJMP sses_draw_sunk_ship_point			; If this cell is part of a sunk ship

			sses_X_loop_end:
			INC R20
			CPI R20, 10
			BRNE sses_X_loop

		INC R21
		CPI R21, 10
		BRNE sses_Y_loop
	RET

	sses_draw_missed_shot_point:
		LDI R16, 1
		MOV R12, R16					; Brightness = minimum for missed shots

		; Add the map's offset of 28 pixels X and 2 pixels Y
		LDI R16, 28
		MOV R10, R20
		ADD R10, R16					; Add offset of 28 to X
		LDI R16, 2
		MOV R11, R21
		ADD R11, R16					; Add offset of 2 to Y

		; Draw the point on the screen
		RCALL screen_set_hline

		RJMP sses_X_loop_end
		
	sses_draw_hit_ship_point:
		; Hit parts of ships are blinking at 2.5 Hz
		; We use the animation counter to determine if we are in ON or OFF state
		LDI R16, 3						; Blink from max to min brightness
		SBRC R9, 2						; Skip next instruction if Bit2 of animation counter cleared (to divide frequency by 4)
			LDI R16, 1					; Min brightness
			
		MOV R12, R16					; Brightness
		
		; Add the map's offset of 28 pixels X and 2 pixels Y
		LDI R16, 28
		MOV R10, R20
		ADD R10, R16					; Add offset of 28 to X
		LDI R16, 2
		MOV R11, R21
		ADD R11, R16					; Add offset of 2 to Y

		; Draw the point on the screen
		RCALL screen_set_hline

		RJMP sses_X_loop_end
		
	sses_draw_sunk_ship_point:
		; Sunk of ships are blinking at 1.25 Hz
		; We use the animation counter to determine if we are in ON or OFF state
		LDI R16, 1						; Blink from min to zero brightness
		SBRC R9, 3						; Skip next instruction if Bit2 of animation counter cleared (to divide frequency by 8)
			LDI R16, 0					; LED off
			
		MOV R12, R16					; Brightness
		
		; Add the map's offset of 28 pixels X and 2 pixels Y
		LDI R16, 28
		MOV R10, R20
		ADD R10, R16					; Add offset of 28 to X
		LDI R16, 2
		MOV R11, R21
		ADD R11, R16					; Add offset of 2 to Y

		; Draw the point on the screen
		RCALL screen_set_hline

		RJMP sses_X_loop_end
		
	
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