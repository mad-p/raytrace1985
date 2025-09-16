/*

	standard texture mapping

*/


#define	EXTERN extern
#include "trace.h"
#include <math.h>

#define PI 3.14159265358979
#define sqr(x) ((x)*(x))
#define Max 255.0

void stripe(Primitive *p, vector *dir, vector *vp, vector *lc)
{
    double w1,w2;

    w1 = sin(w2 = rtof(&lc->y)/4.0);
    w2 = cos(w2);
    ftor(&p->solid.x,Max*sqr(w1));
    ftor(&p->solid.y,Max*sqr(w2));

}

void ring(Primitive *p, vector *dir, vector *vp, vector *lc)
{
    double w1, w2;

    w1 = rtof(&lc->x);  w2 = rtof(&lc->z);
    w1 = sqrt(sqr(w1)+sqr(w2)) / 10.0;
    w2 = PI * modf(w1, &w2);		/* &w2 is dummy */
    w1 = cos(w2);  w2 = sin(w2);
    ftor(&p->solid.x,Max*sqr(w1));
    ftor(&p->solid.y,Max*sqr(w2));

}

void dot(Primitive *p, vector *dir, vector *vp, vector *lc)
{
    double w0, w1, w2, w3, w4, w5;

    w0 = rtof(&lc->x);
    w1 = rtof(&lc->y);
    w2 = rtof(&lc->z);
    w0 *= sqrt(rtof(&p->param.x));
    w1 *= sqrt(rtof(&p->param.y));
    w2 *= sqrt(rtof(&p->param.z));
    w3 = sqrt(sqr(w0)+sqr(w2));
    w4 = 1e+15;   if(fabs(w0)>.0001) w4 = fabs(w2/w0);
    w5 = 1e+15;   if(fabs(w3)>.0001) w5 = fabs(w1/w3);
    w4 = atan(w4)*(30.0/PI);	w5 = atan(w5)*(30.0/PI);
    w4 = modf(w4, &w0);		w5 = modf(w5, &w0);	/* &w0 is dummy */
    if((w5 = .15 - sqr(.5 - w4) - sqr(.5 - w5)) < 0.0) w5 = 0.0;
    ftor(&p->solid.y, ((double)MAXLEVEL/.3)*w5);

}
