;  This file is used to test gpdasm with the 14 bit enhanced instruction set.

	list p=12f1501

	include p12f1501.inc

	__config _CONFIG1, _FOSC_ECL & _WDTE_SWDTEN & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _BOREN_SBODEN & _CLKOUTEN_OFF
	__config _CONFIG2, _WRT_BOOT & _STVREN_ON & _BORV_HI & _LPBOR_OFF & _LVP_OFF

	__idlocs 0x1234

moviw_test macro fsr
	MOVIW	++fsr
	MOVIW	--fsr
	MOVIW	fsr++
	MOVIW	fsr--
	MOVIW	0[fsr]
	MOVIW	0x1f[fsr]
	MOVIW	-0x20[fsr]
    endm

movwi_test macro fsr
	MOVWI	++fsr
	MOVWI	--fsr
	MOVWI	fsr++
	MOVWI	fsr--
	MOVWI	0[fsr]
	MOVWI	0x1f[fsr]
	MOVWI	-0x20[fsr]
    endm

insn macro file 
	ADDFSR	FSR0, 0x1f
	ADDFSR	FSR0, -0x20
	ADDFSR	FSR1, 0x1f
	ADDFSR	FSR1, -0x20
	ADDLW	0xff
	ADDWF	file, F
	ADDWFC	file, F
	ANDLW	0xff
	ANDWF	file, F
	ASRF	file, F
	BCF	file, 7
	BRA	0xff
	BRW 
	BSF	file, 7
	BTFSC	file, F
	BTFSS	file, F
	CALL	0x100
	CALLW 
	CLRF	file
	CLRW
	CLRWDT 
	COMF	file, F
	DECF	file, F
	DECFSZ	file, F
	GOTO	0x100
	INCF	file, F
	INCFSZ	file, F
	IORLW	0xff
	IORWF	file, F
	LSLF	file, F
	LSRF	file, F
	MOVF	file, F
	moviw_test FSR0
	moviw_test FSR1
	MOVLB	0x1f
	MOVLP	0x7f
	MOVLW	0xff
	MOVWF	file
	movwi_test FSR0
	movwi_test FSR1
	NOP 
;	OPTION 
	RESET 
	RETFIE 
	RETLW	0xff
	RETURN 
	RLF	file, F
	RRF	file, F
	SLEEP 
	SUBLW	0xff
	SUBWF	file, F
	SUBWFB	file, F
	SWAPF	file, F
;	TRIS	5
	XORLW	0xff
	XORWF	file, F
    endm

	org	0
	insn	0

	org	0x80
	insn	.10

	org	0x100
	insn	.20

	end
