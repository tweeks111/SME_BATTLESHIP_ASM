;
; Communication.inc
;
; This file is part of the Battleship project.
;
; It contains all the functions needed communicate two boards using the 
; I²C protocol (detailed in the external document: I²C communication protocol).
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

; Packet IDs
#define MASTER_DISCOVERY_ID		0x01
#define MASTER_SHIPS_READY_ID	0x03
#define MASTER_PLAYER_TURN_ID	0x0A
#define MASTER_SHOT_LAUNCH_ID	0x10
#define MASTER_SHOT_SPLASH_ID	0x12
#define MASTER_SHOT_HIT_ID		0x14
#define MASTER_SHOT_SANK_ID		0x16
#define MASTER_GAME_WON_ID		0xA0

#define SLAVE_DISCOVERY_ID		0x02
#define SLAVE_SHIPS_READY_ID	0x04
#define SLAVE_SHOT_LAUNCH_ID	0x11
#define SLAVE_SHOT_SPLASH_ID	0x13
#define SLAVE_SHOT_HIT_ID		0x15
#define SLAVE_SHOT_SANK_ID		0x17


comm_master_discovery:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sends the discovery packet periodically from the 
	; master until a slave ACK it. Then, it waits for the slave to 
	; transmit its discovery packet.
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Send a discovery packet
	i2c_master_send_cst_packet MASTER_DISCOVERY_ID, 0x00, 0x00, 0x00, 0x00
	
	; The function i2c_master_send_cst_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE cmd_retry_again				; Retry after 1s if no slave ACKed
	
	; Wait for stop condition of the previous transmission to be sent correctly
	sleep_ts 1

	; Here, a slave ACKed the master's packet.
	; Now wait for the slave to transmit a discovery packet
	RCALL i2c_master_receive_packet

	; The function i2c_master_receive_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE cmd_retry_again				; Retry after 1s if the read wasn't successful
	
	; Here, the slave transmitted something to the master
	
	LDI R16, SLAVE_DISCOVERY_ID			; 0x02 is the slave discovery packet ID
	CP R10, R16							; R10 is the first packet byte (packet ID)
	BRNE cmd_retry_again				; Retry after 1s if the received packet wasn't the expected one

	; Here, a slave has been recognised as a second game instance.
	; The discovery procedure is finished.

	RET
	
	cmd_retry_again:
	; If we fall to this label, go back to the beginning after 1s
	sleep_ts 10
	RJMP comm_master_discovery

	
comm_slave_discovery:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function waits for a discovery packet sent from a master.
	; Then, it waits for a ST request to write its discovery packet.
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Wait and get the discovery packet from the master
	RCALL i2c_slave_receive_packet

	; The function i2c_slave_receive_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE csd_retry_again				; Retry after 0.1s if something was wrong
	
	; Here, the master transmitted something to the slave

	LDI R16, MASTER_DISCOVERY_ID		; 0x01 is the master discovery packet ID
	CP R10, R16							; R10 is the first packet byte (packet ID)
	BRNE csd_retry_again				; Retry after 0.1s if the received packet wasn't the expected one
	
	; Now, we must wait for a slave transmit request from the master and send slave discovery packet
	
	; Wait and transmit the discovery packet to the master
	i2c_slave_send_cst_packet SLAVE_DISCOVERY_ID, 0, 0, 0, 0
	
	; The function i2c_slave_transmit_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE csd_retry_again				; Retry after 0.1s if something was wrong
	
	; Here, a master has been recognised as a first game instance.
	; The discovery procedure is finished.

	RET
	
	csd_retry_again:
	; If we fall to this label, go back to the beginning after 0.1s
	sleep_ts 1
	RJMP comm_slave_discovery


comm_master_ship_placement_done:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sends the Ships Ready packet to the slave and waits
	; for the slave to read the packet data. Then, it sends a ST request
	; and wait for the slave to transmit its Ships Ready packet.
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Send the Ships Ready packet
	i2c_master_send_cst_packet MASTER_SHIPS_READY_ID, 0x00, 0x00, 0x00, 0x00
	
	; The function i2c_master_send_cst_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE cmspd_retry_again				; Retry after 1s if no slave ACKed
	
	; Wait for stop condition of the previous transmission to be sent correctly
	sleep_ts 1
	
	; Here, a slave ACKed the master's packet.
	; Now wait for the slave to transmit a Ships Ready packet
	RCALL i2c_master_receive_packet

	; The function i2c_master_receive_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE cmspd_retry_again				; Retry after 1s if the read wasn't successful
	
	; Here, the slave transmitted something to the master
	
	LDI R16, SLAVE_SHIPS_READY_ID		; 0x04 is the slave discovery packet ID
	CPSE R10, R16						; R10 is the first packet byte (packet ID)
	RJMP comm_unexpected_packet_id		; Fall to comm_unexpected_packet_id if R10 != R16
	
	; Here, master and slave both finished the ships placement procedure

	RET

	cmspd_retry_again:
	; If we fall to this label, go back to the beginning after 1s
	sleep_ts 10
	RJMP comm_master_ship_placement_done


comm_slave_ship_placement_prepare:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function prepare the slave for ship placement. After a call to
	; this function, the I2C subsystem will automatically ACK the next
	; SLA+R/W from the master.
	; NOTE: this function uses: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Start listening the I2C bus (non-blocking)
	i2c_slave_enter_listening_mode

	RET
	
comm_slave_ship_placement_done:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function reads the Ships Ready packet from the master then 
	; waits for the ST request to be sent by the master to transmit its 
	; Ships Ready packet.
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Wait and get the Ships Ready packet from the master
	RCALL i2c_slave_receive_packet_no_listening

	; The function i2c_slave_receive_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE csspd_retry_again				; Retry after 0.1s if something was wrong
	
	; Here, the master transmitted something to the slave

	LDI R16, MASTER_SHIPS_READY_ID		; 0x01 is the master discovery packet ID
	CPSE R10, R16						; R10 is the first packet byte (packet ID)
	RJMP comm_unexpected_packet_id		; Fall to comm_unexpected_packet_id if R10 != R16
	
	; Now, we must wait for a slave transmit request from the master and send slave discovery packet
	
	; Wait and transmit the Ships Ready packet to the master
	i2c_slave_send_cst_packet SLAVE_SHIPS_READY_ID, 0, 0, 0, 0
	
	; The function i2c_slave_transmit_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE csspd_retry_again				; Retry after 0.1s if something was wrong
	
	; Here, master and slave both finished the ships placement procedure

	RET
	
	csspd_retry_again:
	; If we fall to this label, go back to the beginning after 0.1s
	sleep_ts 1
	RJMP comm_slave_ship_placement_done






comm_unexpected_packet_id:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; If this label is reached, it means that an unexpected packet ID has
	; been received. This label make the program fall in an infinite loop
	; making the two LEDs blinking alternatively.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Init state of LEDs
	CBI PORTC,2
	SBI PORTC,3
	
	; Infinite loop
	cupi_loop:
		; Wait 0.5s
		sleep_ts 5

		; Toggle LEDs state
		SBI PINC,2
		SBI PINC,3

		RJMP cupi_loop