#define EXTERN extern
#include "trace.h"

static VOID Tile(lc)
vector lc;
{
    static real oct = {-3,0,0x8000};		/* 1/8 */
    real w1, w2, r1, r2;
    short i1, i2, i3;
    unsigned short rand();

    *((long *)&r1) = ((long *)lc)[0];
    *((long *)&r2) = ((long *)lc)[2];
    inter(fscale,&r1,-4);
    inter(fscale,&r2,-4);
    i1 = (short)inter(rtoif,&r1);
    i2 = (short)inter(rtoif,&r2);
    i3 = rand(&i1) * 337 + rand(&i2);
    w1.man = rand(&i3) | 0x8000;
    w2.man = rand(&i3) | 0x8000;
    w1.exp = w2.exp = (char)-3;
    w1.sign = w2.sign = 0;
    inter(fsub, &w1, &oct);
    inter(fsub, &w2, &oct);
    w1.sign = (rand(&i3) & 0x4000) == 0;
    w2.sign = (rand(&i3) & 0x4000) == 0;
    inter(fadd, nml  , &w1);
    inter(fadd, nml+2, &w2);
    inter(nrmlize, nml);
}

static unsigned short rand(x)
unsigned short *x;
{
    int i;
    for (i=0; i<7; i++)
	    *x = *x * 8009 + 31957;
    return *x;
}

VOID (*bumping[MAXBUMP])() = {Tile};

VOID grade1(lc,p)
vector lc;
Primitive *p;
{   real w1,w2,w3;
    extern real const1;

    static real const255 = {7,0,0xff00};
    static real Half = {10,0,0x8000};
    static BOOL got = FALSE;

    if(!got) {
	FILE *fp;  double w;
	fp = fopen("RADIUS.DAT","r");
	if(fp == NULL) fprintf(stderr,"\007		Can't open RADIUS.\n");
	    else {	fscanf(fp, "%F", &w);
			ftor(&Half, w*w);
			fclose(fp);
	}
	got = TRUE;
    }

    *((long *)&w1) = ((long *)lc)[0];
    *((long *)&w2) = ((long *)lc)[1];
    *((long *)&w3) = ((long *)lc)[2];
    inter(fsqar,&w1);
    inter(fsqar,&w2);
    inter(fsqar,&w3);
    inter(fadd,&w1,&w2);
    inter(fadd,&w1,&w3);
    inter(fdiv,&w1,&Half);
    inter(fadd,&w1,&const1);
    inter(finv,&w1);
    inter(fmlt,&w1,&const255);
    *((long *)&p->color.solid[1]) = *(long *)&w1;
    inter(fsub,&w1,&const255);
    w1.sign ^= 1;
    *((long *)&p->color.solid[2]) = *(long *)&w1;
}

VOID grade2(lc,p)
vector lc;
Primitive *p;
{   real w1,w2,w3;
    extern real const1;

    static real const255 = {7,0,0xff00};
    static real Half = {10,0,0x8000};
    static BOOL got = FALSE;

    if(!got) {
	FILE *fp;  double w;
	fp = fopen("RADIUS.DAT","r");
	if(fp == NULL) fprintf(stderr,"\007		Can't open RADIUS.\n");
	    else {	fscanf(fp, "%F", &w);
			ftor(&Half, w*w);
			fclose(fp);
	}
	got = TRUE;
    }

    *((long *)&w1) = ((long *)lc)[0];
    *((long *)&w3) = ((long *)lc)[2];
    inter(fsqar,&w1);
    inter(fsqar,&w3);
    inter(fadd,&w1,&w3);
    inter(fdiv,&w1,&Half);
    inter(fadd,&w1,&const1);
    inter(finv,&w1);
    inter(fmlt,&w1,&const255);
    *((long *)&p->color.solid[1]) = *(long *)&w1;
    inter(fsub,&w1,&const255);
    w1.sign ^= 1;
    *((long *)&p->color.solid[2]) = *(long *)&w1;
}

VOID (*usertext[MAXUTEX])() = {grade1,grade2};

