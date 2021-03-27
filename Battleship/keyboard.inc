;
; Keyboard.inc
;
; This file is part of the Battleship project.
;
; This file contains all the functions needed to use the keyboard. 
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

#define KEYBOARD_STATE_ADDR 0x0190

// MACROS

// FUNCTIONS

keyboard_init:
	; Place the Y-pointer on the "keyboard pressed state" byte
	LDI YH, high(KEYBOARD_STATE_ADDR)
	LDI YL, low(KEYBOARD_STATE_ADDR)
		
	; Initialize the keyboard pressed state to 0
	CLR R16
	ST Y,R16

	RET

keyboard_listen:
	; 2-step method
	step1:
		; Configure rows as output pins
		CBI PORTD,4			; Set PD4 to LOW state
		SBI DDRD,4			; Pin PD4 is an output
		CBI PORTD,5			; Set PD5 to LOW state
		SBI DDRD,5			; Pin PD5 is an output
		CBI PORTD,6			; Set PD6 to LOW state
		SBI DDRD,6			; Pin PD6 is an output
		CBI PORTD,7			; Set PD7 to LOW state
		SBI DDRD,7			; Pin PD7 is an output

		; Configure columns as input pins with pull-up
		SBI PORTD,0			; Enable the pull-up resistor
		CBI DDRD,0			; Pin PD0 is an input
		SBI PORTD,1			; Enable the pull-up resistor
		CBI DDRD,1			; Pin PD1 is an input
		SBI PORTD,2			; Enable the pull-up resistor
		CBI DDRD,2			; Pin PD2 is an input
		SBI PORTD,3 		; Enable the pull-up resistor
		CBI DDRD,3			; Pin PD3 is an input

		; Find the pressed column
		SBIS PIND,0			; Skip the next instruction if no button is pressed in this column
		RJMP C4Pressed		; Jump to corresponding section if a button is pressed
		SBIS PIND,1
		RJMP C3Pressed
		SBIS PIND,2
		RJMP C2Pressed
		SBIS PIND,3
		RJMP C1Pressed

		; Else: nothing pressed
		RJMP NothingPressed
		
	C4Pressed:
		LDI R16,0b00001000
		RJMP step2

	C3Pressed:
		LDI R16,0b00000100
		RJMP step2

	C2Pressed:
		LDI R16,0b00000010
		RJMP step2

	C1Pressed:
		LDI R16,0b00000001
		RJMP step2

	
	step2:
		; Configure columns as output pins
		CBI PORTD,0			; Set PD0 to LOW state
		SBI DDRD,0			; Pin PD0 is an output
		CBI PORTD,1			; Set PD1 to LOW state
		SBI DDRD,1			; Pin PD1 is an output
		CBI PORTD,2			; Set PD2 to LOW state
		SBI DDRD,2			; Pin PD2 is an output
		CBI PORTD,3			; Set PD3 to LOW state
		SBI DDRD,3			; Pin PD3 is an output

		; Configure rows as input pins with pull-up
		SBI PORTD,4			; Enable the pull-up resistor
		CBI DDRD,4			; Pin PD4 is an input
		SBI PORTD,5			; Enable the pull-up resistor
		CBI DDRD,5			; Pin PD5 is an input
		SBI PORTD,6			; Enable the pull-up resistor
		CBI DDRD,6			; Pin PD6 is an input
		SBI PORTD,7 		; Enable the pull-up resistor
		CBI DDRD,7			; Pin PD7 is an input

		; Find the pressed row
		SBIS PIND,4			; Skip the next instruction if no button is pressed in this row
		RJMP R1Pressed		; Jump to corresponding section if a button is pressed
		SBIS PIND,5
		RJMP R2Pressed
		SBIS PIND,6
		RJMP R3Pressed
		SBIS PIND,7
		RJMP R4Pressed

		; Else: nothing pressed
		RJMP NothingPressed
		
	R1Pressed:
		; If the pressed column was the 1st => Go to KeyAPressed
		SBRC R16,0
		RJMP KeyAPressed
		SBRC R16,1
		RJMP Key0Pressed
		SBRC R16,2
		RJMP KeyBPressed
		SBRC R16,3
		RJMP KeyCPressed

		RJMP NothingPressed

	R2Pressed:
		; If the pressed column was the 1st => Go to Key1Pressed
		SBRC R16,0
		RJMP Key1Pressed
		SBRC R16,1
		RJMP Key2Pressed
		SBRC R16,2
		RJMP Key3Pressed
		SBRC R16,3
		RJMP KeyDPressed

		RJMP NothingPressed

	R3Pressed:
		; If the pressed column was the 1st => Go to Key4Pressed
		SBRC R16,0
		RJMP Key4Pressed
		SBRC R16,1
		RJMP Key5Pressed
		SBRC R16,2
		RJMP Key6Pressed
		SBRC R16,3
		RJMP KeyEPressed

		RJMP NothingPressed

	R4Pressed:
		; If the pressed column was the 1st => Go to Key7Pressed
		SBRC R16,0
		RJMP Key7Pressed
		SBRC R16,1
		RJMP Key8Pressed
		SBRC R16,2
		RJMP Key9Pressed
		SBRC R16,3
		RJMP KeyFPressed

		RJMP NothingPressed

	NothingPressed:
		; If nothing pressed

		; Reset the keyboard pressed status to 0
		LDI YH, high(KEYBOARD_STATE_ADDR)
		LDI YL, low(KEYBOARD_STATE_ADDR)
		CLR R17
		ST Y,R17

		RET

	KeyboardPressedEnd:
		; At the end of a keyboard pressed action
		LDI YH, high(KEYBOARD_STATE_ADDR)
		LDI YL, low(KEYBOARD_STATE_ADDR)
		LD R17,Y							; Get the current keyboard status from SRAM
		CPI R17,1
		BRNE kpe_long_sleep
			sleep_ts 1
			RJMP kpe_end

		kpe_long_sleep:
			sleep_ts 2
			
		kpe_end:
		
		; If the keyboard was previously released -> mark it as pressed now
		LDI R17,1
		ST Y,R17

		RET


Key0Pressed:
	RJMP KeyboardPressedEnd

Key1Pressed:
	RJMP KeyboardPressedEnd

Key2Pressed:
	RJMP KeyboardPressedEnd

Key3Pressed:
	RJMP KeyboardPressedEnd

Key4Pressed:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)
	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL cursor_move_left
	RJMP KeyboardPressedEnd

Key5Pressed:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)
	SBRC R24, GS_INIT				; Skip next if game state is not GS_INIT
		RCALL player_select_2		; Select Player 2 (bottom)
	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL cursor_move_down
	RJMP KeyboardPressedEnd

Key6Pressed:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)
	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL cursor_move_right
	RJMP KeyboardPressedEnd

Key7Pressed:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)
	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL cursor_rotate
	RJMP KeyboardPressedEnd

Key8Pressed:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)
	SBRC R24, GS_INIT				; Skip next if game state is not GS_INIT
		RCALL player_select_1		; Select Player 1 (top)
	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL cursor_move_up
	RJMP KeyboardPressedEnd

Key9Pressed:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)
	SBRC R24, GS_INIT				; Skip next if game state is not GS_INIT
		RCALL player_select_validate; Validate selected player
	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL place_ship
	RJMP KeyboardPressedEnd

KeyAPressed:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)
	SBRC R24, GS_INIT				; Skip next if game state is not GS_INIT
		JMP init_ships_placement	; Go to Ships Placement if A is pressed to skip I2C discovery (for development purpose)
	RJMP KeyboardPressedEnd

KeyBPressed:
	RJMP KeyboardPressedEnd

KeyCPressed:
	; Reset the game if C is pressed
	JMP init
	RJMP KeyboardPressedEnd

KeyDPressed:
	RJMP KeyboardPressedEnd

KeyEPressed:
	RJMP KeyboardPressedEnd

KeyFPressed:
	RJMP KeyboardPressedEnd