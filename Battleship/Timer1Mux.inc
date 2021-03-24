;
; Timer1Mux.inc
;
; This file is part of the Battleship project.
;
; It handles the Timer1 en multiplexes its ISR into multiple
; sub-routines: the Sleep process routine and the Animations
; process routine.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

#define TCNT1_INIT 40536	; 65536 - 25000 because we are working in overflow

timer1_mux_init:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the Timer1 to a constant
	; frequency of 10Hz. This timer is then used to process
	; the sleep_ts function and the animations.
	; NOTE: this function modifies the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; TIMER1 + INTERRUPTS CONFIGURATION ;
	; Configure prescaler
	LDI R16, (1<<CS11)|(1<<CS10)
	STS TCCR1B,R16				; Set prescaler to 64 (f_tmr = 16e6/64 = 250000 Hz)

	; Frequency: 10Hz -> (16e6/64) / 25000 = 10Hz;
	LDI R16, high(TCNT1_INIT)
	STS TCNT1H,R16
	LDI R16, low(TCNT1_INIT)
	STS TCNT1L,R16				; Set initial counter value to 25000 to have 10Hz (16-bit counter)

	LDI R16,0x01
	STS TIMSK1,R16				; Enable Overflow interrupt of Timer1

	RET


Timer1OverflowInterrupt:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This ISR runs at 10 Hz. The process execution is 
	; multiplexed into the sleep ISR and the animations ISR.
	; NOTE: this function modifies the register R25.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Reset initial timer counter value to 40536 to have 10Hz (16-bit counter)
	LDI R25, high(TCNT1_INIT)
	STS TCNT1H,R25
	LDI R25, low(TCNT1_INIT)
	STS TCNT1L,R25
	
	; Store status register on the stack	
	IN R25,SREG
	PUSH R25

	RCALL SleepInterruptRoutine
	RCALL SoundsInterruptRoutine
	RCALL AnimationsInterruptRoutine
	
	; Restore status register from the stack	
	POP R25
	OUT SREG, R25

	RETI