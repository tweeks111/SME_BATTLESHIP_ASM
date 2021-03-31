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
	draw_rect 13, 2, 3, 14, 9
	draw_rect 14, 3, 0, 12, 7
	draw_word 15, 4, 3, WordGo
.endmacro

.macro draw_enemy_shot
	draw_line 0, 2, 3, 40
	draw_line 0, 10, 3, 40
	draw_word 2, 4, 3, WordEnemyShot
.endmacro

.macro draw_enemy_turn
	draw_line 0, 2, 3, 40
	draw_line 0, 10, 3, 40
	draw_word 2, 4, 3, WordEnemyTurn
.endmacro

.macro draw_your_turn
	draw_line 0, 2, 3, 40
	draw_line 0, 10, 3, 40
	draw_word 4, 4, 3, WordYourTurn
.endmacro


.macro draw_missed
	draw_rect 6, 2, @0, 1 , 9
	draw_rect 32, 2, @0, 1 , 9
	draw_line 6, 2, @0, 27
	draw_line 6, 10, @0, 27
	draw_word 8, 4, @0, WordMissed
.endmacro

.macro draw_hit
	draw_rect 11, 2, @0, 1 , 9
	draw_rect 28, 2, @0, 1 , 9
	draw_line 12, 2, @0, 16
	draw_line 12, 10, @0, 16
	draw_word 13, 4, @0, WordHit
.endmacro

.macro draw_sunk
	draw_rect 9, 2, @0, 1 , 9
	draw_rect 30, 2, @0, 1 , 9
	draw_line 10, 2, @0, 20
	draw_line 10, 10, @0, 20
	draw_word 11, 4, @0, WordSunk
.endmacro

.macro draw_winner
	draw_word 5, 4, @0, WordYouWon
	draw_line 0, 2, 3, 40
	draw_line 0, 10, 3, 40
.endmacro

.macro draw_looser
	draw_word 3, 4, @0, WordYouLost
	draw_line 0, 2, 3, 40
	draw_line 0, 10, 3, 40
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

draw_boards:
	draw_left_board 2
	draw_right_board 2
	RET
