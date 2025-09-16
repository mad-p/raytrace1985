;
;
;     floating point package
;
;	by K.Maeda
;

@BIGMODEL	EQU	0

	include	prologue.h


@code	ends

BDOS	equ	21h

REAL	struc

EXP	db	?
SIGN	db	?
MAN	dw	?

REAL	ends

@datac	segment

	public	DATASEG
	public	LOGTBL
	public	EXPTBL

SEGLG1	segment word public 'LOGT1'
SEGLG1	ends
SEGEX1	segment word public 'EXPT1'
SEGEX1	ends

DATASEG	dw	SEG DGROUP
LOGTBL	dw	SEG SEGLG1
EXPTBL	dw	SEG SEGEX1

@datac	ends

@code	segment

	public	INTER
	public	FADD
	public	FSUB
	public	FMLT
	public	FCMLT
	public	FDIV
	public	FCOMP
	public	FSQAR
	public	FSQRT
	public	FSCALE
	public	FINV
	public	FLOG
	public	FEXP
	public	ITOR
	public	RTOI
	public	RTOIF

INTER	PROC	near

	push	bp
	mov	bp,sp
	push	es
	mov	si,word ptr 6[bp]
	mov	bx,word ptr 8[bp]
	mov	ax,bx
	mov	di,word ptr 10[bp]
	call	ss:word ptr 4[bp]
	pop	es
	pop	bp
	pushf
	pop	dx
	ret

INTER	ENDP

FMLT	PROC	near

	push	dx
	mov	al,byte ptr [si].MAN+1
	and	al,byte ptr [bx].MAN+1
	jns	MLT0
	mov	di,[bx].MAN
	shl	di,1
	mov	ax,LOGTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
FMLTX:	mov	ch,[bx].SIGN
	xor	[si].SIGN,ch
	mov	ch,[si].EXP
	add	ch,[bx].EXP
	jo	MLT1
	mov	di,[si].MAN
	shl	di,1
	add	dx,es:word ptr [di]
	adc	ch,0
	jo	MLT99
	inc	dx
	jnz	MLT100
	mov	dx,8000h
	inc	ch
	jo	MLT99
	jmp	short	MLT98
MLT100:	and	dx,0fffeh
	mov	di,dx
	mov	ax,EXPTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
MLT98:	mov	[si].MAN,dx
	mov	[si].EXP,ch
	pop	dx
	ret

MLT1:	jg	MLT99
MLT0:	xor	ax,ax
	mov	[si].MAN,ax
	mov	word ptr [si].EXP,ax
	pop	dx
	ret

MLT99:	mov	[si].MAN,0ffffh
	mov	[si].EXP,7fh
	pop	dx
	ret

FCMLT	PROC	near

	push	dx
	test	byte ptr [si].MAN+1,80h
	jz	MLT0
	mov	ax,LOGTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
	mov	dx,[bx].MAN
	jmp	FMLTX

FCMLT	ENDP

FMLT	ENDP


FDIV	PROC	near

	push	dx
	mov	al,[bx].SIGN
	xor	[si].SIGN,al
	test	byte ptr [bx].MAN+1,80h
	jz	DIVERR
	test	byte ptr [si].MAN+1,80h
	jz	DIV0
	mov	ch,[si].EXP
	sub	ch,[bx].EXP
	jo	DIV1
	mov	di,[si].MAN
	shl	di,1
	mov	ax,LOGTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
	mov	di,[bx].MAN
	shl	di,1
	sub	dx,es:word ptr [di]
	sbb	ch,0
	jo	DIV0
	inc	dx
	jnz	DIV100
	mov	dx,8000h
	inc	ch
	jo	DIV99
	jmp	short	DIV98
DIV100:	and	dx,0fffeh
	mov	di,dx
	mov	ax,EXPTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
DIV98:	mov	[si].MAN,dx
	mov	[si].EXP,ch
	pop	dx
	ret

DIV1:	jg	DIV99
DIV0:	xor	ax,ax
	mov	[si].MAN,ax
	mov	word ptr [si].EXP,ax
	pop	dx
	ret

DIVERR:	mov	dx,offset DGROUP:D0MSG
	mov	ah,9
	int	BDOS
DIV99:	mov	[si].MAN,0ffffh
	mov	[si].EXP,7fh
	pop	dx
	ret

FINV	PROC	near

	push	dx
	mov	dx,[si].MAN
	test	dh,80h
	je	DIVERR
	neg	[si].EXP
	jo	INV99
	mov	ax,LOGTBL
	mov	es,ax
	mov	di,dx
	shl	di,1
	mov	di,es:word ptr [di]
	neg	di
	sbb	[si].EXP,0
	inc	di
	jnz	INV1
	mov	dx,8000h
	inc	[si].EXP
	jmp	short	INV98
INV1:	and	di,0fffeh
	mov	ax,EXPTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
INV98:	mov	[si].MAN,dx
	pop	dx
	ret
INV99:	mov	[si].MAN,0ffffh
	mov	[si].EXP,7fh
	pop	dx
	ret

FINV	ENDP

FDIV	ENDP

@code	ENDS

@datab	segment

D0MSG	db	'Division by ZERO !!$'

@datab	ENDS

@code	segment

FSQRT	PROC	near

	push	dx
	test	byte ptr [si].MAN+1,80h
	jz	sqrt99
	mov	ch,[si].EXP
	mov	di,[si].MAN
	shl	di,1
	mov	ax,LOGTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
	sar	ch,1
	rcr	dx,1
	inc	dx
	jnz	SQRT10
	mov	dx,8000h
	inc	ch
	jo	SQRT99
	jmp	short	SQRT98	
SQRT10:	and	dx,0fffeh
	mov	di,dx
	mov	ax,EXPTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
SQRT98:	mov	[si].MAN,dx
	mov	[si].EXP,ch
SQRT99:	pop	dx
	ret

FSQRT	ENDP

FSQAR	PROC	near

	push	dx
	test	byte ptr [si].MAN+1,80h
	jz	SQAR999
	mov	di,[si].MAN
	shl	di,1
	mov	ax,LOGTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
	shl	dx,1
	mov	ch,[si].EXP
	adc	ch,ch
	jo	SQAR1
	and	dx,0fffeh
	mov	di,dx
	mov	ax,EXPTBL
	mov	es,ax
	mov	dx,es:word ptr [di]
SQAR98:	mov	[si].MAN,dx
	mov	[si].SIGN,0
	mov	[si].EXP,ch
	pop	dx
	ret
SQAR1:	jg	SQAR99
	xor	ax,ax
	mov	[si].MAN,ax
	mov	word ptr [si].EXP,ax
	pop	dx
	ret
SQAR99:	mov	[si].MAN,0ffffh
	mov	word ptr [si].EXP,7fh
SQAR999:pop	dx
	ret

FSQAR	ENDP

FSUB	PROC	near

	xor	[bx].SIGN,1
	call	FADD
	xor	[bx].SIGN,1
	ret

FSUB	ENDP

FADD	PROC	near

	push	dx
	push	bx
	push	si
	test	byte ptr [bx].MAN+1,80h
	jz	ADD2
	test	byte ptr [si].MAN+1,80h
	jnz	ADD100
	xchg	bx,si
	jmp	short	ADD2
ADD100:	mov	ch,0
	mov	cl,[si].EXP
	sub	cl,[bx].EXP
	jns	ADD1
	neg	cl
	xchg	si,bx
ADD1:	cmp	cl,16
	jae	ADD2
	mov	dx,[bx].MAN
	jcxz	ADD10
	shr	dx,cl
ADD10:	mov	al,[bx].SIGN
	xor	al,[si].SIGN
	mov	di,[si].MAN
	mov	ax,word ptr [si].EXP
	jne	ADD11
	add	dx,di
	jnc	ADD15
	rcr	dx,1
	inc	al
	jo	ADD99
	jmp	short	ADD3
ADD11:	sub	di,dx
	mov	dx,di
	jz	ADD0
	jnc	ADD15
	neg	dx
	xor	ah,1
ADD15:	shl	dx,1
	jc	ADD4
	dec	al
	jo	ADD0
	jmp	short	ADD15
ADD2:	mov	dx,[si].MAN
	mov	ax,word ptr [si].EXP
	jmp	short	ADD3
ADD4:	rcr	dx,1
ADD3:	pop	si
	mov	[si].MAN,dx
	mov	word ptr [si].EXP,ax
	pop	bx
	pop	dx
	ret
ADD0:	xor	ax,ax
	xor	dx,dx
	jmp	short	ADD3
ADD99:	mov	al,7fh
	mov	dx,0ffffh
	jmp	short	ADD3

FADD	ENDP

FCOMP	PROC	near

	mov	al,[si].SIGN
	xor	al,[bx].SIGN
	jz	CMP1
CMP0:	test	[si].SIGN,1
	jz	CMPGT
	jmp	short	CMPLT
CMP1:	test	byte ptr [bx].MAN+1,80h
	jnz	CMP3
CMP2:	test	byte ptr [si].MAN+1,80h
	jz	CMPEQ
	jmp	short	CMP0
CMP3:	test	byte ptr [si].MAN+1,80h
	jnz	CMP4
	xchg	si,bx
	call	CMP2
	xchg	si,bx
	neg	ax
	ret
CMP4:	mov	al,[si].EXP
	cmp	al,[bx].EXP
	jg	CMP5
	jl	CMP6
	mov	ax,[si].MAN
	cmp	ax,[bx].MAN
	jz	CMPEQ
	jl	CMP6
CMP5:	mov	ax,1
	jmp	short	CMP7
CMP6:	mov	ax,-1
CMP7:	test	[si].SIGN,1
	jz	CMP8
	neg	ax
	ret
CMPGT:	mov	ax,1
	jmp	short	CMP8
CMPLT:	mov	ax,-1
CMP8:	or	ax,ax
	ret
CMPEQ:	xor	ax,ax
	ret

FCOMP	ENDP

FSCALE	PROC

	test	byte ptr [si].MAN+1,80h
	jz	SCA98
	add	[si].EXP,al
	jo	SCA1
SCA98:	ret
SCA1:	jnl	SCA99
	xor	ax,ax
	mov	[si].MAN,ax
	mov	word ptr [si].EXP,ax
	jmp	SCA98
SCA99:	mov	[si].MAN,0ffffh
	mov	[si].EXP,7fh
	jmp	SCA98

FSCALE	ENDP

FLOG	PROC

	test	byte ptr [si].MAN+1,80h
	jz	FLOGMI
	cmp	[si].SIGN,0
	jne	FLOG0
	mov	ax,LOGTBL
	mov	es,ax
	mov	ah,0
	mov	cl,[si].EXP
	mov	di,[si].MAN
	shl	di,1
	mov	dx,es:word ptr [di]
	or	cl,cl
	jns	FLOG1
	neg	cl
	neg	dx
	mov	ah,1
FLOG1:	xchg	cl,dl
	xchg	dh,dl
	or	dx,dx
	jnz	FLOG2
	or	cl,cl
	jz	FLOG0
FLOG2:	mov	al,8
FLOG3:	shl	cl,1
	rcl	dx,1
	dec	al
	jnc	FLOG3
	rcr	dx,1
FLOG4:	mov	[si].MAN,dx
	mov	word ptr [si].EXP,ax
	ret

FLOGMI:	mov	dx,0ffffh
	mov	ax,017fh
	jmp	FLOG4
FLOG0:	xor	ax,ax
	xor	dx,dx
	jmp	FLOG4

FLOG	ENDP

FEXP	PROC

	test	byte ptr [si].MAN+1,80h
	jz	FEXPR1
	mov	cl,[si].EXP
	cmp	cl,7
	jge	FEXPI
	cmp	cl,-16
	jle	FEXPR1
	xor	di,di
	mov	ax,EXPTBL
	mov	es,ax
	mov	ax,[si].MAN
	sub	cl,15
	neg	cl
	mov	ch,0
FEXP1:	shr	ax,1
	rcr	di,1
	loop	FEXP1
	test	[si].SIGN,1
	jz	FEXP2
	neg	al
	or	di,di
	jz	FEXP2
	dec	al
	neg	di
FEXP2:	and	di,0fffeh
	mov	di,es:word ptr [di]
	mov	[si].MAN,di
	mov	word ptr [si].EXP,ax
	ret
FEXPI:	test	[si].SIGN,1
	jz	FEXPIP
	xor	ax,ax
	mov	[si].MAN,ax
	mov	word ptr [si].EXP,ax
	ret
FEXPIP:	mov	[si].MAN,0ffffh
	mov	word ptr [si].EXP,7fh
	ret
FEXPR1:	mov	[si].MAN,8000h
	mov	word ptr [si],0
	ret

FEXP	ENDP

ITOR	PROC

	or	ax,ax
	jz	ITOR0
	jns	ITOR1
	mov	[di].SIGN,1
	neg	ax
	jmp	short ITOR2
ITOR1:	mov	[di].SIGN,0
ITOR2:	mov	cl,16
ITOR3:	shl	ax,1
	dec	cl
	jnc	ITOR3
	rcr	ax,1
	mov	[di].MAN,ax
	mov	[di].EXP,cl
	ret
ITOR0:	xor	ax,ax
	mov	[di].MAN,ax
	mov	word ptr [di].EXP,ax
	ret

ITOR	ENDP

RTOI	PROC	near

	mov	ax,[si].MAN
	test	ah,80h
	jz	RTOI0
	mov	cl,[si].EXP
	sub	cl,15
	je	RTOI4
	jnl	RTOI2
	jo	RTOI0
	neg	cl
	shr	ax,cl
	adc	ax,0
RTOI4:	and	ax,7fffh
	test	[si].SIGN,1
	jz	RTOI5
	neg	ax
RTOI5:	ret
RTOI2:	shl	ax,cl
	jmp	short	RTOI4
RTOI0:	xor	ax,ax
	ret

RTOIF	PROC	near

	mov	ax,[si].MAN
	test	ah,80h
	jz	RTOI0
	xor	dx,dx
	mov	cl,[si].EXP
	sub	cl,15
	je	RTOIF4
	jnl	RTOI2
	jo	RTOI0
	neg	cl
	mov	ch,0
RTOIF1:	shr	ax,1
	jnc	RTOIF11
	mov	dl,1
RTOIF11:loop	RTOIF1
RTOIF4:	and	ax,7fffh
	test	[si].SIGN,1
	jz	RTOIF5
	neg	ax
	sub	ax,dx
RTOIF5:	ret

RTOIF	ENDP

RTOI	ENDP

	include	epilogue.h

	end
