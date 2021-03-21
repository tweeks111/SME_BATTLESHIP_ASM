/*
 * timer.inc
 *
 *   Authors: Mathieu Philipart
 *			  Théo    Lepoutte
 */ 

 #define TCNT1_INIT 40536	; 65536 - 25000 because we are working in overflow

 .macro sleep_ts
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro suspend the process for the time given
	; in argument (integer number of tenths of a second).
	; The maximum sleep duration is 25.5 seconds.
	; NOTE: this macro uses the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	// Reset the interrupt counter
	LDI R16, high(TCNT1_INIT)
	STS TCNT1H,R16
	LDI R16, low(TCNT1_INIT)
	STS TCNT1L,R16

	// Prepare the time counter
	LDI R16, @0		; Time in tenths of a second (integer number)
	MOV R7, R16

	// Prepare R16 to compare R2 with 1
	LDI R16,1
	m_ss_loop:
		CP R7,R16		; The sleep counter R7 is decremented by Timer1 interrupt
		BRSH m_ss_loop	; Loop if R7 >= R16
.endmacro



init_timer0:
 	LDI R16, 0x04	
	OUT TCCR0B, R16 ; Setting 1024 prescaler	
		
	LDI R16, 167	; 256 - 89 because we are working in overflow
	MOV R2, R16
	OUT TCNT0, R2	; Setting init value of interrupt counter

	LDI R16, 0x01
	STS TIMSK0, R16	; Enable Overflow interrupt
	RET

init_timer1:
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

Timer0OverflowInterrupt:
	OUT TCNT0,R2			; Set initial counter value
	
	; Reset the falling edge
	SBI PORTB,4

	; Decrement the Row counter
	DEC R8

	; If R8 != 0 -> Don't reset the Row counter
	BRNE skip_row_cnt_rst
		; Reset the Row counter to 7
		LDI R25,7
		MOV R8,R25

		; Reset the Row selector to the last row (refreshing from bottom to top)
		LDI R25,0b01000000
		MOV R9,R25

		; Place the X pointer to address 0x0146 (0x0100 + 70 => refreshing from end to start)
		LDI XL,0x46
		LDI XH,0x01
	
	skip_row_cnt_rst:

	LDI R25,10
	MOV R7,R25				; Register loop counter (#shift registers)
	reg_loop:
		; Get the data for this shift register from the RAM
		LD R5,-X				; Decrement X and read the byte at address X

		LDI R25,8
		MOV R6,R25			; Shift loop counter (8 shifts per register)
		shift_loop:
			; Put data into shift registers
			CBI PORTB,3		; Set PB3 to 0 initially
			ROL R5
			BRCC next_clk	; Let PB3 to 0 (skip next instruction) if C==0
			SBI PORTB,3

			next_clk:		; Send a clock signal
			CBI PORTB,5
			SBI PORTB,5

			DEC R6
			BRNE shift_loop

		DEC R7
		BRNE reg_loop

	; Send the row select data
	LDI R25,8
	MOV R6,R25			; Shift loop counter (8 shifts per register)
	CLC					; Ensure the C flag is cleared
	shift_loop_rs:
		; Put data into shift registers
		CBI PORTB,3		; Set PB3 to 0 initially
		ROL R9
		BRCC nxt_clk_rs ; Let PB3 to 0 (skip next instruction) if C==0
		SBI PORTB,3

		nxt_clk_rs:		; Send a clock signal
		CBI PORTB,5
		SBI PORTB,5

		DEC R6
		BRNE shift_loop_rs

	; After row written -> enable latch output
	CBI PORTB,4			; Send falling edge to registers

	RETI

Timer1OverflowInterrupt:
	; Reset initial timer counter value to 40536 to have 10Hz (16-bit counter)
	LDI R25, high(TCNT1_INIT)
	STS TCNT1H,R25
	LDI R25, low(TCNT1_INIT)
	STS TCNT1L,R25

	; Decrement the sleep counter
	DEC  R7

	RETI