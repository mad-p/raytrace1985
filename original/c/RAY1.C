#define EXTERN		/* このファイルで変数を定義する */
#define _INIT

#include "trace.h"

#define PI  3.14159265358979	/* 円周率 */
#define RAD (PI/180.0)		/* 度からラジアンへの変換係数 */
#define sqr(x) ((x)*(x))	/* sqr 関数がなかった */

static double w[14], sc[3], vp[3], alpha, beta, ratio;
double log();

int quiet = 0;
int verbose = 0;

main(argc, argv)
int argc;
char *argv;
{
    input(argc,argv);		/* データの読み込み */
    prologue();			/* Destination File の Open */
    mainloop();			/* Raytracing 本体 */
    epilogue();			/* Destination File の close */
}

epilogue()
{
    fclose(dfp);
    printf("\007END of process.\n");
}

#define freadln(x) fgets(s,255,x);		/* \n の Cansel */
#define LG(x) ((long *)(x))			/* Real は long長 */
#define iswhite(x) (x=='\n' || x==' ' || x=='\t')

input(argc, argv)
int argc;
char *argv[];
{
    FILE *fopen(), *fp;
    char fname[255], s[255], *cp;
    int  i, j, k, w1,
	 sh, ru, rd, lu, ld;

    fname[0] = '\0';
    if (argc > 1) {
	    int i;
	    ratio = 1.0;
	    step = 1;
	    monitor = 0;
	    shadowing = 1;
	    ystart = 0; ystop = VDOTSIZE;
	    xstart=(HDOTSIZE-VDOTSIZE)/2; xstop=(HDOTSIZE+VDOTSIZE)/2;
	    desname[0] = '\0';
	    for (i=1; i<argc; i++) {
		    if (argv[i][0] == '-') {
			    switch(argv[i][1]) {
				  case 'r':
				    sscanf(&argv[i][2], "%F", &ratio);
				    break;
				  case 's':
				    sscanf(&argv[i][2], "%d", &step);
				    if (step == 0) step = 1;
				    break;
				  case 'q':
				    quiet = 1;
				    break;
				  case 'v':
				    verbose = 1;
				    break;
				  case 'S':
				    shadowing = 0;
				    break;
				  case 'o':
				    strcpy(desname, &argv[i][2]);
				    break;
				  default:
				    abort("Usage: raytrace -q -v -r<aspect> -s<step> -o<outfile> <modelfile>\n");
			    }
		    } else {
			    strcpy(fname, argv[i]);
			    if( (fp = fopen(fname, "r")) == NULL)
				    abort("Can't open %s\n",fname);
			    if (desname[0] == '\0') {
				    strcpy(desname, fname);
				    cp = strrchr(desname, '.');
				    if (cp) {
					    strcpy(cp, ".bmp");
				    } else {
					    strcat(desname, ".bmp");
				    }
			    }
		    }
	    }
    }
    if (fname[0] == '\0') {
	    printf("Source      Filename : ");
	    scanf("%s",fname);
	    printf("Destination Filename : ");
	    scanf("%s",desname);   freadln(stdin);
	    if( (fp = fopen(fname, "r")) == NULL)
		    abort("Can't open %s\n",fname);
	    printf("Y/X ratio [1.0] : ");
	    fgets(s, 255, stdin);
	    for(cp = s;iswhite(*cp); cp++);
	    ratio = 1.0;
	    if(*cp) sscanf(cp,"%F",&ratio);
	    printf("Scan Step [1] : ");
	    fgets(s, 255, stdin);
	    for(cp = s;iswhite(*cp); cp++);
	    step = 1;		/* Return Key のみ押せば Default が入る */
	    sscanf(cp,"%d",&step);
	    printf("Scan Y start[0] stop[%d] : ",VDOTSIZE);
	    fgets(s,255,stdin);
	    ystart=0; ystop=VDOTSIZE;
	    for(cp = s; iswhite(*cp); cp++);
	    sscanf(cp,"%d",&ystart);
	    while(*cp && ! iswhite(*cp) ) cp++;
	    while(iswhite(*cp)) cp++;
	    sscanf(cp,"%d",&ystop);
	    xstart=(HDOTSIZE-VDOTSIZE)/2; xstop=(HDOTSIZE+VDOTSIZE)/2;
	    printf("Scan X start[%d] stop[%d] : ",xstart,xstop);
	    fgets(s,255,stdin);
	    for(cp = s; iswhite(*cp); cp++);
	    sscanf(cp,"%d",&xstart);
	    while(*cp && ! iswhite(*cp) ) cp++;
	    while(iswhite(*cp)) cp++;
	    sscanf(cp,"%d",&xstop);
	    printf("Monitor on 1 / OFF 0 : ");
	    monitor=0;
	    fgets(s,255,stdin);
	    for(cp = s; iswhite(*cp); cp++);
	    sscanf(cp,"%d",&monitor);
	    monitor=0;
	    printf("Shadowing ON 1 / off 0 : ");
	    shadowing=1;
	    fgets(s,255,stdin);
	    for(cp = s; iswhite(*cp); cp++);
	    sscanf(cp,"%d",&shadowing);
	    putchar('\n');
    }
    if (!quiet) {
	    printf("Scaning   (%3d-%3d , %3d-%3d) by %d\n\n",
		   xstart, xstop, ystart, ystop, step);
	    printf("Monitor   (%s)\n",monitor ? "ON" : "OFF");
	    printf("Shadowing (%s)\n",shadowing ? "ON" : "OFF");
	    printf("X : Y  ==  1.000 : %5.3f\n",ratio);
    }
    fscanf(fp,"%F",vp+2); vp[0] = vp[1] = 0.0;
    fscanf(fp,"%F %F %F", sc, sc+1, sc+2);
    fscanf(fp,"%F %F", &alpha, &beta);   freadln(fp);
    fscanf(fp,"%F %F", w, w+1);  freadln(fp);
    if (!quiet) {
	    printf("Viewpoint depth %6.1f\n",vp[2]);
	    printf("Screen Location (%5.1f,%5.1f,%5.1f)\n",sc[0],sc[1],sc[2]);
	    printf("Screen Rotation (%5.1f %5.1f)\n",alpha,beta);
	    printf("Light  Rotation (%5.1f %5.1f)\n\n", w[0], w[1]);
    }

	/*  preprocess for Screen  */
    alpha *= RAD;  beta *= RAD;
    w[0] *= RAD;   w[1] *= RAD;
    ftor(light    ,  cos(w[0]) * sin(w[1]));
    ftor(light + 1,  sin(w[0]));
    ftor(light + 2, -cos(w[0]) * cos(w[1]));
    vp[0] = -vp[2] * cos(alpha) * sin(beta) + sc[0];
    vp[1] = -vp[2] * sin(alpha)		    + sc[1];
    vp[2] =  vp[2] * cos(alpha) * cos(beta) + sc[2];
    ftor(viewpt+0, vp[0]);
    ftor(viewpt+1, vp[1]);
    ftor(viewpt+2, vp[2]);
    ftor(toscr+0, sc[0] -= vp[0]);
    ftor(toscr+1, sc[1] -= vp[1]);
    ftor(toscr+2, sc[2] -= vp[2]);
    ftor(eh+0, (HSCRSIZE/(HDOTSIZE/2)) * cos(beta));
    LG(eh)[1] = 0L;
    ftor(eh+2, (HSCRSIZE/(HDOTSIZE/2)) * sin(beta));
    ftor(ev+0, + ratio * (VSCRSIZE/(VDOTSIZE/2)) * sin(alpha) * sin(beta));
    ftor(ev+1, - ratio * (VSCRSIZE/(VDOTSIZE/2)) * cos(alpha)		 );
    ftor(ev+2, - ratio * (VSCRSIZE/(VDOTSIZE/2)) * sin(alpha) * cos(beta));

	/*  read primitive data  */
    fscanf(fp,"%d",&w1);
    for(i = 0; w1 != EOL; i++) {
	if(i>=MAXPRIM) abort("Primitive Overflow.");
	if (verbose) printf("Primitive %3d\n",i);
	pr[i].kind = w1;
	fscanf(fp,"%d %d %d %d",&pr[i].texture,&pr[i].bump,
				&pr[i].surface,&pr[i].rotation);
	fscanf(fp,"%F %F %F", w, w+1, w+2);
	fscanf(fp,"%F %F %F", w+3, w+4, w+5);
	fscanf(fp,"%d %F %F", &pr[i].sgn, w+6, w+7);
	fscanf(fp,"%F %F %F", w+8, w+9, w+10);
	if(pr[i].rotation)
	    fscanf(fp,"%F %F %F", w+11, w+12, w+13);
	fscanf(fp,"%d %d %d %d %d",&sh,&ru,&rd,&lu,&ld);
	pr[i].sh = networks + sh;
	pr[i].ru = networks + ru;
	pr[i].rd = networks + rd;
	pr[i].lu = networks + lu;
	pr[i].ld = networks + ld;
	freadln(fp);
	if (verbose) {
		printf("%d%x%d%d%d ",pr[i].kind, pr[i].texture, pr[i].bump,
		       pr[i].surface, pr[i].rotation);
		printf("(%5.1f,%5.1f,%5.1f)", w[0], w[1], w[2]);
		printf("(%3.0f,%3.0f,%3.0f)", w[3], w[4], w[5]);
		printf(" %c %3.1f %3.0f ",pr[i].sgn>0 ? '+' : '-', w[6], w[7]);
		if(pr[i].surface == CLEAR)
			printf("[%3.1f %3.1f %3.1f]", w[8], w[9], w[10]);
		else
			printf("[%3.0f %3.0f %3.0f]", w[8], w[9], w[10]);
		if(pr[i].rotation)
			printf("(%2.0f %2.0f %2.0f)", w[11],w[12],w[13]);
		else
			printf("          ");
		printf("%x%x%x%x%x\n",sh,ru,rd,lu,ld);
	}
	transform(pr + i);

	fscanf(fp, "%d", &w1);
    }
    freadln(fp);

    fscanf(fp, "%d", &w1);
    for(i = 0; w1 != EOL; i++) {
	if(i >= MAXRANGE) abort("Range primitive overflow");

	if (verbose) printf("\nRange primitive %3d",i);
	fscanf(fp,"%F %F %F", w, w+1, w+2);
	fscanf(fp,"%F", w+3);

	if (verbose) printf("     (%5.1f %5.1f %5.1f)  %5.1f", w[0], w[1], w[2], w[3]);

	ftor(&rang[i].location[0], w[0]);
	ftor(&rang[i].location[1], w[1]);
	ftor(&rang[i].location[2], w[2]);
	ftor(&rang[i].radius, w[3]*=w[3]);

	ftor(&rang[i].direction[0], w[0] -= vp[0]);
	ftor(&rang[i].direction[1], w[1] -= vp[1]);
	ftor(&rang[i].direction[2], w[2] -= vp[2]);
	inter(nrmlize,&rang[i].direction);
	w[4] = 1-w[3]/(sqr(w[0])+sqr(w[1])+sqr(w[2]));
	if (w[4] > 0.0)	ftor(&rang[i].mincos,sqrt(w[4]));
	    else	ftor(&rang[i].mincos,-2.0);

	freadln(fp);
	fscanf(fp, "%d", &w1);
    }
    freadln(fp);
    if (verbose) printf("\n\n");

    fscanf(fp, "%d", &w1);
    for(j = 0; w1 != EOL ; j++) {
	if(j >= MaxAndList) abort("Andlist overflow");
	if (verbose) printf("Andlist %2d\n",j);

	for(i = 0; w1 != EOL; i++) {
	    if(i >= MaxPrimInAL) abort("Too large Andlist.");
	    andlists[j][i] = pr + w1;
	    fscanf(fp, "%d", &w1);
	}
	andlists[j][i] = NULL;
	freadln(fp);
	fscanf(fp, "%d", &w1);
    }
    andlists[j][0] = NULL;
    freadln(fp);

    fscanf(fp, "%d", &w1);
    for(k = 0; w1 != EOL; k++) {
	if(k >= MAXNET) abort("Network overflow\n");
	if (verbose) printf("\nNetwork %2d\n    Orlist   ",k);
	for(j = 0; w1 != EOL; j++) {
	    if(j >= MaxOrList) abort("\nToo many Orlists");
	    if (verbose) printf("%d ",j);
	    for(i = 0; w1 != EOL; i++) {
		if(i >= MaxALinOL) abort("\nToo large Orlist.");
		networks[k][j][i] = (ALNumber)w1;
		fscanf(fp, "%d", &w1);
	    }
	    networks[k][j][i] = EOL;
	    freadln(fp);
	    fscanf(fp, "%d", &w1);
	}
	networks[k][j][0] = EOL;
	freadln(fp);
	fscanf(fp, "%d", &w1);
    }
    networks[k][0][0] = EOL;
    freadln(fp);
    if (verbose) printf("\n\nEnd of Data Read.\n\n");

    fclose(fp);
}

transform(p)
Primitive *p;
{
    double w1;
    int i;

    switch(p->kind) {
	case PLAIN:
	    w1 = sqrt(sqr(w[0])+sqr(w[1])+sqr(w[2]));
	    if(p->sgn == 1) {
		p->sgn = -1;
		w1 = -w1;
	    }
	    w[0] /= w1; w[1] /= w1; w[2] /= w1;
	    break;
	case SECOND:
	case CONE:
	    for(i=0; i<3; i++)
		if(w[i] != 0.0) {
		    w1 = w[i];
		    w[i] = (w1 > 0.0) ? (1.0/w1/w1) : (-1.0/w1/w1);
	    }
	    break;
	case CUBE:
	    break;
    }

    if(p->rotation) rotate(p);

    ftor(&p->param[0], w[0]);
    ftor(&p->param[1], w[1]);
    ftor(&p->param[2], w[2]);
    ftor(&p->offset[0], w[3]);
    ftor(&p->offset[1], w[4]);
    ftor(&p->offset[2], w[5]);
    ftor(&p->highlight, w[7]);
    if(p->surface >= LIGHT) {
	p->ratio.diff = (int) w[6];
	ftor(&p->color.solid[0], w[8]);
	ftor(&p->color.solid[1], w[9]);
	ftor(&p->color.solid[2], w[10]);
    } else if(p->surface != CLEAR) {
	ftor(&p->ratio.refrate, 1.0 - w[6]);
	ftor(&p->color.solid[0], w[8]);
	ftor(&p->color.solid[1], w[9]);
	ftor(&p->color.solid[2], w[10]);
    } else {
	ftor(&p->ratio.n,w[6]);
	ftor(&p->color.absorb[0], log(w[8])/log(2.0)/10.0 );
	ftor(&p->color.absorb[1], log(w[9])/log(2.0)/10.0 );
	ftor(&p->color.absorb[2], log(w[10])/log(2.0)/10.0 );
    }

}

rotate(p)
Primitive *p;
{
    double cx,cy,cz,sx,sy,sz,
	   r0,r1,r2,r3,r4,r5,r6,r7,r8;
    real *mat;

    cx = cos(w[11] * RAD);   cy = cos(w[12] * RAD);   cz = cos(w[13] * RAD);
    sx = sin(w[11] * RAD);   sy = sin(w[12] * RAD);   sz = sin(w[13] * RAD);

    r0 = cy * cz;
    r1 = sx * sy * cz  -  cx * sz;
    r2 = cx * sy * cz  +  sx * sz;
    r3 = cy * sz;
    r4 = sx * sy * sz  +  cx * cz;
    r5 = cx * sy * sz  -  sx * cz;
    r6 = - sy;
    r7 = sx * cy;
    r8 = cx * cy;

    mat = alloc(sizeof(real) * 9);
    ftor(mat+0,r0);
    ftor(mat+1,r1);
    ftor(mat+2,r2);
    ftor(mat+3,r3);
    ftor(mat+4,r4);
    ftor(mat+5,r5);
    ftor(mat+6,r6);
    ftor(mat+7,r7);
    ftor(mat+8,r8);

    p->matrix = mat;

}

#define filept(arg)	arg

lendshort(fp, n)
FILE *fp;
int n;
{
    putc(n & 0xff, filept(fp));
    putc((n>>8) & 0xff, filept(fp));
}

header(fp, wd, ht)
FILE *fp;
int wd, ht;
{
    lendshort(fp, 0x4d42);
    lendshort(fp, 0);
    lendshort(fp, 0);
    lendshort(fp, 0);
    lendshort(fp, 0);
    lendshort(fp, 0x36);
    lendshort(fp, 0);
    lendshort(fp, 40);
    lendshort(fp, 0);
    lendshort(fp, wd);
    lendshort(fp, 0);
    lendshort(fp, ht);
    lendshort(fp, 0);
    lendshort(fp, 1);
    lendshort(fp, 24);
    lendshort(fp, 0);
    lendshort(fp, 0);
    lendshort(fp, 0);
    lendshort(fp, 0);
    lendshort(fp, 0xec4);
    lendshort(fp, 0);
    lendshort(fp, 0xec4);
    lendshort(fp, 0);
    lendshort(fp, 0);
    lendshort(fp, 0);
    lendshort(fp, 0);
    lendshort(fp, 0);
}

prologue()
{
    FILE *fopen();
    char s[255];

    if (!quiet || verbose) {
	    printf("Making File %s.\n",desname);
	    printf("It requires %D bytes disk space.\n",
		   3L * (long)(xstop-xstart)*(long)(ystop-ystart)/(long)(step*step));
	    printf("Make ready and hit RETURN key.");
	    fgets(s,255,stdin);
    }
    if( (dfp = fopen(desname, "wb")) == NULL)
	abort("Can't open %s.",desname);
    header(dfp, (xstop-xstart)/step, (ystop-ystart)/step);

    ftor(&minenergy, MINENERGY);	/* マシン語ルーチンのために */
    ftor(&background, BACKGROUND);	/* 定数をセット             */
    ftor(&invbg, 1.0 - BACKGROUND);
    ftor(&grade, GRADE);

}

writescanline(py)
int py;
{
    int i;

    printf("Write scan line : %d\t\t\t\t\t\n", py);
    for(i = xstart; i < xstop; i += step) {
	putc(pblu[i], dfp);
	putc(pgrn[i], dfp);
	putc(pred[i], dfp);
    }

}

