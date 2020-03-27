;Source code released to the public domain on March 27th, 2020.

.286
MASM51
QUIRKS
LOCALS
.MODEL MEDIUM,C
;===========================================================================
READ_MAP equ    04h     ;index in GC of the Read Map register
CRTC_INDEX equ  03d4h   ;CRT Controller Index
MISC_OUTPUT equ 03c2h   ;Miscellaneous Output register
MAX_ACTORS equ  35
SC_INDEX   equ  03c4h   ;Sequence Controller Index register port
MAP_MASK   equ  02h     ;index in SC of Map Mask register
GC_INDEX   equ  03ceh   ;Graphics Controller Index register port
BIT_MASK   equ  08h     ;index in GC of Bit Mask register
SCREEN_SEG equ  0a000h  ;segment of display memory in mode X
SCREEN_WIDTH equ 80     ;width of screen in bytes from one scan line
DAC_READ_INDEX	equ  03c7h
DAC_WRITE_INDEX equ  03c8h
DAC_DATA	equ  03c9h
INPUT_STATUS_0  equ  03dah   ;Input status 0 register
;----------------------------------------------------------------------------
mask_image     STRUC

alignments  dw  4 dup(?) ;pointers to align_mask_images for the

mask_image     ENDS
;----------------------------------------------------------------------------
align_mask_image      STRUC

img_width  dw      ? ;image width in addresses (also mask width in bytes)
img_ptr    dw      ? ;offset of image bitmap in display memory
mask_ptr   dw      ? ;pointer to mask bitmap in DS
mask_seg   dw      ? ;

align_mask_image      ENDS
;===========================================================================
.DATA
palloop      db 0
TempPalette  db 768 dup (0)

palclr1 dw 0f300h,003bh,0f000h,003bh,0f100h,003bh,0f200h,003bh
palset1 dw 0f027h,273fh,0f127h,273fh,0f227h,273fh,0f327h,273fh
palcnt1 dw 00

palclr2 dw 0f73bh,0000h,0f43bh,0000h,0f53bh,0000h,0f63bh,0000h
palset2 dw 0f43fh,2727h,0f53fh,2727h,0f63fh,2727h,0f73fh,2727h
palcnt2 dw 00

LeftClipPlaneMask  db 00fh,00eh,00ch,008h
RightClipPlaneMask db 00fh,001h,003h,007h

CRTParms label  word
        dw      00d06h  ;vertical total
        dw      03e07h  ;overflow (bit 8 of vertical counts)
        dw      04109h  ;cell height (2 to double-scan)
        dw      0ea10h  ;v sync start
        dw      0ac11h  ;v sync end and protect cr0-cr7
        dw      0df12h  ;vertical displayed
        dw      00014h  ;turn off dword mode
        dw      0e715h  ;v blank start
        dw      00616h  ;v blank end
        dw      0e317h  ;turn on byte mode
CRT_PARM_LENGTH equ     (($-CRTParms)/2)
;===========================================================================
.CODE

PROC    xdisplay_actors
        PUBLIC  xdisplay_actors
        ARG     actor_addr:WORD
        ARG     dst_page:WORD
        USES    si,di
        LOCAL   src_next_off:WORD
        LOCAL   dst_next_off:WORD
        LOCAL   rect_width:WORD
        LOCAL   rect_height:WORD
        LOCAL   src_bmap_width:WORD
        LOCAL   mask_seg_str:WORD
        LOCAL   actor_ptr:WORD
        LOCAL   actor_num:WORD
        LOCAL   last_actor_addr:WORD
        LOCAL   actor2_storage:WORD

        cld
        mov     dx,GC_INDEX     ;set the bit mask to select all bits
        mov     ax,00000h+BIT_MASK ; from the latches and none from
        out     dx,ax           ; the CPU, so that we can write the
                                ; latch contents directly to memory
        mov     dx,SC_INDEX
        mov     al,MAP_MASK
        out     dx,al           ;point SC Index register to Map Mask
        inc     dx              ;point to SC Data register

        mov     ax,SCREEN_SEG   ;point ES to display memory
        mov     es,ax

        xor     ax,ax
        mov     actor_num,ax   ;actor=0

        mov     bx,actor_addr  ;get and save actor address
        mov     actor_ptr,bx

actor_loop:

        mov     al,[bx+185]    ;get USED flag
        or      al,al
        jnz     cont1
        jmp     next_actor

cont1:  mov     al,[bx+199]    ;get SHOW counter
        test    al,2
        jz      cont2

        jmp     next_actor
cont2:

;calc starting offset in display mem

        mov     di,[bx+171]  ;get X coor

        mov     dx,[bx+173]  ;get Y coor
        mov     ax,80
        mul     dl

        mov     cx,dst_page   ;set LAST_X[PAGE]
        mov     si,bx
        cmp     cx,19000
        jb      page0
        add     si,2

page0:  add     si,177         ;177=LAST_X
        mov     [si],di        ;store LAST_X
        add     si,4
        mov     [si],dx        ;store LAST_Y

        shr     di,1    ;X/4 = offset of first dest rect pixel in
        shr     di,1    ;scan line
        add     di,ax   ;offset of first dest rect pixel in page
        add     di,dst_page ;offset of first dest rect pixel

        push    di      ;save di for later

;lookup which frame to use: PIC[dir][frame_sequence[next]]

        mov     di,40        ;offset of first PIC
        add     di,bx        ;point to PIC[0][0]
        mov     dl,[bx+169]  ;get current direction
        xor     dh,dh
        shl     dx,5         ;multiply dx by 32
        add     di,dx        ;point to PIC[dir][0]

        mov     cl,[bx+186]  ;get NEXT
        xor     ch,ch
        mov     si,bx        ;point to actor
        add     si,6         ;point to FRAME_SEQUENCE
        add     si,cx        ;add NEXT
        mov     dl,[si]      ;get FRAME_SEQUENCE[NEXT]
        xor     dh,dh
        shl     dx,3         ;multiply dx by 8
        add     di,dx        ;di now points to PIC[DIR][FRAME_SEQUENCE[NEXT]]

;look up the image that's aligned to match
;left-edge alignment of destination

        mov     si,[bx+171]  ;get X coor
        and     si,2         ;only even pixels are used
        mov     cx,si        ;save for later
        shr     cx,1

        push    bx
        mov     bx,di
        mov     di,[bx+alignments+si]   ;point to align_mask_image
                                        ;struc for current left edge alignment
        pop     bx
        mov     ax,[di+img_width]       ;image width in addresses
        mov     src_bmap_width,ax  ;remember image width in addresses
        mov     rect_width,ax      ;save width

;calc distance from end of one dest scan line to start of next

        mov     dx,80
        sub     dx,ax
        mov     dst_next_off,dx

        mov     si,[di+mask_seg]
        mov     mask_seg_str,si

        mov     si,[di+mask_ptr] ;point to mask offset of first mask pixel
        mov     dx,[di+img_ptr]  ;offset of first source rect pixel
        push    dx               ;save, will be POPed into BX


        mov     al,[bx+2]            ;get height
        xor     ah,ah
        mov     rect_height,ax  ;save height

        mov     dx,SC_INDEX+1

        pop     bx
        pop     di

CopyRowsLoop:
        mov     cx,rect_width ;width across
        push    ds
        mov     ax,mask_seg_str
        mov     ds,ax
CopyScanLineLoop:
        lodsb                   ;get the mask for this four-pixel set
                                ; and advance the mask pointer
        out     dx,al           ;set the mask
        mov     al,es:[bx]      ;load the latches with 4-pixel set from source
        mov     es:[di],al      ;copy the four-pixel set to the dest
        inc     bx              ;advance the source pointer
        inc     di              ;advance the destination pointer
        loop    CopyScanLineLoop
        pop     ds

        add     di,dst_next_off ; and dest lines
        dec     word ptr rect_height ;count down scan lines
        jnz     CopyRowsLoop

next_actor:
        mov     ax,actor_num
        inc     ax
        cmp     ax,MAX_ACTORS+1  ;max_actors
        je      actors_done

        mov     actor_num,ax
        mov     bx,actor_ptr ;point to next actor
        sub     bx,256            ;256=sizeof ACTOR struct
        mov     actor_ptr,bx

        cmp     ax,MAX_ACTORS-3      ;is it actor[2] (magic)
        je      detour1
        cmp     ax,MAX_ACTORS
        je      detour2

        jmp     actor_loop

detour1:
        mov     actor2_storage,bx
        jmp     next_actor

detour2:
        mov     bx,actor2_storage
        mov     actor_ptr,bx
        jmp     actor_loop

actors_done:
        mov     dx,GC_INDEX+1   ;restore the bit mask to its default,
        mov     al,0ffh         ; which selects all bits from the CPU
        out     dx,al           ; and none from the latches (the GC
                                ; Index still points to Bit Mask)
         ret
xdisplay_actors ENDP
;===========================================================================
PROC    xerase_actors
        PUBLIC xerase_actors
        ARG    actor_addr:WORD
        ARG    dst_page:WORD
        USES   si,di
        LOCAL  src_next_off:WORD
        LOCAL  dst_next_off:WORD
        LOCAL  rect_width:WORD
        LOCAL  rect_height:WORD
        LOCAL  src_bmap_width:WORD
        LOCAL  mask_seg_str:WORD
        LOCAL  actor_ptr:WORD
        LOCAL  actor_num:WORD

        cld
        mov     dx,GC_INDEX        ;set the bit mask to select all bits
        mov     ax,00000h+BIT_MASK ; from the latches and none from
        out     dx,ax              ; the CPU, so that we can write the
                                   ; latch contents directly to memory

        mov     dx,SC_INDEX
        mov     al,MAP_MASK
        out     dx,al           ;point SC Index register to Map Mask
        inc     dx              ;point to SC Data register
        mov     al,0ffh
        out     dx,al

        mov     ax,SCREEN_SEG      ;point ES to display memory
        mov     es,ax

        xor     ax,ax
        mov     actor_num,ax   ;actor=0

        mov     bx,actor_addr  ;get and save actor address
        mov     actor_ptr,bx

actor_loop:

        mov     al,[bx+185]    ;get USED flag
        or      al,al
        jnz     cont2

        mov     al,[bx+195]     ;get DEAD flag
        or      al,al
        jnz     cont1
        jmp     next_actor

cont1:
        dec    al              ;dec DEAD flag
        mov    [bx+195],al

cont2:
;calc starting offset in display mem

        mov     di,bx
        mov     si,dst_page   ;set LAST_X[PAGE]
        cmp     si,19000
        jb      page0
        add     di,2         ;add 2 if second page

page0:
        add     di,181       ;point to LAST_Y
        mov     dx,[di]      ;get LAST_Y coor

        sub     di,4
        mov     di,[di]      ;get LAST_X coor
        mov     ax,80
        mul     dl

        shr     di,1    ;X/4 = offset of first dest rect pixel in
        shr     di,1    ;scan line
        add     di,ax   ;offset of first dest rect pixel in page
        mov     si,di
        add     si,34720         ;offset of background page
        add     di,dst_page ;offset of first dest rect pixel

;calc distance from end of one dest scan line to start of next

        mov     dx,16 ;rect_height ;count down scan lines
        mov     bx,75
        push    ds
        push    es
        pop     ds
CopyRowsLoop:
        mov     cx,5  ;bx   ;rect_width ;width across

        rep     movsb

        add     di,bx  ;dst_next_off
        add     si,bx  ;dst_next_off
        dec     dx      ;word ptr rect_height ;count down scan lines
        jnz     CopyRowsLoop
        pop     ds

next_actor:
        mov     ax,actor_num
        inc     ax
        cmp     ax,MAX_ACTORS
        je      actors_done

        mov     actor_num,ax
        mov     bx,actor_ptr ;point to next actor
        add     bx,256            ;256=sizeof ACTOR struct
        mov     actor_ptr,bx
        jmp     actor_loop

actors_done:
        mov     dx,GC_INDEX+1   ;restore the bit mask to its default,
        mov     al,0ffh         ; which selects all bits from the CPU
        out     dx,al           ; and none from the latches (the GC
                                ; Index still points to Bit Mask)
        ret
xerase_actors ENDP
;===========================================================================
PROC    xput_plane

plp0:
        mov      dx,si     ;get width

plp1:   mov      al,[bx]
        cmp      al,0
        je	 pskp
        cmp      al,15
        je       pskp
        mov      es:[di],al
pskp:   inc	 di
        inc 	 bx
        dec	 dx
        jnz	 plp1
        add      di,80
        sub      di,si
        loop	 plp0

        ret
xput_plane ENDP
;===========================================================================
PROC    xput
        PUBLIC xput
        ARG    x:WORD,y:WORD,pagebase:WORD,buff:WORD
        USES   si,di,es
        LOCAL  wid:WORD
        LOCAL  height:WORD
        LOCAL  off_set:WORD
        LOCAL  invis_color:WORD

        mov     bx,buff

        mov     ax,[bx]    ;get width (in bytes)
jaxn01: mov     wid,ax
        inc     bx
        inc     bx

        mov     ax,[bx]    ;get height

jaxn02: mov     height,ax
        inc     bx
        inc     bx

        mov      ax,[bx]
        mov      invis_color,ax
        inc	 bx
        inc 	 bx

        mov      ax,y           ;calc and store offset
        cmp	 ax,239
        jge	 xpdne
        cmp	 ax,0
        jl       xpdne

        mov	 cl,80
        mul	 cl
        mov      dx,x
        cmp	 dx,319
        jge	 xpdne
        cmp	 dx,0
        jl       xpdne
        shr      dx,1
        shr      dx,1
        add	 ax,dx
        add      ax,pagebase
        mov	 off_set,ax

        mov      ax,0a000h
        mov	 es,ax
        jmp      short xpcnt

xpdne:  jmp      xpdone

xpcnt:  mov      dx,03c4h    ;enable plane 1
        mov	 al,2
        out	 dx,al
        inc	 dx
        mov	 al,1
        out	 dx,al

        mov      si,wid
        mov	 cx,height
        mov      di,off_set
        mov      ax,invis_color
        mov	 ah,al
        call     xput_plane

        mov      dx,03c4h    ;enable plane 2
        mov	 al,2
        out	 dx,al
        inc	 dx
        mov	 al,2
        out	 dx,al

        mov      si,wid
        mov	 cx,height
        mov      di,off_set
        mov      ax,invis_color
        mov	 ah,al
        call     xput_plane

        mov      dx,03c4h    ;enable plane 3
        mov	 al,2
        out	 dx,al
        inc	 dx
        mov	 al,4
        out	 dx,al

        mov      si,wid
        mov	 cx,height
        mov      di,off_set
        mov      ax,invis_color
        mov      ah,al
        call     xput_plane

        mov      dx,03c4h    ;enable plane 4
        mov	 al,2
        out	 dx,al
        inc	 dx
        mov	 al,8
        out	 dx,al

        mov      si,wid
        mov	 cx,height
        mov      di,off_set
        mov      ax,invis_color
        mov	 ah,al
        call     xput_plane

xpdone:
        ret
xput    ENDP
;===========================================================================
EXTRN joy_y:WORD
EXTRN joy_x:WORD
EXTRN joy_b1:BYTE
EXTRN joy_b2:BYTE

PROC    read_joystick
        PUBLIC  read_joystick
        USES di

        pushf
        cli                ;no interrupts
        xor     di,di
        xor     bx,bx
        mov     dx,201h
        out     dx,al      ;Any random number tell hardware to start
        mov     cx,-1
@@10:   in      al,dx
        test    al,3
        jz      @@90
        test    al,1
        jz      @@20
        inc     di
@@20:   test    al,2
        jz      @@30
        inc     bx
@@30:   loop    @@10

@@90:   mov     joy_y,bx
        mov     joy_x,di

        in      al,dx      ;read buttons
        mov     joy_b1,al
        and     joy_b1,10000b
        xor     joy_b1,10000b

        and     al,100000b
        xor     al,100000b
        mov     joy_b2,al

        popf               ;restore flags, (restores int bit)
        ret
read_joystick ENDP
;===========================================================================
EXTRN timer_cnt:WORD
EXTRN magic_cnt:WORD
EXTRN vbl_cnt:WORD
EXTRN slow_mode:BYTE
EXTRN main_loop:BYTE
;---------------------------------------------------------------------------
; Macro to wait for the vertical retrace leading edge

WaitVsyncStart   macro
	mov     dx,INPUT_STATUS_0
@@WaitNotVsync:
	in      al,dx
	test    al,08h
	jnz     @@WaitNotVsync
@@WaitVsync:
	in      al,dx
	test    al,08h
	jz      @@WaitVsync
	endm
;---------------------------------------------------------------------------
; Macro to wait for the vertical retrace trailing edge

WaitVsyncEnd    macro
	mov     dx,INPUT_STATUS_0
@@WaitVsync2:
	in      al,dx
	test    al,08h
	jz     @@WaitVsync2
@@WaitNotVsync2:
	in      al,dx
	test    al,08h
	jnz      @@WaitNotVsync2
	endm
;===========================================================================
;change displayed page in mode X

PAL_SPEED equ 10

PROC    xshowpage
        PUBLIC xshowpage
        ARG    Off:WORD

        mov     bl,0dh
        mov     bh,byte ptr [Off]
        mov     cl,0ch
        mov     ch,byte ptr [Off+1]

        mov     dx,03d4h
        mov     ax,bx
        out     dx,ax   ;start address low
        mov     ax,cx
        out     dx,ax   ;start address high

        mov     dx,03dah

wait_nvs:
        in      al,dx
        test    al,08h
        jnz     wait_nvs
wait_vs:
        in      al,dx
        test    al,08h
        jz      wait_vs

        mov     al,main_loop
        cmp     al,0
        jne     pal_ok
        jmp     xpdone

pal_ok: inc     palloop
        mov     ax,PAL_SPEED
        mov     cl,ds:slow_mode
        shr     ax,cl

        cmp     palloop,al
        ja      pal_reset

        sub     ax,4
        cmp     al,palloop
        je      resetpal2
        inc     ax

        cmp     al,palloop
        je      setpal2
        inc     ax

        cmp     al,palloop
        je      resetpal1
        inc     ax

        cmp     al,palloop
        je      setpal1
        jmp short xpdone

pal_reset:
        mov     palloop,0
        jmp short xpdone
        mov     palloop,0

resetpal2:
        mov     bx,offset palclr2
        mov     ax,palcnt2
        add     bx,ax
        mov     ax,[bx]
        inc     bx
        inc     bx
        mov     cx,[bx]
        jmp short xpst

setpal2:
        mov     bx,offset palset2
        mov     ax,palcnt2
        add     bx,ax
        add     ax,4
        cmp     ax,16
        jb      pals2
        xor     ax,ax
pals2:  mov     palcnt2,ax

        mov     ax,[bx]
        inc     bx
        inc     bx
        mov     cx,[bx]
        mov     bx,ax
        jmp short xpst

resetpal1:
        mov     bx,offset palclr1
        mov     ax,palcnt1
        add     bx,ax
        mov     ax,[bx]
        inc     bx
        inc     bx
        mov     cx,[bx]
        jmp short xpst

setpal1:
        mov     bx,offset palset1
        mov     ax,palcnt1
        add     bx,ax
        add     ax,4
        cmp     ax,16
        jb      pals1
        xor     ax,ax
pals1:  mov     palcnt1,ax

        mov     ax,[bx]
        inc     bx
        inc     bx
        mov     cx,[bx]
        mov     bx,ax

xpst:
        mov     bx,ax
        mov     al,bh
        mov     dx,DAC_WRITE_INDEX  ;tell DAC what color index to
        out     dx,al               ;write to
        mov     dx,DAC_DATA

        mov     al,bl               ;Set the red component
        out     dx,al
        mov     al,ch               ;Set the green component
        out     dx,al
        mov     al,cl               ;Set the blue component
        out     dx,al

xpdone:
        ret

xshowpage    ENDP
;============================================================================
PROC    xrawpalset

        mov  al,bh
        mov  dx,DAC_WRITE_INDEX  ; Tell DAC what colour index to
        out  dx,al               ; write to
        mov  dx,DAC_DATA

        mov  al,bl              ; Set the red component
        out  dx,al
        jmp  $+2

        mov  al,ch              ; Set the green component
        out  dx,al
        jmp  $+2

        mov  al,cl              ; Set the blue component
        out  dx,al
        ret

xrawpalset  ENDP
;============================================================================
PROC    xsetpal
        PUBLIC xsetpal
        ARG    ColorIndex:byte,R:byte,G:byte,B:byte

	mov  al,ColorIndex
	mov  dx,DAC_WRITE_INDEX  ; Tell DAC what colour index to
	out  dx,al               ; write to
	mov  dx,DAC_DATA

	mov  al,R              ; Set the red component
	out  dx,al
        jmp  $+2
	mov  al,G              ; Set the green component
	out  dx,al
        jmp  $+2
	mov  al,B              ; Set the blue component
	out  dx,al
	ret
xsetpal ENDP
;============================================================================
PROC    xgetpal
        PUBLIC xgetpal
        ARG  PalBuff:dword,NumColors:word,StartColor:word
        USES si,di,es,ds

        les   di,dword ptr PalBuff  ; Point es:di to palette buffer

        mov  si,StartColor
        mov  cx,NumColors

ReadPalEntry:
        cld
        WaitVsyncStart
        mov  ax,si
        mov  dx,DAC_READ_INDEX
        out  dx,al                    ; Tell DAC what colour to start reading
        mov  dx,DAC_DATA

        mov  bx,cx                    ; set cx to Num Colors * 3 ( size of
        shl  bx,1                     ; palette buffer)
        add  cx,bx
        rep  insb                     ; read the palette enntries
        ret
xgetpal ENDP
;============================================================================
PROC    xtext_plane

plp0:
        mov        dx,2     ;get width

plp1:   mov        al,[bx]
        cmp	   al,0
        je	   pskp
        mov        es:[di],ah
pskp:   inc	   di
        inc 	   bx
        dec	   dx
        jnz	   plp1
        add        di,80
        sub        di,2
        loop	   plp0
        ret
xtext_plane ENDP
;============================================================================
xtext1  PROC
        PUBLIC  xtext1
        ARG     x:WORD
        ARG     y:WORD
        ARG     pbase:WORD
        ARG     bff:WORD
        ARG     segm:WORD
        ARG     color:WORD
        USES    si,di,es,ds
        LOCAL   off_set:WORD

        mov        ax,segm
        mov        ds,ax
        mov        bx,bff
        mov        ax,y           ;calc and store offset
        cmp	   ax,239
        jge	   xtdne
        cmp	   ax,0
        jl         xtdne

        mov	   cl,80
        mul	   cl
        mov        dx,x
        cmp	   dx,319
        jge	   xtdne
        cmp	   dx,0
        jl         xtdne
        shr        dx,1
        shr        dx,1
        add	   ax,dx
        add        ax,pbase
        add        ax,80
        mov	   off_set,ax

        mov        ax,0a000h
        mov	   es,ax
        jmp        short xtcnt

xtdne:  jmp        xtdone

xtcnt:  mov        dx,03c4h    ;enable plane 2
        mov	   al,2
        out	   dx,al
        inc	   dx
        mov	   al,2
        out	   dx,al

        mov	   cx,9
        mov        di,off_set
        mov        ax,color
        mov	   ah,al
        call       xtext_plane

        mov        dx,03c4h    ;enable plane 3
        mov	   al,2
        out	   dx,al
        inc	   dx
        mov	   al,4
        out	   dx,al

        mov	   cx,9
        mov        di,off_set
        mov        ax,color
        mov	   ah,al
        call       xtext_plane

        mov        dx,03c4h    ;enable plane 4
        mov	   al,2
        out	   dx,al
        inc	   dx
        mov	   al,8
        out	   dx,al

        mov	   cx,9
        mov        di,off_set
        mov        ax,color
        mov	   ah,al
        call       xtext_plane

        mov        dx,03c4h    ;enable plane 1
        mov	   al,2
        out	   dx,al
        inc	   dx
        mov	   al,1
        out	   dx,al

        mov	   cx,9
        mov        di,off_set
        inc        di
        mov        ax,color
        mov	   ah,al
        call       xtext_plane

xtdone:

        ret
xtext1  ENDP
;============================================================================
xtextx  PROC
        PUBLIC  xtextx
        ARG     x:WORD
        ARG     y:WORD
        ARG     pbase:WORD
        ARG     bff:WORD
        ARG     segm:WORD
        ARG     color:WORD
        USES    si,di,es,ds
        LOCAL   off_set:WORD
        LOCAL   xcntr:byte
        LOCAL   xbits:byte

        mov        ax,segm
        mov        ds,ax
        mov        bx,bff
        mov        ax,y           ;calc and store offset
        cmp	   ax,239
        jge	   xtdne
        cmp	   ax,0
        jl         xtdne

        mov	   cl,80
        mul	   cl
        mov        dx,x
        cmp	   dx,319
        jge	   xtdne
        cmp	   dx,0
        jl         xtdne
        shr        dx,1
        shr        dx,1
        add	   ax,dx
        add        ax,pbase
;        add        ax,80
        mov	   off_set,ax

        mov        ax,0a000h
        mov	   es,ax
        jmp        short xtcnt

xtdne:  jmp        xtdone

xtcnt:  mov        xcntr,4
        mov        ax,[x]
        mov        cl,4
        div        cl
        mov        cl,ah
        mov        al,1
        shl        al,cl

xtlp1:  mov        xbits,al
        mov        dx,03c4h    ;enable plane
        mov	   al,2
        out	   dx,al
        inc	   dx
        mov	   al,xbits
        out	   dx,al

        mov	   cx,9
        mov        di,off_set
        mov        ax,color
        mov	   ah,al
        call       xtext_plane

        dec        xcntr
        jz         xtdone

        mov        al,xbits
        shl        al,1
        cmp        al,16
        jl         xtlp1
        inc        off_set
        mov        al,1
        jmp        xtlp1

xtdone:

        ret
xtextx  ENDP
;============================================================================
PROC    xcopyd2d
        PUBLIC  xcopyd2d
        ARG     SourceStartX:WORD
        ARG     SourceStartY:WORD
        ARG     SourceEndX:WORD
        ARG     SourceEndY:WORD
        ARG     DestStartX:WORD
        ARG     DestStartY:WORD
        ARG     SourcePageBase:WORD
        ARG     DestPageBase:WORD
        ARG     SourceBitmapWidth:WORD
        ARG     DestBitmapWidth:WORD
        USES    si,di,ds,es
        LOCAL   SourceNextScanOffset:WORD
        LOCAL   DestNextScanOffset:WORD
        LOCAL   RectAddrWidth:WORD
        LOCAL   Height:WORD

        cld
        mov     dx,GC_INDEX     ;set the bit mask to select all bits
        mov     ax,00000h+BIT_MASK ; from the latches and none from
        out     dx,ax           ; the CPU, so that we can write the
                                ; latch contents directly to memory
        mov     ax,SCREEN_SEG   ;point ES to display memory
        mov     es,ax
        mov     ax,DestBitmapWidth
        shr     ax,1            ;convert to width in addresses
        shr     ax,1
        mul     DestStartY ;top dest rect scan line
        mov     di,DestStartX
        shr     di,1    ;X/4 = offset of first dest rect pixel in
        shr     di,1    ; scan line
        add     di,ax   ;offset of first dest rect pixel in page
        add     di,DestPageBase ;offset of first dest rect pixel
                                ; in display memory
        mov     ax,SourceBitmapWidth
        shr     ax,1            ;convert to width in addresses
        shr     ax,1
        mul     SourceStartY ;top source rect scan line
        mov     si,SourceStartX
        mov     bx,si
        shr     si,1    ;X/4 = offset of first source rect pixel in
        shr     si,1    ; scan line
        add     si,ax   ;offset of first source rect pixel in page
        add     si,SourcePageBase ;offset of first source rect
                        ; pixel in display memory
        and     bx,0003h                 ;look up left edge plane mask
        mov     ah,LeftClipPlaneMask[bx] ; to clip
        mov     bx,SourceEndX
        and     bx,0003h                  ;look up right edge plane
        mov     al,RightClipPlaneMask[bx] ; mask to clip
        mov     bx,ax                   ;put the masks in BX

        mov     cx,SourceEndX   ;calculate # of addresses across
        mov     ax,SourceStartX ; rect
        cmp     cx,ax
        jle     CopyDone        ;skip if 0 or negative width
        dec     cx
        and     ax,not 011b
        sub     cx,ax
        shr     cx,1
        shr     cx,1    ;# of addresses across rectangle to copy - 1
        jnz     MasksSet ;there's more than one address to draw
        and     bh,bl   ;there's only one address, so combine the left
                        ; and right edge clip masks
MasksSet:
        mov     ax,SourceEndY
        sub     ax,SourceStartY  ;AX = height of rectangle
        jle     CopyDone        ;skip if 0 or negative height
        mov     Height,ax
        mov     ax,DestBitmapWidth
        shr     ax,1            ;convert to width in addresses
        shr     ax,1
        sub     ax,cx   ;distance from end of one dest scan line to
        dec     ax      ; start of next
        mov     DestNextScanOffset,ax
        mov     ax,SourceBitmapWidth
        shr     ax,1            ;convert to width in addresses
        shr     ax,1
        sub     ax,cx   ;distance from end of one source scan line to
        dec     ax      ; start of next
        mov     SourceNextScanOffset,ax
        mov     RectAddrWidth,cx ;remember width in addresses - 1

        mov     dx,SC_INDEX
        mov     al,MAP_MASK
        out     dx,al           ;point SC Index reg to Map Mask
        inc     dx              ;point to SC Data reg

        mov     ax,es   ;DS=ES=screen segment for MOVS
        mov     ds,ax
CopyRowsLoop:
        mov     cx,RectAddrWidth ;width across - 1
        mov     al,bh   ;put left-edge clip mask in AL
        out     dx,al   ;set the left-edge plane (clip) mask
        movsb           ;copy the left edge (pixels go through
                        ; latches)
        dec     cx      ;count off left edge address
        js      CopyLoopBottom ;that's the only address
        jz      DoRightEdge ;there are only two addresses
        mov     al,00fh ;middle addresses are drawn 4 pixels at a pop
        out     dx,al   ;set the middle pixel mask to no clip
        rep     movsb   ;draw the middle addresses four pixels apiece
                        ; (pixels copied through latches)
DoRightEdge:
        mov     al,bl   ;put right-edge clip mask in AL
        out     dx,al   ;set the right-edge plane (clip) mask
        movsb           ;draw the right edge (pixels copied through
                        ; latches)
CopyLoopBottom:
        add     si,SourceNextScanOffset ;point to the start of
        add     di,DestNextScanOffset   ; next source & dest lines
        dec     word ptr Height     ;count down scan lines
        jnz     CopyRowsLoop
CopyDone:
        mov     dx,GC_INDEX+1 ;restore the bit mask to its default,
        mov     al,0ffh         ; which selects all bits from the CPU
        out     dx,al           ; and none from the latches (the GC
                                ; Index still points to Bit Mask)
        ret
xcopyd2d ENDP
;============================================================================
PROC    xfillrectangle
        PUBLIC  xfillrectangle
        ARG     StartX:WORD
        ARG     StartY:WORD
        ARG     EndX:WORD
        ARG     EndY:WORD
        ARG     PageBase:WORD
        ARG     Color:WORD
        USES    si,di,bp,ds,es

        cld
        mov     ax,SCREEN_WIDTH
        mul     StartY ;offset in page of top rectangle scan line
        mov     di,StartX
        shr     di,1    ;X/4 = offset of first rectangle pixel in scan
        shr     di,1    ; line
        add     di,ax   ;offset of first rectangle pixel in page
        add     di,PageBase ;offset of first rectangle pixel in
                        ; display memory
        mov     ax,SCREEN_SEG   ;point ES:DI to the first rectangle
        mov     es,ax           ; pixel's address
        mov     dx,SC_INDEX ;set the Sequence Controller Index to
        mov     al,MAP_MASK ; point to the Map Mask register
        out     dx,al
        inc     dx      ;point DX to the SC Data register
        mov     si,StartX
        and     si,0003h                 ;look up left edge plane mask
        mov     bh,LeftClipPlaneMask[si] ; to clip & put in BH
        mov     si,EndX
        and     si,0003h                  ;look up right edge plane
        mov     bl,RightClipPlaneMask[si] ; mask to clip & put in BL

        mov     cx,EndX    ;calculate # of addresses across rect
        mov     si,StartX
        cmp     cx,si
        jle     FillDone        ;skip if 0 or negative width
        dec     cx
        and     si,not 011b
        sub     cx,si
        shr     cx,1
        shr     cx,1    ;# of addresses across rectangle to fill - 1
        jnz     MasksSet ;there's more than one byte to draw
        and     bh,bl   ;there's only one byte, so combine the left
                        ; and right edge clip masks
MasksSet:
        mov     si,EndY
        sub     si,StartY  ;BX = height of rectangle
        jle     FillDone        ;skip if 0 or negative height
        mov     ah,byte ptr Color ;color with which to fill
        mov     bp,SCREEN_WIDTH ;stack frame isn't needed any more
        sub     bp,cx   ;distance from end of one scan line to start
        dec     bp      ; of next
FillRowsLoop:
        push    cx      ;remember width in addresses - 1
        mov     al,bh   ;put left-edge clip mask in AL
        out     dx,al   ;set the left-edge plane (clip) mask
        mov     al,ah   ;put color in AL
        stosb           ;draw the left edge
        dec     cx      ;count off left edge byte
        js      FillLoopBottom ;that's the only byte
        jz      DoRightEdge ;there are only two bytes
        mov     al,00fh ;middle addresses are drawn 4 pixels at a pop
        out     dx,al   ;set the middle pixel mask to no clip
        mov     al,ah   ;put color in AL
        rep     stosb   ;draw the middle addresses four pixels apiece
DoRightEdge:
        mov     al,bl   ;put right-edge clip mask in AL
        out     dx,al   ;set the right-edge plane (clip) mask
        mov     al,ah   ;put color in AL
        stosb           ;draw the right edge
FillLoopBottom:
        add     di,bp   ;point to the start of the next scan line of
                        ; the rectangle
        pop     cx      ;retrieve width in addresses - 1
        dec     si      ;count down scan lines
        jnz     FillRowsLoop
FillDone:
        ret
xfillrectangle ENDP
;============================================================================
xtext   PROC
        PUBLIC  xtext
        ARG     x:WORD
        ARG     y:WORD
        ARG     pbase:WORD
        ARG     bff:WORD
        ARG     segm:WORD
        ARG     color:WORD
        USES    si,di,es,ds
        LOCAL   off_set:WORD

        mov        ax,segm
        mov        ds,ax
        mov        bx,bff
        mov        ax,y           ;calc and store offset
        cmp	   ax,239
        jge	   x0dne
        cmp	   ax,0
        jl         x0dne

        mov	   cl,80
        mul	   cl
        mov        dx,x
        cmp	   dx,319
        jge	   x0dne
        cmp	   dx,0
        jl         x0dne
        shr        dx,1
        shr        dx,1
        add	   ax,dx
        add        ax,pbase
;        add        ax,80
        mov	   off_set,ax

        mov        ax,0a000h
        mov	   es,ax
        jmp        short x0cnt

x0dne:  jmp        x0done

x0cnt:  mov        dx,03c4h    ;enable plane 1
        mov	   al,2
        out	   dx,al
        inc	   dx
        mov	   al,1
        out	   dx,al

        mov	   cx,9
        mov        di,off_set
        mov        ax,color
        mov	   ah,al
        call       xtext_plane

        mov        dx,03c4h    ;enable plane 2
        mov	   al,2
        out	   dx,al
        inc	   dx
        mov	   al,2
        out	   dx,al

        mov	   cx,9
        mov        di,off_set
        mov        ax,color
        mov	   ah,al
        call       xtext_plane

        mov        dx,03c4h    ;enable plane 3
        mov	   al,2
        out	   dx,al
        inc	   dx
        mov	   al,4
        out	   dx,al

        mov	   cx,9
        mov        di,off_set
        mov        ax,color
        mov	   ah,al
        call       xtext_plane

        mov        dx,03c4h    ;enable plane 4
        mov	   al,2
        out	   dx,al
        inc	   dx
        mov	   al,8
        out	   dx,al

        mov	   cx,9
        mov        di,off_set
        mov        ax,color
        mov	   ah,al
        call       xtext_plane

x0done:

        ret
xtext   ENDP
;============================================================================
; Index/data pairs for CRT Controller registers that differ between
; mode 13h and mode X.


PROC    xsetmode
        PUBLIC  xsetmode
        USES    si,di

        mov     ax,13h  ;let the BIOS set standard 256-color
        int     10h     ; mode (320x200 linear)

        mov     dx,SC_INDEX
        mov     ax,0604h
        out     dx,ax   ;disable chain4 mode
        mov     ax,0100h
        out     dx,ax   ;synchronous reset while switching clocks

        mov     dx,MISC_OUTPUT
        mov     al,0e3h  ;was e7
        out     dx,al   ;select 28 MHz dot clock & 60 Hz scanning rate

        mov     dx,SC_INDEX
        mov     ax,0300h
        out     dx,ax   ;undo reset (restart sequencer)

        mov     dx,CRTC_INDEX ;reprogram the CRT Controller
        mov     al,11h  ;VSync End reg contains register write
        out     dx,al   ; protect bit
        inc     dx      ;CRT Controller Data register
        in      al,dx   ;get current VSync End register setting
        and     al,7fh  ;remove write protect on various
        out     dx,al   ; CRTC registers
        dec     dx      ;CRT Controller Index
        cld
        mov     si,offset CRTParms ;point to CRT parameter table
        mov     cx,CRT_PARM_LENGTH ;# of table entries
SetCRTParmsLoop:
        lodsw           ;get the next CRT Index/Data pair
        out     dx,ax   ;set the next CRT Index/Data pair
        loop    SetCRTParmsLoop

        mov     dx,SC_INDEX
        mov     ax,0f02h
        out     dx,ax   ;enable writes to all four planes
        mov     ax,SCREEN_SEG ;now clear all display memory, 8 pixels
        mov     es,ax         ; at a time
        sub     di,di   ;point ES:DI to display memory
        sub     ax,ax   ;clear to zero-value pixels
        mov     cx,8000h ;# of words in display memory
        rep     stosw   ;clear all of display memory

        ret
xsetmode ENDP
;============================================================================
PROC    xfput_plane

        push    ds
        mov     ds,dx

pfp0:
        mov     dx,si     ;get width

pfp1:   mov     al,[bx]
        cmp     al,0
        je	pfkp
        cmp     al,15
        je	pfkp
        mov     es:[di],al
pfkp:   inc	di
        inc 	bx
        dec	dx
        jnz	pfp1
        add     di,80
        sub     di,si
        loop	pfp0
        pop     ds
        ret

xfput_plane ENDP
;============================================================================
PROC    xfput
        PUBLIC xfput
        ARG    x:WORD,y:WORD,pagebase:WORD,buff:WORD,segm:WORD
        USES   si,di,es
        LOCAL  wid:WORD
        LOCAL  height:WORD
        LOCAL  off_set:WORD
        LOCAL  invis_color:WORD

        mov     ax,segm
        mov     es,ax
        mov     bx,buff
        add     bx,6

        mov     ax,4
        mov     wid,ax

        mov     ax,16
        mov     height,ax

        mov     ax,y           ;calc and store offset
        cmp	ax,239
        jge	xfdne
        cmp	ax,0
        jl      xfdne

        mov	cl,80
        mul	cl
        mov     dx,x
        cmp	dx,319
        jge	xfdne
        cmp	dx,0
        jl      xfdne
        shr     dx,1
        shr     dx,1
        add	ax,dx
        add     ax,pagebase
        mov	off_set,ax

        mov     ax,0a000h
        mov	es,ax
        jmp     short xfcnt

xfdne:  jmp     xfdone

xfcnt:  mov     dx,03c4h    ;enable plane 1
        mov	al,2
        out	dx,al
        inc	dx
        mov	al,1
        out	dx,al

        mov     si,wid
        mov	cx,height
        mov     di,off_set
        mov     dx,segm
        call    xfput_plane

        mov     dx,03c4h    ;enable plane 2
        mov	al,2
        out	dx,al
        inc	dx
        mov	al,2
        out	dx,al

        mov     si,wid
        mov	cx,height
        mov     di,off_set
        mov     dx,segm
        call    xfput_plane

        mov     dx,03c4h    ;enable plane 3
        mov	al,2
        out	dx,al
        inc	dx
        mov	al,4
        out	dx,al

        mov     si,wid
        mov	cx,height
        mov     di,off_set
        mov     dx,segm
        call    xfput_plane

        mov     dx,03c4h    ;enable plane 4
        mov	al,2
        out	dx,al
        inc	dx
        mov	al,8
        out	dx,al

        mov     si,wid
        mov	cx,height
        mov     di,off_set
        mov     dx,segm
        call    xfput_plane

xfdone:
        ret
xfput   ENDP
;============================================================================
PROC    xfarput_plane

        push    ds
        mov     ds,dx

pfp0:
        mov     dx,si     ;get width

pfp1:   mov     al,[bx]
        mov     es:[di],al
pfkp:   inc	di
        inc 	bx
        dec	dx
        jnz	pfp1
        add     di,80
        sub     di,si
        loop	pfp0
        pop     ds
        ret

xfarput_plane ENDP
;============================================================================
PROC    xfarput
        PUBLIC xfarput
        ARG    x:WORD,y:WORD,pagebase:WORD,buff:WORD,segm:WORD
        USES   si,di,es
        LOCAL  wid:WORD
        LOCAL  height:WORD
        LOCAL  off_set:WORD
        LOCAL  invis_color:WORD

        mov     ax,segm
        mov     es,ax
        mov     bx,buff

        mov     ax,es:[bx]    ;get width (in bytes)
        mov     wid,ax
        inc     bx
        inc     bx

        mov     ax,es:[bx]    ;get height

        mov     height,ax
        add     bx,4

        mov     ax,y           ;calc and store offset
        cmp	ax,239
        jge	xfdne
        cmp	ax,0
        jl      xfdne

        mov	cl,80
        mul	cl
        mov     dx,x
        cmp	dx,319
        jge	xfdne
        cmp	dx,0
        jl      xfdne
        shr     dx,1
        shr     dx,1
        add	ax,dx
        add     ax,pagebase
        mov	off_set,ax

        mov     ax,0a000h
        mov	es,ax
        jmp     short xfcnt

xfdne:  jmp     xfdone

xfcnt:  mov     dx,03c4h    ;enable plane 1
        mov	al,2
        out	dx,al
        inc	dx
        mov	al,1
        out	dx,al

        mov     si,wid
        mov	cx,height
        mov     di,off_set
        mov     dx,segm
        call    xfarput_plane

        mov     dx,03c4h    ;enable plane 2
        mov	al,2
        out	dx,al
        inc	dx
        mov	al,2
        out	dx,al

        mov     si,wid
        mov	cx,height
        mov     di,off_set
        mov     dx,segm
        call    xfarput_plane

        mov     dx,03c4h    ;enable plane 3
        mov	al,2
        out	dx,al
        inc	dx
        mov	al,4
        out	dx,al

        mov     si,wid
        mov	cx,height
        mov     di,off_set
        mov     dx,segm
        call    xfarput_plane

        mov     dx,03c4h    ;enable plane 4
        mov	al,2
        out	dx,al
        inc	dx
        mov	al,8
        out	dx,al

        mov     si,wid
        mov	cx,height
        mov     di,off_set
        mov     dx,segm
        call    xfarput_plane

xfdone:
        ret
xfarput   ENDP
;============================================================================
PROC    xcopys2d
        PUBLIC  xcopys2d
        ARG     SourceStartX:WORD
        ARG     SourceStartY:WORD
        ARG     SourceEndX:WORD
        ARG     SourceEndY:WORD
        ARG     DestStartX:WORD
        ARG     DestStartY:WORD
        ARG     SourcePtr:WORD
        ARG     DestPageBase:WORD
        ARG     SourceBitmapWidth:WORD
        ARG     DestBitmapWidth:WORD
        USES    si,di,ds,es
        LOCAL   RectWidth:WORD
        LOCAL   LeftMask:WORD

        cld
        mov     ax,SCREEN_SEG   ;point ES to display memory
        mov     es,ax
        mov     ax,SourceBitmapWidth
        mul     SourceStartY ;top source rect scan line
        add     ax,SourceStartX
        add     ax,SourcePtr ;offset of first source rect pixel
        mov     si,ax             ; in DS

        mov     ax,DestBitmapWidth
        shr     ax,1            ;convert to width in addresses
        shr     ax,1
        mov     DestBitmapWidth,ax ;remember address width
        mul     DestStartY ;top dest rect scan line
        mov     di,DestStartX
        mov     cx,di
        shr     di,1    ;X/4 = offset of first dest rect pixel in
        shr     di,1    ; scan line
        add     di,ax   ;offset of first dest rect pixel in page
        add     di,DestPageBase ;offset of first dest rect pixel
                                ; in display memory
        and     cl,011b ;CL = first dest pixel's plane
        mov     al,11h  ;upper nibble comes into play when plane wraps
                        ; from 3 back to 0
        shl     al,cl   ;set the bit for the first dest pixel's plane
        mov     byte ptr LeftMask,al ; in each nibble to 1

        mov     cx,SourceEndX   ;calculate # of pixels across
        sub     cx,SourceStartX ; rect
        jle     CopyDone        ;skip if 0 or negative width
        mov     RectWidth,cx
        mov     bx,SourceEndY
        sub     bx,SourceStartY  ;BX = height of rectangle
        jle     CopyDone        ;skip if 0 or negative height
        mov     dx,SC_INDEX     ;point to SC Index register
        mov     al,MAP_MASK
        out     dx,al           ;point SC Index reg to the Map Mask
        inc     dx              ;point DX to SC Data reg
CopyRowsLoop:
        mov     ax,LeftMask
        mov     cx,RectWidth
        push    si      ;remember the start offset in the source
        push    di      ;remember the start offset in the dest
CopyScanLineLoop:
        out     dx,al           ;set the plane for this pixel
        movsb                   ;copy the pixel to the screen
        rol     al,1            ;set mask for next pixel's plane
        cmc                     ;advance destination address only when
        sbb     di,0            ; wrapping from plane 3 to plane 0
                                ; (else undo INC DI done by MOVSB)
        loop    CopyScanLineLoop
        pop     di      ;retrieve the dest start offset
        add     di,DestBitmapWidth ;point to the start of the
                                        ; next scan line of the dest
        pop     si      ;retrieve the source start offset
        add     si,SourceBitmapWidth ;point to the start of the
                                        ; next scan line of the source
        dec     bx      ;count down scan lines
        jnz     CopyRowsLoop
CopyDone:
        ret
xcopys2d ENDP
;============================================================================
PROC    xpset
        PUBLIC xpset
        ARG    X:WORD,Y:WORD,PageBase:WORD,Color:WORD
        USES   si,di,es

        mov     ax,SCREEN_WIDTH
        mul     Y       	 ;offset of pixel's scan line in page
        mov     bx,X
        shr     bx,1
        shr     bx,1         	 ;X/4 = offset of pixel in scan line
        add     bx,ax        	 ;offset of pixel in page
        add     bx,PageBase 	 ;offset of pixel in display memory
        mov     ax,SCREEN_SEG
        mov     es,ax        	 ;point ES:BX to the pixel's address

        mov     cl,byte ptr X
        and     cl,011b      	 ;CL = pixel's plane
        mov     ax,0100h + MAP_MASK ;AL = index in SC of Map Mask reg
        shl     ah,cl               ;set only bit for the pixel's plane to 1
        mov     dx,SC_INDEX  	 ;set the Map Mask to enable only the
        out     dx,ax        	 ;pixel's plane

        mov     al,byte ptr Color
        mov     es:[bx],al   	 ;draw the pixel in the desired color

        ret
xpset   ENDP
;============================================================================
PROC    xpoint
        PUBLIC xpoint
        ARG    X:WORD,Y:WORD,PageBase:WORD
        USES   si,di,es

        mov     ax,SCREEN_WIDTH
        mul     Y  ;offset of pixel's scan line in page
        mov     bx,X
        shr     bx,1
        shr     bx,1    ;X/4 = offset of pixel in scan line
        add     bx,ax   ;offset of pixel in page
        add     bx,PageBase ;offset of pixel in display memory
        mov     ax,SCREEN_SEG
        mov     es,ax   ;point ES:BX to the pixel's address

        mov     ah,byte ptr X
        and     ah,011b ;AH = pixel's plane
        mov     al,READ_MAP ;AL = index in GC of the Read Map reg
        mov     dx,GC_INDEX ;set the Read Map to read the pixel's
        out     dx,ax       ; plane

        mov     al,es:[bx] ;read the pixel's color
        sub     ah,ah   ;convert it to an unsigned int

        ret
xpoint  ENDP
;============================================================================;
FADE_INCR	equ	2

PROC    pal_fade_in
        PUBLIC pal_fade_in
        ARG    PaletteBuffer:DWORD
        USES   ds,si,di,es

	cld
	mov bl, 0
	mov bh, FADE_INCR

	jmp PALETTEFADER

pal_fade_in endp
;============================================================================;
PROC    pal_fade_out
        PUBLIC pal_fade_in
        ARG    PaletteBuffer:WORD
        USES   ds,si,di,es

	cld
	mov bl, 64
	mov bh, -FADE_INCR

PALETTEFADER:

	mov cx, 64/FADE_INCR
	mov ax, ds
	mov es, ax

@@BIGLOOP:
	push cx
	add bl, bh

; SET COLOR LEVELS
	mov si, [PaletteBuffer]
	mov di, OFFSET TempPalette

	mov cx, 768
@@LOOPLEV1:
	lodsb
	mul bl
	shr ax, 6
	stosb
	dec cx
	jnz @@LOOPLEV1

; SET PALETTE
	mov si, OFFSET TempPalette

	mov dx, 3C8h
	xor al, al
	out dx, al
	inc dx

;	pushf
;	cli

REPT 1
	mov cx, 768/1
        push    dx
        push    ax
        WaitVsyncStart
        pop     ax
        pop     dx
	rep outsb
ENDM

;	popf
	pop cx
	dec cx
	jnz @@BIGLOOP

	ret
pal_fade_out ENDP
;============================================================================;
PROC    xget_plane

        push    ds
        mov     dx,0a000h
        mov     ds,dx
glp0:

        mov     dx,si     ;get width

glp1:   mov     al,ds:[di]
        mov     es:[bx],al
        inc	  di
        inc 	  bx
        dec	  dx
        jnz	  glp1
        add     di,80
        sub     di,si
        loop	  glp0

        pop      ds
        ret
xget_plane ENDP
;============================================================================;
PROC    xget
        PUBLIC xget
        ARG    x1:WORD,y1:WORD,x2:WORD,y2:WORD,pagebase:WORD,buff:WORD,segm:WORD,invis:WORD
        USES   si,di,es
        LOCAL  wid:WORD
        LOCAL  height:WORD
        LOCAL  off_set:WORD

        mov     ax,segm
        mov     es,ax
        mov     bx,[buff]

        mov     ax,[x2]    ;store width (in bytes)
        mov	  cx,[x1]
        cmp	  cx,ax
        jl      x_ok
        mov	  dx,ax
        mov	  ax,cx
        mov	  cx,dx

x_ok:   sub     ax,cx
        inc	  ax
        shr     ax,1
        shr     ax,1
        mov     es:[bx],ax
        mov     [wid],ax
        inc	  bx
        inc 	  bx

        mov     ax,[y2]    ;store height
        mov	  cx,[y1]
        cmp	  cx,ax
        jl      y_ok
        mov	  dx,ax
        mov	  ax,cx
        mov	  cx,dx

y_ok:   sub     ax,cx
        inc	  ax
        mov     es:[bx],ax
        mov     [height],ax
        inc	  bx
        inc 	  bx

        mov     ax,[invis]   ;store invisible color
        mov	es:[bx],ax
        inc     bx
        inc	  bx


        mov     ax,[y1]           ;calc and store offset
        mov	  cl,80
        mul	  cl
        mov     dx,[x1]
        shr     dx,1
        shr     dx,1
        add	  ax,dx
        add     ax,[pagebase]
        mov	  [off_set],ax

;        mov     ax,0a000h
;        mov	  es,ax

        mov     dx,03ceh    ;enable plane 0
        mov	  al,4
        out	  dx,al
        inc	  dx
        mov	  al,0
        out	  dx,al

        mov     si,[wid]
        mov	  cx,[height]
        mov     di,[off_set]
        call    xget_plane

        mov     dx,03ceh    ;enable plane 1
        mov	  al,4
        out	  dx,al
        inc	  dx
        mov	  al,1
        out	  dx,al

        mov     si,[wid]
        mov	  cx,[height]
        mov     di,[off_set]
        call    xget_plane

        mov     dx,03ceh    ;enable plane 2
        mov	  al,4
        out	  dx,al
        inc	  dx
        mov	  al,2
        out	  dx,al

        mov     si,[wid]
        mov	  cx,[height]
        mov     di,[off_set]
        call    xget_plane

        mov     dx,03ceh    ;enable plane 3
        mov	  al,4
        out	  dx,al
        inc	  dx
        mov	  al,3
        out	  dx,al

        mov     si,[wid]
        mov	  cx,[height]
        mov     di,[off_set]
        call    xget_plane

        ret
xget    ENDP

END

