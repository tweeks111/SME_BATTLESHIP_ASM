;
; GameState.inc
;
; This file is part of the Battleship project.
;
; It contains macros and defines to manage the game state.
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

 
; GAME STATES DEFINITION (must be less than or equal to 7) ;
#define GS_INIT				0
#define GS_DISCOVERY		1
#define GS_SHIPS_PLACEMENT	2
#define GS_MAIN_GAME		3
#define GS_GAME_WON			4
#define GS_GAME_LOST		5

; Game state byte address in SRAM
#define GAME_STATE_ADDR		0x0192


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