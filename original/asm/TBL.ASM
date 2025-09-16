;
;	log table
;

@CODE	SEGMENT WORD PUBLIC 'CODE'
@CODE	ENDS

SEGLG1	segment para public 'LOGT1'
	org	0000h
	include	logtbl1.asm
SEGLG1	ends
SEGLG2	segment para public 'LOGT2'
	org	0000h
	include	logtbl2.asm
SEGLG2	ends

;
;	exp table

SEGEX1	segment para public 'EXPT1'
	org	0000h
	include	exptbl1.asm
SEGEX1	ends
SEGEX2	segment para public 'EXPT2'
	org	0000h
	include	exptbl2.asm
SEGEX2	ends

@DATAB	SEGMENT WORD PUBLIC 'DATAB'
@DATAB	ENDS
@DATAC	SEGMENT WORD PUBLIC 'DATAC'
@sb	label	byte
@sw	label	word
@DATAC	ENDS
@DATAI	SEGMENT WORD PUBLIC 'DATAI'
@ib	label	byte
@iw	label	word
@DATAI	ENDS
@DATAT	SEGMENT WORD PUBLIC 'DATAT'
@DATAT	ENDS
@DATAU	SEGMENT WORD PUBLIC 'DATAU'
@ub	label	byte
@uw	label	word
@DATAU	ENDS
@DATAV	SEGMENT WORD PUBLIC 'DATAV'
@DATAV	ENDS
DGROUP	GROUP	@DATAB,@DATAC,@DATAI,@DATAT,@DATAU,@DATAV
LGROUP	GROUP	SEGLG1
EGROUP	GROUP	SEGEX1

	end
