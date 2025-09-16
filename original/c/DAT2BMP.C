#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int wd = 128;
int ht = 128;

#define filept(arg)	arg

static void lendshort(FILE *fp, int n)
{
    putc(n & 0xff, filept(fp));
    putc((n>>8) & 0xff, filept(fp));
}

static void lendlong(FILE *fp, int n)
{
    putc(n & 0xff, filept(fp));
    putc((n>>8) & 0xff, filept(fp));
    putc((n>>16) & 0xff, filept(fp));
    putc((n>>24) & 0xff, filept(fp));
}

void header(FILE *fp)
{
    lendshort(fp, 0x4d42);
    lendlong(fp, 0);
    lendlong(fp, 0);
    lendlong(fp, 0x36);
    lendlong(fp, 40);
    lendlong(fp, wd);
    lendlong(fp, ht);
    lendshort(fp, 1);
    lendshort(fp, 24);
    lendlong(fp, 0);
    lendlong(fp, 0);
    lendlong(fp, 0xec4);
    lendlong(fp, 0xec4);
    lendlong(fp, 0);
    lendlong(fp, 0);
}

void
convert(char *srcname)
{
	char dstname[BUFSIZ];
	char *p;
	FILE *infile, *outfile;
	char *mem;

	strcpy(dstname, srcname);
	if ((p = strrchr(dstname, '.'))) {
		strcpy(p, ".bmp");
	}
	if (!(infile = fopen(srcname, "r"))) {
		perror(srcname);
		exit(1);
	}
	if (!(outfile = fopen(dstname, "w"))) {
		perror(srcname);
		exit(1);
	}
	header(outfile);
	mem = malloc(wd*ht*3);
	fread(mem, wd*ht*3, 1, infile);
	{
		int x, y;
		for (y=ht-1; y>=0; y--) {
			char *b = mem + y*3*wd;
			for (x=0; x<wd; x++) {
				putc(b[2], outfile);
				putc(b[1], outfile);
				putc(b[0], outfile);
				b += 3;
			}
		}
	}
	fclose(infile);
	fclose(outfile);				
}

void
main(int argc, char *argv[])
{
	int i;
	for (i=1; i<argc && argv[i][0] == '-'; i++) {
		switch(argv[i][1]) {
		      case 'h':
			if (argv[i][2]) {
				ht = atoi(&argv[i][2]);
			} else if (i+1 < argc) {
				ht = atoi(argv[++i]);
			}
			break;
		      case 'w':
			if (argv[i][2]) {
				wd = atoi(&argv[i][2]);
			} else if (i+1 < argc) {
				wd = atoi(argv[++i]);
			}
			break;
		}
	}
	if (ht < 1 || wd < 1) {
		fprintf(stderr, "invalid geometry\n");
		exit(1);
	}

	for (;i<argc; i++) {
		convert(argv[i]);
	}
}
