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

; Joystick analog value thresholds (0-255)
#define JSTK_UP_THLD		75
#define JSTK_DOWN_THLD		180
#define JSTK_LEFT_THLD		75
#define JSTK_RIGHT_THLD		180

joystick_init:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the joystick.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Configure input pin PB2 (Joystick Button)
	CBI DDRB,2			; Pin PB2 is an input
	SBI PORTB,2			; Pull-up on PB2
	
	RET


joystick_listen:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function checks if the joystick button is pressed and checks
	; if the joystick is tilted in a direction.
	; Note: this function changes: R16
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
		; Check vertical tilt (AN1)
		;

		; Prepare ADC to get analog value from AN1
		LDI R16, (1 << REFS0)|(1 << ADLAR)|(1 << MUX0)
		STS ADMUX, R16
	
		; Start ADC conversion
		LDI R16, (1 << ADEN)|(1 << ADSC)|(1 << ADPS1)
		STS ADCSRA, R16

		; Wait for conversion finished
		jnp_Y_loop:
			LDS R16, ADCSRA
			SBRS R16, ADIF
			RJMP jnp_Y_loop
		
		; Reset ADC Interrupt flag and stop ADC conversion
		LDI R16, (1 << ADIF)|(1 << ADPS1)
		STS ADCSRA, R16
		
		; Get the upper bits (ADCH) converted and compare to thresholds
		LDS R16, ADCH
		CPI R16, JSTK_DOWN_THLD
		BRSH jnp_Y_down
		CPI R16, JSTK_UP_THLD
		BRLO jnp_Y_up
			RJMP jnp_Y_no_tilt

		jnp_Y_up:
			; Joystick Up
			RCALL JoystickUp
			RJMP jnp_Y_tilt

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
		; Check horizontal tilt (AN0)
		;
		
		; Prepare ADC to get analog value from AN0
		LDI R16, (1 << REFS0)|(1 << ADLAR)
		STS ADMUX, R16

		; Start ADC conversion
		LDI R16, (1 << ADEN)|(1 << ADSC)|(1 << ADPS1)
		STS ADCSRA, R16
	
		; Wait for conversion finished
		jnp_X_loop:
			LDS R16, ADCSRA
			SBRS R16, ADIF
			RJMP jnp_X_loop

		; Reset ADC Interrupt flag and stop ADC conversion
		LDI R16, (1 << ADIF)|(1 << ADPS1)
		STS ADCSRA, R16
		
		; Get the upper bits (ADCH) converted and compare to thresholds
		LDS R16, ADCH
		CPI R16, JSTK_RIGHT_THLD
		BRSH jnp_X_right
		CPI R16, JSTK_LEFT_THLD
		BRLO jnp_X_left
			RJMP jnp_X_no_tilt

		jnp_X_left:
			; Joystick Left
			RCALL JoystickLeft
			RJMP jnp_X_tilt

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
			SBR R16, (1 << JSKSTAT_PV)
			STS JSKSTAT, R16

			; Skip jte_previous_tilted section
			RJMP jte_end

		jte_previous_tilted:
		; If the joystick was tilted previously
			
			; Sleep shot (repeated action mode)
			sleep_ts 1

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