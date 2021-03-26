/*
 * Game.inc
 *
 *  Created: 22-03-21 21:28:08
 *   Author: theol
 */ 

; GAME STATES DEFINITION (must be less than or equal to 7) ;
#define GS_INIT				0
#define GS_DISCOVERY		1
#define GS_SHIPS_PLACEMENT	2
#define GS_MAIN_GAME		3
; TODO.....

; Game state byte address in SRAM
#define GAME_STATE_ADDR	0x0192
#define SPS				0x018D	; Ship Placement Status 
#define CURSOR_X		0x018E
#define CURSOR_Y		0x018F
#define SL				0x019D	; Ship Length

; Ship Storage
#define SHIP1 0x01A0
#define SHIP2 0x01A4
#define SHIP3 0x01A8
#define SHIP4 0x01AC
#define SHIP5 0x01B0

.macro game_change_state
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro changes the current Game State in SRAM to the state
	; given in argument 0.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI YH,high(GAME_STATE_ADDR)
	LDI YL,low(GAME_STATE_ADDR)

	LDI R16, (1 << @0)
	ST Y, R16
.endmacro

.macro game_get_state
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro gets current Game State from SRAM and stores it in R10.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI YH,high(GAME_STATE_ADDR)
	LDI YL,low(GAME_STATE_ADDR)
	LD R10, Y
.endmacro

init_map_cells:
	LDI R16, 100
	LDI R17, 0x01	; Empty Cell
	LDI YH,high(PCS)
	LDI YL,low(PCS)
	imc_PCS_loop:
		ST Y+, R17
		DEC R16
		BRNE imc_PCS_loop
	LDI R16, 100
	LDI YH,high(ECS)
	LDI YL,low(ECS)
	imc_ECS_loop:
		ST Y+, R17
		DEC R16
		BRNE imc_ECS_loop
	RET

move_up:
	LDI YH,high(CURSOR_X)
	LDI YL,low(CURSOR_X)
	LD R22, Y+	; CURSOR_X
	LD R23, Y	; CURSOR_Y
	LDI YH,high(SL)
	LDI YL,low(SL)
	LD R24, Y	; Ship Length

	CPI R23, 0
	BREQ end_move_up
		RCALL screen_clear
		RCALL draw_boards
		RCALL screen_set_PCS
		DEC R23
		LDI YH,high(CURSOR_Y)
		LDI YL,low(CURSOR_Y)
		ST Y, R23

	LDI YH,high(SPS)
	LDI YL,low(SPS)
	LD R21, Y	; SPS

	SBRS R21, 7
		RJMP move_up_h_ship

	move_up_v_ship:
			draw_ship_left R22, R23, 3, R24, 1
			RJMP end_move_up
	move_up_h_ship:
			draw_ship_left R22, R23, 3, R24, 0
	end_move_up:
	RET


move_down:
	LDI YH,high(SPS)
	LDI YL,low(SPS)
	LD R21, Y+	; SPS
	LD R22, Y+	; CURSOR_X
	LD R23, Y	; CURSOR_Y
	LDI YH,high(SL)
	LDI YL,low(SL)
	LD R24, Y	; Ship Length

	SBRS R21, 7
		RJMP move_down_h_ship
	
	move_down_v_ship:
		ADD R23, R24
		CPI R23, 10
		BREQ end_move_down
			RCALL screen_clear
			RCALL draw_boards
			RCALL screen_set_PCS
			SUB R23, R24
			INC R23
			LDI YH,high(CURSOR_Y)
			LDI YL,low(CURSOR_Y)
			ST Y, R23
			draw_ship_left R22, R23, 3, R24, 1
			RJMP end_move_down
	move_down_h_ship:
		CPI R23, 9
		BREQ end_move_down
			RCALL screen_clear
			RCALL draw_boards
			RCALL screen_set_PCS
			INC R23
			LDI YH,high(CURSOR_Y)
			LDI YL,low(CURSOR_Y)
			ST Y, R23
			draw_ship_left R22, R23, 3, R24, 0
	end_move_down:
	RET


move_left:
	LDI YH,high(CURSOR_X)
	LDI YL,low(CURSOR_X)
	LD R22, Y+	; CURSOR_X
	LD R23, Y	; CURSOR_Y
	LDI YH,high(SL)
	LDI YL,low(SL)
	LD R24, Y	; Ship Length

	CPI R22, 0
		BREQ end_move_left
			RCALL screen_clear
			RCALL draw_boards
			RCALL screen_set_PCS
			DEC R22
			LDI YH,high(CURSOR_X)
			LDI YL,low(CURSOR_X)
			ST Y, R22

	LDI YH,high(SPS)
	LDI YL,low(SPS)
	LD R21, Y	; SPS

	SBRS R21, 7
		RJMP move_left_h_ship

	move_left_v_ship:
			draw_ship_left R22, R23, 3, R24, 1
			RJMP end_move_left

	move_left_h_ship:
			draw_ship_left R22, R23, 3, R24, 0

	end_move_left:
	RET

move_right:
	LDI YH,high(SPS)
	LDI YL,low(SPS)
	LD R21, Y+	; SPS
	LD R22, Y+	; CURSOR_X
	LD R23, Y	; CURSOR_Y
	LDI YH,high(SL)
	LDI YL,low(SL)
	LD R24, Y	; Ship Length
		
	SBRS R21, 7
		RJMP move_right_h_ship

	move_right_v_ship:
		CPI R22, 9
		BREQ end_move_right
			RCALL screen_clear
			RCALL draw_boards
			RCALL screen_set_PCS
			INC R22
			LDI YH,high(CURSOR_X)
			LDI YL,low(CURSOR_X)
			ST Y, R22
			draw_ship_left R22, R23, 3, R24, 1
			RJMP end_move_right

	move_right_h_ship:
		ADD R22, R24
		CPI R22, 10
		BREQ end_move_right
			RCALL screen_clear
			RCALL draw_boards
			RCALL screen_set_PCS
			SUB R22, R24
			INC R22
			LDI YH,high(CURSOR_X)
			LDI YL,low(CURSOR_X)
			ST Y, R22
			draw_ship_left R22, R23, 3, R24, 0

	end_move_right:
	RET

rotate_ship:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Rotate the ship
	; If the ship is near a border, it will be moved in order to avoid
	; it to cross the border
	; NOTE: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	RCALL screen_clear
	RCALL draw_boards
	RCALL screen_set_PCS
	LDI YH,high(SL)
	LDI YL,low(SL)
	LD R17, Y	; Ship Length
	LDI YH,high(SPS)
	LDI YL,low(SPS)
	LD R16, Y	; SPS

	SBRC R16, 7
		RJMP set_horizontal
	set_vertical:
		SBR R16, 0b10000000
		ST Y+, R16
		LDI R16, 1		
		MOV R14, R16	; Set orientation to 1
		MOV R13, R17	; Set length
		LD R10, Y+		; Load X value
		LD R16, Y		; Load Y value
		MOV R18, R16
		ADD R18, R17
		SUBI R18, 10
		BRMI rs_no_dec_v
			SUB R16, R18
		rs_no_dec_v:
			ST Y, R16		; Store decremented Y
			MOV R11, R16
			RJMP end_rotate_ship
	set_horizontal:
		CBR R16, 0b10000000
		ST Y+, R16
		LDI R16, 0
		MOV R14, R16	; Set orientation to 0
		MOV R13, R17	; Set length
		LD R16, Y		; Load X value
		MOV R18, R16
		ADD R18, R17
		SUBI R18, 10
		BRMI rs_no_dec_h
			SUB R16, R18
		rs_no_dec_h:
			ST Y+, R16		; Store decremented X
			MOV R10, R16
			LD R11, Y		; Load Y value
	end_rotate_ship:
		LDI R16, 3
		MOV R12, R16
		RCALL screen_set_ship_left
		RET

check_overlap:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Place the ship on left board
	; If the ship overlap another ship it cannot be placed
	; NOTE: this function changes: R16, R17, R18, R21, R22, R23, R24
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI YH,high(SPS)
	LDI YL,low(SPS)
	LD R21, Y+	; SPS
	LD R22, Y+	; CURSOR_X
	LD R23, Y	; CURSOR_Y
	LDI YH,high(SL)
	LDI YL,low(SL)
	LD R24, Y	; Ship Length

	LDI R18, 10		; Row Length
	MUL R18, R23	; Row = Row length * Row index
	MOV R16, R0
	ADD R16, R22	; Row + column index

	SBRS R21, 7		; Skip if vertical
		RJMP check_horizontal
	check_vertical:
		LDI YH, high(PCS)
		MOV YL, R16
		LD R17, Y
		CPI R17, 2		; Ship Placed in PCS => 0x02
		BREQ overlap
		ADD R16, R18	; +10 bytes for next row
		DEC R24
		BRNE check_vertical
		RJMP place_ship
	check_horizontal:
		LDI YH, high(PCS)
		MOV YL, R16
		LD R17, Y
		CPI R17, 2		; Ship Placed in PCS => 0x02
		BREQ overlap
		INC R16
		DEC R24
		BRNE check_horizontal
		RJMP place_ship

	overlap:
		RET

	place_ship:
		SBRC R21, 0
			RJMP place_ship1
		SBRC R21, 1
			RJMP place_ship2
		SBRC R21, 2
			RJMP place_ship3
		SBRC R21, 3
			RJMP place_ship4

		place_ship5:
			LDI YH,high(SHIP5)
			LDI YL,low(SHIP5)
			LDI R16, 5	; Ship ID
			RCALL add_ship_to_PS
			game_change_state 3
			LDI R16, 1	; Next Ship Length
			RJMP add_ship_to_PCS
		place_ship4:
			LDI YH,high(SHIP4)
			LDI YL,low(SHIP4)
			LDI R16, 4	; Ship ID
			RCALL add_ship_to_PS
			LDI R16, 2  ; Next Ship Length
			RJMP add_ship_to_PCS
		place_ship3:
			LDI YH,high(SHIP3)
			LDI YL,low(SHIP3)
			LDI R16, 3	; Ship ID
			RCALL add_ship_to_PS
			LDI R16, 3  ; Next Ship Length
			RJMP add_ship_to_PCS
		place_ship2:
			LDI YH,high(SHIP2)
			LDI YL,low(SHIP2)
			LDI R16, 2	; Ship ID
			RCALL add_ship_to_PS
			LDI R16, 3  ; Next Ship Length
			RJMP add_ship_to_PCS
		place_ship1:
			LDI YH,high(SHIP1)
			LDI YL,low(SHIP1)
			LDI R16, 1	; Ship ID
			RCALL add_ship_to_PS
			LDI R16, 4  ; Next Ship Length
			RJMP add_ship_to_PCS

		add_ship_to_PS:
			ST Y+, R16		; ID
			ST Y+, R22		; X
			ST Y+, R23		; Y
			LDI R16, 0
			SBRC R21, 7
				LDI R16, 1
			ST Y, R16		; Orientation
			RET

		add_ship_to_PCS:
			LDI YH,high(SL)
			LDI YL,low(SL)
			LD R24, Y		; Ship Length
			ST Y, R16		; Store next ship length

			MUL R18, R23	; Row = Row length * Row index
			MOV R16, R0
			ADD R16, R22	; Row + column index
			LDI R17, 2		; 0x02: Cell occupied by a ship (healty part of a ship)

			SBRS R21, 7		; Skip if vertical
				RJMP ship_is_horizontal
			ship_is_vertical:
				LDI YH, high(PCS)
				MOV YL, R16
				ST Y, R17
				ADD R16, R18
				DEC R24
				BRNE ship_is_vertical
				RJMP end_place_ship
			ship_is_horizontal:
				LDI YH, high(PCS)
				MOV YL, R16
				ST Y+, R17
				INC R16
				DEC R24
				BRNE ship_is_horizontal
			
		end_place_ship:
		LDI YH,high(SPS)
		LDI YL,low(SPS)
		LD R21, Y	; SPS
		LSL R21
		ST Y, R21

		RCALL screen_set_PCS

		LDI YH,high(SL)
		LDI YL,low(SL)
		LD R16, Y	; Ship Length
		CPI R16, 1
		BREQ ret_place_ship
			MOV R13, R16
			LDI YH,high(CURSOR_X)
			LDI YL,low(CURSOR_X)
			LD R10, Y+
			LD R11, Y
			LDI R16, 3
			MOV R12, R16
			LDI YH,high(SL)
			LDI YL,low(SL)
			LD R13, Y	; Ship Length
			LDI R16, 0
			MOV R14, R16
			RCALL screen_set_ship_left
		ret_place_ship:
		RET