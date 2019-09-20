; Code for scaling the texture column and floor line
; Designed for register passing under Watcom C/C++
; S J Streeting 1995

; Version 3 - light sourcing + floorcasting
; Texture file now 256 wide for optimisation

.386p
.model flat

.code

;ScaleCol - coloumn scaling routine
; C/C++ callable by "scalecol(int height, char *texture,
;                               char *video, int dheight, char *light);"


scalemacro  MACRO
  LOCAL advance
  LOCAL noadvance
  mov   cl, [edx]               ;get texture byte
  mov   cl, [esi + ecx]         ;look up lightsourced version
  mov   [ebx], cl               ;write it
  add   ebx, 320
  sub   ebp, IMAGE_HEIGHT
  jns    noadvance
advance:
  inc   edx			;advance texture pointer
  add   ebp, eax		;add height to error term
  js    advance			;loop if still signed 
noadvance:

  ENDM


public scalecol_, floorline_
IMAGE_HEIGHT	equ	64

scalecol_  PROC NEAR
; Initial registers:  eax = height
;                     edx = texture pointer
;                     ebx = video pointer
;		      ecx = dheight (ie size on screen)
;                     stack: lightsourcing pointer

  push 	ebp			;store EBP - used for error term
  push  edi			;store EDI - used for jump
  push  esi                     ;store ESI - used for lightsourcing

  mov   esi, [esp+16]           ;get lightsource pointer from stack
  mov   ebp, eax		;put height into error term
  push  ecx			;save ecx
  mov	edi, offset mainscale
  and   ecx, 1111b		;mod 16
  imul  ecx, 27			;multiply by no of bytes in macro
  sub	edi, ecx		;subtract from offset - work back
  pop	ecx			;retrieve ecx
  shr   ecx, 4			;divide count by 16
  inc   ecx
  
  push ecx                     ;save count in high ecx before jump
  xor   ecx, ecx               ;clear out ecx crap

  jmp	edi			;jump into unrolled loop

 scaleloop:
  push ecx                     ;Save count in high ecx
  xor  ecx, ecx                ;clear out the crap
  REPT 16
  scalemacro
  ENDM
 mainscale:
   
  pop   ecx
  dec   ecx
  jnz   scaleloop

  pop	esi
  pop	edi
  pop	ebp
  ret
scalecol_ ENDP


;FloorLine  - routine for drawing a floor texture line
; C callable by "floor_line(fixed xstart,fixed ystart,fixed xinc, fixed yinc, 
;                           unsigned char *textmaps, char *buffer,
;                            unsigned char *lightptr);"


SCREEN_WIDTH 	equ 320
MOD_64		equ 3fh
MOD_64_SHL      equ 3f00h
xinc		dd	0
yinc		dd	0
lightptr        dd      0
bots            dd	0
row             dd	0


FloorMacro MACRO
  local skipit
  mov  ebx, [ebp]
  cmp  ebx, [row]
  ja  skipit
  
  push ebp  
  mov  ebx, eax  		;get current total x
  shr  ebx, 18                   ;revert to int * 256
  and  ebx, MOD_64_SHL          ;restrict to 64x64x256
  mov  ebp, ebx
  mov  ebx, edx 		;same with y
  shr  ebx, 26
  add  ebp, ebx			;add x and y

  xor  ebx, ebx			;clean out ebx
  mov  bl, [esi + ebp]  	;get texture byte
  pop  ebp			;* separate that 486 addresing pipeline!!
  mov  bl, [ecx + ebx]		;get lightsourced byte
  mov  [edi], bl		;put to dest
skipit:
  inc  edi			;move destination on
  add eax, [xinc]
  add edx, [yinc]
  add ebp,4
ENDM



floorline_ PROC NEAR
  ;Initial registers:
  ;	eax - xstart
  ;     edx - ystart
  ;     ebx - xinc
  ;     ecx - yinc
  ;     stack - *textmaps  [+4]     }
  ;           - *buffer    [+8]     } - for some reason it seems the last
  ;           - *lightptr  [+12]    }   param is pushed first?
  ;           - *wall_bots [+16]    }
  ;           - row        [+20]
  
  push ebp			;sp - 4
  push edi			;sp - 4
  mov  ebp, [esp + 20]		;retrieve lightsource pointer now
  mov  edi, [esp + 16]		;and dest. buffer
  mov  esi, [esp + 12]		;and source
  mov  [lightptr], ebp
  mov  ebp, [esp + 28]
  mov  [row], ebp
  mov  ebp, [esp + 24]		;get index to bottoms
  mov  [xinc], ebx
  mov  [yinc], ecx

  mov  ecx, (SCREEN_WIDTH/16)
floorloop:
  push ecx			;save count
  mov  ecx, [lightptr]

  REPT 16
  FloorMacro
  ENDM

  pop  ecx
  dec  ecx
  jnz  floorloop
  
  pop  edi
  pop  ebp

floorline_ ENDP




END
