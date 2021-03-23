;
; Animations.inc
;
; This file is part of the Battleship project.
;
; It contains all the functions needed to display animated content
; such as blinking pixels on the screen.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

AnimationsInterruptRoutine:
	; This routine runs at 10 Hz
	;TODO: this will handle the animations
	RET

anim_title_fade:
	CLR R10
	RCALL screen_fill
	draw_title 1
	sleep_ts 1
	RCALL screen_fill
	draw_title 2
	sleep_ts 1
	RCALL screen_fill
	draw_title 3
	sleep_ts 1
	RCALL screen_fill
	draw_title 2
	sleep_ts 1
	RET

anim_sweep_screen:
	LDI R16, 19		; X
	MOV R10, R16	
	LDI R16, 0		; Y
	MOV R11, R16	
	LDI R16, 0		; Brightness (Off)
	MOV R12, R16	
	LDI R16, 2		; Width
	MOV R13, R16	
	LDI R16, 14		; Height
	MOV R14, R16

	aswp_loop:
		; Display an empty rectangle
		RCALL screen_set_rect
		sleep_ts 1

		; Decrease the X position and add 2 to the width
		INC R13
		INC R13
		DEC R10
		BRNE aswp_loop

	; Clear the screen
	CLR R10
	RCALL screen_fill

	RET

anim_intro:
	RCALL anim_title_fade
	RCALL anim_sweep_screen
	RET