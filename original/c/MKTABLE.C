#include "stdio2.h"

#define max 65536.0
#define half 32768.0

main()
{
    unsigned z,x;
    double log2, floor(), log(), exp();
    FILE *fp;

    log2 = log(2.0);

    if( (fp = fopen("logtbl.tbl","wb")) == NULL)
	abort("Can't open lobtbl.tbl.\n");

    for(x = 0; x < 0x8000; x++) {
	z = (unsigned) floor(log((double)x/half + 1.0)/log2*max + .5);
	putc(z & 0xff,fp);
	putc(z >> 8,fp);
	printf("%4x\r",x);
    }

    fclose(fp);
    putchar('\n');

    if( (fp = fopen("exptbl.tbl","wb")) == NULL)
	abort("Can't open exptbl.tbl.\n");

    for(x = 0; x < 0x8000; x++) {
	z = (unsigned) floor(exp((double)x/half * log2)*half + .5);
	putc(z & 0xff,fp);
	putc(z >> 8,fp);
	printf("%4x\r",x);
    }

    fclose(fp);
    putchar('\n');

}

