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

anim_title:
	CLR R10
	RCALL screen_fill
	draw_title 1
	sleep_ts 1
	CLR R10
	RCALL screen_fill
	draw_title 2
	sleep_ts 1
	CLR R10
	RCALL screen_fill
	draw_title 3
	sleep_ts 1
	CLR R10
	RCALL screen_fill
	draw_title 2
	sleep_ts 1
	RET

anim_fade:
	draw_rect 20, 0, 0, 1, 14 
	sleep_ts 1
	draw_rect 19, 0, 0, 2, 14 
	sleep_ts 1
	draw_rect 18, 0, 0, 4, 14 
	sleep_ts 1
	draw_rect 17, 0, 0, 6, 14 
	sleep_ts 1
	draw_rect 16, 0, 0, 8, 14 
	sleep_ts 1
	draw_rect 15, 0, 0, 10, 14 
	sleep_ts 1
	draw_rect 14, 0, 0, 12, 14 
	sleep_ts 1
	draw_rect 13, 0, 0, 14, 14 
	sleep_ts 1
	draw_rect 12, 0, 0, 16, 14 
	sleep_ts 1
	draw_rect 11, 0, 0, 18, 14 
	sleep_ts 1
	draw_rect 10, 0, 0, 20, 14 
	sleep_ts 1
	draw_rect 9, 0, 0, 22, 14 
	sleep_ts 1
	draw_rect 8, 0, 0, 24, 14 
	sleep_ts 1
	draw_rect 7, 0, 0, 26, 14 
	sleep_ts 1
	draw_rect 6, 0, 0, 28, 14 
	sleep_ts 1
	draw_rect 5, 0, 0, 30, 14 
	sleep_ts 1
	draw_rect 4, 0, 0, 32, 14 
	sleep_ts 1
	draw_rect 3, 0, 0, 34, 14 
	sleep_ts 1
	draw_rect 2, 0, 0, 36, 14 
	sleep_ts 1
	draw_rect 1, 0, 0, 38, 14 
	sleep_ts 1
	draw_rect 0, 0, 0, 40, 14 
	sleep_ts 1
	RET

anim_intro:
	RCALL anim_title
	RCALL anim_fade
	RET