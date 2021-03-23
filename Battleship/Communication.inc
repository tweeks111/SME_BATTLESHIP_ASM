;
; Communication.inc
;
; This file is part of the Battleship project.
;
; It contains all the functions needed communicate two boards using the 
; I²C protocol (detailed in the external document: I²C communication protocol).
;
; Authors : Mathieu Philippart & Théo Lepoutte
;


comm_master_discovery:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; This function sends the discovery packet periodically from the 
	; master until a slave ACK it. Then, it waits for the slave to 
	; transmit its discovery packet.
	; NOTE: this function uses: R16, R17
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Send a discovery packet
	i2c_master_send_cst_packet 0x01, 0x00, 0x00, 0x00, 0x00
	
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
	
	LDI R16, 0x02						; 0x02 is the slave discovery packet ID
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

	LDI R16, 0x01						; 0x01 is the master discovery packet ID
	CP R10, R16							; R10 is the first packet byte (packet ID)
	BRNE csd_retry_again				; Retry after 0.1s if the received packet wasn't the expected one

	CBI PORTC,2

	; Now, we must wait for a slave transmit request from the master and send slave discovery packet
	
	; Wait and transmit the discovery packet to the master
	i2c_slave_send_cst_packet 0x02, 0, 0, 0, 0
	
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