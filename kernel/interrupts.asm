%include "doall.inc"

extern isr_handler

; Common handler for the ISRs
isr_common_format:
	pushall
	cld
	mov rdi, rsp
	call isr_handler
	popall
	add rsp, 24
	iretq

%macro def_isr 1

global isr%1
isr%1:
	push 0
	push %1
	push fs
	jmp isr_common_format

%endmacro

%macro def_err_isr 1

global isr%1
isr%1:
	push %1
	push fs
	jmp isr_common_format

%endmacro

%define has_errcode(i) (i == 8 || (i >= 10 && i <= 14) || i == 17 || i == 21)

%assign i 0
%rep 32
%if !has_errcode(i)
	def_isr i
%else
	def_err_isr i
%endif
%assign i i + 1
%endrep
