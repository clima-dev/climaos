%include "doall.inc"

extern isr_handler

; Common handler for the ISRs
isr_common_format:
	pushall
	mov  r15, rsp  ; stash rsp somewhere it will be safe
	and  rsp, ~0xf ; C expects a 16-byte aligned stack
	cld            ;           and DF=0
	mov  rdi, r15  ; pointer to `regs_t`
	call isr_handler
	mov  rsp, r15  ; restore stack pointer
	popall
	add  rsp, 16
iretq

%macro def_isr 1

global isr%1
isr%1:
	push 0
	push %1
	jmp isr_common_format

%endmacro

%macro def_err_isr 1

global isr%1
isr%1:
	push %1
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
