#include "stdio2.h"

typedef struct {
    char	  exp;
    unsigned char sign;
    unsigned	  man;
}   real;

double	sqrt(),exp(),log(),frexp(),rtof(),floor(),pow();
long int inter();

double
rtof(src)
real *src;
{
    double v;

    v = (double)src->man/32768.0 * pow(2.0,(double)src->exp);
    if(src->sign)  v = -v;
    return v;

}

ftor(des,src)
real *des;
double src;
{
    int ex;

    if(src == 0.0) ((long *)des)[0] = (long) 0;
    else {
	if(src > 0.0) des->sign = (unsigned char)0;
	else {
	    des->sign = (unsigned char)1;
	    src = -src;
	}
	des->man = (unsigned) floor(frexp(src,&ex)*65536.0+.5);
	if(des->man == 0.0) {
	    des->man = 0x8000;
	    ex++;
	}
	des->exp = (char)ex-1;
    }

}

