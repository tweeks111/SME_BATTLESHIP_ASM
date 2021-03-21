;
; Battleship.asm
;
;   Authors: Mathieu Philipart
;			 Théo    Lepoutte
;

.INCLUDE "m328pdef.inc"
.ORG 0x0000
RJMP init
.ORG 0x001A
RJMP Timer1OverflowInterrupt
.ORG 0x0020
RJMP Timer0OverflowInterrupt

.INCLUDE "screen.inc"
.INCLUDE "keyboard.inc"
.INCLUDE "timer.inc"


init:
    RCALL init_screen
	RCALL init_timer0
	RCALL init_timer1
	SEI

	

