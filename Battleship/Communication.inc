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



comm_slave_exchange_prepare:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function prepare the slave for an exchange. After a call to
	; this function, the I2C subsystem will automatically ACK the next
	; SLA+R/W from the master.
	; NOTE: this function uses: R16
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Start listening the I2C bus (non-blocking)
	i2c_slave_enter_listening_mode

	RET


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
	
	LDI R16, SLAVE_DISCOVERY_ID			; Expected Packet ID
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

	LDI R16, MASTER_DISCOVERY_ID		; Expected Packet ID
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
	
	LDI R16, SLAVE_SHIPS_READY_ID		; Expected Packet ID
	CPSE R10, R16						; R10 is the first packet byte (packet ID)
	RJMP comm_unexpected_packet_id		; Fall to comm_unexpected_packet_id if R10 != R16
	
	; Here, master and slave both finished the ships placement procedure

	RET

	cmspd_retry_again:
	; If we fall to this label, go back to the beginning after 1s
	sleep_ts 10
	RJMP comm_master_ship_placement_done

	
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

	LDI R16, MASTER_SHIPS_READY_ID		; Expected Packet ID
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


comm_master_send_shooting_launch_packet:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sends a Shooting Launch packet with the coordinate of
	; the shot from the master to the slave.
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; The shot coordinate is the current cursor's position
	get_cursor_position
	; Returns: R11						; Packet byte 1
	; Returns: R12						; Packet byte 2

	; Send the Shooting Launch packet
	LDI R16, MASTER_SHOT_LAUNCH_ID		; Packet ID
	MOV R10, R16
	CLR R13								; Packet byte 3 (unused)
	CLR R14								; Packet byte 4 (unused)
	RCALL i2c_master_transmit_packet
	
	; The function i2c_master_send_cst_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE cmsslp_retry_again				; Retry after 1s if no slave ACKed
	
	; Wait for stop condition of the previous transmission to be sent correctly
	sleep_ts 1

	; Now wait for the slave to transmit an answer to the Shooting Launch packet
	RCALL i2c_master_receive_packet

	; The function i2c_master_receive_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE cmsslp_retry_again				; Retry after 1s if the read wasn't successful
	
	; Here, we received the answer from the slave
	; This answer can take multiple packet IDs
	
	;TODO: react to following packet IDs: SLAVE_SHOT_SPLASH_ID, SLAVE_SHOT_HIT_ID, SLAVE_SHOT_SANK_ID

	RET

	cmsslp_retry_again:
	; If we fall to this label, go back to the beginning after 1s
	sleep_ts 10
	RJMP comm_master_send_shooting_launch_packet


comm_master_receive_shooting_launch_packet:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function receives a Shooting Launch packet with the coordinate 
	; of the shot executed by the slave (waits for slave's answer).
	; The slave should directly ACK this request, then send data bytes 
	; when it is ready.
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; Wait for the slave to transmit a Shooting Launch packet
	RCALL i2c_master_receive_packet
	
	; The function i2c_master_receive_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE cmrslp_retry_again				; Retry after 0.1s if something was wrong
	
	; Here, the slave transmitted something to the master

	LDI R16, SLAVE_SHOT_LAUNCH_ID		; Expected Packet ID
	CPSE R10, R16						; R10 is the first packet byte (packet ID)
	RJMP comm_unexpected_packet_id		; Fall to comm_unexpected_packet_id if R10 != R16
	
	; React on this shot
	RCALL game_enemy_fired_position

	RET
	
	cmrslp_retry_again:
	; If we fall to this label, go back to the beginning after 1s
	sleep_ts 10
	RJMP comm_master_receive_shooting_launch_packet


comm_slave_send_shooting_launch_packet:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function transmit a Shooting Launch packet with the coordinate 
	; of the shot from the slave to the master (waits for ST request).
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	; The shot coordinate is the current cursor's position
	get_cursor_position
	; Returns: R11						; Packet byte 1
	; Returns: R12						; Packet byte 2
	
	; Send the Shooting Launch packet
	LDI R16, SLAVE_SHOT_LAUNCH_ID		; Packet ID
	MOV R10, R16
	CLR R13								; Packet byte 3 (unused)
	CLR R14								; Packet byte 4 (unused)
	RCALL i2c_slave_transmit_packet
	
	; The function i2c_slave_send_cst_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE cssslp_retry_again				; Retry after 0.1s if something was wrong

	; Now, we must wait for the master's reaction to this Shooting Launch packet
	
	; Wait and receive master's reaction packet
	RCALL i2c_slave_receive_packet
	
	; Here, we received the answer from the master
	; This answer can take multiple packet IDs
	
	;TODO: react to following packet IDs: MASTER_SHOT_SPLASH_ID, MASTER_SHOT_HIT_ID, MASTER_SHOT_SANK_ID
	

	RET

	cssslp_retry_again:
	; If we fall to this label, go back to the beginning after 1s
	sleep_ts 10
	RJMP comm_slave_send_shooting_launch_packet


comm_slave_receive_shooting_launch_packet:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function receives a Shooting Launch packet with the coordinate 
	; of the shot executed by the master (waits for SR request).
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Wait and get the Ships Ready packet from the master
	RCALL i2c_slave_receive_packet

	; The function i2c_slave_receive_packet returns 0x00 in R15 if success.
	CLR R16
	CP R15, R16
	BRNE csrslp_retry_again				; Retry after 0.1s if something was wrong
	
	; Here, the master transmitted something to the slave

	LDI R16, MASTER_SHOT_LAUNCH_ID		; Expected Packet ID
	CPSE R10, R16						; R10 is the first packet byte (packet ID)
	RJMP comm_unexpected_packet_id		; Fall to comm_unexpected_packet_id if R10 != R16
	
	; React on this shot
	RCALL game_enemy_fired_position

	RET
	
	csrslp_retry_again:
	; If we fall to this label, go back to the beginning after 1s
	sleep_ts 10
	RJMP comm_slave_receive_shooting_launch_packet



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