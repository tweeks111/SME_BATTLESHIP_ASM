;
; ScreenDrawings.inc
;
; This file is part of the Battleship project.
;
; It contains marcos describing some drawings on the screen.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

.macro draw_title
	draw_word 1, 4, @0, WordBattleship
	draw_line 0, 2, 3, 40
	draw_line 0, 10, 3, 40
.endmacro

.macro draw_go
	draw_rect 14, 2, 3, 12, 9
	draw_rect 15, 3, 0, 10, 7
	draw_word 16, 4, 3, WordGo
.endmacro

.macro draw_miss
	draw_rect 10, 2, @0, 1 , 9
	draw_rect 28, 2, @0, 1 , 9
	draw_line 10, 2, @0, 19
	draw_line 10, 10, @0, 19
	draw_word 12, 4, @0, WordMiss
.endmacro

.macro draw_hit
	draw_rect 13, 2, @0, 1 , 9
	draw_rect 27, 2, @0, 1 , 9
	draw_line 13, 2, @0, 14
	draw_line 13, 10, @0, 14
	draw_word 15, 4, @0, WordHit
.endmacro

.macro draw_left_board
	draw_rect 0, 0, @0, 1, 14
	draw_rect 13, 0, @0, 1, 14
	draw_line 1, 0, @0, 12
	draw_line 1, 13, @0, 12
.endmacro

.macro draw_right_board
	draw_rect 26, 0, @0, 1, 14
	draw_rect 39, 0, @0, 1, 14
	draw_line 27, 0, @0, 12
	draw_line 27, 13, @0, 12
.endmacro

.macro draw_left_arrow
	draw_line 19, 1, @0, 1
	draw_line 18, 2, @0, 5
	draw_line 17, 3, @0, 6
	draw_line 18, 4, @0, 5
	draw_line 19, 5, @0, 1
.endmacro

.macro draw_right_arrow
	draw_line 20, 1, @0, 1
	draw_line 17, 2, @0, 5
	draw_line 17, 3, @0, 6
	draw_line 17, 4, @0, 5
	draw_line 20, 5, @0, 1
.endmacro

.macro draw_small_right_arrow
	draw_line @0, @1, @2, 4
	draw_line @0+2, @1-1, @2, 1
	draw_line @0+2, @1+1, @2, 1
.endmacro

.macro draw_ship_left ;($X, $Y, Brightness, $Length, Orientation)
	MOV R10, @0		; $X
	MOV R11, @1		; $Y
	LDI R16, @2		; Brightness
	MOV R12, R16
	MOV R13, @3		; $Length	
	LDI R16, @4		; Orientation
	MOV R14, R16
	RCALL screen_set_ship_left
.endmacro

draw_boards:
	draw_left_board 2
	draw_right_board 2
	RET

draw_cursor:
	LDI YH, 0x01
	LDI YL, 0x8E
	LD R16, Y+
	LD R17, Y
	LDI R18, 2		// Board Offset
	ADD R16, R18
	ADD R17, R18
	MOV R10, R16
	MOV R11, R17
	LDI R16, 2
	MOV R12, R16
	LDI R16, 1
	MOV R13, R16
	RCALL screen_set_hline
	RET

screen_set_ship_left:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Draw a ship on left board
	; R10: Xrel position
	; R11: Yrel position
	; R12: LED brightness value (0 = Off; 1 = 33%, 2 = 67%, 3 = 100%)
	; R13: Ship length
	; R14: Ship orientation
	; NOTE: this function changes: R0, R1, R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI R16, 2
	ADD R10, R16
	ADD R11, R16
	LDI R16, 1
	SBRS R14, 0
		RJMP draw_h_ship_left
	draw_v_ship_left:
		MOV R14, R13	; Height
		MOV R13, R16	; Width=1
		RJMP end_draw_ship_left
	draw_h_ship_left:
		MOV R14, R16	; Height=1
	end_draw_ship_left:
	RCALL screen_set_rect
	RET