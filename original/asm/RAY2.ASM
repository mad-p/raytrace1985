	PAGE	64,132
	TITLE	***** ray2.mac ****  Higher level routines
;
;
;	Machine language routine 2
;
;
;

	include def.ah

	.SALL

@code	segment

	extrn	NRMLIZE		: near
	extrn	IRANGE		: near
	extrn	NRANGE		: near
	extrn	NORMAL		: near
	extrn	LINEAR		: near
	extrn	INOUT		: near
	extrn	SOLVE		: near
	extrn	TEXTMAP		: near
	extrn	BUMPMAP		: near
	extrn	WRITESCA	: near
	extrn	ROT		: near
	extrn	ROT2		: near

@code	ends

@datau	segment

FAC0	REAL	<?,?,?>
FAC1	REAL	<?,?,?>
FAC2	REAL	<?,?,?>
FAC3	REAL	<?,?,?>
FAC4	REAL	<?,?,?>
FAC5	REAL	<?,?,?>
FAC6	REAL	<?,?,?>
FAC7	REAL	<?,?,?>
FAC8	REAL	<?,?,?>
FAC9	REAL	<?,?,?>
FAC10	REAL	<?,?,?>
FAC11	REAL	<?,?,?>
FAC12	REAL	<?,?,?>

F0	REAL	<?,?,?>
F1	REAL	<?,?,?>
F2	REAL	<?,?,?>

URA	DW	?

@datau	ends

@datab	segment

MONMES	db	'Scan (   ,   )  :  RGB[   |   |   ]'
SIZEofMONMES	equ	35

@datab	ends

@datac	segment

	extrn	Const1 : DWORD

@datac	ends

@code	segment

;
;
;	mainloop
;	    called from C with no arguments
;
;

	public	MAINLOOP
MAINLOOP	PROC

	link	4
	FPLOD	RESERVE.RE.X,8000h,0,0		;=  strength of color = 1.0
	FPLOD	RESERVE.RE.Y,8000h,0,0		;=  strength of color = 1.0
	FPLOD	RESERVE.RE.Z,8000h,0,0		;=  strength of color = 1.0
	mov	RESERVE.RSIDE,1			;   from Outside
	mov	RESERVE.RRCT,0			;   no reflection or division
	mov	ax,YSTART			;-  -4[bp] is Y address
	mov	word ptr -4[bp],ax		;-	of screen
MAIN00: mov	ax,word ptr -4[bp]		;=
	cmp	ax,YSTOP			;|  end ?
	njnb	MAIN99				;=
	mov	ax,XSTART			;-  -2[bp] is X address
	mov	word ptr -2[bp],ax		;-	of screen
MAIN10:	mov	ax,word ptr -2[bp]		;=
	cmp	ax,XSTOP			;|  end ?
	njnb	MAIN19				;=
	mov	RESERVE.RNET,offset DGROUP:NETWORKS  ;   start from network #0
	mov	RESERVE.RPRIM,0			;   NULL
	MOVMEM	RESERVE.RO,VIEWPT.X,12		;   scan from viewpoint
	sub	ax,XOFFSET			;-  FAC0 = X address of screen
	FITOR	FAC0				;-
	mov	ax,word ptr -4[bp]		;=
	sub	ax,YOFFSET			;|  FAC1 = Y address of screen
	neg	ax				;|
	FITOR	FAC1				;=
	MOVMEM	FAC3,EH,24			;=
	FPMLT	FAC3.X,FAC0			;|
	FPMLT	FAC3.Y,FAC0			;|
	FPMLT	FAC3.Z,FAC0			;|
	FPMLT	FAC6.X,FAC1			;|
	FPMLT	FAC6.Y,FAC1			;|
	FPMLT	FAC6.Z,FAC1			;|
	FPADD	FAC3.X,FAC6.X			;|
	FPADD	FAC3.Y,FAC6.Y			;|
	FPADD	FAC3.Z,FAC6.Z			;|
	FPADD	FAC3.X,TOSCR.X			;|
	FPADD	FAC3.Y,TOSCR.Y			;|
	FPADD	FAC3.Z,TOSCR.Z			;|
	MOVMEM	RESERVE.RV,FAC3,12		;|  RESERVE.RV = TOSCR +
	lea	si,RESERVE.RV			;|	(ray)	 X * EH + 
	call	NRMLIZE				;=		 Y * EV
	call	TRACE				;   Ray-Tracing body
	mov	bx,word ptr -2[bp]
	mov	ax,SRED				;=
	cmp	ax,255				;|
	jb	MA00RED				;|  overflow check of RED
	mov	ax,255				;|
MA00RED:	mov	PRED[bx],al		;=
	mov	ax,SGRN				;=
	cmp	ax,255				;|
	jb	MA00GRN				;|  overflow check of GRN
	mov	ax,255				;|
MA00GRN:	mov	PGRN[bx],al		;=
	mov	ax,SBLU				;=
	cmp	ax,255				;|
	jb	MA00BLU				;|  overflow check of BLU
	mov	ax,255				;|
MA00BLU:	mov	PBLU[bx],al		;=
	cmp	MONITOR,0			;=
	jz	MAIN18				;|  monitoring
;	push	es				;|
;	mov	ax,CVRAM			;|
;	mov	es,ax				;|
;	lea	si,MONMES			;|
;	mov	di,23*160			;|  locate (0,23)
;	mov	cx,SIZEofMONMES			;|  size of monmess
;MAIN110:lodsb					;|
;	cbw					;|  ah <- 0
;	stosw					;|
;	loop	MAIN110				;|
;	mov	di,23*160+8+8*1			;|=
;	mov	ax,word ptr (-2*1)[bp]		;|| display X address
;	call	DECIMAL				;|=
;	mov	di,23*160+8+8*2			;|=
;	mov	ax,word ptr (-2*2)[bp]		;|| display Y address
;	call	DECIMAL				;|=
;	mov	di,23*160+50			;|=  locate (25,23)
;	mov	al,PRED[bx]			;||
;	mov	ah,0				;||  display red
;	call	DECIMAL				;|=
;	mov	di,23*160+58			;|=  locate (29,23)
;	mov	al,PGRN[bx]			;||
;	mov	ah,0				;||  display green
;	call	DECIMAL				;|=
;	mov	di,23*160+66			;|=  locate (33,23)
;	mov	al,PBLU[bx]			;||
;	mov	ah,0				;||  display blue
;	call	DECIMAL				;|=
;	pop	es				;    restore es to tables
;	mov	ah,2				;=
;	mov	dl,0dh				;|   output CR to check ^C
;	int	21h				;=
MAIN18:	mov	ax,STEP				;=
	add	word ptr -2[bp],ax		;|   next dot
	jmp	MAIN10				;=
MAIN19:	mov	es,DATASEG			;=   set es to dataseg
	push	word ptr -4[bp]			;|   push Y for argument
	call	WRITESCA			;|   writescanline (ray1.c)
	pop	ax				;|   dummy pop
	mov	ax,STEP				;=
	add	word ptr -4[bp],ax		;|   next line
	jmp	MAIN00				;=
MAIN99:	mov	es,DATASEG			;    end of process
	unlk
	ret

DECIMAL	PROC

	mov	cx,10				;   output decimal
DECI0:	xor	dx,dx				;      with 0 surpress
	div	cx
	add	dl,'0'
	mov	es:byte ptr [di],dl
	dec	di
	dec	di
	or	ax,ax
	jnz	DECI0
	ret

DECIMAL	ENDP
	public	DECIMAL

MAINLOOP	ENDP

;
;
;	TRACE
;
;	   trace from RESERVE[0]
;		returns color in S???
;
;	Args  :	REAR
;
;	Ret   :	S???
;
;

	public	TRACE
TRACE	PROC

	link	2
	mov	SRED,0				;=
	mov	SGRN,0				;|  clear color buffer
	mov	SBLU,0				;=
	mov	FIRST,1				;   this is the first search
	mov	REAR,0				;   rear = 0
	mov	bx,0				;   front = 0
	jmp	short TR01
TR0:	mov	FIRST,0				;   it's no longer first search
	mov	bx,FRONT			;=
	cmp	bx,REAR				;|
	jb	TR00				;|  end ?
	unlk					;|
	ret					;=
TR00:	add	bx,type ENVIRONMENT		;   next Env
TR01:	mov	FRONT,bx			;   set front
	mov	byte ptr -2[bp],0		;   -2[bp] : flag if reflected
	lea	bx,RESERVE[bx]			;   bx = Env
	MOVMEM	V,[bx].RV,24			;   set viwepoint, direction
	MOVMEM	COL,[bx].RE,12			;   set color diminishment
	MOVMEM	SIDE,[bx].RSIDE,6		;   set SIDE, PRIM, RCT
	mov	si,0				;-  this is 'SEARCH'
	call	SEARCH				;-  search solution
	jz	TR0				;   no solution
	cmp	SIDE,-1				;=
	njne	TR1				;|
	mov	si,PRIM				;|  if Inside -> Outside
	MOVMEM	F0,[si].Absorb,12		;|    diminish ray energy
	FPMLT	F0.X,T				;|
	FPMLT	F0.Y,T				;|
	FPMLT	F0.Z,T				;|
	FPEXP	F0.X				;|
	FPEXP	F0.Y				;|
	FPEXP	F0.Z				;|
	FPMLT	COL.X,F0.X			;|
	FPMLT	COL.Y,F0.Y			;|
	FPMLT	COL.Z,F0.Z			;|
TR1:	mov	bx,ISPRIM
	MOVMEM	CRSPNT,PNT,<type VECTOR>	;=
	lea	si,PNT				;|  get local address
	FPSUB	[si].X,[bx].Offst.X		;|
	FPSUB	[si].Y,[bx].Offst.Y		;|
	FPSUB	[si].Z,[bx].Offst.Z		;|
	call	ROT				;=
	lea	si,V
	lea	di,NML
	call	NORMAL				;   get normal vector
	call	TEXTMAP				;   user defined texture mapping
	call	BUMPMAP				;   user defined bump mapping
	mov	bx,ISPRIM			;=
	cmp	[bx].Surface,5			;|
	jnz	TR10a				;|
	MOVMEM	NML,PNT,<2*(type REAL)>		;|
	FPLOD	NML.Z,8000h,5,1			;|     normal vector gen.
	lea	si,NML				;|
	call	NRMLIZE				;=	
TR10a:	lea	si,NML				;=
	call	ROT2				;=  Rotaion Support
	MOVMEM	FAC0,NML,<type VECTOR>		;=
	FPMLT	FAC0.X,V.X			;|
	FPMLT	FAC0.Y,V.Y			;|
	FPMLT	FAC0.Z,V.Z			;|
	FPADD	FAC0.X,FAC0.Y			;|  CO = ( NML | V )
	FPADD	FAC0.X,FAC0.Z			;|
	FPSGN?	FAC0				;|
	jnz	TR1000				;|
	mov	URA,1				;|
	jmp	short TR1001			;|
TR1000:	mov	URA,0				;|
TR1001:	FPABS	FAC0				;|
	MOVMEM	CO,FAC0				;=
	mov	bx,ISPRIM			;
	MOVMEM	REF,[bx].Refrate		;   REF = reflection ratio
	cmp	[bx].Surface,3			;=  process for clear object
	njne	TR2				;|    (REF = n)
	cmp	SIDE,-1				;|=
	jne	TR11				;|| if In->Out then REF = 1/n
	FPINV	REF				;|=
TR11:	FPSQAR	REF				;|=
	MOVMEM	F0,CO				;||
	FPSQAR	F0				;||
	FPADD	F0,REF				;||
	FPSUB	F0,Const1			;||
	FPSGN?	F0				;||=
	jz	TR12				;|||   all reflect
	FPLOD	REF,8000h,0,0			;|||
	jmp	TR2				;||=
TR12:	FPSQRT	F0				;||	   |       n^2 - 1	 | 2
	MOVMEM	F1,F0				;||  REF = |---------------------|
	FPADD	F0,CO				;||	   |cos - sqrt(n^2-sin^2)|
	FPSQAR	F0				;||	   
	FPSUB	REF,Const1			;||	   |cos - sqrt(...)| 2
	FPDIV	REF,F0				;||	 = |---------------|	energy of ref
	FPSQAR	REF				;|=	   |cos + sqrt(...)|
TR2:	call	DIFFUSE				;-   Random ref & highlight
	mov	bx,ISPRIM			;=   opaque object process
	cmp	[bx].Surface,1			;|	has finished
	nje	TR0				;=
	cmp	[bx].Surface,4			;=   light object
	jnae	TR2000				;|
	mov	bx,FRONT			;|     trace forward
	MOVMEM	RESERVE[bx].RO,CRSPNT,<type VECTOR>
	mov	ax,ISPRIM			;|
	mov	word ptr RESERVE[bx].RPRIM,ax	;|
	mov	FIRST,0				;|
	jmp	TR01				;=
TR2000:	mov	al,byte ptr RCT+1		;|=  if ray division is maximum
	cmp	al,byte ptr MAXDIVCT		;||    then greater energy only
	jb	TR20b				;||
	cmp	REF.EXP,-1			;||
	njl	TR3				;==
TR20b:	mov	si,REAR				;-   next Env
	add	si,type ENVIRONMENT		;-
	mov	ax,RCT				;=
	inc	al				;|   if reflect is maximum
	cmp	al,byte ptr MAXREFCT		;|	then skip this
	njnbe	TR3				;=
	mov	word ptr RESERVE[si].RRCT,ax		;=
	MOVMEM	RESERVE[si].RE,COL,12			;|   make next Env
	FPMLT	RESERVE[si].RE.X,REF			;|
	FPMLT	RESERVE[si].RE.Y,REF			;|
	FPMLT	RESERVE[si].RE.Z,REF			;|
	MOVMEM	F0,RESERVE[si].RE.X			;|=
	FPADD	F0,RESERVE[si].RE.Y			;||  if sum of energy is
	FPADD	F0,RESERVE[si].RE.Z			;||	under limit
	FPCMP	F0,MINENERG			;||	  then skip this
	njl	TR3				;|=
	mov	REAR,si				;|   set this to REAR
	lea	si,RESERVE[si]			;|
	mov	byte ptr -2[bp],1		;|   set reflection flag
	MOVMEM	[si].RO,CRSPNT,12			;|
	MOVMEM	[si].RV,NML,12			;|   RV = 2cos * NML
	FPMLT	[si].RV.X,CO			;|
	FPMLT	[si].RV.Y,CO			;|
	FPMLT	[si].RV.Z,CO			;|
	FPSCALE	[si].RV.X,1			;|
	FPSCALE	[si].RV.Y,1			;|
	FPSCALE	[si].RV.Z,1			;|
	FPADD	[si].RV.X,V.X			;|
	FPADD	[si].RV.Y,V.Y			;|
	FPADD	[si].RV.Z,V.Z			;|
	mov	ax,SIDE				;|   from the same side
	mov	[si].RSIDE,ax			;|
	mov	[si].RPRIM,bx			;|
	call	CHOOSE				;=   choose network
TR3:	cmp	[bx].Surface,2			;-   if mirror object
	nje	TR0				;-	then skip following
	FPSUB	REF,Const1			;=
	FP0?	REF				;|   all reflect
	nje	TR0				;=
	FPCHS	REF				;    REF = energy of 'KUSSETSUKOU'
	mov	si,REAR				;-   next Env
	add	si,type ENVIRONMENT		;-
	mov	ax,RCT				;=
	inc	al				;|  check ray ref limit
	cmp	al,byte ptr MAXREFCT		;|
	njnbe	TR0				;|
	cmp	byte ptr -2[bp],0		;|
	je	TR30				;|  check ray div limit
	inc	ah				;|
	cmp	ah,byte ptr MAXDIVCT		;|
	njnbe	TR0				;=
TR30:	mov	word ptr RESERVE[si].RRCT,ax		;=
	MOVMEM	RESERVE[si].RE,COL,12			;|
	FPMLT	RESERVE[si].RE.X,REF			;|
	FPMLT	RESERVE[si].RE.Y,REF			;|
	FPMLT	RESERVE[si].RE.Z,REF			;|
	MOVMEM	F0,RESERVE[si].RE.X			;|  similar as reflection
	FPADD	F0,RESERVE[si].RE.Y			;|
	FPADD	F0,RESERVE[si].RE.Z			;|
	FPCMP	F0,MINENERG			;|
	njl	TR0				;|
	mov	REAR,si				;|
	lea	si,RESERVE[si]			;|
	MOVMEM	[si].RO,CRSPNT,12		;|
	MOVMEM	[si].RV,NML,12			;|=	  1
	FPSUB	F1,CO				;||  RV = -(V + (cos - sqrt(...)) * NML)
	FPCHS	F1				;||	  n
	FPMLT	[si].RV.X,F1			;||
	FPMLT	[si].RV.Y,F1			;||
	FPMLT	[si].RV.Z,F1			;||
	FPADD	[si].RV.X,V.X			;||
	FPADD	[si].RV.Y,V.Y			;||
	FPADD	[si].RV.Z,V.Z			;||
	FPDIV	[si].RV.X,[bx].N		;||
	FPDIV	[si].RV.Y,[bx].N		;||
	FPDIV	[si].RV.Z,[bx].N		;||
	mov	ax,SIDE				;|=
	neg	ax				;||  from the other side
	mov	[si].RSIDE,ax			;|=
	mov	[si].RPRIM,bx			;|
	call	CHOOSE				;==  choose network
	jmp	TR0				;    end of process

TRACE	ENDP

;
;
;	SEARCH
;
;	Args  :	bx	(Environment *)
;		si	0 : Search
;			6 : Shadowing
;		SIDE	inside / outside
;		FIRST	first search or not
;
;	Ret   :	ax	number of solution
;
;

EXPMAR	equ	-3				;   exponent of margin

	public	SEARCH
SEARCH	PROC

	call	cs:SRCHTBL[si]			;   preprocess for search
	link	8
	push	si				;   push 0/6 -> becomes -10[bp]
	mov	word ptr -2[bp],0		;   -2[bp] : number of solution
	MOVMEM	WV,[bx].RV,24			;   move dir, point to workarea
	mov	si,[bx].RNET			;   si = Network
	push	bx				;   push Env -> becomes -12[bp]
	mov	bx,si				;   bx = Orlist
SRCH00:	mov	word ptr -4[bp],bx		;   -4[bp] : top of Orlist
	mov	bl,[bx]				;=
	cmp	bl,255				;|  end of Network ?
	nje	SRCH0E				;=
	cmp	bl,NORANGE			;=
	je	SRCH01				;|
	mov	bh,0				;|
	mov	cl,5				;|
	shl	bx,cl				;|  range check
	lea	bx,RANG[bx]			;|    if first
	lea	si,WV				;|	 then by IRANGE
	cmp	FIRST,0				;|	 else by NRANGE
	jnz	SRCH001				;|
	call	NRANGE				;|
	jmp	short SRCH002			;|
SRCH001:call	IRANGE				;|
SRCH002:njl	SRCH09				;=
SRCH01:	mov	bx,word ptr -4[bp]		;   bx = top of Orlist
	mov	word ptr -6[bp],bx		;   -6[bx] : Orlist(ALNum *)
SRCH10:	mov	bx,word ptr -6[bp]		;=
	inc	bx				;|  next element of Orlist
	mov	word ptr -6[bp],bx		;=
	mov	bl,[bx]				;=
	or	bl,bl				;|  end of Orlist ?
	njs	SRCH09				;=
	mov	bh,0				;=
	mov	cl,MaxPrimInAL + 1		;|  si = Andlist
	shl	bx,cl				;|
	lea	si,ANDLISTS[bx]			;=
	mov	word ptr -8[bp],si		;   -8[bp] = top of Andlist
SRCH20:	mov	bx,[si]				;   bx = Prim *
	or	bx,bx				;-  end of Andlist
	jz	SRCH10				;-
	cmp	[bx].Surface,4			;=  if light object
	jae	SRCH20a				;=    then skip itself
	cmp	byte ptr -10[bp],0		;-  if Shadowing search
	jz	SRCH20X				;-   then skip itself
SRCH20a:mov	di,word ptr -12[bp]		;=
	cmp	bx,[di].RPRIM			;|  skip itself
	jz	SRCH10				;=
SRCH20X:push	si				;   push Andlist -> -14[bp] <0>
	lea	si,WV				;=
	lea	di,FAC0				;|  get solution
	call	SOLVE				;=
	push	bx				;   push Prim *  -> -16[bp] <1>
	njc	SRCH21				;   there's no solution
	cmp	byte ptr -10[bp],0		;=
	jnz	SRCH200				;|  if SEARCH then
	FPCMP	FAC0,T				;|    nearer solution only
	njnl	SRCH21				;=
SRCH200:lea	bx,FAC1				;-  FAC1 = PNT
	call	LINEAR				;-
	MOVMEM	FAC4,[si],12			;=
	FPSCALE	FAC4.X,EXPMAR			;|
	FPSCALE	FAC4.Y,EXPMAR			;|
	FPSCALE	FAC4.Z,EXPMAR			;|
	mov	di,offset cs:FSUBR		;|
	cmp	SIDE,0				;|  FAC4 = PNT +- Margin
	js	SRCH201				;|   of	+ : Out -> In
	mov	di,offset cs:FADD		;|	- : In -> Out
SRCH201:					;|  only AND is allowed for clear object
	lea	si,FAC4.X			;|
	lea	bx,FAC1.X			;|
	push	di				;|
	call	di				;|
	pop	di				;|
	lea	si,FAC4.Y			;|
	lea	bx,FAC1.Y			;|
	push	di				;|
	call	di				;|
	pop	di				;|
	lea	si,FAC4.Z			;|
	lea	bx,FAC1.Z			;|
	push	di				;|
	call	di				;|
	pop	di				;|
	lea	si,FAC4
	mov	bx,word ptr -8[bp]		;=  bx = top of Andlist
SRCH30:	cmp	bx,word ptr -14[bp]		;|=   if itself then
	jz	SRCH39				;|=	skip this check
	push	bx				;|  <2>
	mov	bx,[bx]				;|  In-Out check
	or	bx,bx				;|-
	jz	SRCH32				;|- end of Andlist
	call	INOUT				;|
	pop	bx				;|  pop for push #<2>
	jl	SRCH21				;|     out of range
SRCH39:	inc	bx				;|=
	inc	bx				;|| next check
	jmp	SRCH30				;|=
SRCH32:	pop	ax				;=     dummy pop for push#<2>
	pop	bx				;   bx = Prim *itself (push#<1>)
	inc	word ptr -2[bp]			;   increment number of solution
	mov	si,word ptr -10[bp]		;   si = 0/6 flag
	call	cs:SRCHTBL[si+2]		;     process for solution
	jnc	SRCH22				;=  if carry then end
	pop	ax				;|    dummy pop for push#<0>
	jmp	SRCH0E				;=
SRCH21:	pop	ax				;  no solution dum pop of <1>
SRCH22:	pop	si				;    restore si = Andlist (push#<0>)
	inc	si				;=
	inc	si				;|   check next primitive
	jmp	SRCH20				;=
SRCH09:	mov	bx,word ptr -4[bp]		;=
	add	bx,MaxALinOL			;|   check next Andlist
	jmp	SRCH00				;=
SRCH0E:	pop	bx				;    restore bx = Env *
	pop	si				;    restore si = 0/6 flag
	call	cs:SRCHTBL[si+4]		;	postprocessing
	mov	ax,word ptr -2[bp]		;=   ax = number of solution
	or	ax,ax				;=	   and check if zero
	unlk
	ret

	even
SRCHTBL	dw	BEGSRCH
	dw	SOLSRCH
	dw	ENDSRCH
	dw	BEGSHDW
	dw	SOLSHDW
	dw	ENDSHDW

BEGSRCH:FPLOD	T,0ffffh,7fh,0			;    T = maximum distance
	ret

SOLSRCH:mov	ISPRIM,bx			;   set ISPRIM
	MOVMEM	PNT,FAC1,12			;   save PNT
	MOVMEM	T,FAC0				;   set new T (distance)
	mov	ax,CUBEX			;-  save CUBEX for later
	mov	CBX,ax				;-
	clc
	ret

ENDSRCH:mov	ax,CBX				;-  restore CUBEX
	mov	CUBEX,ax			;-	for NORMAL
	ret

BEGSHDW:FPLOD	BRIGHT,8000h,0,0		;   BRIGHT = 1.0
ENDSHDW:ret

SOLSHDW:cmp	[bx].Surface,3
	jz	SLSD2
	jnbe	SLSD12
SLSD1:	FPLOD	BRIGHT,0,0,0			;  if opaque or mirror
SLSD11:	stc					;  then no light.  quit rest
	ret
SLSD2:	FPSUB	BRIGHT,GRADE			;= clear object
	FPSGN?	BRIGHT				;|   diminish BRIGHT and
	jne	SLSD1				;|   continue
	FP0?	BRIGHT				;|    until BRIGHT == 0
	jz	SLSD11				;|
SLSD12:	clc					;=
	ret

FSUBR	PROC

	call	FSUB				;  I should have
	FPCHS	[si]				;   prepare this in 'reala.asm'
	ret					;         (programmer)

FSUBR	ENDP

SEARCH	ENDP

@code	ends

@datau	segment

CBX	dw	?
WV	db	(type VECTOR)*2 dup (?)
BRIGHT	REAL	<?,?,?>
	public	BRIGHT
REF	REAL	<?,?,?>
	public	REF

@datau	ends

@code	segment

;
;
;	DIFFUSION
;
;

	public	DIFFUSE
DIFFUSE	PROC

ENV	equ	type ENVIRONMENT + 2

	link	ENV				;   -2[bp] : in shadow of itself
	mov	word ptr -2[bp],0		;   -1[bp] : in shadow of others
	MOVMEM	FAC7,LIGHT,12			;=
	FPMLT	FAC7.X,NML.X			;|
	FPMLT	FAC7.Y,NML.Y			;|
	FPMLT	FAC7.Z,NML.Z			;|
	FPADD	FAC7,FAC8			;|  FAC7 = ( LIGHT | NML )
	FPADD	FAC7,FAC9			;=
	FPSGN?	FAC7				;=
	jz	DF1				;|  if FAC7 < 0.0 then
	FPCHS	FAC7				;|   in the shadow of itself
	mov	byte ptr -2[bp],1		;=
DF1:	FPLOD	FAC8,8000h,0,0			;   FAC8 = BRIGHT  1.0
	cmp	SHADOWIN,0			;=
	jz	DF2				;|  if Shadowing flag is on
	cmp	[bx].Surface,4			;|     then shadowing check
	jae	DF2				;|
	cmp	byte ptr -2[bp],0		;|=
	je	DF11				;|| shadow of itself
	FPLOD	FAC8,0,0,0			;||
	jmp	short DF2			;|=
DF11:	MOVMEM	-ENV[bp].RV,LIGHT,12		;|=
	MOVMEM	-ENV[bp].RO,CRSPNT,12		;||
	mov	bx,ISPRIM			;||  set Env for shadowing
	mov	-ENV[bp].RPRIM,bx		;||
	mov	ax,[bx].SH			;||
	mov	-ENV[bp].RNET,ax		;|=
	lea	bx,-ENV[bp]			;|
	mov	si,6				;|   shadowing search
	mov	FIRST,0				;|   not first search
	call	SEARCH				;|
	jz	DF12				;|    no solution
	mov	byte ptr -1[bp],1		;|  in the shadow of others
DF12:	MOVMEM	FAC8,BRIGHT			;=  get BRIGHT into FAC8
DF2:	mov	bx,ISPRIM			;=
	cmp	[bx].Surface,4			;|
	njnae	DF29				;|
	cmp	URA,0				;|
	njnz	DF4				;|
	MOVMEM	FAC10,[bx].Solid,<type VECTOR>	;|
	MOVMEM	FAC9,CO				;|
	mov	cx,word ptr [bx].Refrate	;|  light object process
	jcxz	DF201				;|
DF202:	push	cx				;|
	FPSQAR	FAC9				;|
	pop	cx				;|
	loop	DF202				;|
DF201:						;|
	FPMLT	FAC10.X,FAC9			;|
	FPMLT	FAC10.Y,FAC9			;|
	FPMLT	FAC10.Z,FAC9			;|
	FPMLT	FAC10.X,COL.X			;|
	FPMLT	FAC10.Y,COL.Y			;|
	FPMLT	FAC10.Z,COL.Z			;|
	FRTOIF	FAC10.X				;|
	add	SRED[0*2],ax			;|    add to color buffer
	FRTOIF	FAC10.Y				;|
	add	SRED[1*2],ax			;|    add to color buffer
	FRTOIF	FAC10.Z				;|
	add	SRED[2*2],ax			;|    add to color buffer
	jmp	DF4				;=
DF29:	mov	ax,word ptr -2[bp]		;==  process for highlighting
	or	al,ah				;||    in the shadow
	njnz	DF3				;|=
	MOVMEM	FAC9,CO				;|=
	FPSCALE	FAC9,1				;||
	MOVMEM	FAC10,NML,12			;||  FAC10 = ( LIGHT | refRAY )
	FPMLT	FAC10.X,FAC9			;||
	FPMLT	FAC10.Y,FAC9			;||
	FPMLT	FAC10.Z,FAC9			;||
	FPADD	FAC10.X,V.X			;||
	FPADD	FAC10.Y,V.Y			;||
	FPADD	FAC10.Z,V.Z			;||
	FPMLT	FAC10.X,LIGHT.X			;||
	FPMLT	FAC10.Y,LIGHT.Y			;||
	FPMLT	FAC10.Z,LIGHT.Z			;||
	FPADD	FAC10,FAC11			;||
	FPADD	FAC10,FAC12			;|=
	FPSGN?	FAC10				;|-  if FAC10 < 0
	njnz	DF3				;|-	then directed 'SOPPO'
	FPSQAR	FAC10				;|=
	FPSQAR	FAC10				;|=  FAC10 = cos^4
	FPMLT	FAC10,FAC8			;|=
	mov	bx,ISPRIM			;||  FAC10 *= BRIGHT * highlight strength
	FPMLT	FAC10,[bx].Highlight		;|=
	MOVMEM	FAC11,FAC10			;|
	FPMLT	FAC11,COL.X			;|
	FRTOIF	FAC11				;|   add to color buffer
	or	ax,ax				;|
	js	DF3A0				;|
	add	SRED[2*0],ax			;|
DF3A0:						;|
	MOVMEM	FAC11,FAC10			;|
	FPMLT	FAC11,COL.Y			;|
	FRTOIF	FAC11				;|   add to color buffer
	or	ax,ax				;|
	js	DF3A1				;|
	add	SRED[2*1],ax			;|
DF3A1:						;|
	MOVMEM	FAC11,FAC10			;|
	FPMLT	FAC11,COL.Z			;|
	FRTOIF	FAC11				;|   add to color buffer
	or	ax,ax				;|
	js	DF3A2				;|
	add	SRED[2*2],ax			;|
DF3A2:						;|
DF3:	mov	bx,ISPRIM			;=   process for random reflection
	cmp	[bx].Surface,3			;|-
	nje	DF4				;|-  skip if clear object
	FPMLT	FAC7,FAC8			;|=
	FPMLT	FAC7,INVBG			;||
	FPADD	FAC7,BACKGROU			;||  FAC7 = (1-ref) {
	MOVMEM	FAC8,REF			;||	    cos*(1-d) + d
	FPSUB	FAC8,Const1			;||	  }
	FPCHS	FAC8				;||  d : background reflect ratio
	FPMLT	FAC7,FAC8			;|=
	MOVMEM	FAC8,[bx].Solid,12		;|=
	FPMLT	FAC8.X,FAC7			;||
	FPMLT	FAC8.Y,FAC7			;||
	FPMLT	FAC8.Z,FAC7			;||
	FPMLT	FAC8.X,COL.X			;||
	FPMLT	FAC8.Y,COL.Y			;||
	FPMLT	FAC8.Z,COL.Z			;||
	FRTOIF	FAC8.X				;||
	or	ax,ax				;||    add to color buffer
	js	DF30				;||
	add	SRED[0*2],ax			;||
DF30:						;||
	FRTOIF	FAC8.Y				;||
	or	ax,ax				;||    add to color buffer
	js	DF31				;||
	add	SRED[1*2],ax			;||
DF31:						;||
	FRTOIF	FAC8.Z				;||
	or	ax,ax				;||    add to color buffer
	js	DF32				;||
	add	SRED[2*2],ax			;||
DF32:						;||
DF4:	unlk					;==
	ret

DIFFUSE	ENDP

;
;
;	CHOOSE
;
;

	public	CHOOSE
CHOOSE	PROC

	mov	dx,0
	FPSGN?	[si].RV.X
	je	CH1
	mov	dl,4
CH1:	FPSGN?	[si].RV.Y
	je	CH2
	add	dl,2
CH2:	xchg	si,dx
	mov	ax,[bx].RU[si]
	xchg	si,dx
	mov	[si].RNET,ax
	ret

CHOOSE	ENDP

	include	epilogue.h

	end
