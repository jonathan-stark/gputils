; HD42.ASM

; This file tests header file usage for the specified processor.

	LIST	P=17C42
	INCLUDE	"p17c42.inc"

 	DATA	_PMC_MODE,	0XFFAF		; This changes per device
	DATA	_XMC_MODE,	0XFFBF
	DATA	_MC_MODE,	0XFFEF
	DATA	_MP_MODE,	0XFFFF
	DATA	_WDT_NORM,	0XFFF3
	DATA	_WDT_64,	0XFFF7
	DATA	_WDT_256,	0XFFFB
	DATA	_WDT_1,		0XFFFF
	DATA	_LF_OSC,	0XFFFC
	DATA	_RC_OSC,	0XFFFD
	DATA	_XT_OSC,	0XFFFE
	DATA	_EC_OSC,	0XFFFF

	DATA	INDF0, 0			; Register files
	DATA	FSR0, 1				; Unbanked
	DATA	PCL, 2
	DATA	PCLATH, 3
	DATA	ALUSTA, 4
	DATA	T0STA, 5
	DATA	CPUSTA, 6
	DATA	INTSTA, 7
	DATA	INDF1, 8
	DATA	FSR1, 9
	DATA	WREG, 0XA
	DATA	TMR0L, 0XB
	DATA	TMR0H, 0XC
	DATA	TBLPTRL, 0XD
	DATA	TBLPTRH, 0XE
	DATA	BSR, 0XF

	DATA	PORTA, 10			; Bank 0
	DATA	DDRB, 11
	DATA	PORTB, 12	
	DATA	RCSTA, 13
	DATA	RCREG, 14
	DATA	TXSTA, 15
	DATA	TXREG, 16
	DATA	SPBRG, 17

	DATA	DDRC, 110			; Bank 1
	DATA	PORTC, 111
	DATA 	DDRD, 112
	DATA	PORTD, 113
	DATA	DDRE, 114
	DATA	PORTE, 115
	DATA	PIR, 116
	DATA	PIE, 117

	DATA	TMR1, 210			; Bank 2
	DATA	TMR2, 211
	DATA	TMR3L, 212
	DATA	TMR3H, 213
	DATA	PR1, 214
	DATA	PR2, 215
	DATA	PR3L, 216
	DATA	PR3H, 217


	DATA	PW1DCL, 310			; Bank 3
	DATA	PW2DCL, 311
	DATA	PW1DCH, 312
	DATA	PW2DCH, 313
	DATA 	CA2L, 314
	DATA	CA2H, 315
	DATA	TCON1, 316
	DATA	TCON2, 317

	DATA	FS3, 7			; ALU status Register
	DATA	FS2, 6
	DATA	FS1, 5
	DATA	FS0, 4
	DATA	OV, 3
	DATA	Z, 2
	DATA	DC, 1
	DATA	C, 0

	DATA	STKAV, 5		; CPU Status Register
	DATA 	GLINTD, 4
	DATA	NOT_TO, 3
	DATA	NOT_PD, 2

	DATA	PEIF, 7			; Interrupt Status Register
	DATA	T0CKIF, 6
	DATA	T0IF, 5
	DATA	INTF, 4
	DATA	PEIE, 3
	DATA	T0CKIE, 2
	DATA	T0IE, 1
	DATA	INTE, 0	

	
	DATA	PEIF,7			; PIE Register
	DATA	T0CKIF, 6
	DATA	T0IF, 5
	DATA	INTF, 4
	DATA	PEIE, 3
	DATA	T0CKIE, 2
	DATA	T0IE, 1
	DATA	INTE, 0

	DATA	RBIF, 7			; PIR Register
	DATA	TMR3IF, 6
	DATA	TMR2IF, 5
	DATA	TMR1IF, 4
	DATA	CA2IF, 3
	DATA	CA1IF, 2
	DATA	TXIF, 1
	DATA	RCIF, 0

	DATA	NOT_RBPU, 7		; Port A
	DATA	T0CKI, 1
	DATA	INT, 0

	DATA	SPEN, 7			; RCSTA
	DATA	RC9, 6
	DATA	NOT_RC8, 6
	DATA	RC8_9, 6
	DATA	SREN, 5
	DATA	CREN, 4
	DATA	FERR, 2
	DATA	OERR, 1
	DATA	RCD8, 0

	DATA	INTEDG, 7		 ; T0STA
	DATA	T0SE, 6
	DATA	T0CS, 5
	DATA	T0PS3, 4
	DATA	T0PS2, 3
	DATA	T0PS1, 2
	DATA	T0PS0, 1

	DATA	CA2ED1, 7		; TCON1
	DATA	CA2ED0, 6
	DATA	CA1ED1, 5
	DATA	CA1ED0, 4
	DATA	T16, 3
	DATA	TMR3CS, 2
	DATA	TMR2CS, 1
	DATA	TMR1CS, 0

	DATA	CA2OVF, 7		; TCON2
	DATA	CA1OVF, 6
	DATA	PWM2ON, 5
	DATA	PWM1ON, 4
	DATA	CA1, 3
	DATA	NOT_PR3, 3
	DATA	CA1_PR3, 3
	DATA	TMR3ON, 2
	DATA	TMR2ON, 1
	DATA	TMR1ON, 0

	DATA	CSRC, 7			; TXSTA 
	DATA	TX9, 6
	DATA	NOT_TX8, 6
	DATA	TX8_9, 6
	DATA	TXEN, 5
	DATA	SYNC, 4
	DATA	TRMT, 1
	DATA	TXD8, 0	

	END
