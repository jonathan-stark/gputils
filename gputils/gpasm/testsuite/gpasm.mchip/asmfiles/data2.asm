; DATA2.ASM

; This file tests legal maximum uses of the various data directives.

	LIST	P=17C44, R=DEC

	DATA	0
	DATA	32767
	DATA	32768
	DATA	-32767
	DATA	-32768
	DATA	65535

	DW	0
	DW	32767
	DW	32768
	DW	-32767
	DW	-32768
	DW	65535

	DB	0
	DB	127
	DB	128
	DB	-127
	DB	-128
	DB	255

	DE	0
	DE	127
	DE	128
	DE	-127
	DE	-128
	DE	255

	DT	0
	DT	127
	DT	128
	DT	-127
	DT	-128
	DT	255

	END
