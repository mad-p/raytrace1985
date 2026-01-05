#include "trace.h"
#include "math.h"

#define PI 3.14159265358979

static uint16_t rand(uint16_t *);

void bumptile(Primitive *p, vector *dir, vector *vp, vector *pnt)
{
    real oct = {-3,0,0x8000};           /* 1/8 */
    real w1, w2;
    uint16_t i1, i2, i3;

    vector lc = *pnt;

    fscale(&lc.x, -4);
    fscale(&lc.z, -4);
    i1 = rtoif(&lc.x);
    i2 = rtoif(&lc.z);
    i3 = rand(&i1) * 337 + rand(&i2);
    w1.man = (rand(&i3) | 0x8000) & 0xffff;
    w2.man = (rand(&i3) | 0x8000) & 0xffff;
    w1.exp = w2.exp = -3;
    w1.sign = w2.sign = 0;
    fsub(&w1, &oct);
    fsub(&w2, &oct);
    w1.sign = (rand(&i3) & 0x4000) == 0;
    w2.sign = (rand(&i3) & 0x4000) == 0;
    fadd(&nml.x, &w1);
    fadd(&nml.z, &w2);
    nrmlize(&nml);
}

static uint16_t rand(uint16_t *x)
{
    int i;
    for (i=0; i<7; i++)
            *x = *x * 8009 + 31957;
    return *x;
}

typedef struct {
    vector center;
    real   radius;
    real   mag;
    real   phase;
} wave;

static wave waves[10];
int wave_read = 0;
int nwave = 0;

void readwav(const char *filename) {
    char buf[BUFSIZ];
    if (nwave > 0) return;
    FILE *fp;

    if ((fp = fopen(filename, "r")) != NULL) {
        while (fgets(buf, BUFSIZ, fp) != NULL) {
            double x,y,z,r,m,p;
            sscanf(buf, "%lf %lf %lf  %lf  %lf  %lf", &x,&y,&z,&r,&m,&p);
            wave *w = &waves[nwave++];
            ftor(&w->center.x, x);
            ftor(&w->center.y, y);
            ftor(&w->center.z, z);
            ftor(&w->radius, r);
            ftor(&w->mag, m);
            ftor(&w->phase, p);
        }
        fclose(fp);
    }
}

void ripple(Primitive *p, vector *dir, vector *vp, vector *pnt)
{
    if (!wave_read) {
        readwav(extra_filename1);
        wave_read = 1;
    }
    for (int i=0; i<nwave; i++) {
        vector disp, disp2;
        real dist, phase;
        disp = *pnt;
        fsub(&disp.x, &waves[i].center.x);
        fsub(&disp.y, &waves[i].center.y);
        fsub(&disp.z, &waves[i].center.z);
        disp2 = disp;
        fsqar(&disp2.x);
        fsqar(&disp2.y);
        fsqar(&disp2.z);
        dist = disp2.x;
        fadd(&dist, &disp2.y);
        fadd(&dist, &disp2.z);
        fsqrt(&dist);
        phase = dist;
        fdiv(&phase, &waves[i].radius);
        fsub(&phase, &waves[i].phase);

        double dphase = rtof(&phase);
        dphase = fmod(dphase, 1.0);
        dphase *= 2 * PI;
        double dmag = sin(dphase);
        real mag;
        ftor(&mag, dmag);
        fmlt(&mag, &waves[i].mag);
        nrmlize(&disp);
        fmlt(&disp.x, &mag);
        fmlt(&disp.y, &mag);
        fmlt(&disp.z, &mag);
        fadd(&nml.x, &disp.x);
        fadd(&nml.y, &disp.y);
        fadd(&nml.z, &disp.z);
    }
    nrmlize(&nml);
}

void grade1(Primitive *p, vector *dir, vector *vp, vector *pnt)
{
    vector w;
    extern real const1;

    static real const255 = {7,0,0xff00};
    real Half = {10,0,0x8000};
    ftor(&Half, 30.0*30.0);

    w = *pnt;
    fsqar(&w.x);
    fsqar(&w.y);
    fsqar(&w.z);
    fadd(&w.x, &w.y);
    fadd(&w.x, &w.z);
    fdiv(&w.x, &Half);
    fadd(&w.x, &const1);
    finv(&w.x);
    fmlt(&w.x, &const255);
    p->solid.y = w.x;
    fsub(&w.x, &const255);
    w.x.sign ^= 1;
    p->solid.z = w.x;
}

mapfunc usertext[MAXUTEX] = {grade1};
mapfunc bumping[MAXBUMP] = {bumptile, ripple};
