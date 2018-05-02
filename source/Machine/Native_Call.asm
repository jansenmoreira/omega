_TEXT		SEGMENT
Native_Call	PROC
	push rbp
	mov rbp, rsp
	
	push rbx
	push rdi
	push rsi
	push r12
	push r13
	push r14
	push r15
	
	mov QWORD PTR [rbp - 8], rcx
	mov r13, rdx	; Value of return { dword type, dword size, qword ptr }
	mov r14, r8		; number of input params
	mov r15, r9		; *Value of inputs

	mov eax, DWORD PTR [r13]
	cmp eax, 2
	je $RETURN_TYPE_STRUCT
	jmp $PUSH_ARGS
	
$RETURN_TYPE_NONSTRUCT_CALL:
	call QWORD PTR [rbp - 8]

	mov rbx, QWORD PTR [r13 + 8]	; rbx points to value
	mov r12, rax					; r12 saves the returned value

	mov eax, DWORD PTR [r13]
	cmp eax, 1
	je $RETURN_TYPE_FLOAT

$RETURN_TYPE_INTEGER:
	mov eax, DWORD PTR [r13 + 4]
	cmp eax, 1
	je $RETURN_TYPE_INTEGER8
	cmp eax, 2
	je $RETURN_TYPE_INTEGER16
	cmp eax, 4
	je $RETURN_TYPE_INTEGER32
	cmp eax, 8
	je $RETURN_TYPE_INTEGER64
	
$RETURN_TYPE_INTEGER8:
	mov BYTE PTR [rbx], r12b
	jmp $END

$RETURN_TYPE_INTEGER16:
	mov WORD PTR [rbx], r12w
	jmp $END

$RETURN_TYPE_INTEGER32:
	mov DWORD PTR [rbx], r12d
	jmp $END

$RETURN_TYPE_INTEGER64:
	mov QWORD PTR [rbx], r12
	jmp $END
	
$RETURN_TYPE_FLOAT:
	mov eax, DWORD PTR [r13 + 4]
	cmp eax, 4
	je $RETURN_TYPE_FLOAT32
	cmp eax, 8
	je $RETURN_TYPE_FLOAT64
	jmp $END

$RETURN_TYPE_FLOAT32:
	movd DWORD PTR [rbx], xmm0
	jmp $END

$RETURN_TYPE_FLOAT64:
	movq QWORD PTR [rbx], xmm0
	jmp $END

$RETURN_TYPE_STRUCT:
	mov eax, DWORD PTR [r13 + 4]
	cmp eax, 1
	je $PUSH_ARGS
	cmp eax, 2
	je $PUSH_ARGS
	cmp eax, 4
	je $PUSH_ARGS
	cmp eax, 8
	je $PUSH_ARGS
	
	mov rcx, QWORD PTR [r13 + 8]
	jmp $PUSH_ARGS

$RETURN_TYPE_STRUCT_CALL:
	call QWORD PTR [rbp - 8]
	jmp $END

$END:
	pop r15
	pop r14
	pop r13
	pop r12
	pop rsi
	pop rdi
	pop rbx

	mov rsp, rbp
	pop rbp
	ret

$PUSH_ARGS:
	mov rsi, 0
	mov eax, DWORD PTR [r13]
	cmp eax, 2
	jne $PUSH_ARGS_NORMAL
	mov eax, DWORD PTR [r13 + 4]
	cmp eax, 1
	je $PUSH_ARGS_NORMAL
	cmp eax, 2
	je $PUSH_ARGS_NORMAL
	cmp eax, 4
	je $PUSH_ARGS_NORMAL
	cmp eax, 8
	je $PUSH_ARGS_NORMAL
	jmp $PUSH_ARGS_STRUCT

$PUSH_ARGS_NORMAL:
	cmp r14, 4
	jle $PUSH_ARGS_NORMAL_ADJUST
	mov rsi, r14
	sub rsi, 4
	shl rsi, 3
	sub rsp, rsi

$PUSH_ARGS_NORMAL_ADJUST:
	sub rsp, 40
	mov rdi, rsp
	jmp $PUSH_ARGS_RCX

$PUSH_ARGS_STRUCT:
	cmp r14, 3
	jle $PUSH_ARGS_STRUCT_ADJUST
	mov rsi, r14
	sub rsi, 3
	shl rsi, 3
	sub rsp, rsi

$PUSH_ARGS_STRUCT_ADJUST:
	sub rsp, 40
	mov rdi, rsp
	sub r15, 16
	jmp $PUSH_ARGS_RDX

$PUSH_ARGS_RCX:
	cmp r14, 0
	je $PUSH_ARGS_END
	dec r14
	mov r12, QWORD PTR [r15 + 8]
	mov eax, DWORD PTR [r15]
	cmp eax, 1
	je $PUSH_ARGS_RCX_FLOAT
	cmp eax, 2
	je $PUSH_ARGS_RCX_STRUCT

$PUSH_ARGS_RCX_INTEGER:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_RCX_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_RCX_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_RCX_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_RCX_INTEGER64
	
$PUSH_ARGS_RCX_INTEGER8:
	mov cl, BYTE PTR [r12]
	jmp $PUSH_ARGS_RDX

$PUSH_ARGS_RCX_INTEGER16:
	mov cx, WORD PTR [r12]
	jmp $PUSH_ARGS_RDX

$PUSH_ARGS_RCX_INTEGER32:
	mov ecx, DWORD PTR [r12]
	jmp $PUSH_ARGS_RDX

$PUSH_ARGS_RCX_INTEGER64:
	mov rcx, QWORD PTR [r12]
	jmp $PUSH_ARGS_RDX

$PUSH_ARGS_RCX_FLOAT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 4
	je $PUSH_ARGS_RCX_FLOAT32
	cmp eax, 8
	je $PUSH_ARGS_RCX_FLOAT64

$PUSH_ARGS_RCX_FLOAT32:
	cvtss2sd xmm0, DWORD PTR [r12]
	movq rcx, xmm0
	jmp $PUSH_ARGS_RDX

$PUSH_ARGS_RCX_FLOAT64:
	movq xmm0, QWORD PTR [r12]
	movq rcx, xmm0
	jmp $PUSH_ARGS_RDX

$PUSH_ARGS_RCX_STRUCT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_RCX_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_RCX_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_RCX_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_RCX_INTEGER64

	mov rcx, QWORD PTR [r15 + 8]
	jmp $PUSH_ARGS_RDX

$PUSH_ARGS_RDX:
	cmp r14, 0
	je $PUSH_ARGS_END
	add rdi, 8
	dec r14
	add r15, 16
	mov r12, QWORD PTR [r15 + 8]
	mov eax, DWORD PTR [r15]
	cmp eax, 1
	je $PUSH_ARGS_RDX_FLOAT
	cmp eax, 2
	je $PUSH_ARGS_RDX_STRUCT

$PUSH_ARGS_RDX_INTEGER:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_RDX_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_RDX_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_RDX_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_RDX_INTEGER64
	
$PUSH_ARGS_RDX_INTEGER8:
	mov dl, BYTE PTR [r12]
	jmp $PUSH_ARGS_R8

$PUSH_ARGS_RDX_INTEGER16:
	mov dx, WORD PTR [r12]
	jmp $PUSH_ARGS_R8

$PUSH_ARGS_RDX_INTEGER32:
	mov edx, DWORD PTR [r12]
	jmp $PUSH_ARGS_R8

$PUSH_ARGS_RDX_INTEGER64:
	mov rdx, QWORD PTR [r12]
	jmp $PUSH_ARGS_R8

$PUSH_ARGS_RDX_FLOAT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 4
	je $PUSH_ARGS_RDX_FLOAT32
	cmp eax, 8
	je $PUSH_ARGS_RDX_FLOAT64

$PUSH_ARGS_RDX_FLOAT32:
	cvtss2sd xmm1, DWORD PTR [r12]
	movq rdx, xmm1
	jmp $PUSH_ARGS_R8

$PUSH_ARGS_RDX_FLOAT64:
	movq xmm1, QWORD PTR [r12]
	movq rdx, xmm1
	jmp $PUSH_ARGS_R8

$PUSH_ARGS_RDX_STRUCT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_RDX_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_RDX_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_RDX_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_RDX_INTEGER64

	mov rdx, QWORD PTR [r15 + 8]
	jmp $PUSH_ARGS_R8

$PUSH_ARGS_R8:
	cmp r14, 0
	je $PUSH_ARGS_END
	add rdi, 8
	dec r14
	add r15, 16
	mov r12, QWORD PTR [r15 + 8]
	mov eax, DWORD PTR [r15]
	cmp eax, 1
	je $PUSH_ARGS_R8_FLOAT
	cmp eax, 2
	je $PUSH_ARGS_R8_STRUCT

$PUSH_ARGS_R8_INTEGER:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_R8_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_R8_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_R8_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_R8_INTEGER64
	
$PUSH_ARGS_R8_INTEGER8:
	mov r8b, BYTE PTR [r12]
	jmp $PUSH_ARGS_R9

$PUSH_ARGS_R8_INTEGER16:
	mov r8w, WORD PTR [r12]
	jmp $PUSH_ARGS_R9

$PUSH_ARGS_R8_INTEGER32:
	mov r8d, DWORD PTR [r12]
	jmp $PUSH_ARGS_R9

$PUSH_ARGS_R8_INTEGER64:
	mov r8, QWORD PTR [r12]
	jmp $PUSH_ARGS_R9

$PUSH_ARGS_R8_FLOAT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 4
	je $PUSH_ARGS_R8_FLOAT32
	cmp eax, 8
	je $PUSH_ARGS_R8_FLOAT64

$PUSH_ARGS_R8_FLOAT32:
	cvtss2sd xmm2, DWORD PTR [r12]
	movq r8, xmm2
	jmp $PUSH_ARGS_R9

$PUSH_ARGS_R8_FLOAT64:
	movq xmm2, QWORD PTR [r12]
	movq r8, xmm2
	jmp $PUSH_ARGS_R9

$PUSH_ARGS_R8_STRUCT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_R8_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_R8_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_R8_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_R8_INTEGER64

	mov r8, QWORD PTR [r15 + 8]
	jmp $PUSH_ARGS_R9

$PUSH_ARGS_R9:
	cmp r14, 0
	je $PUSH_ARGS_END
	add rdi, 8
	dec r14
	add r15, 16
	mov r12, QWORD PTR [r15 + 8]
	mov eax, DWORD PTR [r15]
	cmp eax, 1
	je $PUSH_ARGS_R9_FLOAT
	cmp eax, 2
	je $PUSH_ARGS_R9_STRUCT

$PUSH_ARGS_R9_INTEGER:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_R9_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_R9_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_R9_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_R9_INTEGER64
	
$PUSH_ARGS_R9_INTEGER8:
	mov r9b, BYTE PTR [r12]
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_R9_INTEGER16:
	mov r9w, WORD PTR [r12]
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_R9_INTEGER32:
	mov r9d, DWORD PTR [r12]
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_R9_INTEGER64:
	mov r9, QWORD PTR [r12]
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_R9_FLOAT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 4
	je $PUSH_ARGS_R9_FLOAT32
	cmp eax, 8
	je $PUSH_ARGS_R9_FLOAT64

$PUSH_ARGS_R9_FLOAT32:
	cvtss2sd xmm3, DWORD PTR [r12]
	movq r9, xmm3
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_R9_FLOAT64:
	movq xmm3, QWORD PTR [r12]
	movq r9, xmm3
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_R9_STRUCT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_R9_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_R9_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_R9_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_R9_INTEGER64

	mov r9, QWORD PTR [r15 + 8]
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_EXTRA:
	cmp r14, 0
	je $PUSH_ARGS_END
	add rdi, 8
	dec r14
	add r15, 16
	mov r12, QWORD PTR [r15 + 8]
	mov eax, DWORD PTR [r15]

	cmp eax, 1
	je $PUSH_ARGS_EXTRA_FLOAT
	cmp eax, 2
	je $PUSH_ARGS_EXTRA_STRUCT

$PUSH_ARGS_EXTRA_INTEGER:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_EXTRA_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_EXTRA_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_EXTRA_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_EXTRA_INTEGER64
	
$PUSH_ARGS_EXTRA_INTEGER8:
	mov al, BYTE PTR [r12]
	mov QWORD PTR [rdi], rax
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_EXTRA_INTEGER16:
	mov ax, WORD PTR [r12]
	mov QWORD PTR [rdi], rax
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_EXTRA_INTEGER32:
	mov eax, DWORD PTR [r12]
	mov QWORD PTR [rdi], rax
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_EXTRA_INTEGER64:
	mov rax, QWORD PTR [r12]
	mov QWORD PTR [rdi], rax
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_EXTRA_FLOAT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 4
	je $PUSH_ARGS_EXTRA_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_EXTRA_INTEGER64

$PUSH_ARGS_EXTRA_STRUCT:
	mov eax, DWORD PTR [r15 + 4]
	cmp eax, 1
	je $PUSH_ARGS_EXTRA_INTEGER8
	cmp eax, 2
	je $PUSH_ARGS_EXTRA_INTEGER16
	cmp eax, 4
	je $PUSH_ARGS_EXTRA_INTEGER32
	cmp eax, 8
	je $PUSH_ARGS_EXTRA_INTEGER64

	mov rax, QWORD PTR [r15 + 8]
	push rax
	jmp $PUSH_ARGS_EXTRA

$PUSH_ARGS_END:
	mov eax, DWORD PTR [r13]
	cmp eax, 2
	jne $RETURN_TYPE_NONSTRUCT_CALL
	mov eax, DWORD PTR [r13 + 4]
	cmp eax, 1
	je $RETURN_TYPE_NONSTRUCT_CALL
	cmp eax, 2
	je $RETURN_TYPE_NONSTRUCT_CALL
	cmp eax, 4
	je $RETURN_TYPE_NONSTRUCT_CALL
	cmp eax, 8
	je $RETURN_TYPE_NONSTRUCT_CALL
	jmp $RETURN_TYPE_STRUCT_CALL

Native_Call	ENDP
_TEXT		ENDS
END
