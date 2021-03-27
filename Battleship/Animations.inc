;
; Animations.inc
;
; This file is part of the Battleship project.
;
; It contains all the functions needed to display animated content
; such as blinking pixels on the screen.
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

anim_init:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes animation registers.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Clear the animation counter register
	CLR R9
	
	RET

AnimationsInterruptRoutine:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This routine runs at 10 Hz and increments circularly the animation
	; counter.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Check if animation counter equals 15
	MOV R25, R9
	ANDI R25, 0b00001111		; Mask 4 first bits (others are flags)
	CPI R25, 0x0F
	BRNE air_skip_rst
		; If animation counter reached the max -> reset
		CLR R9

	air_skip_rst:
	
	; Check if first bit of R9 is cleared
	SBRC R9, 0
	RJMP air_skip_freq_div
		; We fall here at 5 Hz
		; We set the "5 Hz interrupt" bit in the animation counter byte (bit 4)
		LDI R25, 0b00010000
		OR R9, R25					; Set bit 4 in anim counter byte

	air_skip_freq_div:
	; Increment the animation counter
	INC R9

	RET


anim_title_fade:
	RCALL screen_clear
	draw_title 1
	sleep_ts 1
	RCALL screen_clear
	draw_title 2
	sleep_ts 1
	RCALL screen_clear
	draw_title 3
	sleep_ts 1
	RCALL screen_clear
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