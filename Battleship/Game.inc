;
; Game.inc
;
; This file is part of the Battleship project.
;
; This file contains the game management functions.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

; GAME STATES DEFINITION (must be less than or equal to 7) ;
#define GS_INIT				0
#define GS_DISCOVERY		1
#define GS_SHIPS_PLACEMENT	2
; TODO.....

; Game state byte address in SRAM
#define GAME_STATE_ADDR	0x0192

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


move_up:
	LDI YH,0x01
	LDI YL,0x8D
	LD R21, Y+	; State
	LD R23, Y+	; X
	LD R24, Y	; Y
	SBRC R21, 0
		RJMP move_up_ship0
	SBRC R21, 1
		RJMP move_up_ship1
	SBRC R21, 2
		RJMP move_up_ship2
	SBRC R21, 3
		RJMP move_up_ship3
	SBRC R21, 4
		RJMP move_up_ship4
		
	move_up_ship0:
		LDI R22, 5	; Length
		SBRC R21, 7
			RJMP move_up_v_ship
		RJMP move_up_h_ship
	move_up_ship1:
		LDI R22, 4
		SBRC R21, 7
			RJMP move_up_v_ship
		RJMP move_up_h_ship
	move_up_ship2:
		LDI R22, 3
		SBRC R21, 7
			RJMP move_up_v_ship
		RJMP move_up_h_ship
	move_up_ship3:
		LDI R22, 3
		SBRC R21, 7
			RJMP move_up_v_ship
		RJMP move_up_h_ship
	move_up_ship4:
		LDI R22, 2
		SBRC R21, 7
			RJMP move_up_v_ship
		RJMP move_up_h_ship

	move_up_v_ship:
		CPI R24, 0
		BREQ end_move_up
			RCALL screen_clear
			RCALL draw_boards
			DEC R24
			LDI YH,0x01
			LDI YL,0x8F
			ST Y, R24
			LDI R16, 2
			ADD R23, R16
			ADD R24, R16
			MOV R10, R23	; X
			MOV R11, R24	; Y
			LDI R16, 3
			MOV R12, R16	; Brightness
			LDI R16, 1
			MOV R13, R16	; Width =1
			MOV R14, R22	; Height
			RCALL screen_set_rect
			RJMP end_move_up

	move_up_h_ship:
		CPI R24, 0
		BREQ end_move_up
			RCALL screen_clear
			RCALL draw_boards
			DEC R24
			LDI YH,0x01
			LDI YL,0x8F
			ST Y, R24
			LDI R16, 2
			ADD R23, R16
			ADD R24, R16
			MOV R10, R23	; X
			MOV R11, R24	; Y
			LDI R16, 3		
			MOV R12, R16	; Brightness
			LDI R16, 1
			MOV R13, R22	; Width
			MOV R14, R16	; Heigth = 1
			RCALL screen_set_rect
	end_move_up:
	RET


move_down:
	LDI YH,0x01
	LDI YL,0x8D
	LD R21, Y+	; State
	LD R23, Y+	; X
	LD R24, Y	; Y
	SBRC R21, 0
		RJMP move_down_ship0
	SBRC R21, 1
		RJMP move_down_ship1
	SBRC R21, 2
		RJMP move_down_ship2
	SBRC R21, 3
		RJMP move_down_ship3
	SBRC R21, 4
		RJMP move_down_ship4
		
	move_down_ship0:
		LDI R22, 5	; Length
		SBRC R21, 7
			RJMP move_down_v_ship
		RJMP move_down_h_ship
	move_down_ship1:
		LDI R22, 4
		SBRC R21, 7
			RJMP move_down_v_ship
		RJMP move_down_h_ship
	move_down_ship2:
		LDI R22, 3
		SBRC R21, 7
			RJMP move_down_v_ship
		RJMP move_down_h_ship
	move_down_ship3:
		LDI R22, 3
		SBRC R21, 7
			RJMP move_down_v_ship
		RJMP move_down_h_ship
	move_down_ship4:
		LDI R22, 2
		SBRC R21, 7
			RJMP move_down_v_ship
		RJMP move_down_h_ship

	move_down_v_ship:
		ADD R24, R22
		CPI R24, 10
		BREQ end_move_down
			RCALL screen_clear
			RCALL draw_boards
			SUB R24, R22
			INC R24
			LDI YH,0x01
			LDI YL,0x8F
			ST Y, R24
			LDI R16, 2
			ADD R23, R16
			ADD R24, R16
			MOV R10, R23	; X
			MOV R11, R24	; Y
			LDI R16, 3
			MOV R12, R16	; Brightness
			LDI R16, 1
			MOV R13, R16	; Width =1
			MOV R14, R22	; Height
			RCALL screen_set_rect
			RJMP end_move_down

	move_down_h_ship:
		CPI R24, 9
		BREQ end_move_down
			RCALL screen_clear
			RCALL draw_boards
			INC R24
			LDI YH,0x01
			LDI YL,0x8F
			ST Y, R24
			LDI R16, 2
			ADD R23, R16
			ADD R24, R16
			MOV R10, R23	; X
			MOV R11, R24	; Y
			LDI R16, 3		
			MOV R12, R16	; Brightness
			LDI R16, 1
			MOV R13, R22	; Width
			MOV R14, R16	; Heigth = 1
			RCALL screen_set_rect
	end_move_down:
	RET


move_left:
	LDI YH,0x01
	LDI YL,0x8D
	LD R21, Y+	; State
	LD R23, Y+	; X
	LD R24, Y	; Y
	SBRC R21, 0
		RJMP move_left_ship0
	SBRC R21, 1
		RJMP move_left_ship1
	SBRC R21, 2
		RJMP move_left_ship2
	SBRC R21, 3
		RJMP move_left_ship3
	SBRC R21, 4
		RJMP move_left_ship4
		
	move_left_ship0:
		LDI R22, 5	; Length
		SBRC R21, 7
			RJMP move_left_v_ship
		RJMP move_left_h_ship
	move_left_ship1:
		LDI R22, 4
		SBRC R21, 7
			RJMP move_left_v_ship
		RJMP move_left_h_ship
	move_left_ship2:
		LDI R22, 3
		SBRC R21, 7
			RJMP move_left_v_ship
		RJMP move_left_h_ship
	move_left_ship3:
		LDI R22, 3
		SBRC R21, 7
			RJMP move_left_v_ship
		RJMP move_left_h_ship
	move_left_ship4:
		LDI R22, 2
		SBRC R21, 7
			RJMP move_left_v_ship
		RJMP move_left_h_ship

	move_left_v_ship:
		CPI R23, 0
		BREQ end_move_left
			RCALL screen_clear
			RCALL draw_boards
			DEC R23
			LDI YH,0x01
			LDI YL,0x8E
			ST Y, R23
			LDI R16, 2
			ADD R23, R16
			ADD R24, R16
			MOV R10, R23	; X
			MOV R11, R24	; Y
			LDI R16, 3
			MOV R12, R16	; Brightness
			LDI R16, 1
			MOV R13, R16	; Width =1
			MOV R14, R22	; Height
			RCALL screen_set_rect
			RJMP end_move_left

	move_left_h_ship:
		CPI R23, 0
		BREQ end_move_left
			RCALL screen_clear
			RCALL draw_boards
			DEC R23
			LDI YH,0x01
			LDI YL,0x8E
			ST Y, R23
			LDI R16, 2
			ADD R23, R16
			ADD R24, R16
			MOV R10, R23	; X
			MOV R11, R24	; Y
			LDI R16, 3		
			MOV R12, R16	; Brightness
			LDI R16, 1
			MOV R13, R22	; Width
			MOV R14, R16	; Heigth = 1
			RCALL screen_set_rect
	end_move_left:
	RET

move_right:
	LDI YH,0x01
	LDI YL,0x8D
	LD R21, Y+	; State
	LD R23, Y+	; X
	LD R24, Y	; Y
	SBRC R21, 0
		RJMP move_right_ship0
	SBRC R21, 1
		RJMP move_right_ship1
	SBRC R21, 2
		RJMP move_right_ship2
	SBRC R21, 3
		RJMP move_right_ship3
	SBRC R21, 4
		RJMP move_right_ship4
		
	move_right_ship0:
		LDI R22, 5	; Length
		SBRC R21, 7
			RJMP move_right_v_ship
		RJMP move_right_h_ship
	move_right_ship1:
		LDI R22, 4
		SBRC R21, 7
			RJMP move_right_v_ship
		RJMP move_right_h_ship
	move_right_ship2:
		LDI R22, 3
		SBRC R21, 7
			RJMP move_right_v_ship
		RJMP move_right_h_ship
	move_right_ship3:
		LDI R22, 3
		SBRC R21, 7
			RJMP move_right_v_ship
		RJMP move_right_h_ship
	move_right_ship4:
		LDI R22, 2
		SBRC R21, 7
			RJMP move_right_v_ship
		RJMP move_right_h_ship

	move_right_v_ship:
		CPI R23, 9
		BREQ end_move_right
			RCALL screen_clear
			RCALL draw_boards
			INC R23
			LDI YH,0x01
			LDI YL,0x8E
			ST Y, R23
			LDI R16, 2
			ADD R23, R16
			ADD R24, R16
			MOV R10, R23	; X
			MOV R11, R24	; Y
			LDI R16, 3
			MOV R12, R16	; Brightness
			LDI R16, 1
			MOV R13, R16	; Width =1
			MOV R14, R22	; Height
			RCALL screen_set_rect
			RJMP end_move_right

	move_right_h_ship:
		ADD R23, R22
		CPI R23, 10
		BREQ end_move_right
			RCALL screen_clear
			RCALL draw_boards
			SUB R23, R22
			INC R23
			LDI YH,0x01
			LDI YL,0x8E
			ST Y, R23
			LDI R16, 2
			ADD R23, R16
			ADD R24, R16
			MOV R10, R23	; X
			MOV R11, R24	; Y
			LDI R16, 3		
			MOV R12, R16	; Brightness
			LDI R16, 1
			MOV R13, R22	; Width
			MOV R14, R16	; Heigth = 1
			RCALL screen_set_rect
	end_move_right:
	RET

rotate_ship:
	LDI YH,0x01
	LDI YL,0x8D
	LD R16, Y
	SBRC R16, 7
		RJMP set_horizontal
	set_vertical:
		SBR R16, 0b10000000
		RJMP end_rotate_ship
	set_horizontal:
		CBR R16, 0b10000000
	end_rotate_ship:
		ST Y, R16
		RET

place_ship:
	LDI YH,0x01
	LDI YL,0x8D
	LD R21, Y+	; State
	LD R22, Y+	; X
	LD R23, Y	; Y

	SBRC R21, 0
		RJMP place_ship0
	SBRC R21, 1
		RJMP place_ship1
	SBRC R21, 2
		RJMP place_ship2
	SBRC R21, 3
		RJMP place_ship3
	SBRC R21, 4
		RJMP place_ship4

	RJMP end_place_ship

	place_ship0:
		LDI YL,0xA0
		LDI R16, 0
		RJMP store_ship
	place_ship1:
		LDI YL,0xA4
		LDI R16, 1
		RJMP store_ship
	place_ship2:
		LDI YL,0xA8
		LDI R16, 2
		RJMP store_ship
	place_ship3:
		LDI YL,0xAC
		LDI R16, 3
		RJMP store_ship
	place_ship4:
		LDI YL,0xB0
		LDI R16, 4

	store_ship:
		ST Y+, R16
		ST Y+, R22
		ST Y+, R23
		LDI R16, 0
		SBRC R21, 7
			INC R16
		ST Y, R16

	end_place_ship:
		LSL R21
		LDI YH,0x01
		LDI YL,0x8D
		ST Y,R21

	RET