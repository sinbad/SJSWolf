
; Screen routines altered for DOS/4GW
; Also modified for register passing
 
.CODE
PUBLIC  cls_,setmode_,setpalette_, copybuffer_

SCREEN_WIDTH    EQU     320
SCREEN_HEIGHT   EQU 200

; cls(char *screen_adr)
;   Clear video memory or offscreen buffer at
;   SCREEN_ADR to zeroes

cls_            PROC NEAR
                push    edi               ; Save DI register
                mov     edi, eax          ; Point EDI at screen or buffer                mov     ecx, SCREEN_WIDTH/4 * SCREEN_HEIGHT ; Count pixels/4
      
                mov     ecx, 16000       ;a 320x200 screens worth             
                mov     eax,0            ; Store zero values...
                rep     stosd            ; ...in all of video memory
                pop     edi              ; Restore DI
                ret
cls_                   ENDP

; setmode(int mode)
;   Set VGA adapter to BIOS mode MODE

setmode_        PROC NEAR
                mov             ah,0            ; AH = function number
                                                ; AL = mode (already passed in AX)
                int             10h             ; Call video BIOS
                ret
setmode_        ENDP

; setpalette(char *palette)
;       Set a palette

setpalette_     PROC
        push    esi 
        mov     esi, eax               ;load pointer to palette info
        mov     dx, 3c8h               ;colour NO selector
        xor     al, al                 ;zero AL
        out     dx, al                 ;select it
        inc     dx                     ;DX+1 = RGB writing location

        mov     cx, 768                ;256*3 colours again
        cli                            ;clear interrupts
        rep     outsb                  ;write all colours
        sti                            ;allow interrupts
        pop     esi
    ret
setpalette_     ENDP

copybuffer_     PROC NEAR
        ;EAX = *source, EDX = *dest
        
        push    esi
        push    edi
                
        mov     esi, eax
        mov     edi, edx
        mov     ecx, 16000        ;assume this is just for a 64k screen copy for now
        
        mov     dx, 3dah        ;wait for vertical retrace
  ret1:
        in      al, dx
        and     al, 08h
        jnz     ret1
  ret2:
        in      al, dx
        and     al, 08h
        jz      ret2
        
               
        rep     movsd        
        pop     edi
        pop     esi
        ret
copybuffer_     ENDP

END
