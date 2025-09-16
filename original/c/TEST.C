#include "stdio2.h"

typedef double vector[3];


vector *outerproduct(a, b, c)
vector *a, *b, *c;
{

	(*c)[0] = (*a)[1] * (*b)[2]  -  (*a)[2] * (*b)[1];
	(*c)[1] = (*a)[2] * (*b)[0]  -  (*a)[0] * (*b)[2];
	(*c)[2] = (*a)[0] * (*b)[1]  -  (*a)[1] * (*b)[0];

	return c;

}

double innerproduct(a,b)
vector *a, *b;
{

	return (
		 (*a)[0] * (*b)[0]  +
		 (*a)[1] * (*b)[1]  +
		 (*a)[2] * (*b)[2]  +
					0 ) ;

}

typedef vector matrix[3];

static char t1[] = "@@@";


matrix *product(a, b, c)
matrix *a, *b, *c;
{

	(*c)[0][0]  =
			(*a)[0][0] * (*b)[0][0]    +
			(*a)[0][1] * (*b)[1][0]    +
			(*a)[0][2] * (*b)[2][0]    +
							0.0 ;

/* 000 @@@ 000 pause atmark */
/* 000 @@@ 111 pause atmark */
/* 000 @@@ 222 pause atmark */
/* %%% @@@ 000 pause atmark and percent */
/* %%% @@@ 111 pause atmark and percent */
/* %%% @@@ 222 pause atmark and percent */
/* %%% 000 000 pause percent */
/* %%% 000 111 pause percent */
/* %%% 000 222 pause percent */
/* %%% 000 &&& pause percent and ampersand */
/* %%% 000 000 pause percent */
/* %%% 000 111 pause percent */
/* %%% 000 222 pause percent */
/* 000 000 &&& noexpand ampersand */

	(*c)[0][1]  =
			(*a)[0][0] * (*b)[0][1]    +
			(*a)[0][1] * (*b)[1][1]    +
			(*a)[0][2] * (*b)[2][1]    +
							0.0 ;

/* 000 @@@ 000 pause atmark */
/* 000 @@@ 111 pause atmark */
/* 000 @@@ 222 pause atmark */
/* %%% @@@ 000 pause atmark and percent */
/* %%% @@@ 111 pause atmark and percent */
/* %%% @@@ 222 pause atmark and percent */
/* %%% 111 000 pause percent */
/* %%% 111 111 pause percent */
/* %%% 111 222 pause percent */
/* %%% 111 &&& pause percent and ampersand */
/* %%% 111 000 pause percent */
/* %%% 111 111 pause percent */
/* %%% 111 222 pause percent */
/* 000 111 &&& noexpand ampersand */

	(*c)[0][2]  =
			(*a)[0][0] * (*b)[0][2]    +
			(*a)[0][1] * (*b)[1][2]    +
			(*a)[0][2] * (*b)[2][2]    +
							0.0 ;

/* 000 @@@ 000 pause atmark */
/* 000 @@@ 111 pause atmark */
/* 000 @@@ 222 pause atmark */
/* %%% @@@ 000 pause atmark and percent */
/* %%% @@@ 111 pause atmark and percent */
/* %%% @@@ 222 pause atmark and percent */
/* %%% 222 000 pause percent */
/* %%% 222 111 pause percent */
/* %%% 222 222 pause percent */
/* %%% 222 &&& pause percent and ampersand */
/* %%% 222 000 pause percent */
/* %%% 222 111 pause percent */
/* %%% 222 222 pause percent */
/* 000 222 &&& noexpand ampersand */
/* 000 @@@ &&& noexpand ampersand and atmark */

	(*c)[1][0]  =
			(*a)[1][0] * (*b)[0][0]    +
			(*a)[1][1] * (*b)[1][0]    +
			(*a)[1][2] * (*b)[2][0]    +
							0.0 ;

/* 111 @@@ 000 pause atmark */
/* 111 @@@ 111 pause atmark */
/* 111 @@@ 222 pause atmark */
/* %%% @@@ 000 pause atmark and percent */
/* %%% @@@ 111 pause atmark and percent */
/* %%% @@@ 222 pause atmark and percent */
/* %%% 000 000 pause percent */
/* %%% 000 111 pause percent */
/* %%% 000 222 pause percent */
/* %%% 000 &&& pause percent and ampersand */
/* %%% 000 000 pause percent */
/* %%% 000 111 pause percent */
/* %%% 000 222 pause percent */
/* 111 000 &&& noexpand ampersand */

	(*c)[1][1]  =
			(*a)[1][0] * (*b)[0][1]    +
			(*a)[1][1] * (*b)[1][1]    +
			(*a)[1][2] * (*b)[2][1]    +
							0.0 ;

/* 111 @@@ 000 pause atmark */
/* 111 @@@ 111 pause atmark */
/* 111 @@@ 222 pause atmark */
/* %%% @@@ 000 pause atmark and percent */
/* %%% @@@ 111 pause atmark and percent */
/* %%% @@@ 222 pause atmark and percent */
/* %%% 111 000 pause percent */
/* %%% 111 111 pause percent */
/* %%% 111 222 pause percent */
/* %%% 111 &&& pause percent and ampersand */
/* %%% 111 000 pause percent */
/* %%% 111 111 pause percent */
/* %%% 111 222 pause percent */
/* 111 111 &&& noexpand ampersand */

	(*c)[1][2]  =
			(*a)[1][0] * (*b)[0][2]    +
			(*a)[1][1] * (*b)[1][2]    +
			(*a)[1][2] * (*b)[2][2]    +
							0.0 ;

/* 111 @@@ 000 pause atmark */
/* 111 @@@ 111 pause atmark */
/* 111 @@@ 222 pause atmark */
/* %%% @@@ 000 pause atmark and percent */
/* %%% @@@ 111 pause atmark and percent */
/* %%% @@@ 222 pause atmark and percent */
/* %%% 222 000 pause percent */
/* %%% 222 111 pause percent */
/* %%% 222 222 pause percent */
/* %%% 222 &&& pause percent and ampersand */
/* %%% 222 000 pause percent */
/* %%% 222 111 pause percent */
/* %%% 222 222 pause percent */
/* 111 222 &&& noexpand ampersand */
/* 111 @@@ &&& noexpand ampersand and atmark */

	(*c)[2][0]  =
			(*a)[2][0] * (*b)[0][0]    +
			(*a)[2][1] * (*b)[1][0]    +
			(*a)[2][2] * (*b)[2][0]    +
							0.0 ;

/* 222 @@@ 000 pause atmark */
/* 222 @@@ 111 pause atmark */
/* 222 @@@ 222 pause atmark */
/* %%% @@@ 000 pause atmark and percent */
/* %%% @@@ 111 pause atmark and percent */
/* %%% @@@ 222 pause atmark and percent */
/* %%% 000 000 pause percent */
/* %%% 000 111 pause percent */
/* %%% 000 222 pause percent */
/* %%% 000 &&& pause percent and ampersand */
/* %%% 000 000 pause percent */
/* %%% 000 111 pause percent */
/* %%% 000 222 pause percent */
/* 222 000 &&& noexpand ampersand */

	(*c)[2][1]  =
			(*a)[2][0] * (*b)[0][1]    +
			(*a)[2][1] * (*b)[1][1]    +
			(*a)[2][2] * (*b)[2][1]    +
							0.0 ;

/* 222 @@@ 000 pause atmark */
/* 222 @@@ 111 pause atmark */
/* 222 @@@ 222 pause atmark */
/* %%% @@@ 000 pause atmark and percent */
/* %%% @@@ 111 pause atmark and percent */
/* %%% @@@ 222 pause atmark and percent */
/* %%% 111 000 pause percent */
/* %%% 111 111 pause percent */
/* %%% 111 222 pause percent */
/* %%% 111 &&& pause percent and ampersand */
/* %%% 111 000 pause percent */
/* %%% 111 111 pause percent */
/* %%% 111 222 pause percent */
/* 222 111 &&& noexpand ampersand */

	(*c)[2][2]  =
			(*a)[2][0] * (*b)[0][2]    +
			(*a)[2][1] * (*b)[1][2]    +
			(*a)[2][2] * (*b)[2][2]    +
							0.0 ;

/* 222 @@@ 000 pause atmark */
/* 222 @@@ 111 pause atmark */
/* 222 @@@ 222 pause atmark */
/* %%% @@@ 000 pause atmark and percent */
/* %%% @@@ 111 pause atmark and percent */
/* %%% @@@ 222 pause atmark and percent */
/* %%% 222 000 pause percent */
/* %%% 222 111 pause percent */
/* %%% 222 222 pause percent */
/* %%% 222 &&& pause percent and ampersand */
/* %%% 222 000 pause percent */
/* %%% 222 111 pause percent */
/* %%% 222 222 pause percent */
/* 222 222 &&& noexpand ampersand */
/* 222 @@@ &&& noexpand ampersand and atmark */

	return c;

}


#if 1 $ 1 == 0
matrix *msub(a, b, c)
#else
matrix *madd(a, b, c)
#endif
matrix *a, *b, *c;
{
	(*c)[0][0] = (*a)[0][0] + (*b)[0][0];
	(*c)[0][1] = (*a)[0][1] + (*b)[0][1];
	(*c)[0][2] = (*a)[0][2] + (*b)[0][2];
	(*c)[1][0] = (*a)[1][0] + (*b)[1][0];
	(*c)[1][1] = (*a)[1][1] + (*b)[1][1];
	(*c)[1][2] = (*a)[1][2] + (*b)[1][2];
	(*c)[2][0] = (*a)[2][0] + (*b)[2][0];
	(*c)[2][1] = (*a)[2][1] + (*b)[2][1];
	(*c)[2][2] = (*a)[2][2] + (*b)[2][2];

	return c;
}

#if 1 $ 1 == 0
matrix *msub(a, b, c)
#else
matrix *madd(a, b, c)
#endif
matrix *a, *b, *c;
{
	(*c)[0][0] = (*a)[0][0] - (*b)[0][0];
	(*c)[0][1] = (*a)[0][1] - (*b)[0][1];
	(*c)[0][2] = (*a)[0][2] - (*b)[0][2];
	(*c)[1][0] = (*a)[1][0] - (*b)[1][0];
	(*c)[1][1] = (*a)[1][1] - (*b)[1][1];
	(*c)[1][2] = (*a)[1][2] - (*b)[1][2];
	(*c)[2][0] = (*a)[2][0] - (*b)[2][0];
	(*c)[2][1] = (*a)[2][1] - (*b)[2][1];
	(*c)[2][2] = (*a)[2][2] - (*b)[2][2];

	return c;
}


matrix *mlin(a, k)
matrix *a;
double k;
{
	(*a)[0][0] *= k;
	(*a)[0][1] *= k;
	(*a)[0][2] *= k;
	(*a)[1][0] *= k;
	(*a)[1][1] *= k;
	(*a)[1][2] *= k;
	(*a)[2][0] *= k;
	(*a)[2][1] *= k;
	(*a)[2][2] *= k;

	return a;
}


static char t2[] = "\212\277\216\232\211\274\226\274\215\254\202\266\202\350\225\266\202\314\203e\203X\203g";
/* ‚±‚ê‚Í•ÏŠ·‚³‚ê‚È‚¢ */
static char t3[] = "\202\261\202\352\202\315\225\317\212\267\202\263\202\352\202\351";
/* ‚±‚ê‚à•ÏŠ·‚³‚ê‚È‚¢ */

teststrip()
{
	/* test for strip test"strip1 test"strip2 test'strip1 test'strip2 */
	/* test for strip test"strip2 test'strip1 test'strip2 test"strip1 */
	/* test for strip test'strip1 test'strip2 test"strip1 test"strip2 */
	/* test for strip test'strip2 test"strip1 test"strip2 test'strip1 */

	printf("this is test for divisionbunkatsu\
	dekitaOk");
	printf("test for octal number 0123456789 00+0-0");
	printf("test for octal number 1234567890 11+1-1");
	printf("test for octal number 2345678901 22+2-2");
	printf("test for octal number 3456789012 33+3-3");
	printf("test for octal number 4567890123 44+4-4");
	printf("test for octal number 5678901234 55+5-5");
	printf("test for octal number 6789012345 66+6-6");
	printf("test for octal number 7890123456 77+7-7");
	printf("test for octal number 8901234567 88+8-8");
	printf("test for octal number 9012345678 99+9-9");
}

	a.X.X = b.Y*c.Z;
	a.Y.Y = b.Z*c.X;
	a.Z.Z = b.X*c.Y;
	a.X.Y = b.Y*c.Z;
	a.Y.Z = b.Z*c.X;
	a.Z.X = b.X*c.Y;
	a.X.Z = b.Y*c.Z;
	a.Y.X = b.Z*c.X;
	a.Z.Y = b.X*c.Y;

