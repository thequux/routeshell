format binary as "bin"
use32
	
	org 0x100000		; 1MB
_img_start:
	include "util.inc"
	include "multiboot.inc"
	include "io.inc"

VGA equ 0xB8000
;;; Entry point
_start:	
	cmp	eax,0x2BADB002
	JNZ	boot_failed
	MOV	ESI, EBX
	MOV	EDI, mbinfo
	MOV	ECX, sizeof.multiboot_info
	CLD
	REP MOVSB
	MOV	ESP, kern_stack.top

	MOV	DX, 0xe9
	MOV	AL, 'q'
	OUT	DX, AL
	;; Test to see if kernel got this far
	CALL	iocls
	PUSH	TEST_STR
	CALL	ioputstr
	;; MOV	word [VGA], 0x1F41
	CLI
@@:	HLT
	JMP 1b

boot_failed:
	INT 1
	;; Set up IDT
	
	


TEST_STR	ds "This is a test", 0x0d, 0x0a, 0x00

_img_end:
mbinfo	multiboot_info
kern_stack:
  .bot:	rb 0x10000 	; 16kb
  .top = $
_bss_end:	
