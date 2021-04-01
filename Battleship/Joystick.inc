;
; Joystick.inc
;
; This file is part of the Battleship project.
;
; This file contains all the functions needed to use the joystick. 
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

; Joystick State byte address in SRAM
#define JSKSTAT			0x0191

; Joystick State bits in JSKSTAT
#define JSKSTAT_V		0		; Vertical Tilt State
#define JSKSTAT_H		1		; Horizontal Tilt State
#define JSKSTAT_PV		2		; Previous Tilt State
#define JSKSTAT_FR		3		; Fast Repeat State

; Joystick analog value thresholds (0-255)
#define JSTK_UP_THLD			80			; Threshold for slow repeating
#define JSTK_UP_FST_THLD		40			; Threshold for fast repeating
#define JSTK_DOWN_THLD			170
#define JSTK_DOWN_FST_THLD		210
#define JSTK_LEFT_THLD			80
#define JSTK_LEFT_FST_THLD		40
#define JSTK_RIGHT_THLD			170
#define JSTK_RIGHT_FST_THLD		210

joystick_init:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the joystick.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Configure input pin PB2 (Joystick Button)
	CBI DDRB,2			; Pin PB2 is an input
	SBI PORTB,2			; Pull-up on PB2
	
	RET

joystick_adc_avg:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function get the analog value from ADX, where X is the number 
	; of the analog pin, given via R10. The function makes an average on
	; the value over 8 captures. The average value is returned in R11.
	; Note: this function changes: R0, R1, R16, R17, R18
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Prepare ADC to get analog value from ADX
	LDI R16, (1 << REFS0)|(1 << ADLAR)
	OR R16, R10			; OR with R10 to select the analog pin number
	STS ADMUX, R16
	
	; Start ADC conversion
	LDI R16, (1 << ADEN)|(1 << ADSC)|(1 << ADPS1)
	STS ADCSRA, R16

	; Prepare averaging registers
	CLR R0				; Averaging register LOW
	CLR R1				; Averaging register HIGH

	; Register to contains 0
	CLR R17

	; Prepare average loop counter
	LDI R18, 8
	jaa_avg_loop:
		; Wait for conversion finished
		jaa_wait_loop:
			LDS R16, ADCSRA
			SBRS R16, ADIF
			RJMP jaa_wait_loop
		
		; Reset ADC Interrupt flag and start a new ADC conversion
		LDI R16, (1 << ADEN)|(1 << ADSC)|(1 << ADIF)|(1 << ADPS1)
		STS ADCSRA, R16
		
		; Add the current ADC value to R0 and R1 (16-bit addition)
		LDS R16, ADCH
		ADD R0, R16				; Add to LOW byte
		ADC R1, R17				; Report previous addition carry on HIGH byte

		; Decrement average loop counter
		DEC R18
		BRNE jaa_avg_loop
		
	; Reset ADC Interrupt flag and stop ADC conversion
	LDI R16, (1 << ADIF)|(1 << ADPS1)
	STS ADCSRA, R16
	
	; Once the loop is finished, divide by 8 (16-bit shifts)
	LSR R1			; Divide 16-bit bloc by 2
	ROR R0
	LSR R1			; Divide 16-bit bloc by 2
	ROR R0
	LSR R1			; Divide 16-bit bloc by 2
	ROR R0
	
	; Store the result in R11
	MOV R11, R0

	RET

joystick_listen:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function checks if the joystick button is pressed and checks
	; if the joystick is tilted in a direction.
	; Note: this function changes: R0, R1, R16, R17, R18
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; First, check if the joystick is pressed
	SBIS PINB, 2
	RJMP JoystickPressed		; If PB2 is low, joystick is pressed

	;
	; We ignore the tilt if the joystick is pressed !
	;

	JoystickNotPressed:
		; If the Joystick was not pressed, check if it is tilted in a direction
		
		;
		; Check vertical tilt (AD1)
		;

		; Get averaged analog value from AD1
		LDI R16, 1
		MOV R10, R16
		RCALL joystick_adc_avg
		
		; Move the returned value into R16
		MOV R16, R11

		; Compare the returned value to thresholds
		CPI R16, JSTK_DOWN_FST_THLD
		BRSH jnp_Y_down_fast
		CPI R16, JSTK_DOWN_THLD
		BRSH jnp_Y_down
		CPI R16, JSTK_UP_FST_THLD
		BRLO jnp_Y_up_fast
		CPI R16, JSTK_UP_THLD
		BRLO jnp_Y_up
			RJMP jnp_Y_no_tilt

		jnp_Y_up_fast:
			; Set the Fast Repeat bit in JSKSTAT
			LDS R16, JSKSTAT
			SBR R16, (1 << JSKSTAT_FR)
			STS JSKSTAT, R16
		jnp_Y_up:
			; Joystick Up
			RCALL JoystickUp
			RJMP jnp_Y_tilt

		jnp_Y_down_fast:
			; Set the Fast Repeat bit in JSKSTAT
			LDS R16, JSKSTAT
			SBR R16, (1 << JSKSTAT_FR)
			STS JSKSTAT, R16
		jnp_Y_down:
			; Joystick Down
			RCALL JoystickDown
			RJMP jnp_Y_tilt
			
		jnp_Y_no_tilt:
			; Joystick not Up or Down
			
			; Clear the Joystick Tilted Vertical bit in JSKSTAT
			LDS R16, JSKSTAT
			CBR R16, (1 << JSKSTAT_V)
			STS JSKSTAT, R16
			
			; Skip jnp_Y_tilt section
			RJMP jnp_Y_end
			
		jnp_Y_tilt:
			; Joystick is Up or Down
			
			; Set the Joystick Tilted Vertical bit in JSKSTAT
			LDS R16, JSKSTAT
			SBR R16, (1 << JSKSTAT_V)
			STS JSKSTAT, R16
			
		jnp_Y_end:
	
		;
		; Check horizontal tilt (AD0)
		;
		
		; Get averaged analog value from AD0
		LDI R16, 0
		MOV R10, R16
		RCALL joystick_adc_avg
		
		; Move the returned value into R16
		MOV R16, R11
		
		; Compare the returned value to thresholds
		CPI R16, JSTK_RIGHT_FST_THLD
		BRSH jnp_X_right_fast
		CPI R16, JSTK_RIGHT_THLD
		BRSH jnp_X_right
		CPI R16, JSTK_LEFT_FST_THLD
		BRLO jnp_X_left_fast
		CPI R16, JSTK_LEFT_THLD
		BRLO jnp_X_left
			RJMP jnp_X_no_tilt

		jnp_X_left_fast:
			; Set the Fast Repeat bit in JSKSTAT
			LDS R16, JSKSTAT
			SBR R16, (1 << JSKSTAT_FR)
			STS JSKSTAT, R16			
		jnp_X_left:
			; Joystick Left
			RCALL JoystickLeft
			RJMP jnp_X_tilt

		jnp_X_right_fast:
			; Set the Fast Repeat bit in JSKSTAT
			LDS R16, JSKSTAT
			SBR R16, (1 << JSKSTAT_FR)
			STS JSKSTAT, R16			
		jnp_X_right:
			; Joystick Right
			RCALL JoystickRight
			RJMP jnp_X_tilt
			
		jnp_X_no_tilt:
			; Joystick not Left or Right
			
			; Clear the Joystick Tilted Horizontal bit in JSKSTAT
			LDS R16, JSKSTAT
			CBR R16, (1 << JSKSTAT_H)
			STS JSKSTAT, R16

			; Check if the Joystick was not tilted H and V -> JoystickNoTiltEnd
			SBRS R16, JSKSTAT_V
			RJMP JoystickNoTiltEnd

			; Skip jnp_X_tilt section
			RJMP jnp_X_end
			
		jnp_X_tilt:
			; Joystick is Up or Down
			
			; Set the Joystick Tilted Horizontal bit in JSKSTAT
			LDS R16, JSKSTAT
			SBR R16, (1 << JSKSTAT_H)
			STS JSKSTAT, R16
			
		jnp_X_end:

	JoystickTiltEnd:
		; At the end, if the joystick was tilted in at least one direction
		
		; Check if the joystick was previously tilt
		LDS R16, JSKSTAT
		SBRC R16, JSKSTAT_PV
			RJMP jte_previous_tilted

		jte_previous_not_tilted:
		; If the joystick was not tilted previously
			
			; Sleep long (avoid double-action)
			sleep_ts 3

			; Set the Joystick Previous Tilt State bit in JSKSTAT
			LDS R16, JSKSTAT
			SBR R16, (1 << JSKSTAT_PV)
			STS JSKSTAT, R16

			; Skip jte_previous_tilted section
			RJMP jte_end

		jte_previous_tilted:
		; If the joystick was tilted previously
			
			; Check if fast repeat bit is set
			SBRC R16, JSKSTAT_FR
				RJMP jte_fast_repeat		; Skip the next delay if fast repeat

			; Sleep short (added to the next one if not fast repeat)
			sleep_ts 1

			jte_fast_repeat:
			; Sleep short (repeated action mode)
			sleep_ts 1

			; Clear the Joystick Fast Repeat bit in JSKSTAT
			LDS R16, JSKSTAT
			CBR R16, (1 << JSKSTAT_FR)
			STS JSKSTAT, R16

		jte_end:
		
		RET

	JoystickNoTiltEnd:
		; At the end, if the joystick was not tilted in any direction

		; Clear the Joystick Previous Tilt State bit in JSKSTAT
		LDS R16, JSKSTAT
		CBR R16, (1 << JSKSTAT_PV)
		STS JSKSTAT, R16
				
		RET

	JoystickPressedEnd:
		; At the end, if the joystick was pressed

		; Delay after the pressed action (avoid double-press)
		sleep_ts 4

		RET

	
	
JoystickPressed:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)

	SBRC R24, GS_INIT				; Skip next if game state is not GS_INIT
		RCALL player_select_validate; Validate selected player

	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL place_ship

	SBRC R24, GS_MAIN_GAME			; Skip next if game state is not GS_MAIN_GAME
		RCALL game_fire

	RJMP JoystickPressedEnd

JoystickUp:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)

	SBRC R24, GS_INIT				; Skip next if game state is not GS_INIT
		RCALL player_select_1		; Select Player 1 (top)

	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL cursor_move_up

	SBRC R24, GS_MAIN_GAME			; Skip next if game state is not GS_MAIN_GAME
		RCALL cursor_move_up

	RET

JoystickDown:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)

	SBRC R24, GS_INIT				; Skip next if game state is not GS_INIT
		RCALL player_select_2		; Select Player 2 (bottom)

	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL cursor_move_down

	SBRC R24, GS_MAIN_GAME			; Skip next if game state is not GS_MAIN_GAME
		RCALL cursor_move_down

	RET

JoystickLeft:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)
	
	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL cursor_move_left

	SBRC R24, GS_MAIN_GAME			; Skip next if game state is not GS_MAIN_GAME
		RCALL cursor_move_left

	RET

JoystickRight:
	game_get_state					; Get the current game state (stored in R10)
	MOV R24, R10					; Copy R10 into R24 (R24 SHOULD NOT BE MODIFIED IN ACTION FUNCTIONS!)

	SBRC R24, GS_SHIPS_PLACEMENT	; Skip next if game state is not GS_SHIPS_PLACEMENT
		RCALL cursor_move_right

	SBRC R24, GS_MAIN_GAME			; Skip next if game state is not GS_MAIN_GAME
		RCALL cursor_move_right

	RET