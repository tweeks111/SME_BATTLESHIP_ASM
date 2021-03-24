;
; Screen.inc
;
; This file is part of the Battleship project.
;
; It contains all the functions needed to display dots, lines,
; recangles and characters on the screen.
;
; Authors : Mathieu Philippart & Théo Lepoutte
;

// Characters definitions
#define _A 10
#define _B 11
#define _C 12
#define _D 13
#define _E 14
#define _F 15
#define _G 16
#define _H 17
#define _I 18
#define _J 19
#define _K 20
#define _L 21
#define _M 22
#define _N 23
#define _O 24
#define _P 25
#define _Q 26
#define _R 27
#define _S 28
#define _T 29
#define _U 30
#define _V 31
#define _W 32
#define _X 33
#define _Y 34
#define _Z 35
#define _EXC 36
#define _INT 37
#define _SPACE 38

#define _SMALL_SPACE 0xF0
#define _WORD_END 0xFF

; The frequency of 56kHz for Timer0 results in a whole screen refreshing rate of 500Hz.
; When a pixel has a value of 1 (lowest PWM value), it is displayed at 100Hz.
; This is the lowest frequency needed to remove any twinkling effect for any possible
; pixel brightness value.
#define TCNT0_INIT 220		; Initial counter value for Timer0 (=56 kHz with prescaler 8)

; NOTE: refreshing the screen byte by byte is needed to avoid the Timer0's interrupt 
;		interferring with the others ISRs (mostly the one of the buzzer), since the
;		screen ISR is a very large function.


 // MACROS
.macro draw_char
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro draws a character on the screen at position
	; given in arg 0 (X) and 1 (Y). The character brightness
	; is given arg 2 and the character's index in arg 3.
	; NOTE: this macro modifies: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI R16, @0		; X
	MOV R10, R16	
	LDI R16, @1		; Y
	MOV R11, R16	
	LDI R16, @2		; Brightness
	MOV R12, R16	
	LDI R16, @3		; G
	MOV R13, R16	
	RCALL screen_set_char
.endmacro

.macro draw_rect
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro draws a rectangle on the screen at position
	; given in arg 0 (X) and 1 (Y). The rectangle brightness
	; is given arg 2 and its size in arg 3 (W) and 4 (H).
	; NOTE: this macro modifies: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI R16, @0		; X
	MOV R10, R16	
	LDI R16, @1		; Y
	MOV R11, R16	
	LDI R16, @2		; Brightness
	MOV R12, R16	
	LDI R16, @3		; Width
	MOV R13, R16	
	LDI R16, @4		; Height
	MOV R14, R16	
	RCALL screen_set_rect
.endmacro

.macro draw_line
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro draws a rectangle on the screen at position
	; given in arg 0 (X) and 1 (Y). The rectangle brightness
	; is given arg 2 and its size in arg 3 (W) and 4 (H).
	; NOTE: this macro modifies: R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI R16, @0		; X
	MOV R10, R16	
	LDI R16, @1		; Y
	MOV R11, R16	
	LDI R16, @2		; Brightness
	MOV R12, R16	
	LDI R16, @3		; Width
	MOV R13, R16
	RCALL screen_set_hline
.endmacro

.macro draw_point
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro draws a point on the screen at position
	; given in arg 0 (X) and 1 (Y). The point brightness
	; is given arg 2.
	; NOTE: this macro modifies: R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDI R16, @0		; X
	MOV R10, R16	
	LDI R16, @1		; Y
	MOV R11, R16	
	LDI R16, @2		; Brightness
	MOV R12, R16	
	LDI R16, 1		; Width = 1
	MOV R13, R16
	RCALL screen_set_hline
.endmacro

.macro draw_word
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro draws a word (multiple characters) from the
	; program memory on the screen.
	; Arguments 0 and 1 are the positions (X,Y), argument 2
	; is the brightness and argument 3 is the address of the
	; word in program memory.
	; NOTE: this macro modifies the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Set the Z-pointer to the word address
	LDI ZH, high(@3 << 1)
	LDI ZL, low(@3 << 1)

	LDI R16, @0
	MOV R10,R16
	LDI R16, @1
	MOV R11,R16
	LDI R16, @2
	MOV R12,R16

	RCALL screen_set_word
.endmacro

screen_init:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the Timer0 to a constant
	; frequency of 56kHz and the output pins for the screen.
	; The Timer0 is used to refresh the screen 4 pixels at a
	; time (byte by byte in SRAM).
	; NOTE: this function modifies the register R16.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; SCREEN PINS CONFIGURATION ;
	; Configure output pin PB3 (Data IN)
	SBI DDRB,3			; Pin PB3 is an output
	CBI PORTB,3			; Output Gnd

	; Configure output pin PB4 (LATCH CLK/OUTPUT EN)
	SBI DDRB,4			; Pin PB4 is an output
	CBI PORTB,4			; Output Gnd

	; Configure output pin PB5 (CLK)
	SBI DDRB,5			; Pin PB5 is an output
	CBI PORTB,5			; Output Gnd

	; TIMER0 + INTERRUPTS CONFIGURATION ;
	; Configure prescaler
	LDI R16,0x2
	OUT TCCR0B,R16		; Set prescaler to 8 (f_tmr = 16e6/8 = 2 MHz)

	; Frequency: 56000Hz -> (16e6/8) / 56000 = 35.71;
	LDI R16,TCNT0_INIT	; 256 - 36 because we are working in overflow
	OUT TCNT0,R16		; Set initial counter value to 220 to have 56000Hz

	LDI R16,0x01
	STS TIMSK0,R16		; Enable Overflow interrupt of Timer0
	; NOTE: We use STS for this one because it is in the internal SRAM
	
	; INITIAL REGISTERS DATA FOR SCREEN INTERRUPT ;
	CLR R3				; Screen PWM counter
	CLR R5				; Screen row byte counter
	CLR R6				; Screen Interrupt Row Selector (must be 0 initially)

	; INITIAL SRAM DATA ;
	; Clear the screen
	CLR R10
	RCALL screen_fill

	RET
	

Timer0OverflowInterrupt:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; The Timer0 is used to refresh the screen.
	; This function reads the pixel values from the SRAM and put the 
	; corresponding data into the screen's shift registers.
	; Each call to this interrupt function updates 4 pixels in a row (one 
	; byte in SRAM per timer overflow).
	; The pixels are coded in the SRAM using 2 bits per pixel (PWM 
	; modulation for brightness). The screen data take a total of 140 
	; bytes in the SRAM (560 pixels * 2 bits per pixel).
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	LDI R25, TCNT0_INIT
	OUT TCNT0,R25			; Set initial counter value

	; Store status register on the stack	
	IN R25,SREG
	PUSH R25

	; Check if we are writing a new row
	CLR R25
	CP R5,R25
	BRNE t0oi_skip_cnt_rst
		; Reset the row byte counter to 20
		;  => 2 bits per pixel, 80 pixels per row => 20 bytes per row in SRAM
		LDI R25,20
		MOV R5,R25

		; If the row selector is 0b00000000 -> Reset the Row selector and the Data memory pointer
		CLR R25
		CP R6,R25
		BRNE t0oi_skip_cnt_rst
			; Increment the PWM counter each time we refreshed the entire screen
			INC R3
		
			; Reset the PWM counter if larger or equal to 5
			; The maximum value of 5 is due to the PWM resolution (2 bits per pixel) and also
			; due to a small hack detailed further (in t0oi_pixel_loop).
			LDI R25,5
			CP R3,R25
			BRNE t0oi_skip_pwm_rst
				CLR R3			; Reset the PWM counter

			t0oi_skip_pwm_rst:
			; Reset the Row selector to the last row (refreshing from bottom to top)
			LDI R25,0b01000000
			MOV R6,R25

			; Place the X pointer to address 0x018C (0x0100 + 140 => refreshing from end to start)
			LDI XL,0x8C
			LDI XH,0x01
	
	t0oi_skip_cnt_rst:

	; Get the pixels data byte from the SRAM
	LD R2,-X				; Decrement X and read the byte at address X
	
	LDI R25,4
	MOV R4,R25				; Pixel loop counter (8 shifts per register)
	t0oi_pixel_loop:
		; Retreive the pixel value
		MOV R25,R2				; Copy the current value of the data
		ANDI R25,0b00000011		; Mask the value for the current pixel
		LSR R2					; ROL two times the pixels data (R2) to place the next pixel at the two first bits
		LSR R2

		; Here, we use a hack to increase the contrast between the different values of the PWM:
		; We multiply the pixel value by 2 then we remove 1. In this way, there is 2 PWM levels between
		; each possible pixel value (except between 0 and 1), without increasing the number of bits per pixel.
		LSL R25					; Multiply the pixel value by 2
		DEC R25

		; Here, the R25 contains the PWM value of the pixel

		; Put data into shift registers
		CBI PORTB,3				; Set PB3 to 0 initially (screen's shift register input)
		CP R3,R25				; Compare the pixel value to the PWM counter
		BRGE t0oi_skip_pwm		; If the PWM counter is higher than the pixel value -> pixel is off
		SBI PORTB,3				; Put a 1 to PB3 (screen's shift register input)

		t0oi_skip_pwm:
		CBI PORTB,5				; Send a clock signal
		SBI PORTB,5

		DEC R4
		BRNE t0oi_pixel_loop

	DEC R5
	
	; Check if we have written a complete row
	CLR R25
	CP R5,R25
	BRNE t0oi_skip_row_select
		; Push the data in the row selector shift register when one complete
		; row has been written in the registers (= when R5 = 0).

		; Turn the LEDs off
		SBI PORTB,4
	
		; Send the row select data
		LDI R25,8					; Shift loop counter (8 shifts per register)
		CLC							; Ensure the C flag is cleared
		t0oi_shift_loop_rs:
			; Put data into shift registers
			CBI PORTB,3				; Set PB3 to 0 initially
			ROL R6
			BRCC t0oi_nxt_clk_rs	; Let PB3 to 0 (skip next instruction) if C==0
			SBI PORTB,3

			t0oi_nxt_clk_rs:
			CBI PORTB,5				; Send a clock signal
			SBI PORTB,5

			DEC R25
			BRNE t0oi_shift_loop_rs

		; At the end of this loop, the Row selector has been circularly shifted left 8 times.
		; This is equivalent to one shift to the right.

		; After row written -> enable latch output
		CBI PORTB,4				; Turn the LEDs on
	
	t0oi_skip_row_select:

	; Restore status register from the stack	
	POP R25
	OUT SREG, R25
	RETI


screen_fill:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function fills the screen data memory with a byte 
	; (= repeat a pattern on the screen)
	; R10: Data byte pattern
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; The screen data use 140 bytes in the SRAM
	LDI R16,140

	; Place the Y pointer to address 0x0100 (starting of the screen's data in SRAM)
	LDI YL,0x00
	LDI YH,0x01
	
	sf_loop:
		; Store the data for the screen to the RAM
		ST Y+,R10			; Write the byte at address Y and increment Y

		DEC R16
		BRNE sf_loop

	RET


screen_clear:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function clears the whole screen
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	CLR R10
	RCALL screen_fill
	RET


screen_set_hline:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Draw an horizontal line on the screen
	; R10: X position
	; R11: Y position
	; R12: LED brightness value (0 = Off; 1 = 33%, 2 = 67%, 3 = 100%)
	; R13: Line width (must be at least 1)
	; NOTE: this function changes: R0, R1, R16, R17, R18, R19
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Compute and add the row offset to the address of the data block
	LDI R16,20				; One row in the screen = 5*16 pixels = 5*4 bytes (2 bits per pixel)

	; Get the address of the start of the row
	MOV R17,R11
	CPI R17,7				; If the row index >= 7,
	BRLT ssh_less			; Then 
		SUBI R17,7			; Remove 7 from the row index
		MUL R17,R16			; Multiply row index with row length
		LDI R17,10			; Add an offset of 10 bytes (8*5 [pixels/line] * 2 [bits/pixel] / 8 [bits/byte])
		ADD R0,R17
		RJMP ssh_addr

	ssh_less:				; If the row index < 7
		MUL R11,R16			; Then simply multiply row index with row length

	; Here, the starting index of the row (in bytes) is stored in R0
	; NOTE: R0 is the result of the previous MUL operation

	ssh_addr:
	; Extract the X coordinate of the data block (byte)
	MOV R16,R10
	LSR R16					; Divide the X coordinate by 4 (two successive shifts left)
	LSR R16					; -> 8 [bits/byte] / 2 [bits/pixel] = 4  ==>  each byte contains 4 pixels

	ADD R16,R0				; Add the X index of the data block to the starting index of the row
	
	; Init Y-pointer to the right address
	MOV YL,R16
	LDI YH,0x01
	
	; Store the line width in R19
	MOV R19,R13

	; Extract the index of the two bits (pixel value) inside the byte
	MOV R16,R10
	ANDI R16,0b00000011		; Mask the last two bits

	SUBI R16,3				; Get the dual pixel index in the byte to decrement it later (b = 3 - a)
	NEG R16					; -> (b = 3 - a) because there is 4 pixels per byte

	ssh_width_loop:
	
	; Prepare the masks
	LDI R18,0b00000011		; Data position mask
	MOV R17,R12				; Pixels value mask (initially: the pixel brightness value)

	CPI R16,0
	; Set the bit to 1 at the index position
	ssh_bit_loop:
	BREQ ssh_bit_select			; Initially: BREQ on result of CPI operation, next: BREQ on result of DEC
		LSL R18					; Shift the data position mask left twice (two bits per pixel)
		LSL R18
		LSL R17					; Shift the pixel value mask left twice (two bits per pixel)
		LSL R17
		CPI R19,1				; Check if the remaining width (R19) == 1
		BREQ ssh_bl_next		; Skip the next part if the remaining width (R19) == 1
			DEC R19				; Decrement the remaining width
			ORI R18,0b00000011	; Add a pixel to the currently drawn line (data position mask)
			OR R17,R12			; Add a pixel to the currently drawn line (pixel value mask)

		ssh_bl_next:
		DEC R16
		RJMP ssh_bit_loop

	ssh_bit_select:

	; Get the data block from the RAM
	LD R16,Y

	; Clear the SRAM bits according to the data position mask
	COM R18					; Invert the data position mask
	AND R16,R18				; Keep only the bits which are not impacted by the pixels of the line

	; Add the pixel values into the SRAM bits
	OR R16,R17				; Merge the new pixel values with old ones

	; Write the modified data block into the SRAM
	ssh_write:
	ST Y+,R16

	; In case of overlapping over the next data block(s):

	LDI R16,4				; Prepare R16 (the line continues on the first pixel of the next data block)

	CPI R19,1				; If the line overlaps on another 8-bit data block, redo an iteration on the next data block
	BRNE ssh_width_loop

	RET


screen_set_rect:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Draw a rectangle on the screen
	; R10: X position
	; R11: Y position
	; R12: LED brightness value (0 = Off; 1 = 33%, 2 = 67%, 3 = 100%)
	; R13: Rectangle width (must be at least 1)
	; R14: Rectangle height (must be at least 1)
	; NOTE: this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Copy X position and height in R20 and R21
	MOV R20, R11
	MOV R21, R14

	; Repeat drawing lines of the same width across the height
	ssr_height_loop:
		; Draw an horizontal line at this Y position
		RCALL screen_set_hline
		INC R11				; Increment the Y position
		DEC R14				; Decrement the remaining height
		BRNE ssr_height_loop
	
	; Restore the X and height registers
	MOV R11, R20
	MOV R14, R21

	RET


screen_set_char:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Write a character in data memory (3px*5px)
	; R10 : X position (from 0 to 37)
	; R11 : Y position (from 0 to 9)
	; R12 : LED brightness value (0 = Off; 1 = 33%, 2 = 67%, 3 = 100%)
	; R13 : The character index
	; Note : this function changes: R16, R17, R18, R19, R20, R21
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Place the Z-pointer to the right address (starting of the CharTable)
	LDI ZH, high(CharTable<<1)	
	LDI ZL, low(CharTable<<1)
	
	; Constants for multiplication and addition
	LDI R16, 6
	LDI R19, 0

	; Compute the starting address of the needed character
	MUL R13, R16
	ADD ZL, R0
	ADC ZH, R19

	swc_draw:
		; Get the coordinate of the start of the row
		LDI R16, 20
		MOV R17, R11			; Y -> R17
		ADD R17, R19			; Y + row -> R17
		CPI R17,7				; If the row index >= 7,
		BRLT swc_less			; Then 
			SUBI R17,7			; Remove 7 from the row index
			MUL R17,R16			; Multiply row index with row length
			LDI R17, 10			; Add an offset of 10 (40pixels/row / 4pixels/byte = 10 bytes/row)
			ADD R0,R17
			RJMP swc_addr

		swc_less:				; If the row index < 7
			MUL R17,R16			; Then simply multiply row index with row length

		swc_addr:
		; Extract the X coordinate of the data block (byte)
		MOV R16,R10
		LSR R16					; Divide the X coordinate by 4 (two successive shifts left)
		LSR R16					; -> 8 [bits/byte] / 2 [bits/pixel] = 4  ==>  each byte contains 4 pixels
		ADD R16,R0				; Add the X index of the data block to the starting index of the row

		; Init Y-pointer to the right address
		MOV YL, R16
		LDI YH, 0x01

		; Extract the index of the bit inside the 8-bit block
		MOV R16,R10
		ANDI R16,0b00000011		; Mask the last 2 bits -> R16 = Xrel
		
		LPM R18, Z+				; Put the actual row of the character to R18
		CPI R16, 0
		BREQ swc_no_shift
		swc_shift:
			LSR R18
			DEC R16
			BRNE swc_shift
		swc_no_shift:

		LDI R16, 4
		LDI R21, 0
		MOV R17, R12
		swc_comp1_loop:
			ROR R18
			BRCC swc_comp1_jmp
				OR R21, R17
			swc_comp1_jmp:
			LSL R17
			LSL R17
			DEC R16
			BRNE swc_comp1_loop

		LDI R16, 4
		LDI R20, 0
		MOV R17, R12
		swc_comp2_loop:
			ROR R18
			BRCC swc_comp2_jmp
				OR R20, R17
			swc_comp2_jmp:
			LSL R17
			LSL R17
			DEC R16
			BRNE swc_comp2_loop
		
		LD R16, Y
		OR R16, R20
		ST Y+, R16

		LD R16, Y
		OR R16, R21
		ST Y, R16

		INC R19
		CPI R19, 5
		BRNE swc_draw			; Branch until the 5 rows of the character are written
	RET

screen_set_word:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Write a Word (multiple characters) on the screen (SRAM)
	; R10 : Starting X position
	; R11 : Starting Y position
	; R12 : LED brightness value (0 = Off; 1 = 33%, 2 = 67%, 3 = 100%)
	; Note : This function changes: *R10*, R16, R17, R18, R19, R20, R21.
	;		 The Z-pointer must be previoulsy placed on the word address.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	ssw_next_char:
	; Get the next character index from program memory
	LPM R13, Z+

	; Check if word index is _WORD_END
	LDI R16, _WORD_END
	CP R13, R16
	BREQ ssw_word_end
	
	; Check if word index is _SMALL_SPACE
	LDI R16, _SMALL_SPACE
	CP R13, R16
	BRNE ssw_write_word
		; Skip two pixels column (increment X position)
		INC R10
		INC R10
		RJMP ssw_next_char		; Write next char

	ssw_write_word:
	; Store the Z-pointer in the stack
	PUSH ZH
	PUSH ZL

	; Write this char on the screen
	RCALL screen_set_char

	; Restore the Z-pointer from the stack
	POP ZL
	POP ZH

	; Increment the X position for the next char
	LDI R16, 4
	ADD R10, R16

	; Write the next char
	RJMP ssw_next_char

	ssw_word_end:
	RET


; Characters table (6 bytes per character)
CharTable:
	.db 0b11100000, 0b10100000, 0b10100000, 0b10100000, 0b11100000, 0b00000000 ;0
	.db 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b00000000 ;1
	.db 0b11100000, 0b00100000, 0b11100000, 0b10000000, 0b11100000, 0b00000000 ;2
	.db 0b11100000, 0b00100000, 0b11100000, 0b00100000, 0b11100000, 0b00000000 ;3
	.db 0b10100000, 0b10100000, 0b11100000, 0b00100000, 0b00100000, 0b00000000 ;4
	.db 0b11100000, 0b10000000, 0b11100000, 0b00100000, 0b11100000, 0b00000000 ;5
	.db 0b11100000, 0b10000000, 0b11100000, 0b10100000, 0b11100000, 0b00000000 ;6
	.db 0b11100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00000000 ;7
	.db 0b11100000, 0b10100000, 0b11100000, 0b10100000, 0b11100000, 0b00000000 ;8
	.db 0b11100000, 0b10100000, 0b11100000, 0b00100000, 0b11100000, 0b00000000 ;9

	.db 0b01000000, 0b10100000, 0b11100000, 0b10100000, 0b10100000, 0b00000000 ;A
	.db 0b11000000, 0b10100000, 0b11000000, 0b10100000, 0b11000000, 0b00000000 ;B
	.db 0b01100000, 0b10000000, 0b10000000, 0b10000000, 0b01100000, 0b00000000 ;C
	.db 0b11000000, 0b10100000, 0b10100000, 0b10100000, 0b11000000, 0b00000000 ;D
	.db 0b11100000, 0b10000000, 0b11000000, 0b10000000, 0b11100000, 0b00000000 ;E
	.db 0b11100000, 0b10000000, 0b11000000, 0b10000000, 0b10000000, 0b00000000 ;F
	.db 0b01100000, 0b10000000, 0b10100000, 0b10100000, 0b01100000, 0b00000000 ;G
	.db 0b10100000, 0b10100000, 0b11100000, 0b10100000, 0b10100000, 0b00000000 ;H
	.db 0b11100000, 0b01000000, 0b01000000, 0b01000000, 0b11100000, 0b00000000 ;I
	.db 0b11100000, 0b00100000, 0b00100000, 0b10100000, 0b01100000, 0b00000000 ;J
	.db 0b10100000, 0b10100000, 0b11000000, 0b10100000, 0b10100000, 0b00000000 ;K
	.db 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b11100000, 0b00000000 ;L
	.db 0b10100000, 0b11100000, 0b11100000, 0b10100000, 0b10100000, 0b00000000 ;M
	.db 0b11000000, 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b00000000 ;N
	.db 0b01000000, 0b10100000, 0b10100000, 0b10100000, 0b01000000, 0b00000000 ;O
	.db 0b11000000, 0b10100000, 0b11000000, 0b10000000, 0b10000000, 0b00000000 ;P
	.db 0b01000000, 0b10100000, 0b10100000, 0b11100000, 0b01100000, 0b00000000 ;Q
	.db 0b11000000, 0b10100000, 0b11000000, 0b10100000, 0b10100000, 0b00000000 ;R
	.db 0b11100000, 0b10000000, 0b11100000, 0b00100000, 0b11100000, 0b00000000 ;S
	.db 0b11100000, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b00000000 ;T
	.db 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b11100000, 0b00000000 ;U
	.db 0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b01000000, 0b00000000 ;V
	.db 0b10100000, 0b10100000, 0b11100000, 0b11100000, 0b10100000, 0b00000000 ;W
	.db 0b10100000, 0b10100000, 0b01000000, 0b10100000, 0b10100000, 0b00000000 ;X
	.db 0b10100000, 0b10100000, 0b01000000, 0b01000000, 0b01000000, 0b00000000 ;Y
	.db 0b11100000, 0b00100000, 0b01000000, 0b10000000, 0b11100000, 0b00000000 ;Z

	.db 0b01000000, 0b01000000, 0b01000000, 0b00000000, 0b01000000, 0b00000000 ;!
	.db 0b01100000, 0b00100000, 0b01000000, 0b00000000, 0b01000000, 0b00000000 ;?

	.db 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 ; White space
	

WordPlayer1:
	.db _P, _L, _A, _Y, _E, _R, _SMALL_SPACE, 1, _WORD_END, 0		; Add a zero padding to keep even number of bytes (16-bit alignment)
	
WordPlayer2:
	.db _P, _L, _A, _Y, _E, _R, _SMALL_SPACE, 2, _WORD_END, 0		; Add a zero padding to keep even number of bytes (16-bit alignment)
	
WordBattleship:
	.db _B, _A, _T, _T, _L, _E, _S, _H, _I, _P, _WORD_END, 0		; Add a zero padding to keep even number of bytes (16-bit alignment)
	
WordHit:
	.db _H, _I, _T, _WORD_END
	
WordMiss:
	.db _M, _I, _S, _S, _WORD_END, 0

WordGo:
	.db _G, _O, _EXC, _WORD_END
