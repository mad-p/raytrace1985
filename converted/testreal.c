#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "reala.h"

uint64_t pass = 0;
uint64_t fail = 0;
double max_err = 0;
#define MAX_ERR 6e-5 // 2^-14

int test_value(double expected, double actual, char *message) {
    int ok = 0;
    if (!isfinite(expected)) {
        if (expected < 0) {
            // minus infinity
            if (actual == -340277174624079928635746076935438991360.0) {
                ok = 1;
            } else {
                ok = 0;
            }
        } else {
            // plus infinity
            if (actual == 340277174624079928635746076935438991360.0) {
                ok = 1;
            } else {
                ok = 0;
            }
        }
    } else if (fabs(expected) < MAX_ERR) {
        if (fabs(actual) < MAX_ERR) {
            ok = 1;
        } else {
            ok = 0;
        }
        if (fabs(actual) > max_err) {
            max_err = fabs(actual);
        }
    } else {
        double ratio = fabs(actual) / fabs(expected) - 1.0;
        if (fabs(ratio) < MAX_ERR) {
            ok = 1;
        } else {
            ok = 0;
        }
        if (fabs(ratio) > max_err) {
            max_err = fabs(ratio);
        }
    }
    if (ok) {
        pass++;
    } else {
        fail++;
        printf("%s: failed: expected %f, actual: %f\n", message, expected, actual);
    }
    return ok;
}

int test_comp(double a, double b) {
    int expected;
    if (a == b) {
        expected = 0;
    } else if (a < b) {
        expected = -1;
    } else {
        expected = 1;
    }
    Real ra, rb;
    ftor(&ra, a);
    ftor(&rb, b);
    int r = fcomp(&ra, &rb);
    int ret;
    if (r == expected) {
        pass++;
        ret = 1;
    } else {
        fail++;
        printf("fcomp(%f, %f) failed: expected %d, actual %d\n", a, b, expected, r);
        ret = 0;
    }
    return ret;
}

int test_binary(double a, double b,
                void (*func)(Real *dst, const Real *src),
                double expected, char *message) {
    Real ra, rb;
    ftor(&ra, a);
    ftor(&rb, b);
    (*func)(&ra, &rb);
    double r = rtof(&ra);
    return test_value(expected, r, message);
}

int test_unary(double a,
               void (*func)(Real *dst),
                double expected, char *message) {
    Real ra;
    ftor(&ra, a);
    (*func)(&ra);
    double r = rtof(&ra);
    return test_value(expected, r, message);
}

int main() {
    char message[BUFSIZ];

    test_binary(1.0, 1.0, fadd, 2.0, "1 + 1 == 2.0");
    test_binary(14.0, 16.0, fmlt, 224.0, "14 + 16 == 224");
    test_unary(3, fexp, 8.0, "exp2(3) == 8.0");
    test_unary(-3, fexp, 0.125, "exp2(-3) == 0.125");
    test_unary(32, flog, 5.0, "log2(32) == 5.0");
    test_unary(0.125, flog, -3.0, "log2(0.125) == -3.0");
    test_unary(0.125, finv, 8.0, "1/0.125 == 8.0");
    test_unary(-0.125, finv, 8.0, "1/-0.125 == -8.0");
    test_unary(8, finv, 0.125, "1/8 == 0.125");
    test_comp(-20, 20);
    test_comp(1.0001, 1);
    test_comp(0, 0);
    test_comp(2, 2);
    test_comp(8, 0.125);
    test_comp(-1.0001, -1);
    test_comp(0, 0);
    test_comp(-2, -2);
    test_comp(-8, -0.125);
    test_comp(-4, 0.125);
    test_comp(-432, 32);
    
    int a, b;
#define MAG 32768
    for (a=0; a<MAG; a++) {
        for (int e=-15; e<15; e++) {
            for (int sign = 0; sign < 2; sign++) {
                Real x = {e, sign, (a & 0xffff) | 0x8000};
                double xd = rtof(&x);
                double fd = floor(xd);
                int fi = rtoif(&x);
                if ((int)fd == fi) {
                    pass++;
                } else {
                    fail++;
                    printf("x=%lf; true floor=%d, rtoif=%d\n", xd, (int)fd, fi);
                }
            }
        }
        /*
        for (b=-32768; b<32768; b++) {
            sprintf(message, "%d + %d == %d", a, b, a+b);
            test_binary((double)a, (double)b, fadd, (double)(a+b), message);
            sprintf(message, "%d - %d == %d", a, b, a-b);
            test_binary((double)a, (double)b, fsub, (double)(a-b), message);
            sprintf(message, "%d * %d == %d", a, b, a*b);
            test_binary((double)a, (double)b, fmlt, (double)(a*b), message);
            if (b != 0) {
                sprintf(message, "%d / %d == %d", a, b, a/b);
                test_binary((double)a, (double)b, fdiv, ((double)a)/((double)b), message);
            }
        }
        */
        /*
        sprintf(message, "%d ** 2 == %d", a, a*a);
        test_unary((double)a, fsqar, (double)(a*a), message);
        if (a >= 0) {
            sprintf(message, "sqrt(%d) == %f", a, sqrt((double)a));
            test_unary((double)a, fsqrt, sqrt((double)a), message);
            sprintf(message, "log(%d) == %f", a, log2((double)a));
            test_unary((double)a, flog, log2((double)a), message);
        }
        if (-128 <= a && a <= 127) {
            sprintf(message, "exp(%d) == %f", a, exp2((double)a));
            test_unary((double)a, fexp, exp2((double)a), message);
        }
        if (a != 0) {
            sprintf(message, "1/%d == %f", a, 1.0/(double)a);
            test_unary((double)a, finv, 1.0/(double)a, message);
        }
        */
    }

    // add more tests here

    printf("%lld passed, %lld failed\n", pass, fail);
    printf("max error ratio = %.6e\n", max_err);

    exit(0);
}
