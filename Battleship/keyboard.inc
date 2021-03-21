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
		LDI R16, 0x00	; Columns register
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

		LDI R18, 0x00
		CP R16, R18
		BREQ NoButtonPressed
		RJMP step2

		Column0:
			LDI R18, 0b00000001
			ADD R16, R18
			RET
		Column1: 
			LDI R18, 0b00000010
			ADD R16, R18
			RET
		Column2:
			LDI R18, 0b00000100
			ADD R16, R18
			RET
		Column3:
			LDI R18, 0b00001000
			ADD R16, R18
			RET

	NoButtonPressed:
		RJMP main_keyboard

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
			ADD R16, R18
			RET
		Row1: 
			LDI R18, 0b00100000
			ADD R16, R18
			RET
		Row2:
			LDI R18, 0b01000000
			ADD R16, R18
			RET
		Row3:
			LDI R18, 0b10000000
			ADD R16, R18
			RET

	test:
		CheckC0:
			SBRS R16, 0
			RJMP CheckC1
			C0High:
				SBRC R16, 4
				RCALL Btn7
				SBRC R16, 5
				RCALL Btn8
				SBRC R16, 6
				RCALL Btn9
				SBRC R16, 7
				RCALL BtnF
		CheckC1:
			SBRS R16, 1
			RJMP CheckC2
			C1High:
				SBRC R16, 4
				RCALL Btn4
				SBRC R16, 5
				RCALL Btn5
				SBRC R16, 6
				RCALL Btn6
				SBRC R16, 7
				RCALL BtnE
		CheckC2:
			SBRS R16, 2
			RJMP CheckC3
			C2High:
				SBRC R16, 4
				RCALL Btn1
				SBRC R16, 5
				RCALL Btn2
				SBRC R16, 6
				RCALL Btn3
				SBRC R16, 7
				RCALL BtnD
		CheckC3:
			SBRS R16, 3
			RJMP main_keyboard
			C3High:
				SBRC R16, 4
				RCALL BtnA
				SBRC R16, 5
				RCALL Btn0
				SBRC R16, 6
				RCALL BtnB
				SBRC R16, 7
				RCALL BtnC
		RET



Btn0:
	;RCALL BuzzerOn
	RET
Btn1:
	;RCALL BuzzerOn
	RET
Btn2:
	;RCALL BuzzerOn
	RET
Btn3:
	;RCALL BuzzerOn
	RET
Btn4:
	;RCALL LEDPC2On
	RET
Btn5:
	;RCALL BuzzerOn
	RET
Btn6:
	;RCALL BuzzerOn
	RET
Btn7:
	;RCALL LEDPC2On
	;RCALL LEDPC3On
	RET
Btn8:
	;RCALL LEDPC3On
	RET
Btn9:
	;RCALL BuzzerOn
	RET
BtnA:
	;RCALL BuzzerOn
	RET
BtnB:
	;RCALL BuzzerOn
	RET
BtnC:
	;RCALL BuzzerOn
	RET
BtnD:
	;RCALL BuzzerOn
	RET
BtnE:
	;RCALL BuzzerOn
	RET
BtnF:
	;RCALL BuzzerOn
	RET