;
; CoreI2C.inc
;
; This file is part of the Battleship project.
;
; It contains all the functions needed to send/receive packets
; between two boards using I2C peripheral.
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

#define I2C_START			0x08	// Start condition has been received
#define I2C_REP_START		0x10	// Repeated start condition has been received
#define I2C_MT_SLA_ACK		0x18	// Master Transmit: Slave ACK has been received
#define I2C_MT_DATA_ACK		0x28    // Master Transmit: Master ACK has been received
#define I2C_MR_SLA_ACK		0x40	// Master Receive: Slave ACK has been received
#define I2C_MR_DATA_ACK		0x50    // Master Receive: Master ACK has been received
#define I2C_MR_DATA_NACK	0x58    // Master Receive: Master NACK has been received
#define I2C_SR_SLA_ACK		0x60	// Slave Receive: Own SLA+W has been received; ACK has been returned
#define I2C_SR_DATA_ACK		0x80	// Slave Receive: Data has been received; ACK has been returned
#define I2C_SR_DATA_NACK	0x88	// Slave Receive: Data has been received; NACK has been returned
#define I2C_SR_STOP			0xA0	// Slave Receive: A STOP condition or repeated START condition has been received
#define I2C_ST_SLA_ACK		0xA8	// Slave Transmit: Own SLA+R has been received; ACK has been returned
#define I2C_ST_DATA_ACK		0xB8	// Slave Transmit: Data has been transmitted; ACK has been returned
#define I2C_ST_DATA_NACK	0xC0	// Slave Transmit: Data has been transmitted; NOT ACK has been received
#define I2C_ST_END_ACK		0xC8	// Slave Transmit: Last data byte has been transmitted; ACK has been received

#define I2C_ISR_EN	0	// Enables the I2C interrupt ISR if 1

; Slave address is 20
#define I2C_SLAVE_ADDR	20	// Slave address


.macro i2c_check_status
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro checks the I2C Status Register and compare the status 
	; to the value given in argument 0.
	; If the Status Register equals the value in argument 0, the macro
	; jumps to the label given in argument 1.
	; NOTE: this macro changes: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Store the comparison status into R17
	LDI R17, @0

	; Check value of TWI Status Register. Mask prescaler bits.
	LDS R16, TWSR
	ANDI R16, 0b11111000
	CPSE R16, R17			; Skip the RJMP if R16 == R17
	RJMP @1
	; NOTE: we use CPSE and RJMP to take advantage of RJMP 12-bit address capabilities
.endmacro

.macro i2c_master_start_cond
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro sends a start condition on the I2C bus.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Send START condition
	LDI R16, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_master_stop_cond
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro sends a stop condition on the I2C bus.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Send STOP condition
	LDI R16, (1<<TWINT)|(1<<TWEN)|(1<<TWSTO)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_master_send_SLA_W
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro sends the slave address and the Write bit on the bus.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Load SLAVE ADDRESS in write mode (SLA+W) into TWDR Register. Clear TWINT bit in TWCR to start transmission of address
	LDI R16, I2C_SLAVE_ADDR
	LSL R16
	STS TWDR, R16
	LDI R16, (1<<TWINT)|(1<<TWEN)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_master_send_SLA_R
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro sends the slave address and the Read bit on the bus.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Load SLAVE ADDRESS in read mode (SLA+R) into TWDR Register. Clear TWINT bit in TWCR to start transmission of address
	LDI R16, I2C_SLAVE_ADDR
	LSL R16
	ORI R16, 1				; Add 1 to set the address in read mode
	STS TWDR, R16
	LDI R16, (1<<TWINT)|(1<<TWEN)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_master_send_data
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro sends a data byte on the bus in master mode.
	; The data byte is taken from the register given in argument 0.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Load DATA into TWDR Register. Clear TWINT bit in TWCR to start transmission of data
	STS TWDR, @0
	LDI R16, (1<<TWINT)|(1<<TWEN)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_master_start_receive_data_ack
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro informs the I2C system to start the reception of one 
	; data byte in master receiver mode.
	; This macro should be used if there are other bytes following (ACK).
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Clear TWINT bit in TWCR to resume reception of data.
	; NOTE: Since there is an other byte to read after this one, the master should answer with an ACK => (1 << TWEA)
	LDI R16, (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_master_start_receive_data_nack
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro informs the I2C system to start the reception of one 
	; data byte in master receiver mode.
	; This macro should be used if this is the last byte to read (NACK).
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Clear TWINT bit in TWCR to resume reception of data.
	; NOTE: A NACK should be transmitted because the slave has no more data to transmit
	LDI R16, (1<<TWINT)|(1<<TWEN)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_slave_enter_listening_mode
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro setup the slave in listening mode. This indicates to
	; the I2C hardware to ACK the next SLA+R/W from the master.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Place slave in listening mode
	LDI R16, (1<<TWEA)|(1<<TWEN)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_slave_exit_listening_mode
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro exits the slave listening mode. This indicates to
	; the I2C hardware stop listening the bus.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Exit the slave listening mode
	LDI R16, (1<<TWINT)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_slave_send_data
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro sends a data byte on the bus in slave mode.
	; The data byte is taken from the register given in argument 0.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Clear TWINT bit in TWCR to start transmission of data.
	STS TWDR, @0
	LDI R16, (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_slave_start_receive_data
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro informs the I2C system to start the reception of one 
	; data byte in slave receiver mode.
	; NOTE: this macro changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Clear TWINT bit in TWCR to start reception of data.
	LDI R16, (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(I2C_ISR_EN<<TWIE)
	STS TWCR, R16
.endmacro

.macro i2c_master_send_cst_packet
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro sends a packet of constant data bytes (from arguments
	; [0,4]) on the I2C bus in master mode.
	; NOTE: this macro changes: R16,R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	LDI R16,@0
	MOV R10, R16
	LDI R16,@1
	MOV R11, R16
	LDI R16,@2
	MOV R12, R16
	LDI R16,@3
	MOV R13, R16
	LDI R16,@4
	MOV R14, R16
	RCALL i2c_master_transmit_packet
.endmacro

.macro i2c_slave_send_cst_packet
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This macro sends a packet of constant data bytes (from arguments
	; [0,4]) on the I2C bus in slave mode.
	; NOTE: this macro changes: R16,R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	LDI R16,@0
	MOV R10, R16
	LDI R16,@1
	MOV R11, R16
	LDI R16,@2
	MOV R12, R16
	LDI R16,@3
	MOV R13, R16
	LDI R16,@4
	MOV R14, R16
	RCALL i2c_slave_transmit_packet
.endmacro


i2c_init:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the I2C device for both master and slave
	; operation modes (clock generator and slave address configured).
	; It also configures the top LED (PC2) as error indicator.
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Setup I2C clock generator prescaler to 4
	LDI R16,0x01
	STS TWSR,R16

	; Setup I2C clock generator division to 200 => Bit Rate clock = 10kHz
	LDI R16,200
	STS TWBR,R16
	
	; Set the slave address
	LDI R16,I2C_SLAVE_ADDR
	LSL R16
	STS TWAR,R16
	
	; Configure output pin PC2 (LED TOP)
	SBI DDRC,2			; Pin PC2 is an output
	SBI PORTC,2			; Output Vcc => Off
	
	; Configure output pin PC3 (LED BOTTOM)
	SBI DDRC,3			; Pin PC3 is an output
	SBI PORTC,3			; Output Vcc => Off
	
	RET

	
i2c_wait_int_flag:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function suspends the execution until the I2C interrupt flag
	; is set by the hardware.
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	i2c_wif_loop:
		LDS R16, TWCR
		SBRS R16, TWINT
		RJMP i2c_wif_loop
	
	RET


i2c_wait_stop_cond:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function suspends the execution until the stop condition has 
	; been completely sent.
	; NOTE: this function changes: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	i2c_wsc_loop:
		LDS R16, TWCR
		SBRC R16, TWSTO
		RJMP i2c_wsc_loop

	RET
	


i2c_master_transmit_packet:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function transmits a packet (5 bytes) in the I2C bus.
	; The 5 bytes to send are taken from 5 registers: R[10;14].
	; The exit status is written in R15 (0x00 if succes, 0xFF if error). 
	; NOTE: this function modifies: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Reset the error LED
	SBI PORTC,2

	;
	; INITIALIZE TRANSMISSION
	;
	; Send START condition
	i2c_master_start_cond
	
	; Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	RCALL i2c_wait_int_flag
	
	; Check value of TWI Status Register. If status different from START go to ERROR
	i2c_check_status I2C_START, i2c_mtp_error
	
	; Send SLAVE ADDRESS in write mode (SLA+W)
	i2c_master_send_SLA_W
	
	; Wait for TWINT Flag set. This indicates that the SLA+W has been transmitted, and ACK/NACK has been received.
	RCALL i2c_wait_int_flag
	
	; Check value of TWI Status Register. If status different from MT_SLA_ACK go to ERROR
	i2c_check_status I2C_MT_SLA_ACK, i2c_mtp_error
	
	;
	; SEND DATA (5 bytes)
	;
	; Send the first data byte on the bus
	i2c_master_send_data R10
	
	; Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
	RCALL i2c_wait_int_flag

	; Check value of TWI Status Register. If status different from MT_DATA_ACK go to ERROR
	i2c_check_status I2C_MT_DATA_ACK, i2c_mtp_error
	
	; Send the second data byte on the bus
	i2c_master_send_data R11
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_MT_DATA_ACK, i2c_mtp_error
	
	; Send the third data byte on the bus
	i2c_master_send_data R12
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_MT_DATA_ACK, i2c_mtp_error

	; Send the fourth data byte on the bus
	i2c_master_send_data R13
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_MT_DATA_ACK, i2c_mtp_error

	; Send the last data byte on the bus
	i2c_master_send_data R14
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_MT_DATA_ACK, i2c_mtp_error

	;
	; END TRANSMISSION
	;
	; Send stop condition
	i2c_master_stop_cond

	; Wait for stop condition completed
	RCALL i2c_wait_stop_cond
	
	; Clear R15 to indicate Success exit status
	CLR R15

	RET
	
	; In case of error => send STOP condition and enable TOP LED

	i2c_mtp_error:
		; Send stop condition
		i2c_master_stop_cond

		; LED ON if error
		CBI PORTC,2

		; Store 0xFF in R15 to indicate Error exit status
		LDI R16, 0xFF
		MOV R15, R16

	RET


i2c_master_receive_init:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function initializes the I2C master receive communication.
	; NOTE: this function modifies: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Reset the error LED
	SBI PORTC,3

	;
	; INITIALIZE RECEPTIOIN
	;
	; Send START condition
	i2c_master_start_cond
	
	; Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	RCALL i2c_wait_int_flag

	; Check value of TWI Status Register. If status different from START go to ERROR
	i2c_check_status I2C_START, i2c_mrp_error

	; Send SLAVE ADDRESS in read mode (SLA+R)
	i2c_master_send_SLA_R
	
	; Wait for TWINT Flag set. This indicates that the SLA+R has been transmitted, and ACK/NACK has been received.
	RCALL i2c_wait_int_flag

	; Check value of TWI Status Register. Mask prescaler bits. If status different from MR_SLA_ACK go to ERROR
	i2c_check_status I2C_MR_SLA_ACK, i2c_mrp_error

	;
	; READ DATA (5 bytes)
	;
	; Start the reception of the first data byte
	; NOTE: Since there is an other byte to read after this one, the master should answer with an ACK
	i2c_master_start_receive_data_ack

	RET


i2c_master_receive_packet:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function reads a packet (5 bytes) from the I2C bus.
	; The 5 bytes read are stored into 5 registers: R[10;14].
	; The exit status is written in R15 (0x00 if succes, 0xFF if error).
	; NOTE: this function modifies: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Reset the error LED
	SBI PORTC,3

	; Send start condition then SLA+R
	RCALL i2c_master_receive_init

	;
	; READ DATA (5 bytes)
	;
	; Start the reception of the first data byte
	; NOTE: Since there is an other byte to read after this one, the master should answer with an ACK
	;i2c_master_start_receive_data_ack
	
i2c_master_receive_packet_no_sla:
	; Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
	RCALL i2c_wait_int_flag

	; Check value of TWI Status Register. Mask prescaler bits. If status different from MR_DATA_ACK go to ERROR
	i2c_check_status I2C_MR_DATA_ACK, i2c_mrp_error
	
	; Store the reveived data into TWDR Register.
	LDS R10, TWDR

	; Read the second data byte from the bus
	i2c_master_start_receive_data_ack
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_MR_DATA_ACK, i2c_mrp_error
	LDS R11, TWDR

	; Read the third data byte from the bus
	i2c_master_start_receive_data_ack
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_MR_DATA_ACK, i2c_mrp_error
	LDS R12, TWDR

	; Read the fourth data byte from the bus
	i2c_master_start_receive_data_ack
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_MR_DATA_ACK, i2c_mrp_error
	LDS R13, TWDR

	; Read the last data byte from the bus
	; NOTE: A NACK should be transmitted because the slave has no more data to transmit (5 bytes)
	i2c_master_start_receive_data_nack
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_MR_DATA_NACK, i2c_mrp_error
	LDS R14, TWDR

	;
	; END RECEPTION
	;
	; Send STOP condition
	i2c_master_stop_cond

	; Wait for stop condition completed
	RCALL i2c_wait_stop_cond
	
	; Clear R15 to indicate Success exit status
	CLR R15

	RET

	; In case of error => send STOP condition and enable BOTTOM LED

	i2c_mrp_error:
		; Send STOP condition
		i2c_master_stop_cond
	
		; LED ON if error
		CBI PORTC,3

		; Store 0xFF in R15 to indicate Error exit status
		LDI R16, 0xFF
		MOV R15, R16

	RET


i2c_slave_transmit_packet:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function transmits a packet (5 bytes) on the I2C bus.
	; The 5 bytes to send are taken from 5 registers: R[10;14].
	; The exit status is written in R15 (0x00 if succes, 0xFF if error).
	; This function can also be accessed with _no_listening suffix if the
	; listening mode was already set previously.
	; NOTE: this function modifies: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Reset the error LED
	SBI PORTC,2

	;
	; INITIALIZE TRANSMISSION
	;
	; Place slave in listening mode
	i2c_slave_enter_listening_mode
	
i2c_slave_transmit_packet_no_listening:
	; Wait for TWINT Flag set. This indicates that the slave has been addressed by its own slave address
	RCALL i2c_wait_int_flag

	; Check value of TWI Status Register. If status different from ST_SLA_ACK go to ERROR
	i2c_check_status I2C_ST_SLA_ACK, i2c_stp_error
	
	;
	; TRANSMIT DATA (5 bytes)
	;
	; Send the first data byte on the bus
	i2c_slave_send_data R10

	; Wait for TWINT Flag set. This indicates that the DATA has been received, and ACK/NACK has been sent.
	RCALL i2c_wait_int_flag

	; Check value of TWI Status Register. If status different from SR_DATA_ACK go to ERROR
	i2c_check_status I2C_ST_DATA_ACK, i2c_stp_error

	; Send the second data byte on the bus
	i2c_slave_send_data R11
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_ST_DATA_ACK, i2c_stp_error

	; Send the third data byte on the bus
	i2c_slave_send_data R12
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_ST_DATA_ACK, i2c_stp_error

	; Send the fourth data byte on the bus
	i2c_slave_send_data R13
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_ST_DATA_ACK, i2c_stp_error

	; Send the last data byte on the bus
	; NOTE: A NACK should be received because the master doesn't want more data (5 bytes)
	i2c_slave_send_data R14
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_ST_DATA_NACK, i2c_stp_error
	
	;
	; END TRANSMISSION
	;
	; Exit the slave listening mode
	i2c_slave_exit_listening_mode
	
	; Clear R15 to indicate Success exit status
	CLR R15

	RET
	
	; In case of error => exit the slave listening mode and enable TOP LED

	i2c_stp_error:
		; Exit the slave listening mode
		i2c_slave_exit_listening_mode
	
		; LED ON if error
		CBI PORTC,2

		; Store 0xFF in R15 to indicate Error exit status
		LDI R16, 0xFF
		MOV R15, R16

	RET


i2c_slave_receive_packet:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function receives a packet (5 bytes) from the I2C bus.
	; The 5 bytes read are stored into 5 registers: R[10;14].
	; The exit status is written in R15 (0x00 if succes, 0xFF if error).
	; This function can also be accessed with _no_listening suffix if the
	; listening mode was already set previously.
	; NOTE: this function modifies: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Reset the error LED
	SBI PORTC,3

	;
	; INITIALIZE RECEPTIOIN
	;
	; Place slave in listening mode
	i2c_slave_enter_listening_mode
	
i2c_slave_receive_packet_no_listening:
	; Wait for TWINT Flag set. This indicates that the slave has been addressed by its own slave address
	RCALL i2c_wait_int_flag

	; Check value of TWI Status Register. If status different from SR_SLA_ACK go to ERROR
	i2c_check_status I2C_SR_SLA_ACK, i2c_srp_error
	
	;
	; RECEIVE DATA (5 bytes)
	;
	; Start the reception of the first data byte
	i2c_slave_start_receive_data

	; Wait for TWINT Flag set. This indicates that the DATA has been received, and ACK/NACK has been sent.
	RCALL i2c_wait_int_flag

	; Check value of TWI Status Register. If status different from SR_DATA_ACK go to ERROR
	i2c_check_status I2C_SR_DATA_ACK, i2c_srp_error

	; Store the first received data byte into R10
	LDS R10, TWDR
	
	; Receive the second data byte and store in R11
	i2c_slave_start_receive_data
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_SR_DATA_ACK, i2c_srp_error
	LDS R11, TWDR
	
	; Receive the third data byte and store in R12
	i2c_slave_start_receive_data
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_SR_DATA_ACK, i2c_srp_error
	LDS R12, TWDR
	
	; Receive the fourth data byte and store in R13
	i2c_slave_start_receive_data
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_SR_DATA_ACK, i2c_srp_error
	LDS R13, TWDR
	
	; Receive the last data byte and store in R14
	i2c_slave_start_receive_data
	RCALL i2c_wait_int_flag
	i2c_check_status I2C_SR_DATA_ACK, i2c_srp_error
	LDS R14, TWDR
	
	;
	; END RECEPTION
	;
	; Exit the slave listening mode
	i2c_slave_exit_listening_mode
	
	; Clear R15 to indicate Success exit status
	CLR R15

	RET
	
	; In case of error => exit the slave listening mode and enable BOTTOM LED

	i2c_srp_error:
		; Exit the slave listening mode
		i2c_slave_exit_listening_mode

		; LED ON if error
		CBI PORTC,3

		; Store 0xFF in R15 to indicate Error exit status
		LDI R16, 0xFF
		MOV R15, R16

	RET
