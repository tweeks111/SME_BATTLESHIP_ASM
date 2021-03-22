;
; ScreenDrawings.inc
;
; This file is part of the Battleship project.
;
; It contains marcos describing some drawings on the screen.
;
; Authors : Mathieu Philippart & Th�o Lepoutte
;

.macro draw_title
	draw_char 1, 4, @0, _B 
	draw_char 5, 4, @0, _A
	draw_char 9, 4, @0, _T
	draw_char 13, 4, @0, _T
	draw_char 17, 4, @0, _L
	draw_char 21, 4, @0, _E
	draw_char 25, 4, @0, _S
	draw_char 29, 4, @0, _H
	draw_char 33, 4, @0, _I
	draw_char 37, 4, @0, _P
	draw_line 0, 2, 3, 40
	draw_line 0, 10, 3, 40
.endmacro

.macro draw_go
	draw_rect 14, 2, 3, 12, 9
	draw_rect 15, 3, 0, 10, 7
	draw_char 16, 4, 3, _G
	draw_char 20, 4, 3, _O
	draw_char 24, 4, 3, _EXC
.endmacro

.macro draw_miss
	draw_rect 10, 2, @0, 1 , 9
	draw_rect 28, 2, @0, 1 , 9
	draw_line 10, 2, @0, 19
	draw_line 10, 10, @0, 19
	draw_char 12, 4, @0, _M
	draw_char 16, 4, @0, _I
	draw_char 20, 4, @0, _S
	draw_char 24, 4, @0, _S
.endmacro

.macro draw_hit
	draw_rect 13, 2, @0, 1 , 9
	draw_rect 27, 2, @0, 1 , 9
	draw_line 13, 2, @0, 14
	draw_line 13, 10, @0, 14
	draw_char 15, 4, @0, _H
	draw_char 19, 4, @0, _I
	draw_char 23, 4, @0, _T
.endmacro
