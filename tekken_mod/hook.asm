PUBLIC cmd_store_hook
PUBLIC cmd_reset_hook
PUBLIC cmd_frame

EXTERN draw_frames: PROC

_DATA SEGMENT

; Frame each command was input on
cmd_frame dd 40 dup (0)

_DATA ENDS

_TEXT SEGMENT

cmd_store_hook PROC
	; Original instructions
	mov [r8+rcx*4+20E8h], edx
	mov eax, [r8+0AACh]
	mov [r8+rcx*4+2188h], eax
	mov [r8+rcx*4+2228h], r10d
	mov eax, [r8+11A4h]
	mov [r8+rcx*4+22C8h], eax

	; Check if this is the active player
	push rcx
	mov rcx, 14334ABB0h
	mov rcx, [rcx]
	add rcx, 460h
	mov rax, [rcx]
	call qword ptr [rax+10h]
	mov rax, [rax+14h]
	mov rcx, 143364920h
	mov rax, [rcx+rax*8]
	pop rcx
	cmp r8, rax
	jne @F

	; Store frame number
	mov eax, [r8+810h]
	push r8
	lea r8, cmd_frame
	mov [r8+rcx*4], eax
	pop r8

@@:
	mov rax, 140612B1Bh
	jmp rax
cmd_store_hook ENDP

cmd_reset_hook PROC
	; Original instruction
	xor r8d, r8d

	mov rdx, 0

@@:
	lea rax, cmd_frame
	mov [rax+rdx*4], r8d

	inc rdx
	cmp rdx, 40
	jl @B

	; Original instructions
	mov qword ptr [rcx], 1
	mov [rcx+0Ch], r8

	mov rax, 14062C72Eh
	jmp rax
cmd_reset_hook ENDP

hud_postrender_hook PROC
	push rcx
	push rbp
	mov rbp, rsp

	; K2_DrawText needs 16 byte stack alignment
	and rsp, 0FFFFFFFFFFFFFFF0h
	sub rsp, 20h
	call draw_frames

	mov rsp, rbp
	pop rbp
	pop rcx

	; Original instructions
	push rbp
	push rdi
	lea rbp, [rsp-4Fh]
	sub rsp, 0C8h

	mov rax, 1413E25FFh
	jmp rax
hud_postrender_hook ENDP

_TEXT ENDS

END