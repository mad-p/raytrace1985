#include "stdio2.h"

main(argc,argv)
int argc;
char *argv[];
{   FILE *fp1,*fp2;
    int c1,c2;
    long adr = 0L;

    fp1 = fopen(argv[1],"rb");
    fp2 = fopen(argv[2],"rb");

    if(!fp1 || !fp2) abort("can't open source files.");

    while((c1 = getc(fp1)) != EOF) {
	c2 = getc(fp2);
	if(c1 != c2) {
	    if(!((adr & 1)==0 && (c1-c2==1 || c2-c1==1)))
		printf("%08X  :  %02x %02x\n",adr,c1,c2);
	}
	adr++;
    }

    fclose(fp1);	fclose(fp2);

}


