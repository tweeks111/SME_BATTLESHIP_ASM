/*
 * Game.inc
 *
 *  Created: 22-03-21 21:28:08
 *   Author: theol
 */ 


 // Cursor Byte -> 0x0101

move_up:
 	LDI ZH,0x01
	LDI ZL,0x8F
	LD R22, Z
	CPI R22, 0				// Y=0
	BREQ end_move_up
		RCALL screen_clear
		RCALL draw_boards
		DEC R22
		ST Z, R22
		RCALL draw_cursor
	end_move_up:
	RET


move_down:
	LDI ZH,0x01
	LDI ZL,0x8F
	LD R22, Z
	CPI R22, 9
	BREQ end_move_down
		RCALL screen_clear
		RCALL draw_boards
		INC R22
		ST Z, R22
		RCALL draw_cursor
	end_move_down:
	RET


move_left:
  	LDI ZH,0x01
	LDI ZL,0x8E
	LD R22, Z
	CPI R22, 0
	BREQ end_move_left
		RCALL screen_clear
		RCALL draw_boards
		DEC R22
		ST Z, R22
		RCALL draw_cursor
	end_move_left:
	RET


move_right:
   	LDI ZH,0x01
	LDI ZL,0x8E
	LD R22, Z
	CPI R22, 9
	BREQ end_move_right
		RCALL screen_clear
		RCALL draw_boards
		INC R22
		ST Z, R22
		RCALL draw_cursor
	end_move_right:
	RET


place_ship:
	
	RET