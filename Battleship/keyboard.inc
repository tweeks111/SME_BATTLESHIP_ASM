/*
 * keyboard.inc
 *
 *   Authors: Mathieu Philipart
 *			  Théo    Lepoutte
 */ 


// MACROS

// FUNCTIONS

main_keyboard:
	; 2-step method
	step1:
		LDI R23, 0x00	; Columns register
		LDI R18, 0x00	; Temp register

		; Row -> LOW
		CBI PORTD, 4
		CBI PORTD, 5
		CBI PORTD, 6
		CBI PORTD, 7
		; Row -> output
		SBI DDRD, 4
		SBI DDRD, 5
		SBI DDRD, 6
		SBI DDRD, 7
		; Columns -> enable pull-up
		SBI PORTD, 0
		SBI PORTD, 1
		SBI PORTD, 2
		SBI PORTD, 3
		; Columns -> input
		CBI DDRD, 0
		CBI DDRD, 1
		CBI DDRD, 2
		CBI DDRD, 3

		SBIS PIND, 0 ; Skip if no button pressed on C3
		RCALL Column3

		SBIS PIND, 1 ; Skip if no button pressed on C2
		RCALL Column2

		SBIS PIND, 2 ; Skip if no button pressed on C1
		RCALL Column1

		SBIS PIND, 3 ; Skip if no button pressed on C0
		RCALL Column0

		CPI R23, 0
		BREQ NoButtonPressed
		RJMP step2

		Column0:
			LDI R18, 0b00000001
			ADD R23, R18
			RET
		Column1: 
			LDI R18, 0b00000010
			ADD R23, R18
			RET
		Column2:
			LDI R18, 0b00000100
			ADD R23, R18
			RET
		Column3:
			LDI R18, 0b00001000
			ADD R23, R18
			RET

	NoButtonPressed:
		RJMP end_main_keyboard

	step2:
		; Columns -> LOW
		CBI PORTD, 0
		CBI PORTD, 1
		CBI PORTD, 2
		CBI PORTD, 3
		; Columns -> output
		SBI DDRD, 0
		SBI DDRD, 1
		SBI DDRD, 2
		SBI DDRD, 3
		; Row -> enable pull-up
		SBI PORTD, 4
		SBI PORTD, 5
		SBI PORTD, 6
		SBI PORTD, 7
		; Row -> input
		CBI DDRD, 4
		CBI DDRD, 5
		CBI DDRD, 6
		CBI DDRD, 7

		SBIS PIND, 4 ; Skip if no button pressed on R3
		RCALL Row3

		SBIS PIND, 5 ; Skip if no button pressed on R2
		RCALL Row2

		SBIS PIND, 6 ; Skip if no button pressed on R1
		RCALL Row1

		SBIS PIND, 7 ; Skip if no button pressed on R0
		RCALL Row0

		RJMP test

		Row0:
			LDI R18, 0b00010000
			ADD R23, R18
			RET
		Row1: 
			LDI R18, 0b00100000
			ADD R23, R18
			RET
		Row2:
			LDI R18, 0b01000000
			ADD R23, R18
			RET
		Row3:
			LDI R18, 0b10000000
			ADD R23, R18
			RET

	test:
		CheckC0:
			SBRS R23, 0
			RJMP CheckC1
			C0High:
				SBRC R23, 4
				RCALL Btn7
				SBRC R23, 5
				RCALL Btn4
				SBRC R23, 6
				RCALL Btn1
				SBRC R23, 7
				RCALL BtnA
		CheckC1:
			SBRS R23, 1
			RJMP CheckC2
			C1High:
				SBRC R23, 4
				RCALL Btn8
				SBRC R23, 5
				RCALL Btn5
				SBRC R23, 6
				RCALL Btn2
				SBRC R23, 7
				RCALL Btn0
		CheckC2:
			SBRS R23, 2
			RJMP CheckC3
			C2High:
				SBRC R23, 4
				RCALL Btn9
				SBRC R23, 5
				RCALL Btn6
				SBRC R23, 6
				RCALL Btn3
				SBRC R23, 7
				RCALL BtnB
		CheckC3:
			SBRS R23, 3
			RJMP end_main_keyboard
			C3High:
				SBRC R23, 4
				RCALL BtnF
				SBRC R23, 5
				RCALL BtnE
				SBRC R23, 6
				RCALL BtnD
				SBRC R23, 7
				RCALL BtnC

		end_main_keyboard:
			sleep_ts 1
			RET

Btn0:
	RET
Btn1:
	RET
Btn2:
	
	RET
Btn3:
	RET
Btn4:
	RCALL move_left
	RET
Btn5:
	RCALL move_down
	RET
Btn6:
	RCALL move_right
	RET
Btn7:
	RCALL rotate_ship
	RET
Btn8:
	RCALL move_up
	RET
Btn9:
	RCALL place_ship
	sleep_ts 5
	RET
BtnA:
	RET
BtnB:
	RET
BtnC:
	RET
BtnD:
	RET
BtnE:
	RET
BtnF:
	RET