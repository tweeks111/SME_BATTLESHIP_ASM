;
; Sleep.inc
;
; This file is part of the Battleship project.
;
; It contains all the functions needed to generate a sleep (C-like) function.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

.macro sleep_ts
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro suspends the process for the time given
	; in argument (integer number of tenths of a second).
	; The maximum sleep duration is 25.6 seconds.
	; NOTE: this macro modifies the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	LDI R16, @0				; The function sleep_ts_reg uses R16
	RCALL sleep_ts_reg
.endmacro
		

SleepInterruptRoutine:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function is called within an ISR. It decrement
	; the sleep counter register.
	; NOTE: this function modifies the register R25.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Don't decrement R7 if it is 0
	LDI R25,0
	CPSE R7,R25		; Compare R7 to 0 and skip the DEC if R7 == 0

	; Decrement the sleep counter
	DEC  R7

	RET


sleep_ts_reg:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function suspends the process for the time 
	; given in R16 (integer number of tenths of a second).
	; The maximum sleep duration is 25.6 seconds.
	; NOTE: this function modifies the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	// Prepare the time counter
	MOV R7, R16				; Time in tenths of a second (integer number)

	// Reset the interrupt counter
	LDI R16, high(TCNT1_INIT)
	STS TCNT1H,R16
	LDI R16, low(TCNT1_INIT)
	STS TCNT1L,R16
	
	// Prepare R16 to compare R2 with 1 (exit loop if R7 == 0)
	LDI R16,1
	stsr_loop:
		CP R7,R16			; The sleep counter R7 is decremented by Timer1 interrupt
		BRSH stsr_loop		; Loop if R7 >= R16
		
	RET