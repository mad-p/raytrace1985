#include "stdio2.h"

typedef struct {
    char	  exp;
    unsigned char sign;
    unsigned	  man;
}   real;

double	sqrt(),exp(),log(),frexp(),rtof(),floor(),pow();
VOID	fadd(),fsub(),fsqrt(),fsqar(),fmlt(),fdiv(),finv(),fscale(),
	fexp(),flog(),itor();
int	rtoi(),rtoif(),fcomp();
long int inter();

main()
{
    double x,y;
    int i;
    real r[3];
#define BUFSIZ 256
    char s[BUFSIZ];

#define R ((long *)r)

    while(1) {
	printf("Input two numbers : ");
	fgets(s, sizeof s, stdin);
	if(strncmp(s,"end",3)==0) break;
	sscanf(s,"%F",&x);
	 scanf(  "%F",&y);		while(getchar()!='\n');
	ftor(r+1,x);   ftor(r+2,y);
	printf("x = %16.8e  (%08X)  ;  y = %16.8e  (%08X)\n",
		rtof(r+1),R[1],rtof(r+2),R[2]);
	printf("Hit return key");	while(getchar()!='\n');
	debug();
	R[0] = R[1];
	inter(fadd,r,r+2);
	printf("  x + y  = %16.8e  (%08X)\n",rtof(r),R[0]);
	R[0] = R[1];
	inter(fsub,r,r+2);
	printf("  x - y  = %16.8e  (%08X)\n",rtof(r),R[0]);
	R[0] = R[1];
	inter(fmlt,r,r+2);
	printf("  x * y  = %16.8e  (%08X)\n",rtof(r),R[0]);
	R[0] = R[1];
	inter(fdiv,r,r+2);
	printf("  x / y  = %16.8e  (%08X)\n",rtof(r),R[0]);
	R[0] = R[1];
	switch((int)inter(fcomp,r,r+2)) {
	    case 0:	printf("  x = y\n");
			break;
	    case 1:	printf("  x > y\n");
			break;
	    case -1:	printf("  x < y\n");
	}
	R[0] = R[1];
	inter(fscale,r,(int)y);
	printf("x sc %-3d = %16.8e  (%08X)\n",(int)y,rtof(r),R[0]);
	R[0] = R[1];
	inter(finv,r);
	printf("  1 / x  = %16.8e  (%08X)\n",rtof(r),R[0]);
	R[0] = R[1];
	inter(fsqrt,r);
	printf(" sqrt(x) = %16.8e  (%08X)\n",rtof(r),R[0]);
	R[0] = R[1];
	inter(fsqar,r);
	printf("  x^2    = %16.8e  (%08X)\n",rtof(r),R[0]);
	R[0] = R[1];
	inter(fexp,r);
	printf("  2^x    = %16.8e  (%08X)\n",rtof(r),R[0]);
	R[0] = R[1];
	inter(flog,r);
	printf(" log2(x) = %16.8e  (%08X)\n",rtof(r),R[0]);
	R[0] = R[1];
	printf("rtoi (x) = %6d\n",(int)inter(rtoi,r));
	R[0] = R[1];
	printf("rtoif(x) = %6d\n",(int)inter(rtoif,r));
	inter(itor,r,(int)x,r);
	printf("itor(%3d)= %16.8e  (%08X)\n",(int)x,rtof(r),R[0]);
	printf("\n");
    }

    printf("100 times multiple\n");
    r[0].man = 0xaaaa ; r[0].exp = r[0].sign = 0 ; R[1] = R[0];
    x = y = rtof(r);
    printf("Real start !  %16.8e *= %16.8e", rtof(r), rtof(r+1)); fgets(s, sizeof s, stdin);
    for(i=0; i<100; i++) inter(fmlt,r,r+1);
    printf("End\n");
    printf("	result = %16.8e  (%08X)\n",rtof(r),R[0]);
    printf("Double start !"); fgets(s, sizeof s, stdin);
    for(i=0; i<100; i++) x *= y;
    printf("End\n");
    printf("	result = %16.8e  \n",x);

    printf("100 times division\n");
    R[0] = 0xaaaa0060;
    R[1] = 0x88880000;
    x = rtof(r); y = rtof(r+1);
    printf("Real start !  %16.8e /= %16.8e", rtof(r), rtof(r+1)); fgets(s, sizeof s, stdin);
    for(i=0; i<100; i++) inter(fdiv,r,r+1);
    printf("End\n");
    printf("	result = %16.8e  (%08X)\n",rtof(r),R[0]);
    printf("Double start !"); fgets(s, sizeof s, stdin);
    for(i=0; i<100; i++) x /= y;
    printf("End\n");
    printf("	result = %16.8e  \n",x);

    printf("1000 times addition\n");
    r[1].man = 0xa001 ; r[1].exp = r[1].sign = 0 ; R[0] = (long)0;
    x = rtof(r); y = rtof(r+1);
    printf("Real start !  %16.8e += %16.8e", rtof(r), rtof(r+1)); fgets(s, sizeof s, stdin);
    for(i=0; i<1000; i++) inter(fadd,r,r+1);
    printf("End\n");
    printf("	result = %16.8e  (%08X)\n",rtof(r),R[0]);
    printf("Double start !"); fgets(s, sizeof s, stdin);
    for(i=0; i<1000; i++) y += x;
    printf("End\n");
    printf("	result = %16.8e  \n",y);

    printf("1000 times square root\n");
    *R = 0xaaaa0070;
    x = y = rtof(r);
    printf("Real start !  sqrt(%16.8e)", rtof(r)); fgets(s, sizeof s, stdin);
    for(i=0; i<1000; i++) {
	    *R = 0xaaaa0070;
	    inter(fsqrt,r);
    }
    printf("End\n");
    printf("	result = %16.8e  (%08X)\n",rtof(r),R[0]);
    printf("Double start !"); fgets(s, sizeof s, stdin);
    for(i=0; i<1000; i++) {
	    x = y;
	    x = sqrt(x);
    }
    printf("End\n");
    printf("	result = %16.8e  \n",x);

    printf("1000 times exponential\n");
    *R = 0xaaaa0003;
    x = y = rtof(r);
    printf("Real start !  exp2(%16.8e)", rtof(r)); fgets(s, sizeof s, stdin);
    for(i=0; i<1000; i++) {
	*R = 0xaaaa0003;
	inter(fexp,r);
    }
    printf("End\n");
    printf("	result = %16.8e  (%08X)\n",rtof(r),R[0]);
    printf("Double start !"); fgets(s, sizeof s, stdin);
    for(i=0; i<1000; i++) {
	    x = y;
	    x = exp(x);
    }
    printf("End\n");
    printf("	result = %16.8e  \n",exp(y*log(2.0)));

    printf("1000 times logarithem\n");
    printf("Real start !"); fgets(s, sizeof s, stdin);
    *R = 0xaaaa0003;
    x = y = rtof(r);
    for(i=0; i<1000; i++) {
	*R = 0xaaaa0003;
	inter(flog,r);
    }
    printf("End\n");
    printf("	result = %16.8e  (%08X)\n",rtof(r),R[0]);
    printf("Double start !"); fgets(s, sizeof s, stdin);
    for(i=0; i<1000; i++) {
	    x = y;
	    x = log(x);
    }
    printf("End\n");
    printf("	result = %16.8e  \n",log(y)/log(2.0));

    printf("1000 times comparison\n");
    r[0].man = 0xffff ; r[0].exp = 0x7f; r[0].sign = 0;
    r[1].man = 0xfffe ; r[0].exp = 0x7f; r[0].sign = 0;
    printf("Real start !"); fgets(s, sizeof s, stdin);
    for(i=0; i<1000; i++) inter(fcomp,r,r+1);
    printf("End\n");
    x = (double) 3.4027e+38; y = (double) 3.4026e+38;
    printf("Double start !"); fgets(s, sizeof s, stdin);
    for(i=0; i<1000; i++) x < y;
    printf("End\n");
}

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
	des->exp = (char)ex-1;
    }

}

debug()
{}
