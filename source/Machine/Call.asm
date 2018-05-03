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
	
	mov         r12, rcx        ; return_type
	mov         r13, rdx        ; return_value
	mov         r14, r8         ; number_of_parameters
    mov         rdi, [r9]       ; parameter type
    mov         rsi, [r9 + 8]   ; parameter value

    mov         rbx, $TABLE1
    mov         rbx, [rbx + r12 * 8]
    jmp         rbx
    
$CASE_RETURN_TYPE_IS_NOT_STRUCT:
    mov         rax, 0
    jmp         $RETURN

$CASE_RETURN_TYPE_IS_STRUCT:
    mov         rax, 1
    jmp         $RETURN
    
$PUSH_PARAMETER_IN_RCX:
    cmp         r14, 0
    je          $RETURN
    dec         r14
    mov         rbx, $TABLE2
    mov         rbx, [rbx + rdi]
    jmp         rbx

$PUSH_PARAMETER_IN_RCX_INTEGRAL:
    mov         rcx, QWORD PTR [rsi]
    jmp         $PUSH_PARAMETER_IN_RCX_INCREMENT

$PUSH_PARAMETER_IN_RCX_FP32:
    cvtss2sd    xmm0, DWORD PTR [rsi]
	movq        rcx, xmm0
    jmp         $PUSH_PARAMETER_IN_RCX_INCREMENT

$PUSH_PARAMETER_IN_RCX_FP64:
	movq        xmm0, QWORD PTR [rsi]
	movq        rcx, xmm0
    jmp         $PUSH_PARAMETER_IN_RCX_INCREMENT

$PUSH_PARAMETER_IN_RCX_INCREMENT
    add         rdi, 16
    add         rsi, 16
    jmp         $PUSH_PARAMETER_IN_RDX

$PUSH_PARAMETER_IN_RDX:
    cmp         r14, 0
    je          $RETURN
    dec         r14
    mov         rbx, $TABLE3
    mov         rbx, [rbx + rdi]
    jmp         rbx

$PUSH_PARAMETER_IN_RDX_INTEGRAL:
    mov         rdx, QWORD PTR [rsi]
    jmp         $PUSH_PARAMETER_IN_RCX_INCREMENT

$PUSH_PARAMETER_IN_RDX_FP32:
    cvtss2sd    xmm1, DWORD PTR [rsi]
	movq        rdx, xmm1
    jmp         $PUSH_PARAMETER_IN_RCX_INCREMENT

$PUSH_PARAMETER_IN_RDX_FP64:
	movq        xmm1, QWORD PTR [rsi]
	movq        rdx, xmm1
    jmp         $PUSH_PARAMETER_IN_RCX_INCREMENT

$PUSH_PARAMETER_IN_RDX_INCREMENT
    add         rdi, 16
    add         rsi, 16
    jmp         $PUSH_PARAMETER_IN_R8

$PUSH_PARAMETER_IN_R8:
$PUSH_PARAMETER_IN_R8_INTEGRAL:
$PUSH_PARAMETER_IN_R8_FP32:
$PUSH_PARAMETER_IN_R8_FP64:

$PUSH_PARAMETER_IN_R9:
$PUSH_PARAMETER_IN_R9_INTEGRAL:
$PUSH_PARAMETER_IN_R9_FP32:
$PUSH_PARAMETER_IN_R9_FP64:

$RETURN:
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
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT ; integer_8
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT ; integer_16
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT ; integer_32
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT ; integer_64
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT ; float_32
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT ; float_64
    DQ $CASE_RETURN_TYPE_IS_STRUCT     ; other
    DQ $CASE_RETURN_TYPE_IS_NOT_STRUCT ; void
    
$TABLE2:
    DQ $PUSH_PARAMETER_IN_RCX_INTEGRAL ; integer_8
    DQ $PUSH_PARAMETER_IN_RCX_INTEGRAL ; integer_16
    DQ $PUSH_PARAMETER_IN_RCX_INTEGRAL ; integer_32
    DQ $PUSH_PARAMETER_IN_RCX_INTEGRAL ; integer_64
    DQ $PUSH_PARAMETER_IN_RCX_FP32     ; float_32
    DQ $PUSH_PARAMETER_IN_RCX_FP64     ; float_64
    DQ $PUSH_PARAMETER_IN_RCX_INTEGRAL ; other
    
$TABLE3:
    DQ $PUSH_PARAMETER_IN_RDX_INTEGRAL ; integer_8
    DQ $PUSH_PARAMETER_IN_RDX_INTEGRAL ; integer_16
    DQ $PUSH_PARAMETER_IN_RDX_INTEGRAL ; integer_32
    DQ $PUSH_PARAMETER_IN_RDX_INTEGRAL ; integer_64
    DQ $PUSH_PARAMETER_IN_RDX_FP32     ; float_32
    DQ $PUSH_PARAMETER_IN_RDX_FP64     ; float_64
    DQ $PUSH_PARAMETER_IN_RCX_INTEGRAL ; other
    
$TABLE4:
    DQ $PUSH_PARAMETER_IN_R8_INTEGRAL  ; integer_8
    DQ $PUSH_PARAMETER_IN_R8_INTEGRAL  ; integer_16
    DQ $PUSH_PARAMETER_IN_R8_INTEGRAL  ; integer_32
    DQ $PUSH_PARAMETER_IN_R8_INTEGRAL  ; integer_64
    DQ $PUSH_PARAMETER_IN_R8_FP32      ; float_32
    DQ $PUSH_PARAMETER_IN_R8_FP64      ; float_64
    DQ $PUSH_PARAMETER_IN_RCX_INTEGRAL ; other
    
$TABLE5:
    DQ $PUSH_PARAMETER_IN_R9_INTEGRAL  ; integer_8
    DQ $PUSH_PARAMETER_IN_R9_INTEGRAL  ; integer_16
    DQ $PUSH_PARAMETER_IN_R9_INTEGRAL  ; integer_32
    DQ $PUSH_PARAMETER_IN_R9_INTEGRAL  ; integer_64
    DQ $PUSH_PARAMETER_IN_R9_FP32      ; float_32
    DQ $PUSH_PARAMETER_IN_R9_FP64      ; float_64
    DQ $PUSH_PARAMETER_IN_RCX_INTEGRAL ; other

Native_Call ENDP
_TEXT		ENDS
END
