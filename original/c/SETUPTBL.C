/*****************************************************************

	Setup LOG & EXP Tables

		by K.Maeda
		10-Aug-85

	Two files "LOGTBL.TBL" & "EXPTBL.TBL"
	    should be available on the current directory

	CAUTION : This program must be compiled in BIGMODEL

*****************************************************************/

#include "stdio2.h"

#define PAGING	0xa6
#define LOGTBL	0
#define EXPTBL	1
#define GDCCOM	0xa2
#define	STOP	0xc

main()
{
    extern char *abstoptr();
    extern int fread();
    extern FILE *fopen();

    FILE *fp;

    outportb(PAGING,LOGTBL);
    if( (fp = fopen("logtbl.tbl","rb")) == NULL)
	abort("Can't open LOGTBL.TBL");
    if(fread(abstoptr(0xa8000),0x8000,2,fp) != 2)
	abort("Read error LOGTBL.TBL");
    fclose(fp);
    outportb(PAGING,EXPTBL);
    if( (fp = fopen("exptbl.tbl","rb")) == NULL)
	abort("Can't open EXPTBL.TBL");
    if(fread(abstoptr(0xa8000),0x8000,2,fp) != 2)
	abort("Read error EXPTBL.TBL");
    fclose(fp);

    outportb(GDCCOM,STOP);

}

