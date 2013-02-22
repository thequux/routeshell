; -*- mode: asm -*-
iodata:	
.row	db	0
.col	db	0
.color	db	0x1F 		;bg[BGRI]fg[BGRI] -- blue bg, white fg
iodata.vga	equ	0xB8000

delay:
	push	ECX
	mov	ECX, 0x200000
	@@: NOP
	LOOP	@b
	POP	ECX
	RET
	
ioputcraw:
	;; Place a character in raw mode.
	;; Args: x,y,c; each is a dword
	PUSH	EBP
	MOV	EBP, ESP
	PUSH	EAX
	PUSH	EBX
	MOV	EBX, [EBP+12]
	IMUL	EBX, 160
	MOV	EAX, [EBP+8]
	LEA	EBX, [iodata.vga + EBX + EAX * 2]
	MOV	EAX, [EBP+16]
	MOV	AH, [iodata.color]
	MOV	[EBX], AX

	POP	EBX
	POP	EAX
	POP	EBP
	RET

iosetcur:
	;; Set cursor position
	;; Args: x,y (as dwords)
	;; TODO: This could almost certainly be done with less code.
	PUSH	EBP
	MOV	EBP, ESP
	PUSH	EAX, EBX, ECX, EDX

	MOV	EAX, [EBP+8] 	; x
	MOV	EBX, [EBP+12] 	; y
	MOV	[iodata.col], AL
	MOV	[iodata.row], BL
	IMUL	EBX, 80
	ADD	EBX, EAX

	MOV	DX, 0x3CC
	IN	AL, DX
	MOV	CX, 0x3B4
	TEST	AL, 1
	JZ	@f
	MOV	CX, 0x3D4
@@:	MOV	DX, CX
	MOV	AL, 0x0f
	OUT	DX, AL
	INC	DX
	MOV	AL, BL
	OUT	DX, AL
	DEC	DX
	MOV	AL, 0xe
	OUT	DX, AL
	INC	DX
	MOV	AL, BH
	OUT	DX, AL
	
	POP	EAX, EBX, ECX, EDX
	POP	EBP
	RET
iocls:
	;; No args
	PUSH	EBP
	MOV	EBP, ESP
	PUSH	EAX, EBX, ECX

	MOV	ECX, 80*25
	MOV	AH, [iodata.color]
	MOV	AL, 0x20
	;MOV	EBX, iodata.VGA
@@:
	MOV	[iodata.vga + ECX * 2], AX
	LOOP	@b

	XOR	EAX, EAX
	PUSH	EAX
	PUSH	EAX
	CALL	iosetcur
	ADD	ESP, 8
	
	POP	EAX, EBX, ECX
	POP	EBP
	RET

ioputstr:
	;; Put a null-terminated string to the console
	;; Args: str
	PUSH	EBP
	MOV	EBP, ESP
	PUSH	EAX, EBX, ECX, EDX, ESI
	
	MOV	ESI, [EBP+8]
	SUB	ESP, 12

.resume:
	MOVZX	ECX, [iodata.row]
	MOVZX	EBX, [iodata.col]
.nchr:  ;CALL 	delay
	MOV	[ESP], EBX
	MOV	[ESP+4], ECX
	LODS	byte [ESI]
	TEST	AL, AL
	JZ	.end
	CMP	AL, 0x0d
	JZ	.cr
	CMP	AL, 0x0a
	JZ	.lf
	MOVZX	EAX, AL
	MOV	[ESP+8], EAX
	;; TODO: Handle newline, tab
	CALL	ioputcraw
	CALL	iosetcur
	INC	EBX
	CMP	EBX, 80
	JL	.nchr
	INC	ECX
	XOR	EBX, EBX
	JMP	.nchr
.cr:	XOR	EBX, EBX
	JMP	.nchr
.lf:	INC	ECX
	CMP	ECX, 25
	JL	.nchr
	PUSH	dword 1
	CALL	ioscroll
	ADD	ESP, 4
	DEC	ECX
	JMP	.nchr
.end:	CALL	iosetcur
	ADD	ESP, 12
	POP	EAX, EBX, ECX, EDX, ESI
	POP	EBX
	RET
	
ioscroll:
	;; Scroll display
	;; arg: number of lines
	CLD
	PUSH	EBP
	MOV	EBP, ESP
	PUSH	EAX, ECX, ESI, EDI

	MOV	ECX, [EBP+8]
	IMUL	ECX, 80	
	PUSH	ECX		; ECX contains number of words delta
	LEA	ESI, [iodata.vga + ECX * 2]
	MOV	EDI, iodata.vga
	NEG	ECX
	ADD	ECX, 25 * 80
@@:	LODS	word [ESI]
	STOS	word [EDI]
	LOOP	@b
	POP	ECX
	MOV	AH, [iodata.color]
	MOV	AL, 0x20
	REP STOSW

	POP	EAX, ECX, ESI, EDI
	POP	EBP
	RET
