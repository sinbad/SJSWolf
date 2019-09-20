; *******************************************************************************
; * Fixed point mul/div assembly routines                                       *
; *                                                                             *
; * By Steven J Streeting (aka Sinbad)                                          *
; * Version 1.1, 20 August 1995                                                 *
; *                                                                             *
; * Optimised for Watcom C/C++ 10.0, 32-bit register passing under DOS4/GW      *
; *                                                                             * 
; *                                                                             *
; *                                                                             *
; *******************************************************************************



.code
public FixedMul_, FixedDiv_

;-----------------------------------------------------------
;Fixed point multiply
; C/C++ callable by "fixed FixedMul(fixed M1, fixed M2);"

FixedMul_ PROC NEAR

        ;EAX will already contain M1
        ;EDX will already contain M2
        
        imul    edx
        add     eax, 8000h              ;round by adding 2^(-17)
        adc     edx, 0                  ;add carry flag to EDX
                                        ;64-bit result now in EDX:EAX, 32:32 point
        shrd     eax, edx, 16           ;make a 16:16 return value in EAX
        
        ret
FixedMul_ ENDP


;------------------------------------------------------------
;Fixed Point Divide
; C/C++ callable by "fixed FixedDiv(fixed Divisor, fixed Dividend);"
; NO ROUNDING for the moment


FixedDiv_ PROC NEAR
        
        ; EAX will contain Divisor
        ; EDX will contain Dividend
        
        mov     ecx, eax        ;save divisor
        xor     eax, eax
        shrd    eax, edx, 16    ;make 32-bit value in DX:AX
        sar     edx, 16
        idiv    ecx             ;returns EAX ie 16:16 divided figure
        
        
                
        ret
FixedDiv_ ENDP
                


END
