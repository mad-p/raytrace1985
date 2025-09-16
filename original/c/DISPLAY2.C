#include "stdio2.h"

main(argc,argv)
int argc;
char *argv[];
{
    unsigned char filename[256], buff[256], *cp;
    unsigned int r, g, b;
    FILE *fp;
    int i, j, x,y, xstart, xstop, ystart, ystop, xsize, ysize, xwind, ywind;

    printf("Source file name : ");
    scanf("%s", filename);
    fgets(buff, 256, stdin);		/*  skip caridge return  */

    if( (fp = fopen(filename, "rb")) == NULL)
	abort("Can't open %s.",filename);

    gscreen(3,0);
    if(argc != 1) gcls();

    xstart = 0; xstop = 320;
    printf("Xstart[0] Xstop[320] : ");
    fgets(buff, 256, stdin);
    for(cp = buff; isspace(*cp); cp++)
		;
    if(*cp) sscanf(cp, "%d", &xstart);
    while( *cp && !isspace(*cp)) cp++;
    for(; isspace(*cp); cp++)
		;
    if(*cp) sscanf(cp, "%d", &xstop);

    ystart = 0; ystop = 200;
    printf("Ystart[0] Ystop[200] : ");
    fgets(buff, 256, stdin);
    for(cp = buff; isspace(*cp); cp++)
		;
    if(*cp) sscanf(cp, "%d", &ystart);
    while( *cp && !isspace(*cp)) cp++;
    for(; isspace(*cp); cp++)
		;
    if(*cp) sscanf(cp, "%d", &ystop);

    xsize = 320; ysize = 200;
    printf("Xsize[320] Ysize[200] : ");
    fgets(buff, 256, stdin);
    for(cp = buff; isspace(*cp); cp++)
		;
    if(*cp) sscanf(cp, "%d", &xsize);
    while( *cp && !isspace(*cp)) cp++;
    for(; isspace(*cp); cp++)
		;
    if(*cp) sscanf(cp, "%d", &ysize);

    xwind = 0; xwind = 0;
    printf("Xwind[0] Ywind[0] : ");
    fgets(buff, 256, stdin);
    for(cp = buff; isspace(*cp); cp++)
		;
    if(*cp) sscanf(cp, "%d", &xwind);
    while( *cp && !isspace(*cp)) cp++;
    for(; isspace(*cp); cp++)
		;
    if(*cp) sscanf(cp, "%d", &ywind);

    fseek(fp, (long)ywind*(long)xsize*3L, 1);

    for(y = ystart; y < ystop; y++) {
	fseek(fp, (long)xwind*3L, 1);
	for(x = xstart; x < xstop; x++) {
	    r = getc(fp);
	    g = getc(fp);
	    b = getc(fp);
	    pset(x, y, r, g, b);
	}
	fseek(fp, (long)(xsize-xwind-xstop+xstart)*3L, 1);
	printf("line %3d\r",y);
    }

    printf("Complete.\t\t\007\n");

}

