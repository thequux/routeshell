;; Much of this was stolen from http://board.flatassembler.net/topic.php?t=93
; --- Tomasz's useful macro: 
macro struct name 
 { virtual at 0 
   name name 
   sizeof.#name = $ - name 
   name equ sizeof.#name 
   end virtual }
struc multiboot_info 
{ 
        .flags          dd      ? 
        .mem_lower      dd      ? 
        .mem_upper      dd      ? 
        .boot_device    rb      4 
        .cmdline        dd      ? 
        .mods_count     dd      ? 
        .mods_addr      dd      ? 
        ;-- ELF specific info: 
        .num            dd      ? 
        .size_          dd      ? 
        .addr_          dd      ? 
        .shndx          dd      ? 
        ;---- 
        .mmap_length    dd      ? 
        .mmap_addr      dd      ? 
        .size=$-.flags 
} 
struct multiboot_info

_mbheader:
	dd 0x1BADB002 		; magic
	dd (1 or (1 shl 16))
.ck = 0
repeat 	2
	load .bv dword from _mbheader+(%-1)*4
	.ck = .ck - .bv
end repeat
	dd .ck			; checksum
	dd _mbheader		; header_addr
	dd _img_start		; load_addr
	dd _img_end		; load_end_addr
	dd _bss_end		; bss_end_addr
	dd _start		; entry point
