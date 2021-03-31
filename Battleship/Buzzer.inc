;
; Buzzer.inc
;
; This file is part of the Battleship project.
;
; It contains all the functions needed to play tones and 
; sounds, and the sounds tables.
;
; Authors: Mathieu Philippart & Th�o Lepoutte
;

#define SNDCNT_D	0x01C0
#define SNDCNT_ZH	0x01C1
#define SNDCNT_ZL	0x01C2


/*
 * The two next macros are commented since we use asynchronous sound playback
 *
.macro buzzer_tone
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro plays a frequency given in argument 0 with
	; the buzzer and suspends the process for the time given
	; in argument 1 (integer number of tenths of a second).
	; The maximum tone duration is 25.5 seconds.
	; NOTE: this macro modifies the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Reset the interrupt counter (counter given in argument 0)
	LDI R16, @0				; Copy tone frequency in R10
	MOV R10, R16
	LDI R16, @1				; Copy tone duration in R11
	MOV R11, R16
	RCALL buzzer_tone_reg
.endmacro

.macro buzzer_sound
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro plays a sound from the sound table given in 
	; argument 0. The process is suspended while the sound
	; is playing (synchronous playing).
	; NOTE: this macro modifies the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set the Z-pointer to the sound table address
	LDI ZH, high(@0 << 1)
	LDI ZL, low(@0 << 1)

	RCALL buzzer_sound_reg
.endmacro
*/

.macro buzzer_sound_async
	; Set Duration counter to 1 (= get the next tone)
	LDI R16, 1
	STS SNDCNT_D,R16
	
	; Write the sound table address in SRAM
	LDI R16, high(@0 << 1)
	STS SNDCNT_ZH,R16
	LDI R16, low(@0 << 1)
	STS SNDCNT_ZL,R16
.endmacro


buzzer_init:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the buzzer pin as an 
	; output and the switch pin as an input, and configures
	; the Timer2's prescaler.
	; NOTE: this function modifies the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; IN/OUT PIN CONFIGURATION ;
	; Configure input pin PB0 (Switch)
	CBI DDRB,0				; Pin PB0 is an input
	SBI PORTB,0				; Pull-up

	; Configure output pin PB1 (BUZZER)
	SBI DDRB,1				; Pin PB1 is an output
	CBI PORTB,1				; Output Gnd => Off

	; TIMER2 + INTERRUPTS CONFIGURATION ;
	; Configure prescaler
	LDI R16, (1<<CS22)|(1<<CS21)
	STS TCCR2B,R16			; Set prescaler to 64 (f_tmr = 16e6/64 = 250000 Hz)

	; INITIAL SRAM DATA ;

	; Set Duration counter to 0 (= no sound to play)
	CLR R16
	STS SNDCNT_D, R16

	RET
	

Timer2OverflowInterrupt:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This ISR toggles the buzzer pin at the frequency
	; configured in R8. If the switch is in position LOW,
	; the buzzer stay at position OFF.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Reset initial timer counter value to R8
	STS TCNT2,R8
	
	; Store status register on the stack	
	IN R25,SREG
	PUSH R25

	SBIS PINB,0				; If the switch PB0 is off -> jump to t2oi_buzzer_off
	RJMP t2oi_buzzer_off
		; Toggle PB1 (buzzer)
		SBI PINB,1
		RJMP t2oi_end

	t2oi_buzzer_off:
		; Set the buzzer pin to Gnd
		CBI PORTB,1

	t2oi_end:
	
	; Restore status register from the stack	
	POP R25
	OUT SREG, R25

	RETI

	
SoundsInterruptRoutine:
	; This routine runs at 10 Hz
	
	; Get the Duration counter value
	LDS R25, SNDCNT_D

	; Check and skip if there is no sound to play
	CPI R25,0
	BREQ sir_no_sound_to_play

	; Check and skip if the tone doesn't need to change
	CPI R25,1
	BRNE sir_dec_duration

	; Store the Z-pointer on the stack (because we are in an interrupt)
	PUSH ZH
	PUSH ZL
	
	; Get the Z-pointer position from the SRAM (current sound reading position)
	LDS R25, SNDCNT_ZH
	MOV ZH,R25
	LDS R25, SNDCNT_ZL
	MOV ZL,R25

	; Get the next tone and copy the frequency value in R8 (Timer2 counter)
	LPM R25,Z+
	MOV R8, R25
	STS TCNT2,R25				; Apply to Timer2
	
	; Get duration of next tone
	LPM R25,Z+
	
	; Save the sound status in SRAM
	STS SNDCNT_D, R25
	STS SNDCNT_ZH, ZH
	STS SNDCNT_ZL, ZL

	; Restore the Z-pointer from the stack
	POP ZL
	POP ZH

	; If the next duration is 0 -> end of the sound
	CPI R25,0
	BREQ sir_no_sound_to_play

	; If the next tone is a silence -> disable buzzer for the duration
	MOV R25, R8
	CPI R25, _SHUSH
	BREQ sir_no_sound_to_play

	; Enable the Timer2
	LDI R25,0x01
	STS TIMSK2,R25				; Enable Overflow interrupt of Timer2
	
	RJMP sir_end_routine

	sir_dec_duration:
	; Decrement the duration
	DEC R25
	STS SNDCNT_D, R25

	RJMP sir_end_routine

	sir_no_sound_to_play:
	; Disable the Timer2
	LDI R25,0x00
	STS TIMSK2,R25				; Disable Overflow interrupt of Timer2

	; Ensure the buzzer if connected to Gnd
	CBI PORTB,1

	sir_end_routine:
	RET

/*
 * The two next functions are not used anymore (we use asynchonous sound playing instead)
 *
buzzer_tone_reg:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function plays a frequency given in R10 with
	; the buzzer and suspends the process for the time
	; given in R11 (integer number of tenths of a second).
	; The maximum tone duration is 25.5 seconds.
	; NOTE: this function modifies the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Reset the interrupt counter (counter given in R10)
	MOV R8, R10					; Store the Timer2 counter in R8
	STS TCNT2,R10				; Apply to Timer2
	
	; Enable the Timer2
	LDI R16,0x01
	STS TIMSK2,R16				; Enable Overflow interrupt of Timer2

	; Sleep during the time given in R11
	MOV R16, R11				; The function sleep_ts_reg uses R16 as argument
	RCALL sleep_ts_reg
	
	; Disable the Timer2
	LDI R16,0x00
	STS TIMSK2,R16				; Disable Overflow interrupt of Timer2

	; Ensure the buzzer if connected to Gnd
	CBI PORTB,1

	RET


buzzer_sound_reg:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function plays a sound stored in the program 
	; memory. The Z-pointer must be set previously to the
	; address of a sound table in the program memory.
	; The process is suspended while playing the sound.
	; NOTE: this function modifies the registers R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	bsr_sound_loop:
		; The first byte is the tone 'frequency'
		LPM R16, Z+	
		MOV R10, R16		; The function buzzer_tone_reg uses R10 as tone 'frequency'

		; The second byte is the tone duration
		LPM R16, Z+	
		MOV R11, R16		; The function buzzer_tone_reg uses R11 as tone duration

		; If the tone duration is 0 -> we reached the end of the sound
		CPI R16,0
		BREQ bsr_sound_end

		; Interpret the _SHUSH as a simple delay (no tone frequency)
		LDI R16, _SHUSH
		CP R10,R16					; If tone is _SHUSH...
		BRNE bsr_not_silence
			MOV R16,R11				; The function sleep_ts_reg uses R16 as the delay
			RCALL sleep_ts_reg
			RJMP bsr_sound_loop

		bsr_not_silence:	; If tone is not _SHUSH

		; Play the tone
		RCALL buzzer_tone_reg

		; The loop ends when a tone of duration 0 has been reached
		RJMP bsr_sound_loop

	bsr_sound_end:
	RET
*/

	