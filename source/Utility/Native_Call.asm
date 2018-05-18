_TEXT		SEGMENT
Native_Call PROC

	push        rbp
	mov         rbp, rsp
	
	push        rbx
	push        rdi
	push        rsi
	push        r12
	push        r13
	push        r14
	push        r15
	
    mov         r15, rcx        ; function_pointer

	mov         r12, [rdx]      ; return_type
    mov         r13, [rdx + 8]  ; return_value
	mov         r14, r8         ; number_of_parameters
    mov         rdi, r9         ; parameter type
    mov         rsi, rdi        
    add         rsi, 8          ; parameter value

    sub         rsp, 40
    xor         rbx, rbx

    mov         rax, $TABLE1
    mov         rax, [rax + r12 * 8]
    jmp         rax

$CASE_RETURN_TYPE_IS_NOT_STRUCT:
    jmp         $MOV_PARAMETER_TO_RCX
        
$CASE_RETURN_TYPE_IS_STRUCT:
    mov         rcx, r13
    jmp         $MOV_PARAMETER_TO_RDX
    
$MOV_PARAMETER_TO_RCX:
    cmp         r14, 0
    je          $MAKE_CALL
    dec         r14
    mov         rax, $TABLE2
    mov         r10, [rdi]
    mov         rax, [rax + r10 * 8]
    jmp         rax

$MOV_PARAMETER_TO_RCX_INTEGRAL:
    mov         rcx, QWORD PTR [rsi]
    jmp         $MOV_PARAMETER_TO_RCX_INCREMENT

$MOV_PARAMETER_TO_RCX_FP32:
    movss       xmm0, DWORD PTR [rsi]
	movd        ecx, xmm0
    jmp         $MOV_PARAMETER_TO_RCX_INCREMENT

$MOV_PARAMETER_TO_RCX_FP64:
	movq        xmm0, QWORD PTR [rsi]
	movq        rcx, xmm0

$MOV_PARAMETER_TO_RCX_INCREMENT:
    add         rdi, 16
    add         rsi, 16

$MOV_PARAMETER_TO_RDX:
    cmp         r14, 0
    je          $MAKE_CALL
    dec         r14
    mov         rax, $TABLE3
    mov         r10, [rdi]
    mov         rax, [rax + r10 * 8]
    jmp         rax

$MOV_PARAMETER_TO_RDX_INTEGRAL:
    mov         rdx, QWORD PTR [rsi]
    jmp         $MOV_PARAMETER_TO_RDX_INCREMENT

$MOV_PARAMETER_TO_RDX_FP32:
    movss       xmm1, DWORD PTR [rsi]
	movd        edx, xmm1
    jmp         $MOV_PARAMETER_TO_RDX_INCREMENT

$MOV_PARAMETER_TO_RDX_FP64:
	movq        xmm1, QWORD PTR [rsi]
	movq        rdx, xmm1

$MOV_PARAMETER_TO_RDX_INCREMENT:
    add         rdi, 16
    add         rsi, 16

$MOV_PARAMETER_TO_R8:
    cmp         r14, 0
    je          $MAKE_CALL
    dec         r14
    mov         rax, $TABLE4
    mov         r10, [rdi]
    mov         rax, [rax + r10 * 8]
    jmp         rax

$MOV_PARAMETER_TO_R8_INTEGRAL:
    mov         r8, QWORD PTR [rsi]
    jmp         $MOV_PARAMETER_TO_R8_INCREMENT

$MOV_PARAMETER_TO_R8_FP32:
	movss       xmm2, DWORD PTR [rsi]
	movd        r8d, xmm2
    jmp         $MOV_PARAMETER_TO_R8_INCREMENT

$MOV_PARAMETER_TO_R8_FP64:
	movq        xmm2, QWORD PTR [rsi]
	movq        r8, xmm2
    
$MOV_PARAMETER_TO_R8_INCREMENT:
    add         rdi, 16
    add         rsi, 16

$MOV_PARAMETER_TO_R9:
    cmp         r14, 0
    je          $MAKE_CALL
    dec         r14
    mov         rax, $TABLE5
    mov         r10, [rdi]
    mov         rax, [rax + r10 * 8]
    jmp         rax

$MOV_PARAMETER_TO_R9_INTEGRAL:
    mov         r9, QWORD PTR [rsi]
    jmp         $MOV_PARAMETER_TO_R9_INCREMENT

$MOV_PARAMETER_TO_R9_FP32:
    movss       xmm3, DWORD PTR [rsi]
	movd        r9d, xmm3
    jmp         $MOV_PARAMETER_TO_R9_INCREMENT

$MOV_PARAMETER_TO_R9_FP64:
	movq        xmm3, QWORD PTR [rsi]
	movq        r9, xmm3
    
$MOV_PARAMETER_TO_R9_INCREMENT:
    add         rdi, 16
    add         rsi, 16

$ADJUST_STACK:
    mov         rbx, r14
    mov         rax, 1
    and         rax, rbx
    cmp         rax, 0
    je          $ADJUST_STACK_IS_ALIGNED
    inc         rbx
    
$ADJUST_STACK_IS_ALIGNED:
    shl         rbx, 3
    sub         rsp, rbx
    xor         rbx, rbx
    jmp         $PUSH_PARAMETER_TO_STACK

$PUSH_PARAMETER_TO_STACK:
    cmp         r14, 0
    je          $MAKE_CALL
    dec         r14
    mov         rax, $TABLE6
    mov         r10, [rdi]
    mov         rax, [rax + r10 * 8]
    jmp         rax

$PUSH_PARAMETER_TO_STACK_INTEGRAL:
    mov         rax, QWORD PTR [rsi]
    mov         QWORD PTR [rsp + rbx + 32], rax
    jmp         $PUSH_PARAMETER_TO_STACK_INCREMENT

$PUSH_PARAMETER_TO_STACK_FP32:
    movss       xmm4, DWORD PTR [rsi]
	movd        rax, xmm4
    mov         QWORD PTR [rsp + rbx + 32], rax
    jmp         $PUSH_PARAMETER_TO_STACK_INCREMENT

$PUSH_PARAMETER_TO_STACK_FP64:
	movq        xmm4, QWORD PTR [rsi]
	movq        rax, xmm4
    mov         QWORD PTR [rsp + rbx + 32], rax
    
$PUSH_PARAMETER_TO_STACK_INCREMENT:
    add         rbx, 8
    add         rdi, 16
    add         rsi, 16
    jmp         $PUSH_PARAMETER_TO_STACK

$MAKE_CALL:
    call        r15
    add         rsp, rbx
    add         rsp, 40
    
    mov         rbx, $TABLE7
    mov         rbx, [rbx + r12 * 8]
    jmp         rbx

$MOV_RETURN_TO_POINTER_I8:
    mov         BYTE PTR [r13], al
    jmp         $END

$MOV_RETURN_TO_POINTER_I16:
    mov         WORD PTR [r13], ax
    jmp         $END

$MOV_RETURN_TO_POINTER_I32:
    mov         DWORD PTR [r13], eax
    jmp         $END

$MOV_RETURN_TO_POINTER_I64:
    mov         QWORD PTR [r13], rax
    jmp         $END

$MOV_RETURN_TO_POINTER_FP32:
    movd        DWORD PTR [r13], xmm0
    jmp         $END

$MOV_RETURN_TO_POINTER_FP64:
    movq        QWORD PTR [r13], xmm0

$END:
    pop         r15
	pop         r14
	pop         r13
	pop         r12
	pop         rsi
	pop         rdi
	pop         rbx

	mov         rsp, rbp
	pop         rbp
	ret

$TABLE1:
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT
    DQ $CASE_RETURN_TYPE_IS_STRUCT
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT

$TABLE2:
    DQ $MOV_PARAMETER_TO_RCX_INTEGRAL
    DQ $MOV_PARAMETER_TO_RCX_INTEGRAL
    DQ $MOV_PARAMETER_TO_RCX_INTEGRAL
    DQ $MOV_PARAMETER_TO_RCX_INTEGRAL
    DQ $MOV_PARAMETER_TO_RCX_FP32
    DQ $MOV_PARAMETER_TO_RCX_FP64
    DQ $MOV_PARAMETER_TO_RCX_INTEGRAL
    
$TABLE3:
    DQ $MOV_PARAMETER_TO_RDX_INTEGRAL
    DQ $MOV_PARAMETER_TO_RDX_INTEGRAL
    DQ $MOV_PARAMETER_TO_RDX_INTEGRAL
    DQ $MOV_PARAMETER_TO_RDX_INTEGRAL
    DQ $MOV_PARAMETER_TO_RDX_FP32
    DQ $MOV_PARAMETER_TO_RDX_FP64
    DQ $MOV_PARAMETER_TO_RDX_INTEGRAL
    
$TABLE4:
    DQ $MOV_PARAMETER_TO_R8_INTEGRAL
    DQ $MOV_PARAMETER_TO_R8_INTEGRAL
    DQ $MOV_PARAMETER_TO_R8_INTEGRAL
    DQ $MOV_PARAMETER_TO_R8_INTEGRAL
    DQ $MOV_PARAMETER_TO_R8_FP32
    DQ $MOV_PARAMETER_TO_R8_FP64
    DQ $MOV_PARAMETER_TO_R8_INTEGRAL
    
$TABLE5:
    DQ $MOV_PARAMETER_TO_R9_INTEGRAL
    DQ $MOV_PARAMETER_TO_R9_INTEGRAL
    DQ $MOV_PARAMETER_TO_R9_INTEGRAL
    DQ $MOV_PARAMETER_TO_R9_INTEGRAL
    DQ $MOV_PARAMETER_TO_R9_FP32
    DQ $MOV_PARAMETER_TO_R9_FP64
    DQ $MOV_PARAMETER_TO_R9_INTEGRAL
    
$TABLE6:
    DQ $PUSH_PARAMETER_TO_STACK_INTEGRAL
    DQ $PUSH_PARAMETER_TO_STACK_INTEGRAL
    DQ $PUSH_PARAMETER_TO_STACK_INTEGRAL
    DQ $PUSH_PARAMETER_TO_STACK_INTEGRAL
    DQ $PUSH_PARAMETER_TO_STACK_FP32
    DQ $PUSH_PARAMETER_TO_STACK_FP64
    DQ $PUSH_PARAMETER_TO_STACK_INTEGRAL
        
$TABLE7:
    DQ $MOV_RETURN_TO_POINTER_I8
    DQ $MOV_RETURN_TO_POINTER_I16
    DQ $MOV_RETURN_TO_POINTER_I32
    DQ $MOV_RETURN_TO_POINTER_I64
    DQ $MOV_RETURN_TO_POINTER_FP32
    DQ $MOV_RETURN_TO_POINTER_FP64
    DQ $END

Native_Call ENDP
_TEXT		ENDS
END
