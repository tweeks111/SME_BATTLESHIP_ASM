;
; ProgMemTables.inc
;
; This file is part of the Battleship project.
;
; It contains all the tables stored into the Program Memory.
; This includes the Sounds, the Words (for the screen), and the characters (for the screen).
; This file should be included first to avoid excessive "Relative branch out of reach" situations.
;
; Authors: Mathieu Philippart & Théo Lepoutte
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BUZZER TONES FREQUENCIES DEFINITIONS
; Tones frequencies -> TCNT value for prescaler 256 (see Excel document)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#define _SI_1	3
#define _DO_2	17
#define _REb_2	31
#define _RE_2	43
#define _MIb_2	55
#define _MI_2	66
#define _FA_2	77
#define _SOLb_2	87
#define _SOL_2	97
#define _LAb_2	106
#define _LA_2	114
#define _SIb_2	122
#define _SI_2	129
#define _DO_3	137
#define _REb_3	143
#define _RE_3	150
#define _MIb_3	156
#define _MI_3	161
#define _FA_3	167
#define _SOLb_3	172
#define _SOL_3	176
#define _LAb_3	181
#define _LA_3	185
#define _SIb_3	189
#define _SI_3	193
#define _DO_4	196
#define _REb_4	200
#define _RE_4	203
#define _MIb_4	206
#define _MI_4	209
#define _FA_4	211
#define _SOLb_4	214
#define _SOL_4	216
#define _LAb_4	218
#define _LA_4	220
#define _SIb_4	222
#define _SI_4	224

#define _SHUSH	255		; Silence


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SOUND TABLES
; NOTE: each sound table must end with two zero-bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Sound_Test:
	; Test sound
	.db _DO_3, 2,  _RE_3, 2,  _MI_3, 2,  _DO_3, 2,  _RE_3, 2, _SHUSH, 2,	\
		_RE_3, 2,  _MI_3, 2,  _FA_3, 2,  _SHUSH, 2,							\
		_FA_3, 2,  _SHUSH, 2,												\
		_MI_3, 2,  _SHUSH, 2,												\
		_MI_3, 2,  _SHUSH, 2,												\
		_DO_3, 2,  _RE_3, 2,  _MI_3, 2,  _DO_3, 2,  _RE_3, 2, _SHUSH, 2,	\
		_RE_3, 2,  _MI_3, 2,  _FA_3, 2,  _SHUSH, 2,							\
		_SOL_3, 2, _SHUSH, 2,												\
		_DO_3, 2,  _SHUSH, 2, 0, 0
		
Sound_Intro:
	; Intro sound
	.db _SOL_3, 2, _SHUSH, 1, _SOL_3, 1, _DO_4, 5, _SHUSH, 1, _DO_3, 1, _DO_3, 1, _DO_3, 2, 0, 0

Sound_Intro_Long:
	; Long Intro sound
	.db _SOL_3, 2, _SHUSH, 1, _SOL_3, 1, _DO_4, 5, _SHUSH, 1, _SOL_3, 2,  _RE_4, 3,  _SHUSH, 1,						\
		_DO_4, 2,  _SHUSH, 1, _SI_3, 1,  _DO_4, 4, _SHUSH, 1,  _DO_3, 2,  _SHUSH, 1, _DO_3, 1, _DO_3, 2, 0, 0

Sound_Winner:
	; Winner sound
	.db _SOL_3, 1, _SOL_3, 1, _SOL_3, 2, _SOL_3, 2, _DO_4, 2, _SOL_3, 2, _DO_4, 4, 0, 0
	
Sound_Looser:
	; Looser sound
	.db _LA_3, 2, _LAb_3, 2, _SOL_3, 2, _SOLb_3, 2, _FA_3, 8, 0, 0
		
Sound_PlacementRefused:
	; Placement Refused sound
	.db _DO_2, 1, _DO_2, 1, 0, 0
		
Sound_PlacementAccepted:
	; Placement Accepted sound
	.db _SOL_3, 1, _DO_4, 1, 0, 0
		
Sound_FiringRefused:
	; Firing Refused sound
	.db _DO_2, 1, _DO_2, 1, 0, 0
	
Sound_FireLaunch:
	; Fire Launch sound
	.db _FA_4, 1, _MI_4, 1, _MIb_4, 1, _RE_4, 1, _REb_4, 1, _DO_4, 1, 0, 0
	
Sound_ShotMissed:
	; Shot Missed sound
	.db _FA_4, 2, _SHUSH, 1, _DO_4, 4, 0, 0
	
Sound_ShotHit:
	; Shot Hit sound
	.db _DO_4, 1, _SHUSH, 1, _DO_4, 1, _FA_4, 2, 0, 0
		
Sound_ShotSunk:
	; Shot Sunk sound
	.db _DO_4, 1, _SHUSH, 1, _LA_4, 1, _FA_4, 3, 0, 0
	
Sound_ShotEnemyMissed:
	; Shot Enemy Missed sound
	.db _SOL_3, 1, _SI_3, 3, 0, 0

Sound_ShotEnemyHit:
	; Shot Enemy Hit sound
	.db _SOL_3, 2, _SHUSH, 2, _SOL_3, 2, _DO_3, 4, 0, 0

Sound_ShotEnemySunk:
	; Shot Enemy Sunk sound
	.db _SOL_2, 2, _SHUSH, 2, _SOL_2, 2, _DO_3, 4, _SHUSH, 2, _DO_2, 4, 0, 0

	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; CHARACTERS DEFINITIONS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; CHARACTERS TABLE
; NOTE: each char has a size of 6 bytes (last by is even-padding)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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
	.db 0b11100000, 0b10100000, 0b11100000, 0b10000000, 0b10000000, 0b00000000 ;P
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
	
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; WORDS TABLES
; NOTE: a word is terminated by _WORD_END (some have trailing 0 for even-padding)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
WordPlayer1:
	.db _P, _L, _A, _Y, _E, _R, _SMALL_SPACE, 1, _WORD_END, 0		; Add a zero padding to keep even number of bytes (16-bit alignment)
	
WordPlayer2:
	.db _P, _L, _A, _Y, _E, _R, _SMALL_SPACE, 2, _WORD_END, 0		; Add a zero padding to keep even number of bytes (16-bit alignment)
	
WordBattleship:
	.db _B, _A, _T, _T, _L, _E, _S, _H, _I, _P, _WORD_END, 0		; Add a zero padding to keep even number of bytes (16-bit alignment)
	
WordWaitingFor:
	.db _W, _A, _T, _I, _N, _G, _SMALL_SPACE, _F, _O, _R, _WORD_END, 0

WordMissed:
	.db _M, _I, _S, _S, _E, _D, _WORD_END, 0
	
WordHit:
	.db _H, _I, _T, _EXC, _WORD_END, 0
	
WordSunk:
	.db _S, _U, _N, _K, _EXC, _WORD_END
	
WordGo:
	.db _G, _O, _EXC, _WORD_END
	
WordEnemyShot:
	.db _E, _N, _E, _M, _Y, _SMALL_SPACE, _S, _H, _O, _T, _WORD_END, 0
	
WordYouWon:
	.db _Y, _O, _U, _SPACE, _W, _O, _N, _EXC, _WORD_END, 0

WordYouLost:
	.db _Y, _O, _U, _SPACE, _L, _O, _S, _T, _EXC, _WORD_END
