	PAGE	66,132
	TITLE	*** ray4.asm   raw calculating functions ***
;
;
;	Machine language functions
;
;	    For Ray-Tracing
;
;		Aug/1985
;		Oct/1985
;
;	    by  Kaoru.Maeda
;
;

	include	def.ah		;	definition of structures & macros

	.SALL

@DATAU	SEGMENT

FAC0	db	(type REAL) dup (?)
FAC1	db	(type REAL) dup (?)
FAC2	db	(type REAL) dup (?)
FAC3	db	(type REAL) dup (?)
FAC4	db	(type REAL) dup (?)
FAC5	db	(type REAL) dup (?)
FAC6	db	(type REAL) dup (?)
FAC7	db	(type REAL) dup (?)
FAC8	db	(type REAL) dup (?)
FAC9	db	(type REAL) dup (?)
FAC10	db	(type REAL) dup (?)
FAC11	db	(type REAL) dup (?)
FAC12	db	(type REAL) dup (?)
FAC13	db	(type REAL) dup (?)
FAC14	db	(type REAL) dup (?)

@DATAU	ENDS

@CODE	SEGMENT

;
;	Normalize a Vector
;
;	Args	si : destination
;	Ret	carry if ZERO
;

	public	NRMLIZE

NRMLIZE	PROC

	link
	MOVMEM	FAC10,[si],<type VECTOR>	;  FAC10 = source
	FPSQAR	FAC10.X				;=
	FPSQAR	FAC10.Y				;|
	FPSQAR	FAC10.Z				;| FAC12 = | source | ^ 2
	FPADD	FAC12,FAC10			;|
	FPADD	FAC12,FAC11			;=
	FP0?	FAC12				;- if | source | == 0
	jz	NRM1				;-    then return (1,0,0)
	FPSQRT	FAC12				;  FAC12 = | source |
	FPDIV	[si].X,FAC12			;=
	FPDIV	[si].Y,FAC12			;| source /= | source |
	FPDIV	[si].Z,FAC12			;=
	clc					;  source wasn't zero
NRM0:	unlk
	ret
NRM1:	mov	[si].X.MAN,8000h		;=
	xor	ax,ax				;|
	mov	[si].Y.MAN,ax			;|
	mov	[si].Z.MAN,ax			;| return (1.0, 0.0, 0.0)
	mov	word ptr [si].X.EXP,ax		;|
	mov	word ptr [si].Y.EXP,ax		;|
	mov	word ptr [si].Z.EXP,ax		;=
	stc					;  source was zero
	jmp	NRM0

NRMLIZE	ENDP

;
;	Linear calculation
;
;	Args	si : vector dir,point
;		di : real   distance
;		bx : destination
;

	public	LINEAR

LINEAR	PROC

	push	di				;  save di for MOVMEM
	MOVMEM	[bx],[si],<type VECTOR>		;  [bx] = dir
	pop	di				;    (restore di)
	xchg	si,di				;  di will be destroyed in MLT
	push	di
	FPMLT	[bx].X,[si]			;=
	FPMLT	[bx].Y,[si]			;| [bx] = dir * distance
	FPMLT	[bx].Z,[si]			;=
	pop	di
	xchg	si,di
	FPADD	[bx].X,<[si+type VECTOR].X>	;=
	FPADD	[bx].Y,<[si+type VECTOR].Y>	;| [bx] += point
	FPADD	[bx].Z,<[si+type VECTOR].Z>	;=
	ret

LINEAR	ENDP

;
;	Rotation Support
;
;	bx : Primitive *
;	si : destination
;
;
	public	ROT
	public	ROT2

ROT	PROC

	cmp	[bx].Rotation,0
	njz	ROT19
	link	36
	push	bx
	mov	bx,[bx].Matrix
	MOVMEM	[bp-36],[bx],36
	FPMLT	[bp-36].X,[si].X
	FPMLT	[bp-36].Y,[si].Y
	FPMLT	[bp-36].Z,[si].Z
	FPMLT	[bp-24].X,[si].X
	FPMLT	[bp-24].Y,[si].Y
	FPMLT	[bp-24].Z,[si].Z
	FPMLT	[bp-12].X,[si].X
	FPMLT	[bp-12].Y,[si].Y
	FPMLT	[bp-12].Z,[si].Z
	FPADD	[bp-36],[bp-36].Y
	FPADD	[bp-36],[bp-36].Z
	FPADD	[bp-24],[bp-24].Y
	FPADD	[bp-24],[bp-24].Z
	FPADD	[bp-12],[bp-12].Y
	FPADD	[bp-12],[bp-12].Z
	MOVMEM	[si].X,[bp-36]
	MOVMEM	[si].Y,[bp-24]
	MOVMEM	[si].Z,[bp-12]
	pop	bx
	unlk
ROT19:	ret

ROT	ENDP

ROT2	PROC

	cmp	[bx].Rotation,0
	njz	ROT29
	link	36
	push	bx
	mov	bx,[bx].Matrix
	MOVMEM	[bp-36],[bx],36
	FPMLT	[bp-36].X,[si].X
	FPMLT	[bp-24].X,[si].Y
	FPMLT	[bp-12].X,[si].Z
	FPMLT	[bp-36].Y,[si].X
	FPMLT	[bp-24].Y,[si].Y
	FPMLT	[bp-12].Y,[si].Z
	FPMLT	[bp-36].Z,[si].X
	FPMLT	[bp-24].Z,[si].Y
	FPMLT	[bp-12].Z,[si].Z
	FPADD	[bp-36].X,[bp-24].X
	FPADD	[bp-36].X,[bp-12].X
	FPADD	[bp-36].Y,[bp-24].Y
	FPADD	[bp-36].Y,[bp-12].Y
	FPADD	[bp-36].Z,[bp-24].Z
	FPADD	[bp-36].Z,[bp-12].Z
	MOVMEM	[si],[bp-36],<type VECTOR>
	pop	bx
	unlk
ROT29:	ret

ROT2	ENDP

;
;	In-Out Function
;
;	Args	bx : primitive
;		si : vector point
;	Ret	Flags :	l  outside
;			ge  inside
;

	public	INOUT

INOUT	PROC

	link
	MOVMEM	FAC3,[si],<type VECTOR>		;=
	FPSUB	FAC3.X,[bx].Offst.X		;| FAC3 = local address
	FPSUB	FAC3.Y,[bx].Offst.Y		;|
	FPSUB	FAC3.Z,[bx].Offst.Z		;=
	push	si				;=
	lea	si,FAC3				;| Rotation Support
	call	ROT				;|
	pop	si				;=
	mov	cx,word ptr [bx].Kind
	mov	dx,[bx].Sgn			;  for later use
	loop	INOUT2

INOUT1:	FPABS	FAC3.X				;  CUBE
	FPCMP	FAC3.X,[bx].Param.X		;-  if |local| > Param
	jnl	INOUT10				;-    then Outside
	FPABS	FAC3.Y				;=
	FPCMP	FAC3.Y,[bx].Param.Y		;|
	jnl	INOUT10				;|  similar as X
	FPABS	FAC3.Z				;|
	FPCMP	FAC3.Z,[bx].Param.Z		;|
	jl	INOUT11				;=
INOUT10:neg	dx				;  OUTSIDE
INOUT11:or	dx,dx				;  INSIDE
INOUTEND:
	unlk
	ret

INOUT2:	loop	INOUT3				;  PLAIN
	FPMLT	FAC3.X,[bx].Param.X		;=
	FPMLT	FAC3.Y,[bx].Param.Y		;|
	FPMLT	FAC3.Z,[bx].Param.Z		;|  FAC5 = (local | Param)
	FPADD	FAC5,FAC4			;|
	FPADD	FAC5,FAC3			;=
	FPSGN?	FAC5				;=  if FAC5 < 0
	jne	INOUT11				;|     then Inside
	jmp	INOUT10				;=     else Outside

INOUT3:	MOVMEM	FAC0,FAC3,<type VECTOR>		;  2ND & CONE
	FPSQAR	FAC0.X				;=
	FPSQAR	FAC0.Y				;|
	FPSQAR	FAC0.Z				;|
	FPMLT	FAC0.X,[bx].Param.X		;|  FAC2 = Sum (
	FPMLT	FAC0.Y,[bx].Param.Y		;|	     Param * local^2
	FPMLT	FAC0.Z,[bx].Param.Z		;|  	   )
	FPADD	FAC2,FAC0			;|
	FPADD	FAC2,FAC1			;=
	cmp	[bx].Kind,3			;  2ND?
	jne	INOUT32
	FPCMP	FAC2,Const1			;-  if FAC2 < 1.0
	jl	INOUT33				;-     then Inside
INOUT34:neg	dx				;  OUTSIDE
INOUT33:or	dx,dx				;  INSIDE
	unlk
	ret
INOUT32:FPSGN?	FAC2				;=  if FAC2 > 0.0
	je	INOUT34				;|    then Outside
	jmp	INOUT33				;=    else Inside

INOUT	ENDP

@CODE	ENDS

@DATAC	SEGMENT

	public	Const1

Const1	REAL	<0,0,8000h>			;  Constant +1.0

@DATAC	ENDS

@CODE	SEGMENT

;
;
;	Range primitive checker
;
;	Args	bx : Ranger primitive
;		si : dir, point
;	Ret	flags : l   out of range
;			ge  in the range
;
;

;
;	Normal type check
;

	public	NRANGE

NRANGE	PROC

	link
	MOVMEM	FAC0,[si],<2*(type VECTOR)>	;  FAC0 = dir
	FPSUB	FAC3.X,[bx].Location.X		;=
	FPSUB	FAC3.Y,[bx].Location.Y		;| FAC3 = local address
	FPSUB	FAC3.Z,[bx].Location.Z		;=
	FPMLT	FAC0.X,FAC3.X			;= Equation
	FPMLT	FAC0.Y,FAC3.Y			;|   w1*x^2 + 2*w2*x + w3 == 0
	FPMLT	FAC0.Z,FAC3.Z			;| w1 == constantly 1.0
	FPADD	FAC2,FAC0			;| FAC2 = w2  (See SOLVE)
	FPADD	FAC2,FAC1			;=
	FPSQAR	FAC3.X				;=
	FPSQAR	FAC3.Y				;|
	FPSQAR	FAC3.Z				;| FAC5 = w3  (See SOLVE)
	FPADD	FAC5,FAC3			;|
	FPADD	FAC5,FAC4			;|
	FPSUB	FAC5,[bx].Radius		;=
	FPSGN?	FAC5				;-  if FAC5 < 0.0
	jnz	NRNG0				;-    then has solution
	FPSGN?	FAC2				;=  if FAC2 > 0.0 then
	jz	NRNG1				;=    ray looks another dir
	FPSQAR	FAC2				;-  if w2^2 - w1*w3 < 0.0
	FPCMP	FAC2,FAC5			;-    then no solution
NRNG9:	unlk
	ret
NRNG0:	xor	ax,ax				;  In the Range
	jmp	NRNG9
NRNG1:	mov	al,-1				;  Out of Range
	or	al,al
	jmp	NRNG9

NRANGE	ENDP

;
;	Initial type check
;	   (Ray from viewpoint only)
;	   (Much faster than Normal type)
;

	public	IRANGE

IRANGE	PROC

	link
	MOVMEM	FAC0,[si],<type VECTOR>		;=
	FPMLT	FAC0.X,[bx].Direction.X		;|
	FPMLT	FAC0.Y,[bx].Direction.Y		;| FAC0 = cosine of
	FPMLT	FAC0.Z,[bx].Direction.Z		;|		'SHISA'
	FPADD	FAC0,FAC1			;|
	FPADD	FAC0,FAC2			;=
	FPCMP	FAC0,[bx].Mincos		;  if FAC0 < Mincos then Out
	unlk
	ret

IRANGE	ENDP

;
;
;	Equation Sovler
;
;	Args	bx : primitive
;		si : dir, point
;		di : where to put distance
;		SIDE : solve inside->outside if this is INSIDE
;	Ret	[di] : distance
;		CUBEX : plain no which has the crossing point
;		carry : 1 if there is no solution
;
;

EXPMINDIS	equ	0			; Exponent of minimum distance

	public	SOLVE

SOLVE	PROC

	link
	push	di				; Save destination address
	MOVMEM	FAC0,[si],<2*(type VECTOR)>	;  FAC0 = dir
	FPSUB	FAC3.X,[bx].Offst.X		;=
	FPSUB	FAC3.Y,[bx].Offst.Y		;| FAC3 = local address
	FPSUB	FAC3.Z,[bx].Offst.Z		;=
	push	si				;=
	lea	si,FAC0				;|
	call	ROT				;| Rotation Support
	lea	si,FAC3				;|
	call	ROT				;|
	pop	si				;=
	mov	dx,[bx].Sgn			;= dx = 0  if out->in solution
	xor	dx,SIDE				;|	1  if in->out solution
	je	SL1				;|  is required
	mov	dx,1				;=
SL1:	mov	di,word ptr [bx].Kind		;=
	shl	di,1				;|  jump to each case
	jmp	cs:SLTBL[di-2]			;=

	even

SLTBL	dw	SLCUBE				;=
	dw	SLPLAIN				;| Jump address table
	dw	SL2ND				;|
	dw	SLCONE				;=

SLSC:	clc					;  Success  ( solution found )
SLEND:	unlk
	ret
SLNS0:	pop	di				;  No solution
SLNS:	stc
	jmp	SLEND

SLPLAIN:FPMLT	FAC0.X,[bx].Param.X		;=  Plain
	FPMLT	FAC0.Y,[bx].Param.Y		;|
	FPMLT	FAC0.Z,[bx].Param.Z		;|  FAC2 = (dir | Param)
	FPADD	FAC2,FAC0			;|
	FPADD	FAC2,FAC1			;=
	xor	dl,FAC2.SIGN			;-  directed 'ASATTE NO HOU'
	njz	SLNS0				;-
	FPMLT	FAC3.X,[bx].Param.X		;=
	FPMLT	FAC3.Y,[bx].Param.Y		;|
	FPMLT	FAC3.Z,[bx].Param.Z		;|  FAC5 = - (local | Param)
	FPADD	FAC5,FAC3			;|	       / (dir | Param)
	FPADD	FAC5,FAC4			;|   this is the solution
	FPDIV	FAC5,FAC2			;|	(such that distance)
	FPCHS	FAC5				;=
	FPSGN?	FAC5				;-  Crossing-point
	njnz	SLNS0				;-    is behind the viewpoint
	cmp	FAC5.EXP,EXPMINDIS		;=  if distance < minimum
	njl	SLNS0				;=    then ignore this solution
	pop	di				;-  Store to destination
	MOVMEM	[di],FAC5,4			;-
	jmp	SLSC				;   we got a solution

SLCUBE:	FP0?	FAC0.X				;-  if dir.X == 0 then
	njz	SLC2				;-   no crossing on Y-Z plain
	MOVMEM	FAC9,FAC3.X,4
	mov	al,FAC0.X.SIGN			;=
	xor	al,dl				;|  if further solution
	jz	SLC11				;=
	FPSUB	FAC9,[bx].Param.X		;   then FAC9 = local - Param
	jmp	short SLC12
SLC11:	FPADD	FAC9,[bx].Param.X		;   else FAC9 = local + Param
SLC12:	FPDIV	FAC9,FAC0.X			;-  FAC9 = - (local +- Param)
	FPCHS	FAC9				;-		/ dir
	njnz	SLC2				;   if FAC9 < 0.0 nothing on YZ
	cmp	FAC9.EXP,EXPMINDIS		;-  if distance < minimum
	njl	SLC2				;-    then ignore this solution
	MOVMEM	FAC10,FAC9,4			;=
	FPMLT	FAC10,FAC0.Y			;|
	FPADD	FAC10,FAC3.Y			;|  if |crossing point| > Param
	FPABS	FAC10				;|    then cross at the point
	FPCMP	FAC10,[bx].Param.Y		;|         out of the cube
	njnle	SLC2				;=
	MOVMEM	FAC10,FAC9,4			;=
	FPMLT	FAC10,FAC0.Z			;|
	FPADD	FAC10,FAC3.Z			;|  similar as Y
	FPABS	FAC10				;|
	FPCMP	FAC10,[bx].Param.Z		;|
	jnle	SLC2				;=
	mov	CUBEX,0				;   Crossed on Y-Z plain
	pop	di				;-  Store distance
	MOVMEM	[di],FAC9,4			;-    to destination
	jmp	SLSC				;   we got a solution

SLC2:	FP0?	FAC0.Y				;=
	njz	SLC3				;|  SIMILAR AS Y-Z PLAIN
	MOVMEM	FAC9,FAC3.Y,4			;|   this is about Z-X plain
	mov	al,FAC0.Y.SIGN			;|
	xor	al,dl				;|
	jz	SLC21				;|
	FPSUB	FAC9,[bx].Param.Y		;|
	jmp	short SLC22			;|
SLC21:	FPADD	FAC9,[bx].Param.Y		;|
SLC22:	FPDIV	FAC9,FAC0.Y			;|
	FPCHS	FAC9				;|
	njnz	SLC3				;|
	cmp	FAC9.EXP,EXPMINDIS		;|
	njl	SLC3				;|
	MOVMEM	FAC10,FAC9,4			;|
	FPMLT	FAC10,FAC0.Z			;|
	FPADD	FAC10,FAC3.Z			;|
	FPABS	FAC10				;|
	FPCMP	FAC10,[bx].Param.Z		;|
	njnle	SLC3				;|
	MOVMEM	FAC10,FAC9,4			;|
	FPMLT	FAC10,FAC0.X			;|
	FPADD	FAC10,FAC3.X			;|
	FPABS	FAC10				;|
	FPCMP	FAC10,[bx].Param.X		;|
	jnle	SLC3				;|
	mov	CUBEX,4				;|  Crossed on Z-X plain
	pop	di				;|
	MOVMEM	[di],FAC9,4			;|
	jmp	SLSC				;=

SLC3:	FP0?	FAC0.Z				;=
	njz	SLC4				;|  SIMILAR AS Y-Z PLAIN
	MOVMEM	FAC9,FAC3.Z,4			;|    this is about X-Y plain
	mov	al,FAC0.Z.SIGN			;|
	xor	al,dl				;|
	jz	SLC31				;|
	FPSUB	FAC9,[bx].Param.Z		;|
	jmp	short SLC32			;|
SLC31:	FPADD	FAC9,[bx].Param.Z		;|
SLC32:	FPDIV	FAC9,FAC0.Z			;|
	FPCHS	FAC9				;|
	njnz	SLC4				;|
	cmp	FAC9.EXP,EXPMINDIS		;|
	njl	SLC4				;|
	MOVMEM	FAC10,FAC9,4			;|
	FPMLT	FAC10,FAC0.X			;|
	FPADD	FAC10,FAC3.X			;|
	FPABS	FAC10				;|
	FPCMP	FAC10,[bx].Param.X		;|
	njnle	SLC4				;|
	MOVMEM	FAC10,FAC9,4			;|
	FPMLT	FAC10,FAC0.Y			;|
	FPADD	FAC10,FAC3.Y			;|
	FPABS	FAC10				;|
	FPCMP	FAC10,[bx].Param.Y		;|
	jnle	SLC4				;|
	mov	CUBEX,8				;|  Crossed on X-Y plain
	pop	di				;|
	MOVMEM	[di],FAC9,4			;|
	jmp	SLSC				;=

SLC4:	jmp	SLNS0				;   no solution on this cube

SL2ND:			; 2nd order surface	;  w1*x^2 + w2*x + w3 == 0
SLCONE:	MOVMEM	FAC12,FAC0,<type VECTOR>
	FPSQAR	FAC0.X				;=
	FPMLT	FAC0.X,[bx].Param.X		;|
	FPSQAR	FAC0.Y				;| FAC2 = w1
	FPMLT	FAC0.Y,[bx].Param.Y		;|	= Sum (
	FPSQAR	FAC0.Z				;|	   Param * dir^2
	FPMLT	FAC0.Z,[bx].Param.Z		;|	 )
	FPADD	FAC2,FAC0			;|
	FPADD	FAC2,FAC1			;=
	FP0?	FAC2				;-  if w1 == 0.0
	njz	SLNS0				;-     then no solution
	MOVMEM	FAC9,FAC3,<type VECTOR>		;=
	FPMLT	FAC9.X,FAC12.X			;|
	FPMLT	FAC9.X,[bx].Param.X		;|  FAC9 = w2
	FPMLT	FAC9.Y,FAC12.Y			;|	 = 2 * Sum (
	FPMLT	FAC9.Y,[bx].Param.Y		;|	    Param * dir * local
	FPMLT	FAC9.Z,FAC12.Z			;|	   )
	FPMLT	FAC9.Z,[bx].Param.Z		;|
	FPADD	FAC9,FAC10			;|
	FPADD	FAC9,FAC11			;|
	FPSCALE	FAC9,1				;=
	MOVMEM	FAC10,FAC3,<type VECTOR>	;|
	FPSQAR	FAC10.X				;|
	FPMLT	FAC10.X,[bx].Param.X		;|  FAC10 = w3
	FPSQAR	FAC10.Y				;|	  = Sum (
	FPMLT	FAC10.Y,[bx].Param.Y		;|	     Param * local^2
	FPSQAR	FAC10.Z				;|	    )
	FPMLT	FAC10.Z,[bx].Param.Z		;|
	FPADD	FAC10,FAC11			;|
	FPADD	FAC10,FAC12			;=
	cmp	[bx].Kind,3			;=  if 2nd order surface  
	jnz	SL22				;|    then w3 -= 1.0
	FPSUB	FAC10,Const1			;=
SL22:	MOVMEM	FAC0,FAC9,<2*(type REAL)>	;  FAC0 = w2; FAC1 = w3;
	FPSQAR	FAC0				;=
	FPMLT	FAC1,FAC2			;| FAC0 =
	FPSCALE	FAC1,2				;| Determinant  w2^2 - 4*w1*w3
	FPSUB	FAC0,FAC1			;=
	FPSGN?	FAC0				;-
	njnz	SLNS0				;-  D < 0  no solution
SL221:	FPSQRT	FAC0				;   determinant
	MOVMEM	FAC11,FAC0			;=
	mov	al,byte ptr FAC9.SIGN		;|
	xor	al,1				;|
	mov	byte ptr FAC11.SIGN,al		;|  FAC11 is greater absolute
	FPSUB	FAC11,FAC9			;|	solution
	MOVMEM	FAC12,FAC10			;|
	FPSCALE	FAC12,1				;|  FAC12 = (w3/w1) / FAC11
	FPSCALE	FAC2,1				;|
	FPDIV	FAC12,FAC11			;|
	FPDIV	FAC11,FAC2			;=
	FPCMP	FAC11,FAC12			;=
	jnl	SL23				;|
	or	dl,dl				;|
	jz	SL24				;|
	jmp	short SL25			;|
SL23:	or	dl,dl				;|
	jz	SL25				;|
SL24:	FP0?	FAC11				;|
	jz	SL241				;|
	FPSGN?	FAC11				;|  return
	jnz	SL241				;|  if dl == 0
	cmp	FAC11.EXP,EXPMINDIS		;|    then nearer solution
	jnl	SL31				;|    else farther solution
SL241:	FP0?	FAC12				;|
	jz	SLNS2				;|
	FPSGN?	FAC12				;|
	jnz	SLNS2				;|  which is
	cmp	FAC12.EXP,EXPMINDIS		;|   1. plus
	jl	SLNS2				;|   2. greater than minimum
SL32:	pop	di				;|
	MOVMEM	[di],FAC12,4			;|
	jmp	SLSC				;|
SLNS2:	jmp	SLNS0				;|
SL31:	pop	di				;|
	MOVMEM	[di],FAC11,4			;|
	jmp	SLSC				;|
SL25:	FP0?	FAC12				;|
	jz	SL251				;|
	FPSGN?	FAC12				;|
	jnz	SL251				;|
	cmp	FAC12.EXP,EXPMINDIS		;|
	jnl	SL32				;|
SL251:	FP0?	FAC11				;|
	jz	SLNS2				;|
	FPSGN?	FAC11				;|
	jnz	SLNS2				;|
	cmp	FAC11.EXP,EXPMINDIS		;|
	jl	SLNS2				;|
	jmp	SL31				;=

SOLVE	ENDP

;
;
;	Normal Vector Generater
;
;	Args	bx : Primitive
;		si : dir, vp, local point
;		di : destination
;		CUBEX : plain of cube
;		SIDE  : Inside / Outside
;
;	Ret	[di] : Result
;
;

	public	NORMAL

NORMAL	PROC

	link
	push	di				;  Save destination address
	MOVMEM	FAC0,[si+24],<type VECTOR>	;
	mov	dx,SIDE				;=  dl = 0 if outer normal
	xor	dx,[bx].Sgn			;|	 1 if inner normal
	jz	NOR1				;|  is required
	mov	dl,1				;=
NOR1:	mov	di,word ptr [bx].Kind		;=
	shl	di,1				;|  call each routine
	call	cs:NTBL[di-2]			;=
	pop	di
	push	di
	MOVMEM	[di],FAC0,<type VECTOR>		;   store the result
	pop	di
	unlk
	ret

	even

NTBL	dw	NORCUBE				;=
	dw	NORPLN				;|  jump table
	dw	NOR2ND				;|
	dw	NORCONE				;=

NORCUBE:mov	di,CUBEX			;  CUBE
	mov	dh,FAC0[di].SIGN
	xor	ax,ax				;=
	push	di				;|
	mov	cx,6				;|
	lea	di,FAC0				;|
	cld					;| clear FAC0 to 0.0
	push	es				;|
	mov	es,DATASEG			;|
	rep	stosw				;|
	pop	es				;|
	pop	di				;=
	xor	dh,dl				;=
	jnz	NORCUB1				;| if crossing point equals to
	FPLOD	FAC0[di],8000h,0,0		;|	required direction
	ret					;| then return  1.0
NORCUB1:FPLOD	FAC0[di],8000h,0,1		;| else	       -1.0
	ret					;=

NORPLN:	MOVMEM	FAC0,[bx].Param,<type VECTOR>	;=  Plain
	xor	FAC0.X.SIGN,dl			;|  return Param
	xor	FAC0.Y.SIGN,dl			;|   with its flag modified
	xor	FAC0.Z.SIGN,dl			;|
	ret					;=

NORCONE:					;  Cone & 2nd order surface
NOR2ND:	MOVMEM	FAC3,FAC0,<type VECTOR>		;=
	FPMLT	FAC0.X,[bx].Param.X		;| FAC0 = local * Param
	FPMLT	FAC0.Y,[bx].Param.Y		;|
	FPMLT	FAC0.Z,[bx].Param.Z		;=
	xor	FAC0.X.SIGN,dl			;=
	xor	FAC0.Y.SIGN,dl			;|  modify sign
	xor	FAC0.Z.SIGN,dl			;=
	push	si				;=
	lea	si,FAC0				;|  normalize it
	call	NRMLIZE				;|
	pop	si				;=
	ret

NORMAL	ENDP

;
;
;	Texture Mapping
;
;	Arg	bx : primitive
;		si : dir, vp, pnt
;
;

	public	TEXTMAP

TEXTMAP	PROC

	cmp	[bx].Texture,0			;=
	jnz	TXMP0				;|  UNFIG
	ret					;=
TXMP0:	link
	MOVMEM	FAC0,[si+24],<type VECTOR>	;
	push	si				;  Save si
	push	bx				;  Save bx
	push	si				;    Push si for 3rd argument
	push	bx				;    Push bx for 2nd argument
	lea	si,FAC0				;-   Push FAC0 for 1st arg
	push	si				;-
	mov	di,word ptr [bx].Texture	;=
	shl	di,1				;|
	cmp	di,2*USRTXTOFST			;|
	jae	TXMP1				;|
	cmp	di,2*STDTXTNUM			;|
	ja	TXMP3				;|
	mov	es,DATASEG			;|  Call the function
	call	DGROUP:STDTXT[di-2]		;|   with es set to
;	mov	es,SEGTABLE			;|	data segment
	jmp	short TXMP2			;|
TXMP1:	cmp	di,(MAXUTEX+USRTXTOFST)*2	;|  if table points NULL
	jae	TXMP3				;|   output error message
	mov	di,DGROUP:USERTEXT[di-2*USRTXTOFST]
	or	di,di				;|
	jz	TXMP3				;|
	mov	es,DATASEG			;|
	call	di				;|
;	mov	es,SEGTABLE			;|
	jmp	short TXMP2			;=
TXMP3:	lea	dx,ILTXMP			;=
	mov	ah,9				;|  illegal texture mapping
	int	21h				;=
TXMP2:	pop	ax				;=
	pop	ax				;|  dummy pop for arguments
	pop	ax				;=
	pop	bx				;   restore bx
	pop	si				;   restore si
	unlk
	ret

TEXTMAP	ENDP

	extrn	STRIPE	:  NEAR			;=
	extrn	RING	:  NEAR			;|  written in C (ray3.vc)
	extrn	DOT	:  NEAR			;=

@CODE	ENDS

@DATAI	SEGMENT

	even

STDTXT	dw	TILE				;=
	dw	STRIPE				;|
	dw	RING				;|  standard texture mapping
	dw	DOT				;|     function table
	dw	CHECK				;=

@DATAI	ENDS

@DATAB	SEGMENT

ILTXMP	db	'Illegal Texture Mapping$'
ILBPMP	db	'Illegal Bump Mapping$'

@DATAB	ENDS

@CODE	SEGMENT

;
;
;	Standard Texture Mapping Function
;
;

TILE	PROC

;	mov	es,SEGTABLE
	lea	si,FAC0.X
	call	RTOIF
	test	al,8
	jnz	TILE1
	lea	si,FAC0.Z
	call	RTOIF
	test	al,8
	jnz	TILE1
	FPLOD	[bx].Solid.Y,0,0,0		;    0.0
	ret
TILE1:	FPLOD	[bx].Solid.Y,0ff00h,7,0		;  255.0
	ret

TILE	ENDP

CHECK	PROC

;	mov	es,SEGTABLE
	lea	si,FAC0.X
	call	RTOIF
	mov	di,ax
	lea	si,FAC0.Z
	call	RTOIF
	xor	ax,di
	test	al,8
	jnz	CHECK1
	FPLOD	[bx].Solid.Y,0,0,0		;    0.0
	ret
CHECK1:	FPLOD	[bx].Solid.Y,0ff00h,7,0		;  255.0
	ret

CHECK	ENDP

;
;
;	Bump Mapping
;
;	Arg	bx : primitive
;		si : dir, vp, pnt
;
;

	public	BUMPMAP

BUMPMAP	PROC

	cmp	[bx].Bump,0			;=
	jnz	BPMP0				;|  UNFIG
	ret					;=
BPMP0:	link					;=
	MOVMEM	FAC0,[si+24],<type VECTOR>	;|  Similar as TEXTMAP
	push	si				;|
	push	bx
	push	si
	push	bx
	lea	si,FAC0
	push	si
	mov	di,word ptr [bx].Bump
	cmp	di,MAXBUMP
	ja	BPMP1
	shl	di,1
	mov	di,DGROUP:BUMPING[di-2]
	or	di,di
	jz	BPMP1
	mov	es,DATASEG
	call	di
;	mov	es,SEGTABLE
	jmp	short BPMP2
BPMP1:	lea	dx,ILBPMP
	mov	ah,9
	int	21h
BPMP2:	pop	ax
	pop	ax
	pop	ax
	pop	bx
	pop	si				;|
	unlk					;|
	ret					;=

BUMPMAP	ENDP

	include	epilogue.h

	END
