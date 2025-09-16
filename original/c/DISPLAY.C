#include "stdio2.h"

main(argc,argv)
int argc;
char *argv[];
{
    unsigned char filename[256], buff[256], *cp;
    unsigned int r, g, b;
    FILE *fp;
    int x,y, xstart, xstop, ystart, ystop;

    printf("Source file name : ");
    scanf("%s", filename);
    fgets(buff, 256, stdin);		/*  skip caridge return  */

    if( (fp = fopen(filename, "rb")) == NULL)
	abort("Can't open %s.",filename);

    gscreen(3,0);
    if(argc != 1) gcls();

    xstart = 0; xstop = 256;
    printf("Xstart[0] Xstop[256] : ");
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

    for(y = ystart; y < ystop; y++) {
	for(x = xstart; x < xstop; x++) {
	    r = getc(fp);
	    g = getc(fp);
	    b = getc(fp);
	    pset(x, y, r, g, b);
	}
	printf("line %3d\r",y);
    }

    printf("Complete.\t\t\007\n");

}


