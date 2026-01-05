#include "trace.h"
extern int verbose;
                                            //  PAGE    66,132
                                            //  TITLE   *** ray4.asm   raw calculating functions ***
//
//
//      Machine language functions
//
//          For Ray-Tracing
//
//              Aug/1985
//              Oct/1985
//
//          by  Kaoru.Maeda
//
//
                                            //
                                            //  include def.ah
                                            //
                                            //  .SALL
                                            //
                                            // @DATAU   SEGMENT
                                            //
                                            // FAC0     db      (type REAL) dup (?)
                                            // FAC1     db      (type REAL) dup (?)
                                            // FAC2     db      (type REAL) dup (?)
                                            // FAC3     db      (type REAL) dup (?)
                                            // FAC4     db      (type REAL) dup (?)
                                            // FAC5     db      (type REAL) dup (?)
                                            // FAC6     db      (type REAL) dup (?)
                                            // FAC7     db      (type REAL) dup (?)
                                            // FAC8     db      (type REAL) dup (?)
                                            // FAC9     db      (type REAL) dup (?)
                                            // FAC10    db      (type REAL) dup (?)
                                            // FAC11    db      (type REAL) dup (?)
                                            // FAC12    db      (type REAL) dup (?)
                                            // FAC13    db      (type REAL) dup (?)
                                            // FAC14    db      (type REAL) dup (?)
                                            //
                                            // @DATAU   ENDS
                                            //
                                            // @CODE    SEGMENT
                                            //
//
//      Normalize a Vector
//
//      Args    si : destination
//      Ret     carry if ZERO
//
                                            //
                                            //  public  NRMLIZE
                                            //
/*
 * NRMLIZE
 */

int nrmlize(vector *dest) {
int ax, cond;
real fac12;
 vector fac10;

                                            // NRMLIZE  PROC
                                            //
                                            //  link
    fac10.x = dest->x; fac10.y = dest->y; fac10.z = dest->z;
                                            //  MOVMEM  FAC10,[si],<type VECTOR>
    fsqar(&fac10.x);                        //  FPSQAR  FAC10.X
    fsqar(&fac10.y);                        //  FPSQAR  FAC10.Y
    fsqar(&fac10.z);                        //  FPSQAR  FAC10.Z
    fac12 = fac10.x;
    fadd(&fac12, &fac10.y);                 //  FPADD   FAC12,FAC10
    fadd(&fac12, &fac10.z);                 //  FPADD   FAC12,FAC11
    fsqrt(&fac12);                          //  FPSQRT  FAC12
    cond = (((fac12).man & 0x8000) == 0);   //  FP0?    FAC12
    if (cond) {goto nrm1;}                  //  jz      NRM1
    fdiv(&dest->x, &fac12);                 //  FPDIV   [si].X,FAC12
    fdiv(&dest->y, &fac12);                 //  FPDIV   [si].Y,FAC12
    fdiv(&dest->z, &fac12);                 //  FPDIV   [si].Z,FAC12
    ax = 0;                                 //  clc
  nrm0:
                                            // NRM0:    unlk
    goto endp;                              //  ret
  nrm1:
    dest->x.man = 0x8000;                   // NRM1:    mov     [si].X.MAN,8000h
    ax = 0;                                 //  xor     ax,ax
    dest->y.man = ax;                       //  mov     [si].Y.MAN,ax
    dest->z.man = ax;                       //  mov     [si].Z.MAN,ax
    dest->x.exp = ax;                       //  mov     word ptr [si].X.EXP,ax
    dest->y.exp = ax;                       //  mov     word ptr [si].Y.EXP,ax
    dest->z.exp = ax;                       //  mov     word ptr [si].Z.EXP,ax
    ax = 1;                                 //  stc
    goto nrm0;                              //  jmp     NRM0
                                            //
                                            // NRMLIZE  ENDP
  endp:;
    return ax;
}


                                            //
//
//      Linear calculation
//
//      Args    si : vector dir,point
//              di : real   distance
//              bx : destination
//
                                            //
                                            //  public  LINEAR
                                            //
/*
 * LINEAR
 */

void linear(vector *dir, vector *pnt, real *dist, vector *dest) {

    if ((verbose & 1)) {
        fprintvector(stderr, "linear\n  dir  ", dir);
        fprintvector(stderr, "  point", pnt);
        fprintreal(stderr, "  dist ", dist);
        fflush(stderr);
    }

                                            // LINEAR   PROC
                                            //
                                            //  push    di
    *dest = *dir;
                                            //  MOVMEM  [bx],[si],<type VECTOR>
                                            //  pop     di
                                            //  xchg    si,di
                                            //  push    di
    fmlt(&dest->x, dist);                   //  FPMLT   [bx].X,[si]
    fmlt(&dest->y, dist);                   //  FPMLT   [bx].Y,[si]
    fmlt(&dest->z, dist);                   //  FPMLT   [bx].Z,[si]
                                            //  pop     di
                                            //  xchg    si,di
    fadd(&dest->x, &pnt->x);                //  FPADD   [bx].X,<[si+type VECTOR].X>
    fadd(&dest->y, &pnt->y);                //  FPADD   [bx].Y,<[si+type VECTOR].Y>
    fadd(&dest->z, &pnt->z);                //  FPADD   [bx].Z,<[si+type VECTOR].Z>
    goto endp;                              //  ret
                                            //
                                            // LINEAR   ENDP
  endp:;
    if ((verbose & 1)) {
        fprintvector(stderr, "  result", dest);
        fflush(stderr);
    }
}


                                            //
//
//      Rotation Support
//
//      bx : Primitive *
//      si : destination
//
//
                                            //  public  ROT
                                            //  public  ROT2
                                            //
/*
 * ROT
 */

void rot(Primitive *p, vector *dest) {
 int cond;
 real matrix[9];

    if ((verbose & 1)) {
        fprintf(stderr, "rot: prim=%p\n", (void*)p); fflush(stderr);
        fprintvector(stderr, "  dest", dest);
        fflush(stderr);
    }

                                            // ROT      PROC
                                            //
    cond = ((p->rotation) == (0));          //  cmp     [bx].Rotation,0
    if (cond) {goto rot19;}                 //  njz     ROT19
                                            //  link    36
                                            //  push    bx
                                            //  mov     bx,[bx].Matrix
    memcpy(matrix, p->matrix, 9 * sizeof(real));
                                            //  MOVMEM  [bp-36],[bx],36
    fmlt(&matrix[0], &dest->x);             //  FPMLT   [bp-36].X,[si].X
    fmlt(&matrix[1], &dest->y);             //  FPMLT   [bp-36].Y,[si].Y
    fmlt(&matrix[2], &dest->z);             //  FPMLT   [bp-36].Z,[si].Z
    fmlt(&matrix[3], &dest->x);             //  FPMLT   [bp-24].X,[si].X
    fmlt(&matrix[4], &dest->y);             //  FPMLT   [bp-24].Y,[si].Y
    fmlt(&matrix[5], &dest->z);             //  FPMLT   [bp-24].Z,[si].Z
    fmlt(&matrix[6], &dest->x);             //  FPMLT   [bp-12].X,[si].X
    fmlt(&matrix[7], &dest->y);             //  FPMLT   [bp-12].Y,[si].Y
    fmlt(&matrix[8], &dest->z);             //  FPMLT   [bp-12].Z,[si].Z
    fadd(&matrix[0], &matrix[1]);           //  FPADD   [bp-36],[bp-36].Y
    fadd(&matrix[0], &matrix[2]);           //  FPADD   [bp-36],[bp-36].Z
    fadd(&matrix[3], &matrix[4]);           //  FPADD   [bp-24],[bp-24].Y
    fadd(&matrix[3], &matrix[5]);           //  FPADD   [bp-24],[bp-24].Z
    fadd(&matrix[6], &matrix[7]);           //  FPADD   [bp-12],[bp-12].Y
    fadd(&matrix[6], &matrix[8]);           //  FPADD   [bp-12],[bp-12].Z
    dest->x = matrix[0];                    //  MOVMEM  [si].X,[bp-36]
    dest->y = matrix[3];                    //  MOVMEM  [si].Y,[bp-24]
    dest->z = matrix[6];                    //  MOVMEM  [si].Z,[bp-12]
                                            //  pop     bx
                                            //  unlk
  rot19:
    goto endp;                              // ROT19:   ret
                                            //
                                            // ROT      ENDP
  endp:;
    if ((verbose & 1)) {
        fprintvector(stderr, "  result", dest);
        fflush(stderr);
    }
}


                                            //
/*
 * ROT2
 */

void rot2(Primitive *p, vector *dest) {
int cond;
 real matrix[9];

                                            // ROT2     PROC
                                            //
    cond = ((p->rotation) == (0));          //  cmp     [bx].Rotation,0
    if (cond) {goto rot29;}                 //  njz     ROT29
                                            //  link    36
                                            //  push    bx
                                            //  mov     bx,[bx].Matrix
    memcpy(matrix, p->matrix, 9 * sizeof(real));
                                            //  MOVMEM  [bp-36],[bx],36
    fmlt(&matrix[0], &dest->x);             //  FPMLT   [bp-36].X,[si].X
    fmlt(&matrix[3], &dest->y);             //  FPMLT   [bp-24].X,[si].Y
    fmlt(&matrix[6], &dest->z);             //  FPMLT   [bp-12].X,[si].Z
    fmlt(&matrix[1], &dest->x);             //  FPMLT   [bp-36].Y,[si].X
    fmlt(&matrix[4], &dest->y);             //  FPMLT   [bp-24].Y,[si].Y
    fmlt(&matrix[7], &dest->z);             //  FPMLT   [bp-12].Y,[si].Z
    fmlt(&matrix[2], &dest->x);             //  FPMLT   [bp-36].Z,[si].X
    fmlt(&matrix[5], &dest->y);             //  FPMLT   [bp-24].Z,[si].Y
    fmlt(&matrix[8], &dest->z);             //  FPMLT   [bp-12].Z,[si].Z
    fadd(&matrix[0], &matrix[3]);           //  FPADD   [bp-36].X,[bp-24].X
    fadd(&matrix[0], &matrix[6]);           //  FPADD   [bp-36].X,[bp-12].X
    fadd(&matrix[1], &matrix[4]);           //  FPADD   [bp-36].Y,[bp-24].Y
    fadd(&matrix[1], &matrix[7]);           //  FPADD   [bp-36].Y,[bp-12].Y
    fadd(&matrix[2], &matrix[5]);           //  FPADD   [bp-36].Z,[bp-24].Z
    fadd(&matrix[2], &matrix[8]);           //  FPADD   [bp-36].Z,[bp-12].Z
    dest->x = matrix[0]; dest->y = matrix[1]; dest->z = matrix[2];
                                            //  MOVMEM  [si],[bp-36],<type VECTOR>
                                            //  pop     bx
                                            //  unlk
  rot29:
    goto endp;                              // ROT29:   ret
                                            //
                                            // ROT2     ENDP
  endp:;
}


                                            //
//
//      In-Out Function
//
//      Args    bx : primitive
//              si : vector point
//      Ret     Flags : l  outside
//                      ge  inside
//
                                            //
                                            //  public  INOUT
                                            //
/*
 * INOUT
 */

extern real const1;

int inout(Primitive *p, vector *point) {
  int ax, cond, cx, dx;
  vector fac0, fac3;
    if ((verbose & 1)) {
        fprintf(stderr, "inout: prim=%p\n", (void*)p); fflush(stderr);
        fprintf(stderr, "  side=%d\n", side);
        fprintf(stderr, "  prim kind=%d sgn=%d rotation=%d\n", p->kind, p->sgn, p->rotation);
        fprintvector(stderr, "  prim param ", &p->param);
        fprintvector(stderr, "  prim offset", &p->offset);
        fprintvector(stderr, "  point", point);
        fflush(stderr);
    }

                                            // INOUT    PROC
                                            //
                                            //  link
    fac3.x = point->x; fac3.y = point->y; fac3.z = point->z;
                                            //  MOVMEM  FAC3,[si],<type VECTOR>
    fsub(&fac3.x, &p->offset.x);            //  FPSUB   FAC3.X,[bx].Offst.X
    fsub(&fac3.y, &p->offset.y);            //  FPSUB   FAC3.Y,[bx].Offst.Y
    fsub(&fac3.z, &p->offset.z);            //  FPSUB   FAC3.Z,[bx].Offst.Z
                                            //  push    si
                                            //  lea     si,FAC3
    rot(p, &fac3);                          //  call    ROT
                                            //  pop     si
    if ((verbose & 1)) {
        fprintvector(stderr, "  object local point", &fac3); fflush(stderr);
    }
    cx = p->kind;                           //  mov     cx,word ptr [bx].Kind
    dx = p->sgn;                            //  mov     dx,[bx].Sgn
    if (--cx != 0) {goto inout2;}           //  loop    INOUT2
                                            //
  inout1: /* CUBE */
    if ((verbose & 1)) {
        fprintf(stderr, "  inout cube\n"); fflush(stderr);
    }
    fac3.x.sign = 0;                        // INOUT1:  FPABS   FAC3.X
    ax = fcomp(&fac3.x, &p->param.x); cond = (!(ax < 0));
                                            //  FPCMP   FAC3.X,[bx].Param.X
    if (cond) {goto inout10;}               //  jnl     INOUT10
    fac3.y.sign = 0;                        //  FPABS   FAC3.Y
    ax = fcomp(&fac3.y, &p->param.y); cond = (!(ax < 0));
                                            //  FPCMP   FAC3.Y,[bx].Param.Y
    if (cond) {goto inout10;}               //  jnl     INOUT10
    fac3.z.sign = 0;                        //  FPABS   FAC3.Z
    ax = fcomp(&fac3.z, &p->param.z); cond = (ax < 0);
                                            //  FPCMP   FAC3.Z,[bx].Param.Z
    if (cond) {goto inout11;}               //  jl      INOUT11
  inout10:
    dx = - dx;                              // INOUT10:neg      dx
    if ((verbose & 1)) {
        fprintf(stderr, "  inout negate result: %d\n", dx); fflush(stderr);
    }
  inout11:
    ax = dx;                                // INOUT11:or       dx,dx
                                            // INOUTEND:
                                            //  unlk
    goto endp;                              //  ret
                                            //
  inout2: /* PLANE */
    if (--cx != 0) {goto inout3;}           // INOUT2:  loop    INOUT3
    if ((verbose & 1)) {
        fprintf(stderr, "  inout plane\n"); fflush(stderr);
    }
    fmlt(&fac3.x, &p->param.x);             //  FPMLT   FAC3.X,[bx].Param.X
    fmlt(&fac3.y, &p->param.y);             //  FPMLT   FAC3.Y,[bx].Param.Y
    fmlt(&fac3.z, &p->param.z);             //  FPMLT   FAC3.Z,[bx].Param.Z
    fadd(&fac3.z, &fac3.y);                 //  FPADD   FAC5,FAC4
    fadd(&fac3.z, &fac3.x);                 //  FPADD   FAC5,FAC3
    if ((verbose & 1)) {
        fprintreal(stderr, "  inout pnt . nrm", &fac3.z); fflush(stderr);
    }
    cond = (!((fac3.z).sign == 0));         //  FPSGN?  FAC5
    if (cond) {goto inout11;}               //  jne     INOUT11
    goto inout10;                           //  jmp     INOUT10
                                            //
   inout3: /* QUADRATIC */
    if ((verbose & 1)) {
        fprintf(stderr, "  inout quadratic\n"); fflush(stderr);
    }
    fac0.x = fac3.x; fac0.y = fac3.y; fac0.z = fac3.z;
                                            // INOUT3:  MOVMEM  FAC0,FAC3,<type VECTOR>
    fsqar(&fac0.x);                         //  FPSQAR  FAC0.X
    fsqar(&fac0.y);                         //  FPSQAR  FAC0.Y
    fsqar(&fac0.z);                         //  FPSQAR  FAC0.Z
    fmlt(&fac0.x, &p->param.x);             //  FPMLT   FAC0.X,[bx].Param.X
    fmlt(&fac0.y, &p->param.y);             //  FPMLT   FAC0.Y,[bx].Param.Y
    fmlt(&fac0.z, &p->param.z);             //  FPMLT   FAC0.Z,[bx].Param.Z
    fadd(&fac0.z, &fac0.x);                 //  FPADD   FAC2,FAC0
    fadd(&fac0.z, &fac0.y);                 //  FPADD   FAC2,FAC1
    cond = (!((p->kind) == (3)));           //  cmp     [bx].Kind,3
    if (cond) {goto inout32;}               //  jne     INOUT32
    ax = fcomp(&fac0.z, &const1); cond = (ax < 0);
                                            //  FPCMP   FAC2,Const1
    if (cond) {goto inout33;}               //  jl      INOUT33
  inout34:
    dx = - dx;                              // INOUT34:neg      dx
  inout33:
    ax = dx;                                // INOUT33:or       dx,dx
                                            //  unlk
    goto endp;                              //  ret
  inout32:
    cond = ((fac0.z).sign == 0);            // INOUT32:FPSGN?   FAC2
    if (cond) {goto inout34;}               //  je      INOUT34
    goto inout33;                           //  jmp     INOUT33
                                            //
                                            // INOUT    ENDP
  endp:;
    return ax;
}


                                            //
                                            // @CODE    ENDS
                                            //
                                            // @DATAC   SEGMENT
                                            //
                                            //  public  Const1
                                            //
                                            // Const1   REAL    <0,0,8000h>
                                            //
                                            // @DATAC   ENDS
                                            //
                                            // @CODE    SEGMENT
                                            //
//
//
//      Range primitive checker
//
//      Args    bx : Ranger primitive
//              si : dir, point
//      Ret     flags : l   out of range
//                      ge  in the range
//
//
                                            //
//
//      Normal type check
//
                                            //
                                            //  public  NRANGE
                                            //
/*
 * NRANGE
 */

int nrange(Ranger *p, vector *dir, vector *point) {
 int ax, cond;
 vector fac0, fac3;
    if ((verbose & 2)) {
        fprintf(stderr, "nrange: ranger=%p\n", (void*)p); fflush(stderr);
        fprintvector(stderr, "  ranger location", &p->location);
        fprintreal(stderr, "  ranger radius", &p->radius);
        fflush(stderr);
    }

                                            // NRANGE   PROC
                                            //
                                            //  link
    fac0 = *dir;
    fac3 = *point;
                                            //  MOVMEM  FAC0,[si],<2*(type VECTOR)>
    fsub(&fac3.x, &p->location.x);          //  FPSUB   FAC3.X,[bx].Location.X
    fsub(&fac3.y, &p->location.y);          //  FPSUB   FAC3.Y,[bx].Location.Y
    fsub(&fac3.z, &p->location.z);          //  FPSUB   FAC3.Z,[bx].Location.Z
    fmlt(&fac0.x, &fac3.x);                 //  FPMLT   FAC0.X,FAC3.X
    fmlt(&fac0.y, &fac3.y);                 //  FPMLT   FAC0.Y,FAC3.Y
    fmlt(&fac0.z, &fac3.z);                 //  FPMLT   FAC0.Z,FAC3.Z
    fadd(&fac0.z, &fac0.x);                 //  FPADD   FAC2,FAC0
    fadd(&fac0.z, &fac0.y);                 //  FPADD   FAC2,FAC1
    fsqar(&fac3.x);                         //  FPSQAR  FAC3.X
    fsqar(&fac3.y);                         //  FPSQAR  FAC3.Y
    fsqar(&fac3.z);                         //  FPSQAR  FAC3.Z
    fadd(&fac3.z, &fac3.x);                 //  FPADD   FAC5,FAC3
    fadd(&fac3.z, &fac3.y);                 //  FPADD   FAC5,FAC4
    fsub(&fac3.z, &p->radius);              //  FPSUB   FAC5,[bx].Radius
    cond = (!((fac3.z).sign == 0));         //  FPSGN?  FAC5
    if (cond) {goto nrng0;}                 //  jnz     NRNG0
    cond = ((fac0.z).sign == 0);            //  FPSGN?  FAC2
    if (cond) {goto nrng1;}                 //  jz      NRNG1
    fsqar(&fac0.z);                         //  FPSQAR  FAC2
    ax = fcomp(&fac0.z, &fac3.z);           //  FPCMP   FAC2,FAC5
  nrng9:
                                            // NRNG9:   unlk
    goto endp;                              //  ret
  nrng0:
    ax = 0;                                 // NRNG0:   xor     ax,ax
    goto nrng9;                             //  jmp     NRNG9
  nrng1:
    ax = -1;                                // NRNG1:   mov     al,-1
                                            //  or      al,al
    goto nrng9;                             //  jmp     NRNG9
                                            //
                                            // NRANGE   ENDP
  endp:;
    if ((verbose & 2)) {
        fprintf(stderr, "  result %d\n", ax); fflush(stderr);
    }
    return ax;
}


                                            //
//
//      Initial type check
//         (Ray from viewpoint only)
//         (Much faster than Normal type)
//
                                            //
                                            //  public  IRANGE
                                            //
/*
 * IRANGE
 */

int irange(Ranger *p, vector *dir, vector *_) {
  int ax;
  vector fac0;
    if ((verbose & 2)) {
        fprintf(stderr, "irange: ranger=%p\n", (void*)p); fflush(stderr);
        fprintvector(stderr, "  ranger direction", &p->direction);
        fprintreal(stderr, "  ranger mincos", &p->mincos);
        fflush(stderr);
    }

                                            // IRANGE   PROC
                                            //
                                            //  link
    fac0 = *dir;                            //  MOVMEM  FAC0,[si],<type VECTOR>
    fmlt(&fac0.x, &p->direction.x);         //  FPMLT   FAC0.X,[bx].Direction.X
    fmlt(&fac0.y, &p->direction.y);         //  FPMLT   FAC0.Y,[bx].Direction.Y
    fmlt(&fac0.z, &p->direction.z);         //  FPMLT   FAC0.Z,[bx].Direction.Z
    fadd(&fac0.x, &fac0.y);                 //  FPADD   FAC0,FAC1
    fadd(&fac0.x, &fac0.z);                 //  FPADD   FAC0,FAC2
    if ((verbose & 2)) {
        fprintreal(stderr, "  got cos(theta)", &fac0.x);
    }
    ax = fcomp(&fac0.x, &p->mincos);        //  FPCMP   FAC0,[bx].Mincos
                                            //  unlk
    goto endp;                              //  ret
                                            //
                                            // IRANGE   ENDP
  endp:;
    if ((verbose & 2)) {
        fprintf(stderr, "  result %d\n", ax); fflush(stderr);
    }
    return ax;
}


                                            //
//
//
//      Equation Sovler
//
//      Args    bx : primitive
//              si : dir, point
//              di : where to put distance
//              SIDE : solve inside->outside if this is INSIDE
//      Ret     [di] : distance
//              CUBEX : plane no which has the crossing point
//              carry : 1 if there is no solution
//
//
                                            //
#define expmindis (0)                       // EXPMINDIS        equ     0
                                            //
                                            //  public  SOLVE
                                            //
/*
 * SOLVE
 */

int solve(Primitive *p, vector *dir, vector *point, real *dist) {
  int al, ax, cond, dx;
  vector fac0, fac3;
  real fac10, fac9;

    if ((verbose & 2)) {
        fprintf(stderr, "solve: prim=%p\n", (void*)p); fflush(stderr);
        fprintf(stderr, "  side=%d\n", side);
        fprintf(stderr, "  prim kind=%d sgn=%d rotation=%d\n", p->kind, p->sgn, p->rotation);
        fprintvector(stderr, "  prim param ", &p->param);
        fprintvector(stderr, "  prim offset", &p->offset);
        fprintvector(stderr, "  dir  ", dir);
        fprintvector(stderr, "  point", point);
        fflush(stderr);
    }
                                            // SOLVE    PROC
                                            //
                                            //  link
                                            //  push    di
    fac0 = *dir; fac3 = *point;             //  MOVMEM  FAC0,[si],<2*(type VECTOR)>
    fsub(&fac3.x, &p->offset.x);            //  FPSUB   FAC3.X,[bx].Offst.X
    fsub(&fac3.y, &p->offset.y);            //  FPSUB   FAC3.Y,[bx].Offst.Y
    fsub(&fac3.z, &p->offset.z);            //  FPSUB   FAC3.Z,[bx].Offst.Z
                                            //  push    si
                                            //  lea     si,FAC0
    rot(p, &fac0);                          //  call    ROT
                                            //  lea     si,FAC3
    rot(p, &fac3);                          //  call    ROT
                                            //  pop     si
    dx = p->sgn;                            //  mov     dx,[bx].Sgn
                                            //  xor     dx,SIDE
    if (dx == side) {dx = 0; goto sl1;}     //  je      SL1
    dx = 1;                                 //  mov     dx,1
  sl1:
    if (p->kind == 1) { goto slcube;}
    if (p->kind == 2) { goto slplane;}
    if (p->kind == 3) { goto sl2nd;}
    if (p->kind == 4) { goto slcone;}
                                            // SL1:     mov     di,word ptr [bx].Kind
                                            //  shl     di,1
                                            //  jmp     cs:SLTBL[di-2]
                                            //
                                            //  even
                                            //
                                            // SLTBL    dw      SLCUBE
                                            //  dw      SLPLAIN
                                            //  dw      SL2ND
                                            //  dw      SLCONE
                                            //
  slsc:
    ax = 0;                                 // SLSC:    clc
  slend:
                                            // SLEND:   unlk
    goto endp;                              //  ret
  slns0:
                                            // SLNS0:   pop     di
  slns: /* no solution */
    if ((verbose & 2)) {
        fprintf(stderr, "  slns0: no solution\n");
        fflush(stderr);
    }
    ax = 1;                                 // SLNS:    stc
    goto slend;                             //  jmp     SLEND
                                            //
  slplane:
    if ((verbose & 2)) {
        fprintf(stderr, "slplane\n"); fflush(stderr);
    }
    fmlt(&fac0.x, &p->param.x);             // SLPLAIN:FPMLT    FAC0.X,[bx].Param.X
    fmlt(&fac0.y, &p->param.y);             //  FPMLT   FAC0.Y,[bx].Param.Y
    fmlt(&fac0.z, &p->param.z);             //  FPMLT   FAC0.Z,[bx].Param.Z
    fadd(&fac0.z, &fac0.x);                 //  FPADD   FAC2,FAC0
    fadd(&fac0.z, &fac0.y);                 //  FPADD   FAC2,FAC1
    if ((verbose & 2)) {
        fprintreal(stderr, "  dir . nrm", &fac0.z); fflush(stderr);
    }
    dx ^= fac0.z.sign;
    cond = (dx == 0);                       //  xor     dl,FAC2.SIGN
    if ((verbose & 2) && cond) {
        fprintf(stderr, "  no solution (wrong side)\n"); fflush(stderr);
    }
    if ((fac0.z.man & 0x8000) == 0) {
        cond = 1;
        if ((verbose & 2)) {
            fprintf(stderr, "  no solution (parallel)\n"); fflush(stderr);
        }
    }
    if (cond) {goto slns0;}                 //  njz     SLNS0
    fmlt(&fac3.x, &p->param.x);             //  FPMLT   FAC3.X,[bx].Param.X
    fmlt(&fac3.y, &p->param.y);             //  FPMLT   FAC3.Y,[bx].Param.Y
    fmlt(&fac3.z, &p->param.z);             //  FPMLT   FAC3.Z,[bx].Param.Z
    fadd(&fac3.z, &fac3.x);                 //  FPADD   FAC5,FAC3
    fadd(&fac3.z, &fac3.y);                 //  FPADD   FAC5,FAC4
    fdiv(&fac3.z, &fac0.z);                 //  FPDIV   FAC5,FAC2
    fac3.z.sign ^= 1;                       //  FPCHS   FAC5
    cond = (!((fac3.z).sign == 0));         //  FPSGN?  FAC5
    if ((verbose & 2) && (cond)) {
        fprintf(stderr, "  no solution (plane is behind the start)\n"); fflush(stderr);
    }
    if (cond) {goto slns0;}                 //  njnz    SLNS0
    cond = ((fac3.z.exp) < (expmindis));    //  cmp     FAC5.EXP,EXPMINDIS
    if (cond) {goto slns0;}                 //  njl     SLNS0
                                            //  pop     di
    *dist = fac3.z;                         //  MOVMEM  [di],FAC5,4
    goto slsc;                              //  jmp     SLSC
                                            //
  slcube:
    if ((verbose & 2)) {
        fprintf(stderr, "slcube\n"); fflush(stderr);
    }

    cond = (((fac0.x).man & 0x8000) == 0);  // SLCUBE:  FP0?    FAC0.X
    if (cond) {goto slc2;}                  //  njz     SLC2
    if ((verbose & 2)) {
        fprintf(stderr, "  solve about x\n"); fflush(stderr);
    }
    fac9 = fac3.x;                          //  MOVMEM  FAC9,FAC3.X,4
    ax = fac0.x.sign;                       //  mov     al,FAC0.X.SIGN
    ax ^= dx; cond = ((ax) == 0);           //  xor     al,dl
    if (cond) {goto slc11;}                 //  jz      SLC11
    fsub(&fac9, &p->param.x);               //  FPSUB   FAC9,[bx].Param.X
    goto slc12;                             //  jmp     short SLC12
  slc11:
    fadd(&fac9, &p->param.x);               // SLC11:   FPADD   FAC9,[bx].Param.X
  slc12:
    fdiv(&fac9, &fac0.x);                   // SLC12:   FPDIV   FAC9,FAC0.X
    fac9.sign ^= 1;                         //  FPCHS   FAC9
    cond = !(fac9.sign == 0);
    if (cond) {goto slc2;}                  //  njnz    SLC2
    cond = ((fac9.exp) < (expmindis));      //  cmp     FAC9.EXP,EXPMINDIS
    if (cond) {goto slc2;}                  //  njl     SLC2
    fac10 = fac9;                           //  MOVMEM  FAC10,FAC9,4
    fmlt(&fac10, &fac0.y);                  //  FPMLT   FAC10,FAC0.Y
    fadd(&fac10, &fac3.y);                  //  FPADD   FAC10,FAC3.Y
    fac10.sign = 0;                         //  FPABS   FAC10
    if ((verbose & 2)) {
        fprintreal(stderr, "  abs(y)", &fac10); fflush(stderr);
    }

    ax = fcomp(&fac10, &p->param.y); cond = (!(ax <= 0));
                                            //  FPCMP   FAC10,[bx].Param.Y
    if (cond) {goto slc2;}                  //  njnle   SLC2
    fac10 = fac9;                           //  MOVMEM  FAC10,FAC9,4
    fmlt(&fac10, &fac0.z);                  //  FPMLT   FAC10,FAC0.Z
    fadd(&fac10, &fac3.z);                  //  FPADD   FAC10,FAC3.Z
    fac10.sign = 0;                         //  FPABS   FAC10
    if ((verbose & 2)) {
        fprintreal(stderr, "  abs(z)", &fac10); fflush(stderr);
    }
    ax = fcomp(&fac10, &p->param.z); cond = (!(ax <= 0));
                                            //  FPCMP   FAC10,[bx].Param.Z
    if (cond) {goto slc2;}                  //  jnle    SLC2
    cubex = 0;                              //  mov     CUBEX,0
    if ((verbose & 2)) {
        fprintreal(stderr, "  found solution about x", &fac9); fflush(stderr);
    }
                                            //  pop     di
    *dist = fac9;                           //  MOVMEM  [di],FAC9,4
    goto slsc;                              //  jmp     SLSC
                                            //
  slc2:
    cond = (((fac0.y).man & 0x8000) == 0);  // SLC2:    FP0?    FAC0.Y
    if (cond) {goto slc3;}                  //  njz     SLC3
    if ((verbose & 2)) {
        fprintf(stderr, "  solve about y\n"); fflush(stderr);
    }
    fac9 = fac3.y;                          //  MOVMEM  FAC9,FAC3.Y,4
    ax = fac0.y.sign;                       //  mov     al,FAC0.Y.SIGN
    ax ^= dx; cond = ((ax) == 0);           //  xor     al,dl
    if (cond) {goto slc21;}                 //  jz      SLC21
    fsub(&fac9, &p->param.y);               //  FPSUB   FAC9,[bx].Param.Y
    goto slc22;                             //  jmp     short SLC22
  slc21:
    fadd(&fac9, &p->param.y);               // SLC21:   FPADD   FAC9,[bx].Param.Y
  slc22:
    fdiv(&fac9, &fac0.y);                   // SLC22:   FPDIV   FAC9,FAC0.Y
    fac9.sign ^= 1;                         //  FPCHS   FAC9
    cond = !(fac9.sign == 0);
    if (cond) {goto slc3;}                  //  njnz    SLC3
    cond = ((fac9.exp) < (expmindis));      //  cmp     FAC9.EXP,EXPMINDIS
    if (cond) {goto slc3;}                  //  njl     SLC3
    fac10 = fac9;                           //  MOVMEM  FAC10,FAC9,4
    fmlt(&fac10, &fac0.z);                  //  FPMLT   FAC10,FAC0.Z
    fadd(&fac10, &fac3.z);                  //  FPADD   FAC10,FAC3.Z
    fac10.sign = 0;                         //  FPABS   FAC10
    if ((verbose & 2)) {
        fprintreal(stderr, "  abs(z)", &fac10); fflush(stderr);
    }
    ax = fcomp(&fac10, &p->param.z); cond = (!(ax <= 0));
                                            //  FPCMP   FAC10,[bx].Param.Z
    if (cond) {goto slc3;}                  //  njnle   SLC3
    fac10 = fac9;                           //  MOVMEM  FAC10,FAC9,4
    fmlt(&fac10, &fac0.x);                  //  FPMLT   FAC10,FAC0.X
    fadd(&fac10, &fac3.x);                  //  FPADD   FAC10,FAC3.X
    fac10.sign = 0;                         //  FPABS   FAC10
    if ((verbose & 2)) {
        fprintreal(stderr, "  abs(x)", &fac10); fflush(stderr);
    }
    ax = fcomp(&fac10, &p->param.x); cond = (!(ax <= 0));
                                            //  FPCMP   FAC10,[bx].Param.X
    if (cond) {goto slc3;}                  //  jnle    SLC3
    cubex = 4;                              //  mov     CUBEX,4
    if ((verbose & 2)) {
        fprintreal(stderr, "  found solution about y", &fac9); fflush(stderr);
    }
                                            //  pop     di
    *dist = fac9;                           //  MOVMEM  [di],FAC9,4
    goto slsc;                              //  jmp     SLSC
                                            //
  slc3:
    cond = (((fac0.z).man & 0x8000) == 0);  // SLC3:    FP0?    FAC0.Z
    if (cond) {goto slc4;}                  //  njz     SLC4
    if ((verbose & 2)) {
        fprintf(stderr, "  solve about z\n"); fflush(stderr);
    }
    fac9 = fac3.z;                          //  MOVMEM  FAC9,FAC3.Z,4
    ax = fac0.z.sign;                       //  mov     al,FAC0.Z.SIGN
    ax ^= dx; cond = ((ax) == 0);           //  xor     al,dl
    if (cond) {goto slc31;}                 //  jz      SLC31
    fsub(&fac9, &p->param.z);               //  FPSUB   FAC9,[bx].Param.Z
    goto slc32;                             //  jmp     short SLC32
  slc31:
    fadd(&fac9, &p->param.z);               // SLC31:   FPADD   FAC9,[bx].Param.Z
  slc32:
    fdiv(&fac9, &fac0.z);                   // SLC32:   FPDIV   FAC9,FAC0.Z
    fac9.sign ^= 1;                         //  FPCHS   FAC9
    if (fac9.sign != 0) {goto slc4;}        //  njnz    SLC4
    cond = ((fac9.exp) < (expmindis));      //  cmp     FAC9.EXP,EXPMINDIS
    if (cond) {goto slc4;}                  //  njl     SLC4
    fac10 = fac9;                           //  MOVMEM  FAC10,FAC9,4
    fmlt(&fac10, &fac0.x);                  //  FPMLT   FAC10,FAC0.X
    fadd(&fac10, &fac3.x);                  //  FPADD   FAC10,FAC3.X
    fac10.sign = 0;                         //  FPABS   FAC10
    if ((verbose & 2)) {
        fprintreal(stderr, "  abs(x)", &fac10); fflush(stderr);
    }
    ax = fcomp(&fac10, &p->param.x); cond = (!(ax <= 0));
                                            //  FPCMP   FAC10,[bx].Param.X
    if (cond) {goto slc4;}                  //  njnle   SLC4
    fac10 = fac9;                           //  MOVMEM  FAC10,FAC9,4
    fmlt(&fac10, &fac0.y);                  //  FPMLT   FAC10,FAC0.Y
    fadd(&fac10, &fac3.y);                  //  FPADD   FAC10,FAC3.Y
    fac10.sign = 0;                         //  FPABS   FAC10
    if ((verbose & 2)) {
        fprintreal(stderr, "  abs(y)", &fac10); fflush(stderr);
    }
    ax = fcomp(&fac10, &p->param.y); cond = (!(ax <= 0));
                                            //  FPCMP   FAC10,[bx].Param.Y
    if (cond) {goto slc4;}                  //  jnle    SLC4
    cubex = 8;                              //  mov     CUBEX,8
    if ((verbose & 2)) {
        fprintreal(stderr, "  found solution about z", &fac9); fflush(stderr);
    }
                                            //  pop     di
    *dist = fac9;                           //  MOVMEM  [di],FAC9,4
    goto slsc;                              //  jmp     SLSC
                                            //
  slc4:
    goto slns0;                             // SLC4:    jmp     SLNS0
                                            //
                                            // SL2ND:
  sl2nd:
  slcone:
    {
      real fac1, fac2, fac9, fac10, fac11, fac12, fac13, fac14;
    fac12 = fac0.x; fac13 = fac0.y; fac14 = fac0.z;
    if ((verbose & 2)) {
        fprintf(stderr, "sl2nd\n"); fflush(stderr);
        fprintvector(stderr, "  obj local dir", &fac0);
        fprintvector(stderr, "  obj local vp", &fac3);
    }
                                            // SLCONE:  MOVMEM  FAC12,FAC0,<type VECTOR>
    fsqar(&fac0.x);                         //  FPSQAR  FAC0.X
    fmlt(&fac0.x, &p->param.x);             //  FPMLT   FAC0.X,[bx].Param.X
    fsqar(&fac0.y);                         //  FPSQAR  FAC0.Y
    fmlt(&fac0.y, &p->param.y);             //  FPMLT   FAC0.Y,[bx].Param.Y
    fsqar(&fac0.z);                         //  FPSQAR  FAC0.Z
    fmlt(&fac0.z, &p->param.z);             //  FPMLT   FAC0.Z,[bx].Param.Z
    fac2 = fac0.z;
    fadd(&fac2, &fac0.x);                   //  FPADD   FAC2,FAC0
    fadd(&fac2, &fac0.y);                   //  FPADD   FAC2,FAC1
    cond = (((fac2).man & 0x8000) == 0);    //  FP0?    FAC2
    if ((verbose & 2) && cond) {
        fprintf(stderr, "  a == 0: no solution\n"); fflush(stderr);
    }
    if (cond) {goto slns0;}                 //  njz     SLNS0
    fac9 = fac3.x; fac10 = fac3.y; fac11 = fac3.z;
                                            //  MOVMEM  FAC9,FAC3,<type VECTOR>
    fmlt(&fac9, &fac12);                    //  FPMLT   FAC9.X,FAC12.X
    fmlt(&fac9, &p->param.x);               //  FPMLT   FAC9.X,[bx].Param.X
    fmlt(&fac10, &fac13);                   //  FPMLT   FAC9.Y,FAC12.Y
    fmlt(&fac10, &p->param.y);              //  FPMLT   FAC9.Y,[bx].Param.Y
    fmlt(&fac11, &fac14);                   //  FPMLT   FAC9.Z,FAC12.Z
    fmlt(&fac11, &p->param.z);              //  FPMLT   FAC9.Z,[bx].Param.Z
    fadd(&fac9, &fac10);                    //  FPADD   FAC9,FAC10
    fadd(&fac9, &fac11);                    //  FPADD   FAC9,FAC11
    fscale(&fac9, 1);                       //  FPSCALE FAC9,1
    fac10 = fac3.x; fac11 = fac3.y; fac12 = fac3.z;
                                            //  MOVMEM  FAC10,FAC3,<type VECTOR>
    fsqar(&fac10);                          //  FPSQAR  FAC10.X
    fmlt(&fac10, &p->param.x);              //  FPMLT   FAC10.X,[bx].Param.X
    fsqar(&fac11);                          //  FPSQAR  FAC10.Y
    fmlt(&fac11, &p->param.y);              //  FPMLT   FAC10.Y,[bx].Param.Y
    fsqar(&fac12);                          //  FPSQAR  FAC10.Z
    fmlt(&fac12, &p->param.z);              //  FPMLT   FAC10.Z,[bx].Param.Z
    fadd(&fac10, &fac11);                   //  FPADD   FAC10,FAC11
    fadd(&fac10, &fac12);                   //  FPADD   FAC10,FAC12
    cond = (!((p->kind) == (3)));           //  cmp     [bx].Kind,3
    if (cond) {goto sl22;}                  //  jnz     SL22
    fsub(&fac10, &const1);                  //  FPSUB   FAC10,Const1
  sl22:;
    real fac0 = fac9;
    fac1 = fac10;                           // SL22:    MOVMEM  FAC0,FAC9,<2*(type REAL)>
    if ((verbose & 2)) {
        fprintreal(stderr, "  a", &fac2);
        fprintreal(stderr, "  b", &fac9);
        fprintreal(stderr, "  c", &fac10);
        fflush(stderr);
    }
    {
    fsqar(&fac0);                           //  FPSQAR  FAC0
    real fac99 = fac0;
    fmlt(&fac1, &fac2);                     //  FPMLT   FAC1,FAC2
    fscale(&fac1, 2);                       //  FPSCALE FAC1,2
    fsub(&fac0, &fac1);                     //  FPSUB   FAC0,FAC1
    if ((verbose & 2)) {
        fprintreal(stderr, "  D = b^2 - 4ac", &fac0);
        fprintreal(stderr, "    b^2", &fac99);
        fprintreal(stderr, "    4ac", &fac1);
        fflush(stderr);
    }
    }
    cond = (!((fac0).sign == 0));           //  FPSGN?  FAC0
    if (cond) {goto slns0;}                 //  njnz    SLNS0
  sl221:
    fsqrt(&fac0);                           // SL221:   FPSQRT  FAC0
    fac11 = fac0;                           //  MOVMEM  FAC11,FAC0
    al = fac9.sign;                         //  mov     al,byte ptr FAC9.SIGN
    al ^= 1;                                //  xor     al,1
    fac11.sign = al;                        //  mov     byte ptr FAC11.SIGN,al
    fsub(&fac11, &fac9);                    //  FPSUB   FAC11,FAC9
    fac12 = fac10;                          //  MOVMEM  FAC12,FAC10
    fscale(&fac12, 1);                      //  FPSCALE FAC12,1
    fscale(&fac2, 1);                       //  FPSCALE FAC2,1
    if ((verbose & 2)) {
        fprintreal(stderr, "  fac12", &fac12);
        fprintreal(stderr, "  fac11", &fac11);
        fprintreal(stderr, "  fac2", &fac2);
        fflush(stderr);
    }
    fdiv(&fac12, &fac11);                   //  FPDIV   FAC12,FAC11
    fdiv(&fac11, &fac2);                    //  FPDIV   FAC11,FAC2
    ax = fcomp(&fac11, &fac12); cond = (!(ax < 0));
                                            //  FPCMP   FAC11,FAC12
    if (cond) {goto sl23;}                  //  jnl     SL23
    cond = ((dx) == 0);                     //  or      dl,dl
    if (cond) {goto sl24;}                  //  jz      SL24
    goto sl25;                              //  jmp     short SL25
  sl23:
    cond = ((dx) == 0);                     // SL23:    or      dl,dl
    if (cond) {goto sl25;}                  //  jz      SL25
  sl24:
    cond = (((fac11).man & 0x8000) == 0);   // SL24:    FP0?    FAC11
    if (cond) {goto sl241;}                 //  jz      SL241
    cond = (!((fac11).sign == 0));          //  FPSGN?  FAC11
    if (cond) {goto sl241;}                 //  jnz     SL241
    cond = (!((fac11.exp) < (expmindis)));  //  cmp     FAC11.EXP,EXPMINDIS
    if (cond) {goto sl31;}                  //  jnl     SL31
  sl241:
    cond = (((fac12).man & 0x8000) == 0);   // SL241:   FP0?    FAC12
    if (cond) {goto slns2;}                 //  jz      SLNS2
    cond = (!((fac12).sign == 0));          //  FPSGN?  FAC12
    if (cond) {goto slns2;}                 //  jnz     SLNS2
    cond = ((fac12.exp) < (expmindis));     //  cmp     FAC12.EXP,EXPMINDIS
    if (cond) {goto slns2;}                 //  jl      SLNS2
  sl32:
                                            // SL32:    pop     di
    *dist = fac12;                          //  MOVMEM  [di],FAC12,4
    goto slsc;                              //  jmp     SLSC
  slns2:
    goto slns0;                             // SLNS2:   jmp     SLNS0
  sl31:
                                            // SL31:    pop     di
    *dist = fac11;                          //  MOVMEM  [di],FAC11,4
    goto slsc;                              //  jmp     SLSC
  sl25:
    cond = (((fac12).man & 0x8000) == 0);   // SL25:    FP0?    FAC12
    if (cond) {goto sl251;}                 //  jz      SL251
    cond = (!((fac12).sign == 0));          //  FPSGN?  FAC12
    if (cond) {goto sl251;}                 //  jnz     SL251
    cond = (!((fac12.exp) < (expmindis)));  //  cmp     FAC12.EXP,EXPMINDIS
    if (cond) {goto sl32;}                  //  jnl     SL32
  sl251:
    cond = (((fac11).man & 0x8000) == 0);   // SL251:   FP0?    FAC11
    if (cond) {goto slns2;}                 //  jz      SLNS2
    cond = (!((fac11).sign == 0));          //  FPSGN?  FAC11
    if (cond) {goto slns2;}                 //  jnz     SLNS2
    cond = ((fac11.exp) < (expmindis));     //  cmp     FAC11.EXP,EXPMINDIS
    if (cond) {goto slns2;}                 //  jl      SLNS2
    goto sl31;                              //  jmp     SL31
                                            //
                                            // SOLVE    ENDP
    }
  endp:;
    if ((verbose & 2)) {
        fprintf(stderr, "solve result: solution: %s\n", !ax ? "found" : "not found");
        if (!ax) {
            fprintreal(stderr, "  dist", dist);
        }
        fflush(stderr);
    }
    return ax;
}


                                            //
//
//
//      Normal Vector Generater
//
//      Args    bx : Primitive
//              si : dir, vp, local point
//              di : destination
//              CUBEX : plane of cube
//              SIDE  : Inside / Outside
//
//      Ret     [di] : Result
//
//
                                            //
                                            //  public  NORMAL
                                            //
/*
 * NORMAL
 */

/*ARGSUSED*/
void normal(Primitive *p, vector *dir, vector *vp, vector *point, vector *dest) {
  int cond, dx;
  vector fac0;
    if ((verbose & 4)) {
        fprintf(stderr, "normal prim=%p\n", (void*)p);
        fprintf(stderr, "  side=%d\n", side);
        fprintf(stderr, "  prim kind=%d sgn=%d rotation=%d\n", p->kind, p->sgn, p->rotation);
        fprintvector(stderr, "  prim param ", &p->param);
        fprintvector(stderr, "  prim offset", &p->offset);
        fprintvector(stderr, "  dir  ", dir);
        fprintvector(stderr, "  point", point);
        fflush(stderr);
    }
                                            // NORMAL   PROC
                                            //
                                            //  link
                                            //  push    di
    fac0 = *point;                          //  MOVMEM  FAC0,[si+24],<type VECTOR>
    dx = side;                              //  mov     dx,SIDE
    cond = ((dx) == (p->sgn)); dx ^= p->sgn;
                                            //  xor     dx,[bx].Sgn
    if (cond) {goto nor1;}                  //  jz      NOR1
    dx = 1;                                 //  mov     dl,1
  nor1:
                                            // NOR1:    mov     di,word ptr [bx].Kind
                                            //  shl     di,1
    if (p->kind == 1) { goto norcube; }
    if (p->kind == 2) { goto norpln ; }
    if (p->kind == 3) { goto nor2nd ; }
    if (p->kind == 4) { goto norcone; }
                                            //  call    cs:NTBL[di-2]
                                            //  pop     di
                                            //  push    di
                                            //  MOVMEM  [di],FAC0,<type VECTOR>
                                            //  pop     di
                                            //  unlk
                                            //  ret
                                            //
                                            //  even
                                            //
                                            // NTBL     dw      NORCUBE
                                            //  dw      NORPLN
                                            //  dw      NOR2ND
                                            //  dw      NORCONE
                                            //
 norcube:;
    if ((verbose & 4)) {
        fprintf(stderr, "cubex = %d\n", cubex);
        fflush(stderr);
    }
    real *face = &dest->x;
    int sign = fac0.x.sign;
    if (cubex == 0) { face = &dest->x; sign = fac0.x.sign; }
    if (cubex == 4) { face = &dest->y; sign = fac0.y.sign; }
    if (cubex == 8) { face = &dest->z; sign = fac0.z.sign; }
    // NORCUBE:mov      cubex,CUBEX
    cond = (!((dx) == (sign)));        //       mov     dh,FAC0[di].SIGN
    real zero = {0,0,0};
    dest->x = zero; dest->y = zero; dest->z = zero;
                                            //  xor     ax,ax
                                            //  push    di
                                            //  mov     cx,6
                                            //  lea     di,FAC0
                                            //  cld
                                            //  push    es
                                            //  mov     es,DATASEG
                                            //  rep     stosw
                                            //  pop     es
                                            //  pop     di
                                            //  xor     dh,dl
    if (cond) {goto norcub1;}               //  jnz     NORCUB1
    face->man = 0x8000; face->exp = 0; face->sign = 0;
                                            //  FPLOD   FAC0[di],8000h,0,0
    goto endp;                              //  ret
  norcub1:
    face->man = 0x8000; face->exp = 0; face->sign = 1;
                                            // NORCUB1:FPLOD    FAC0[di],8000h,0,1
    goto endp;                              //  ret
                                            //
  norpln:
    dest->x = p->param.x; dest->y = p->param.y; dest->z = p->param.z;
                                            // NORPLN:  MOVMEM  FAC0,[bx].Param,<type VECTOR>
    dest->x.sign ^= dx;                     //  xor     DEST->X.SIGN,dl
    dest->y.sign ^= dx;                     //  xor     DEST->Y.SIGN,dl
    dest->z.sign ^= dx;                     //  xor     DEST->Z.SIGN,dl
    goto endp;                              //  ret
                                            //
  norcone:                                  // NORCONE:
  nor2nd:
                                            // NOR2ND:  MOVMEM  FAC3,FAC0,<type VECTOR>
    fmlt(&fac0.x, &p->param.x);             //  FPMLT   FAC0.X,[bx].Param.X
    fmlt(&fac0.y, &p->param.y);             //  FPMLT   FAC0.Y,[bx].Param.Y
    fmlt(&fac0.z, &p->param.z);             //  FPMLT   FAC0.Z,[bx].Param.Z
    fac0.x.sign ^= dx;                      //  xor     FAC0.X.SIGN,dl
    fac0.y.sign ^= dx;                      //  xor     FAC0.Y.SIGN,dl
    fac0.z.sign ^= dx;                      //  xor     FAC0.Z.SIGN,dl
                                            //  push    si
                                            //  lea     si,FAC0
    nrmlize(&fac0);                         //  call    NRMLIZE
    *dest = fac0;                           //  pop     si
    goto endp;                              //  ret
                                            //
                                            // NORMAL   ENDP
  endp:;
    if ((verbose & 4)) {
        fprintvector(stderr, "normal result", dest); fflush(stderr);
    }
}
                                            //

void tile  (Primitive* p, vector *dir, vector *vp, vector *point);
void stripe(Primitive* p, vector *dir, vector *vp, vector *point);
void ring  (Primitive* p, vector *dir, vector *vp, vector *point);
void dot   (Primitive* p, vector *dir, vector *vp, vector *point);
void check (Primitive* p, vector *dir, vector *vp, vector *point);

mapfunc stdtxt[] = {
  tile,
  stripe,
  ring,
  dot,
  check
};
#define  STDTXTMAX ((int)(sizeof(stdtxt) / sizeof(stdtxt[0])))
                                            // STDTXT   dw      TILE
                                            //  dw      STRIPE
                                            //  dw      RING
                                            //  dw      DOT
                                            //  dw      CHECK

//
//
//      Texture Mapping
//
//      Arg     bx : primitive
//              si : dir, vp, pnt
//
//
                                            //
                                            //  public  TEXTMAP
                                            //
/*
 * TEXTMAP
 */

void textmap(Primitive* p, vector *dir, vector *vp, vector *point) {
int cond, di;
    if ((verbose & 32)) {
        fprintf(stderr, "textmap: prim=%p\n", (void*)p); fflush(stderr);
        fprintf(stderr, "  textmap: %d\n", p->texture);
    }
                                            // TEXTMAP  PROC
                                            //
    cond = (!((p->texture) == (0)));        //  cmp     [bx].Texture,0
    if (cond) {goto txmp0;}                 //  jnz     TXMP0
    goto endp;                              //  ret
  txmp0:
                                            // TXMP0:   link
                                            //  MOVMEM  FAC0,[si+24],<type VECTOR>
                                            //  push    si
                                            //  push    bx
                                            //  push    si
                                            //  push    bx
                                            //  lea     si,FAC0
                                            //  push    si
    di = p->texture;                        //  mov     di,word ptr [bx].Texture
                                            //  shl     di,1
    cond = ((di) >= (USER));                //  cmp     di,2*USRTXTOFST
    if (cond) {goto txmp1;}                 //  jae     TXMP1
    cond = ((di) > (STDTXTMAX));            //  cmp     di,2*STDTXTNUM
    if (cond) {goto txmp3;}                 //  ja      TXMP3
                                            //  mov     es,DATASEG
    if ((verbose & 32)) {
        fprintf(stderr, "  calling texture %d\n", di); fflush(stderr);
    }
    (*stdtxt[di-1])(p, dir, vp, point);     //  call    DGROUP:STDTXT[di-2]
    if ((verbose & 32)) {
        fprintf(stderr, "  returned from texture %d\n", di); fflush(stderr);
    }
                                            //  mov     es,SEGTABLE
    goto txmp2;                             //  jmp     short TXMP2
  txmp1:
    cond = ((di) >= ((MAXUTEX+USER)));
                                            // TXMP1:   cmp     di,(MAXUTEX+USRTXTOFST)*2
    if (cond) {goto txmp3;}                 //  jae     TXMP3
    mapfunc func = usertext[di-USER];
                                            //  mov     di,DGROUP:USERTEXT[di-2*USRTXTOFST]
    cond = ((func) == NULL);                //  or      di,di
    if (cond) {goto txmp3;}                 //  jz      TXMP3

    if ((verbose & 32)) {
        fprintf(stderr, "  calling user texture %p\n", (void*)func); fflush(stderr);
    }
    (*func)(p, dir, vp, point);             //  mov     es,DATASEG
    if ((verbose & 32)) {
        fprintf(stderr, "  returned from user texture %p\n", (void*)func); fflush(stderr);
    }
                                            //  call    di
//      mov     es,SEGTABLE                     ;|
    goto txmp2;                             //  jmp     short TXMP2
  txmp3:
    fprintf(stderr, "Illegal Texture Mapping %d\n", di);
                                            // TXMP3:   lea     dx,ILTXMP
                                            //  mov     ah,9
                                            //  int     21h
  txmp2:
                                            // TXMP2:   pop     ax
                                            //  pop     ax
                                            //  pop     ax
                                            //  pop     bx
                                            //  pop     si
                                            //  unlk
    goto endp;                              //  ret
                                            //
                                            // TEXTMAP  ENDP
  endp:;
}


                                            //
                                            //  extrn   STRIPE  :  NEAR
                                            //  extrn   RING    :  NEAR
                                            //  extrn   DOT     :  NEAR
                                            //
                                            // @CODE    ENDS
                                            //
                                            // @DATAI   SEGMENT
                                            //
                                            //  even
                                            //
                                            // STDTXT   dw      TILE
                                            //  dw      STRIPE
                                            //  dw      RING
                                            //  dw      DOT
                                            //  dw      CHECK
                                            //
                                            // @DATAI   ENDS
                                            //
                                            // @DATAB   SEGMENT
                                            //
                                            // ILTXMP   db      'Illegal Texture Mapping$'
                                            // ILBPMP   db      'Illegal Bump Mapping$'
                                            //
                                            // @DATAB   ENDS
                                            //
                                            // @CODE    SEGMENT
                                            //
//
//
//      Standard Texture Mapping Function
//
//
                                            //
/*
 * TILE
 */

void tile(Primitive* p, vector *dir, vector *vp, vector *point) {
  int ax, cond;
                                            // TILE     PROC
                                            //
//      mov     es,SEGTABLE
                                            //  lea     si,FAC0.X
    ax = rtoif(&point->x);                  //  call    RTOIF
    cond = !((ax & 8) == 0);                //  test    al,8
    if (cond) {goto tile1;}                 //  jnz     TILE1
                                            //  lea     si,FAC0.Z
    ax = rtoif(&point->z);                  //  call    RTOIF
    cond = !((ax & 8) == 0);                //  test    al,8
    if (cond) {goto tile1;}                 //  jnz     TILE1
    p->solid.y.man = 0; p->solid.y.exp = 0; p->solid.y.sign = 0;
                                            //  FPLOD   [bx].Solid.Y,0,0,0
    goto endp;                              //  ret
  tile1:
    p->solid.y.man = 0x0ff00; p->solid.y.exp = 7; p->solid.y.sign = 0;
                                            // TILE1:   FPLOD   [bx].Solid.Y,0ff00h,7,0
    goto endp;                              //  ret
                                            //
                                            // TILE     ENDP
  endp:;
}


                                            //
/*
 * CHECK
 */

void check(Primitive* p, vector *dir, vector *vp, vector *point) {
int ax, cond, di;

                                            // CHECK    PROC
                                            //
//      mov     es,SEGTABLE
                                            //  lea     si,FAC0.X
    ax = rtoif(&point->x);                  //  call    RTOIF
    di = ax;                                //  mov     di,ax
                                            //  lea     si,POINT->Z
    ax = rtoif(&point->z);                  //  call    RTOIF
    ax ^= di;                               //  xor     ax,di
    cond = !((ax & 8) == 0);                //  test    al,8
    if (cond) {goto check1;}                //  jnz     CHECK1
    p->solid.y.man = 0; p->solid.y.exp = 0; p->solid.y.sign = 0;
                                            //  FPLOD   [bx].Solid.Y,0,0,0
    goto endp;                              //  ret
  check1:
    p->solid.y.man = 0x0ff00; p->solid.y.exp = 7; p->solid.y.sign = 0;
                                            // CHECK1:  FPLOD   [bx].Solid.Y,0ff00h,7,0
    goto endp;                              //  ret
                                            //
                                            // CHECK    ENDP
  endp:;
}


                                            //
//
//
//      Bump Mapping
//
//      Arg     bx : primitive
//              si : dir, vp, pnt
//
//
                                            //
                                            //  public  BUMPMAP
                                            //
/*
 * BUMPMAP
 */

void bumpmap(Primitive* p, vector *dir, vector *vp, vector *point) {
int cond, di;

                                            // BUMPMAP  PROC
                                            //
    cond = (!((p->bump) == (0)));           //  cmp     [bx].Bump,0
    if (cond) {goto bpmp0;}                 //  jnz     BPMP0
    goto endp;                              //  ret
  bpmp0:
                                            // BPMP0:   link
                                            //  MOVMEM  FAC0,[si+24],<type VECTOR>
                                            //  push    si
                                            //  push    bx
                                            //  push    si
                                            //  push    bx
                                            //  lea     si,FAC0
                                            //  push    si
    di = p->bump;                           //  mov     di,word ptr [bx].Bump
    cond = ((di) > (MAXBUMP));              //  cmp     di,MAXBUMP
    if (cond) {goto bpmp1;}                 //  ja      BPMP1
    mapfunc func = bumping[di-1];           //  mov     di,DGROUP:BUMPING[di-2]
    cond = ((di) == 0);                     //  or      di,di
    if (cond) {goto bpmp1;}                 //  jz      BPMP1
                                            //  mov     es,DATASEG
    (*func)(p, dir, vp, point);             //  call    di
//      mov     es,SEGTABLE
    goto bpmp2;                             //  jmp     short BPMP2
  bpmp1:
    fprintf(stderr, "Illegal Bump Mapping %d\n", di);
                                            // BPMP1:   lea     dx,ILBPMP
                                            //  mov     ah,9
                                            //  int     21h
  bpmp2:
                                            // BPMP2:   pop     ax
                                            //  pop     ax
                                            //  pop     ax
                                            //  pop     bx
                                            //  pop     si
                                            //  unlk
    goto endp;                              //  ret
                                            //
                                            // BUMPMAP  ENDP
  endp:;
}


                                            //
                                            //  include epilogue.h
                                            //
                                            //  END
