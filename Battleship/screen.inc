/*
 * screen.inc
 *
 *   Authors: Mathieu Philipart
 *			  Théo    Lepoutte
 */

 // MACROS
.macro draw_char
	LDI R16, @0		; X
	MOV R10, R16	
	LDI R16, @1		; Y
	MOV R11, R16	
	LDI R16, @2		; On/Off
	MOV R12, R16	
	LDI R16, @3		; G
	MOV R13, R16	
	RCALL screen_set_char
.endmacro

.macro draw_rect
	LDI R16, @0		; X
	MOV R10, R16	
	LDI R16, @1		; Y
	MOV R11, R16	
	LDI R16, @2		; On/Off
	MOV R12, R16	
	LDI R16, @3		; Width
	MOV R13, R16	
	LDI R16, @4		; Height
	MOV R14, R16	
	RCALL screen_set_rect
.endmacro

.macro draw_line
	LDI R16, @0		; X
	MOV R10, R16	
	LDI R16, @1		; Y
	MOV R11, R16	
	LDI R16, @2		; On/Off
	MOV R12, R16	
	LDI R16, @3		; Width
	MOV R13, R16
	RCALL screen_set_hline
.endmacro

.macro draw_point
	LDI R16, @0		; X
	MOV R10, R16	
	LDI R16, @1		; Y
	MOV R11, R16	
	LDI R16, @2		; On/Off
	MOV R12, R16	
	RCALL screen_set_point
.endmacro

.macro draw_battleship
	draw_point 

.endmacro

 // DEFINITIONS
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


// TABLE
CharTable:
	.db 0b00000111, 0b00000101, 0b00000101, 0b00000101, 0b00000111, 0b00000000 ;0
	.db 0b00000100, 0b00000100, 0b00000100, 0b00000100, 0b00000100, 0b00000000 ;1
	.db 0b00000111, 0b00000100, 0b00000111, 0b00000001, 0b00000111, 0b00000000 ;2
	.db 0b00000111, 0b00000100, 0b00000111, 0b00000100, 0b00000111, 0b00000000 ;3
	.db 0b00000101, 0b00000101, 0b00000111, 0b00000100, 0b00000100, 0b00000000 ;4
	.db 0b00000111, 0b00000001, 0b00000111, 0b00000100, 0b00000111, 0b00000000 ;5
	.db 0b00000111, 0b00000001, 0b00000111, 0b00000101, 0b00000111, 0b00000000 ;6
	.db 0b00000111, 0b00000100, 0b00000100, 0b00000100, 0b00000100, 0b00000000 ;7
	.db 0b00000111, 0b00000101, 0b00000111, 0b00000101, 0b00000111, 0b00000000 ;8
	.db 0b00000111, 0b00000101, 0b00000111, 0b00000100, 0b00000111, 0b00000000 ;9

	.db 0b00000010, 0b00000101, 0b00000111, 0b00000101, 0b00000101, 0b00000000 ;A
	.db 0b00000011, 0b00000101, 0b00000011, 0b00000101, 0b00000011, 0b00000000 ;B
	.db 0b00000110, 0b00000001, 0b00000001, 0b00000001, 0b00000110, 0b00000000 ;C
	.db 0b00000011, 0b00000101, 0b00000101, 0b00000101, 0b00000011, 0b00000000 ;D
	.db 0b00000111, 0b00000001, 0b00000011, 0b00000001, 0b00000111, 0b00000000 ;E
	.db 0b00000111, 0b00000001, 0b00000011, 0b00000001, 0b00000001, 0b00000000 ;F
	.db 0b00000110, 0b00000001, 0b00000101, 0b00000101, 0b00000110, 0b00000000 ;G
	.db 0b00000101, 0b00000101, 0b00000111, 0b00000101, 0b00000101, 0b00000000 ;H
	.db 0b00000111, 0b00000010, 0b00000010, 0b00000010, 0b00000111, 0b00000000 ;I
	.db 0b00000111, 0b00000100, 0b00000100, 0b00000101, 0b00000110, 0b00000000 ;J
	.db 0b00000101, 0b00000101, 0b00000011, 0b00000101, 0b00000101, 0b00000000 ;K
	.db 0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000111, 0b00000000 ;L
	.db 0b00000101, 0b00000111, 0b00000111, 0b00000101, 0b00000101, 0b00000000 ;M
	.db 0b00000011, 0b00000101, 0b00000101, 0b00000101, 0b00000101, 0b00000000 ;N
	.db 0b00000010, 0b00000101, 0b00000101, 0b00000101, 0b00000010, 0b00000000 ;O
	.db 0b00000011, 0b00000101, 0b00000011, 0b00000001, 0b00000001, 0b00000000 ;P
	.db 0b00000010, 0b00000101, 0b00000101, 0b00000111, 0b00000110, 0b00000000 ;Q
	.db 0b00000011, 0b00000101, 0b00000011, 0b00000101, 0b00000101, 0b00000000 ;R
	.db 0b00000111, 0b00000001, 0b00000111, 0b00000100, 0b00000111, 0b00000000 ;S
	.db 0b00000111, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000000 ;T
	.db 0b00000101, 0b00000101, 0b00000101, 0b00000101, 0b00000111, 0b00000000 ;U
	.db 0b00000101, 0b00000101, 0b00000101, 0b00000101, 0b00000010, 0b00000000 ;V
	.db 0b00000101, 0b00000101, 0b00000101, 0b00000111, 0b00000111, 0b00000000 ;W
	.db 0b00000101, 0b00000101, 0b00000010, 0b00000101, 0b00000101, 0b00000000 ;X
	.db 0b00000101, 0b00000101, 0b00000010, 0b00000010, 0b00000010, 0b00000000 ;Y
	.db 0b00000111, 0b00000100, 0b00000010, 0b00000001, 0b00000111, 0b00000000 ;Z

	.db 0b00000001, 0b00000001, 0b00000001, 0b00000000, 0b00000001, 0b00000000 ;!
	.db 0b00000110, 0b00000100, 0b00000010, 0b00000000, 0b00000010, 0b00000000 ;?

// FUNCTIONS //
init_screen:
	; Data In (PB3) is output
	SBI DDRB, 3	
	CBI PORTB, 3
	; Clock (PB5) is output
	SBI DDRB, 5		
	CBI PORTB, 5
	; Output Enable/Latch CLK (PB4) is output
	SBI DDRB, 4		
	CBI PORTB, 4
	RET


screen_fill:
	; Fill the screen data memory with a byte (repeat a pattern on the screen)
	; R10: Data byte pattern
	; NOTE: this function changes the registers: R16

	LDI R16,70

	; Place the Y pointer to address 0x0100
	LDI YL,0x00
	LDI YH,0x01
	
	sf_loop:
		; Store the data for the screen to the RAM
		ST Y+,R10			; Write the byte at address Y and increment Y

		DEC R16
		BRNE sf_loop
	RET

screen_set_char:
	; Write a number in data memory (3px*5px)
	; R10 : X position (from 0 to 37)
	; R11 : Y position (from 0 to 9)
	; R12 : LED status (On/Off)
	; R13 : number (0 to 9)
	; Note : this function changes the registers : R0, R16, R17, R18, R19, R20, R21
	; Load 
	LDI ZH, high(CharTable<<1)	
	LDI ZL, low(CharTable<<1)
	
	LDI R16, 6
	LDI R19, 0

	MUL R13, R16
	ADD ZL, R0
	BRCC swc_draw	; Skip if no need to INC ZH
		LDI R16, 1
		ADD ZH, R16

	swc_draw:
		; Get the address of the start of the row
		LDI R16, 10
		MOV R17, R11			; Y -> R17
		ADD R17, R19			; Y + row -> R17
		CPI R17,7				; If the row index >= 7,
		BRLT swc_less			; Then 
			SUBI R17,7			; Remove 7 from the row index
			MUL R17,R16			; Multiply row index with row length
			LDI R17,5			; Add an offset of 5 (8*5 pixels / 8 bits)
			ADD R0,R17
			RJMP swc_addr

		swc_less:				; If the row index < 7
			MUL R17,R16			; Then simply multiply row index with row length

		swc_addr:
		; Extract the X index of the data block (8-bit block)
		MOV R16, R10
		LSR R16					; Divide the X index by 8 (three successive shift left)
		LSR R16												
		LSR R16

		ADD R16, R0				; Add the X index of the data block to the starting index of the row
		; NOTE: R0 is the result of the previous MUL operation
		; Init Y-pointer to the right address
		MOV YL, R16
		LDI YH, 0x01
		; Extract the index of the bit inside the 8-bit block
		MOV R17,R10
		ANDI R17,0b00000111		; Mask the last three bits

		LDI R21, 0
		LPM R18, Z+				; Put the actual row of the character to R18
		LD R20, Y				; Put the displayed bits to R20
		MOV R16, R17
		CPI R17, 0
		BREQ swc_no_shift
		swc_shift:
			LSL R21
			ROL R18
			BRCC swc_shift_no_carry			; If a character is written at the end of the register then continue writting in the following
				SBR R21, 1					; Register
				CLC
			swc_shift_no_carry:
			DEC R16							; Dec until the pixels are set in the two registers
			BRNE swc_shift
		swc_no_shift:
		
		
		SBRC R12,0				; Skip next if we want LED OFF
		RJMP swc_enable
			SUB R20, R18		; Sub the char pixels from the actual displayed pixels
			ST Y+, R20
			LD R16, Y
			SUB R16, R21
			RJMP swc_end

		swc_enable:
			OR R20, R18			; Add the char pixels to the actual displayed pixels
			ST Y+, R20
			LD R16, Y
			OR R16, R21
			
		swc_end:
		ST Y, R16				; Store in the second register
		INC R19
		CPI R19, 5
		BRNE swc_draw			; Branch until the 5 rows of the character are written
	RET

screen_set_hline:
	; Draw an horizontal line on the screen
	; R10: X position
	; R11: Y position
	; R12: LED status (0 = Off; 1 = On)
	; R13: Line width (must be at least 1)
	; NOTE: this function changes the registers: R0, R1, R16, R17, R18, R19

	; Compute and add the row offset to the address of the data block
	LDI R16,10				; One row in the screen = 5*16 pixels = 5*2 bytes

	; Get the address of the start of the row
	MOV R17,R11
	CPI R17,7				; If the row index >= 7,
	BRLT ssh_less			; Then 
		SUBI R17,7			; Remove 7 from the row index
		MUL R17,R16			; Multiply row index with row length
		LDI R17,5			; Add an offset of 5 bytes (8*5 [pixels/line] / 8 [bits/byte])
		ADD R0,R17
		RJMP ssh_addr

	ssh_less:				; If the row index < 7
		MUL R11,R16			; Then simply multiply row index with row length

	ssh_addr:
	; Extract the X index of the data block (8-bit block)
	MOV R16,R10
	LSR R16					; Divide the X index by 8 (three successive shift left)
	LSR R16
	LSR R16

	ADD R16,R0				; Add the X index of the data block to the starting index of the row
	; NOTE: R0 is the result of the previous MUL operation
	
	; Init Y-pointer to the right address
	MOV YL,R16
	LDI YH,0x01
	
	; Store the line width in R19
	MOV R19,R13

	; Extract the index of the bit inside the 8-bit block
	MOV R17,R10
	ANDI R17,0b00000111		; Mask the last three bits

	SUBI R17,7				; Get the dual bit index of the 8-bit block (b = 7 - a)
	NEG R17

	ssh_width_loop:

	; Get the data block from the RAM
	LD R16,Y
	
	; Prepare the mask
	LDI R18,0b10000000

	CPI R17,0
	; Set the bit to 1 at the index position
	ssh_bit_loop:
	BREQ ssh_bit_select		; Initially: BREQ on result of CPI operation, next: BREQ on result of DEC
		LSR R18				; Shift the bit mask left
		CPI R19,1			; Check if the remaining width (R19) == 1
		BREQ ssh_bl_next	; Skip the next part if the remaining width (R19) == 1
			DEC R19			; Decrement the remaining Width
			ORI R18,0b10000000	; Set LSB to 1 (add a pixel to the currently drawn line)

		ssh_bl_next:
		DEC R17
		RJMP ssh_bit_loop

	ssh_bit_select:

	; Action depending on the wanted LED status
	SBRC R12,0				; Skip next if we want LED OFF
	RJMP ssh_enable
		COM R18				; Get the invert of the mask
		AND R16,R18			; Mask the current data block to clear only the selected bit
		RJMP ssh_write

	ssh_enable:
		OR R16,R18			; Then overlap the mask to the data block to set only the selected bit

	; Write the modified data block into the SRAM
	ssh_write:
	ST Y+,R16

	; In case of overlapping over the next data block(s):

	LDI R17,8				; Prepare R17 (the line continues on the first byte of the next data block)

	CPI R19,1				; If the line overlaps on another 8-bit data block, redo an iteration on the next data block
	BRNE ssh_width_loop

	RET

screen_set_rect:
	; Draw a rectangle on the screen
	; R10: X position
	; R11: Y position
	; R12: LED status (0 = Off; 1 = On)
	; R13: Rectangle width (must be at least 1)
	; R14: Rectangle height (must be at least 1)
	; NOTE: this function changes the registers: *R11*, *R14*, R16, R17, R18, R19

	; Repeat drawing lines of the same width across the height
	ssr_height_loop:
		; Draw an horizontal line at this Y position
		RCALL screen_set_hline
		INC R11				; Increment the Y position
		DEC R14				; Decrement the remaining height
		BRNE ssr_height_loop
	RET

screen_set_point:
	; Draw a point on the screen
	; R10: X position
	; R11: Y position
	; R12: point status (0 = Off; 1 = On)

	; Compute and add the row offset to the address of the data block
	LDI R16,10				; One row in the screen = 5*16 pixels = 5*2 bytes

	; Get the address of the start of the row
	MOV R17,R11
	CPI R17,7				; If the row index >= 7,
	BRLT ssp_less			; Then 
		SUBI R17,7			; Remove 7 from the row index
		MUL R17,R16			; Multiply row index with row length
		LDI R17,5			; Add an offset of 5 (8*5 pixels / 8 bits)
		ADD R0,R17
		RJMP ssp_addr

	ssp_less:				; If the row index < 7
		MUL R11,R16			; Then simply multiply row index with row length

	ssp_addr:
	; Extract the X index of the data block (8-bit block)
	MOV R16,R10
	LSR R16					; Divide the X index by 8 (three successive shift left)
	LSR R16
	LSR R16

	ADD R16,R0				; Add the X index of the data block to the starting index of the row
	; NOTE: R0 is the result of the previous MUL operation
	
	; Init Y-pointer to the right address
	MOV YL,R16
	LDI YH,0x01
	
	; Get the data block from the RAM
	LD R16,Y

	; Extract the index of the bit inside the 8-bit block
	MOV R17,R10
	ANDI R17,0b00000111		; Mask the last three bits
	
	; Prepare the mask
	LDI R18,1

	; Set the bit to 1 at the index position
	ssp_bit_loop:
	BREQ ssp_bit_select		; Initially: BRNE on result of ANDI operation, next: BRNE on result of DEC
		LSL R18				; Shift the bit mask left
		DEC R17
		RJMP ssp_bit_loop

	; Get the invert of the mask
	ssp_bit_select:
	COM R18

	; Action depending on the wanted LED status
	SBRC R12,0				; Skip next if we want LED OFF
	RJMP ssp_enable
		AND R16,R18			; Mask the current data block to clear only the selected bit
		RJMP ssp_write

	ssp_enable:
		COM R16				; Invert the current data block
		AND R16,R18			; Then mask the inverted data block to (un)clear only the selected bit
		COM R16				; Finally, invert the masked set the selected bit

	; Write the modified data block into the SRAM
	ssp_write:
	ST Y,R16

	RET

write_go:
	draw_rect 14, 2, 1, 12, 9
	draw_rect 15, 3, 0, 10, 7
	draw_char 16, 4, 1, _G
	draw_char 20, 4, 1, _O
	draw_char 24, 4, 1, _EXC
	RET

write_battleship:
	draw_char 1, 5, 1, _B 
	draw_char 5, 5, 1, _A
	draw_char 9, 5, 1, _T
	draw_char 13, 5, 1, _T
	draw_char 17, 5, 1, _L
	draw_char 21, 5, 1, _E
	draw_char 25, 5, 1, _S
	draw_char 29, 5, 1, _H
	draw_char 33, 5, 1, _I
	draw_char 37, 5, 1, _P
	draw_line 0, 3, 1, 40
	draw_line 0, 11, 1, 40
	RET
