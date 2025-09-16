#include <stdio.h>

main(int argc, char *argv[])
{
	FILE *fp;
	int x;
	int hb, lb;

	if (argc != 2) {
		fprintf(stderr, "Usage: tbl2asm tblfile\n");
		exit(1);
	}
	if ((fp = fopen(argv[1], "r")) == NULL) {
		perror(argv[1]);
	}

	x = 0;
	while ((lb = getc(fp)) != EOF) {
		hb = getc(fp);
		if (x == 0) 
			printf("\tdw\t");
		printf("0%02x%02xh%s", hb, lb, (x == 7) ? "\n" : ", ");
		if (++x == 8) x = 0;
	}
	exit(0);
}
