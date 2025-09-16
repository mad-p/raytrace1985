/*********************************************************
        						  
       raytracing rendering system based on C.G.A.
        						  
	      R A Y F O R C E  version 1.0
        						  
	   copyright (C) 1987  by Kaoru Maeda
                                                          
	%Z%  %M% %I%  %E% %U%
        						  
 *********************************************************/
/*****	this file contains following built-in operators  *****
DEFSUBR("writeheader",	wrheader)
DEFSUBR("writescanline",	wrscline)
DEFSUBR("writetailer",	wrtailer)
DEFSUBR("writebmpheader",	wrbmpheader)
DEFSUBR("writebmpscanline",	wrbmpscline)
DEFSUBR("writexvscanline",	wrxvscline)
 *****  ----- end of operator listing -----  *****/

#include "ray.h"
#include "forth.h"
#include "trace.h"

void writeshort(n)
    int n;
{
    putc((n>>8) & 0xff, filept(outfile));
    putc(n & 0xff, filept(outfile));
}

void wrheader()
{
    writeshort(0x9f1a);
    writeshort(devsiz[0]);
    writeshort(devsiz[1]);
    writeshort(0);
    putc(1, filept(outfile));
    putc(24, filept(outfile));		/* 24bits/pixel */
    fflush(filept(outfile));
}

#define ypix	pixind[1]

void wrscline()
{
    register int n, c;
    register real *x;

    x = pixbuf;
    n = devsiz[0]*3;
    while(n--) {
	c = (int)floor( colreso * (*x++));
	if(c > colreso)
	  c = colreso;
	putc(c, filept(outfile));
    }
    printf("scanline %d (%d%%) \r", (int)ypix, 100*(int)ypix/devsiz[1]);
    fflush(stdout);
}

void wrtailer()
{
}

/* Windows bitmap file 24bit color */

static void lendshort(n)
    int n;
{
    putc(n & 0xff, filept(outfile));
    putc((n>>8) & 0xff, filept(outfile));
}

static void lendlong(n)
    int n;
{
    putc(n & 0xff, filept(outfile));
    putc((n>>8) & 0xff, filept(outfile));
    putc((n>>16) & 0xff, filept(outfile));
    putc((n>>24) & 0xff, filept(outfile));
}

void wrbmpheader()
{
    lendshort(0x4d42);
    lendlong(0);
    lendlong(0);
    lendlong(0x36);
    lendlong(40);
    lendlong(devsiz[0]);
    lendlong(devsiz[1]);
    lendshort(1);
    lendshort(24);
    lendlong(0);
    lendlong(0);
    lendlong(0xec4);
    lendlong(0xec4);
    lendlong(0);
    lendlong(0);
}

void wrbmpscline()
{
    register int n, r, g, b;
    register real *x;

    x = pixbuf;
    n = devsiz[0];
    while(n--) {
	r = (int)floor( colreso * (*x++));
	if(r > colreso) r = colreso;
	g = (int)floor( colreso * (*x++));
	if(g > colreso) g = colreso;
	b = (int)floor( colreso * (*x++));
	if(b > colreso) b = colreso;
	putc(b, filept(outfile));
	putc(r, filept(outfile));
	putc(g, filept(outfile));
    }
    printf("scanline %d (%d%%) \r", (int)ypix, 100*(int)ypix/devsiz[1]);
    fflush(stdout);
}

void wrxvscline()
{
    register int n, r, g, b;
    register real *x;

    x = pixbuf;
    n = devsiz[0];
    while(n--) {
	r = (int)floor( colreso * (*x++));
	if(r > colreso) r = colreso;
	g = (int)floor( colreso * (*x++));
	if(g > colreso) g = colreso;
	b = (int)floor( colreso * (*x++));
	if(b > colreso) b = colreso;
	putc(b, filept(outfile));
	putc(g, filept(outfile));
	putc(r, filept(outfile));
    }
    printf("scanline %d (%d%%) \r", (int)ypix, 100*(int)ypix/devsiz[1]);
    fflush(stdout);
}
