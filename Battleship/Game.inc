;
; Game.inc
;
; This file is part of the Battleship project.
;
; It contains all the game management functions.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

; GAME STATES DEFINITION (must be less than or equal to 7) ;
#define GS_INIT				0
#define GS_DISCOVERY		1
#define GS_SHIPS_PLACEMENT	2
#define GS_MAIN_GAME		3
; TODO.....

; Game state byte address in SRAM
#define GAME_STATE_ADDR		0x0192

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

	
update_game_maps:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function updates the game maps (left and right)
	; NOTE: this function uses: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Draw two rectangles to clear the two game maps
	draw_rect 2, 2, 0, 10, 10
	draw_rect 29, 2, 0, 10, 10

	; Re-draw the PCS
	RCALL screen_set_PCS

	; Re-draw the cursor
	RCALL screen_set_cursor
	RET
