#include "stdio2.h"

typedef struct {
	     char  exp;
    unsigned char  sign;
    unsigned short man;
}   real;

typedef real vector[3];

#define MAXBUMP 10

extern vector nrm;
long int inter();
VOID fscale(),rtoif(),fsub(),fadd(),nrmlize();

static VOID TILE(lc)
vector *lc;
{
    static real oct = {-3,0,0x8000};		/* 1/8 */
    real w1, w2;
    short i1, i2, rand();

    inter(fscale,(*lc)  ,-3);
    inter(fscale,(*lc)+2,-3);
    i1 = (short)inter(rtoif,(*lc)  );
    i2 = (short)inter(rtoif,(*lc)+2);
    i1 |= i2 << 4;
    w1.man = rand(&i1) | 0x8000;
    w2.man = rand(&i1) | 0x8000;
    w1.exp = w2.exp = (char)-3;
    w1.sign = w2.sign = 0;
    inter(fsub, &w1, &oct);
    inter(fsub, &w2, &oct);
    w1.sign = rand(&i1) & 1;
    w2.sign = rand(&i1) & 1;
    inter(fadd, nrm  , &w1);
    inter(fadd, nrm+2, &w2);
    inter(nrmlize, nrm);

}

static short rand(x)
short *x;
{   int i;
    short y;

    for(i=19, y = *x; i>0; i--)
	if((y=(*x <<= 1) -31973) > 0)
	    *x = y;
    return *x;
}

VOID (*bumping[MAXBUMP])() = {TILE};


