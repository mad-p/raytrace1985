#define EXTERN

#include "trace.h"

extern int verbose;


                                            //  PAGE    64,132
                                            //  TITLE   ***** ray2.mac ****  Higher level routines
//
//
//      Machine language routine 2
//
//
//
                                            //
                                            //  include def.ah
                                            //
                                            //  .SALL
                                            //
                                            // @code    segment
                                            //
                                            //  extrn   NRMLIZE         : near
                                            //  extrn   IRANGE          : near
                                            //  extrn   NRANGE          : near
                                            //  extrn   NORMAL          : near
                                            //  extrn   LINEAR          : near
                                            //  extrn   INOUT           : near
                                            //  extrn   SOLVE           : near
                                            //  extrn   TEXTMAP         : near
                                            //  extrn   BUMPMAP         : near
                                            //  extrn   WRITESCA        : near
                                            //  extrn   ROT             : near
                                            //  extrn   ROT2            : near
                                            //
                                            // @code    ends
                                            //
                                            // @datau   segment
                                            //
                                            // FAC0     REAL    <?,?,?>
                                            // FAC1     REAL    <?,?,?>
                                            // FAC2     REAL    <?,?,?>
                                            // FAC3     REAL    <?,?,?>
                                            // FAC4     REAL    <?,?,?>
                                            // FAC5     REAL    <?,?,?>
                                            // FAC6     REAL    <?,?,?>
                                            // FAC7     REAL    <?,?,?>
                                            // FAC8     REAL    <?,?,?>
                                            // FAC9     REAL    <?,?,?>
                                            // FAC10    REAL    <?,?,?>
                                            // FAC11    REAL    <?,?,?>
                                            // FAC12    REAL    <?,?,?>
                                            //
                                            // F0       REAL    <?,?,?>
                                            // F1       REAL    <?,?,?>
                                            // F2       REAL    <?,?,?>
                                            //
                                            // URA      DW      ?
                                            //
                                            // @datau   ends
                                            //
                                            // @datab   segment
                                            //
                                            // MONMES   db      'Scan (   ,   )  :  RGB[   |   |   ]'
                                            // SIZEofMONMES     equ     35
                                            //
                                            // @datab   ends
                                            //
                                            // @datac   segment
                                            //
                                            //  extrn   Const1 : DWORD
                                            //
                                            // @datac   ends
                                            //
                                            // @code    segment
                                            //
//
//
//      mainloop
//          called from C with no arguments
//
//
                                            //
                                            //  public  MAINLOOP
/*
 * MAINLOOP
 */

void mainloop(void) {
int ax, bp_2, bp_4, bx, cond;
int subx, suby;
int subr, subg, subb;
real fac0, fac1, fac20, fac21, fac22;
vector fac3, fac6;
                                            // MAINLOOP PROC
                                            //
                                            //  link    4
    reserve[0].re.x.man = 0x8000; reserve[0].re.x.exp = 0; reserve[0].re.x.sign = 0;
                                            //  FPLOD   RESERVE.RE.X,8000h,0,0
    reserve[0].re.y.man = 0x8000; reserve[0].re.y.exp = 0; reserve[0].re.y.sign = 0;
                                            //  FPLOD   RESERVE.RE.Y,8000h,0,0
    reserve[0].re.z.man = 0x8000; reserve[0].re.z.exp = 0; reserve[0].re.z.sign = 0;
                                            //  FPLOD   RESERVE.RE.Z,8000h,0,0
    reserve[0].rside = 1;                   //  mov     RESERVE.RSIDE,1
    reserve[0].rrct = 0;
    reserve[0].rdct = 0;                    //  mov     RESERVE.RRCT,0
    ax = ystart;                            //  mov     ax,YSTART
    bp_4 = ax;                              //  mov     word ptr -4[bp],ax
  main00:
    if (verbose >= 1) {
      fprintf(stderr, "\nmain00: y = %d\n", bp_4); fflush(stderr);
    }
    ax = bp_4;                              // MAIN00: mov      ax,word ptr -4[bp]
    cond = (!((ax) < (ystop)));             //  cmp     ax,YSTOP
    if (cond) {goto main99;}                //  njnb    MAIN99
    ax = xstart;                            //  mov     ax,XSTART
    bp_2 = ax;                              //  mov     word ptr -2[bp],ax
  main10:
    if (verbose >= 1) {
      fprintf(stderr, "\nmain10: x = %d\n", bp_2); fflush(stderr);
    }
    ax = bp_2;                              // MAIN10:  mov     ax,word ptr -2[bp]
    cond = (!((ax) < (xstop)));             //  cmp     ax,XSTOP
    if (cond) {goto main19;}                //  njnb    MAIN19

    subr = subg = subb = 0;
    suby = 0;
  main10sy:
    if (suby >= subdiv) { goto main10sye; }
    subx = 0;
  main10sx:
    if (subx >= subdiv) { goto main10sxe; }

    reserve[0].rnet = &(networks[0]);       //  mov     RESERVE.RNET,offset DGROUP:NETWORKS
    reserve[0].rprim = 0;                   //  mov     RESERVE.RPRIM,0
    reserve[0].ro.x = viewpt.x; reserve[0].ro.y = viewpt.y; reserve[0].ro.z = viewpt.z;
                                            //  MOVMEM  RESERVE.RO,VIEWPT.X,12
    ax = bp_2;
    ax -= xoffset;                          //  sub     ax,XOFFSET
    itor(&fac0, ax);                        //  FITOR   FAC0
    ax = bp_4;                              //  mov     ax,word ptr -4[bp]
    ax -= yoffset;                          //  sub     ax,YOFFSET
    ax = - ax;                              //  neg     ax
    itor(&fac1, ax);                        //  FITOR   FAC1

    itor(&fac20, subdiv);
    itor(&fac21, subx);
    fdiv(&fac21, &fac20);
    fadd(&fac0, &fac21);
    itor(&fac22, suby);
    fdiv(&fac22, &fac20);
    fadd(&fac1, &fac22);

    fac3 = eh; fac6 = ev;                   //  MOVMEM  FAC3,EH,24
    fmlt(&fac3.x, &fac0);                   //  FPMLT   FAC3.X,FAC0
    fmlt(&fac3.y, &fac0);                   //  FPMLT   FAC3.Y,FAC0
    fmlt(&fac3.z, &fac0);                   //  FPMLT   FAC3.Z,FAC0
    fmlt(&fac6.x, &fac1);                   //  FPMLT   FAC6.X,FAC1
    fmlt(&fac6.y, &fac1);                   //  FPMLT   FAC6.Y,FAC1
    fmlt(&fac6.z, &fac1);                   //  FPMLT   FAC6.Z,FAC1
    fadd(&fac3.x, &fac6.x);                 //  FPADD   FAC3.X,FAC6.X
    fadd(&fac3.y, &fac6.y);                 //  FPADD   FAC3.Y,FAC6.Y
    fadd(&fac3.z, &fac6.z);                 //  FPADD   FAC3.Z,FAC6.Z
    fadd(&fac3.x, &toscr.x);                //  FPADD   FAC3.X,TOSCR.X
    fadd(&fac3.y, &toscr.y);                //  FPADD   FAC3.Y,TOSCR.Y
    fadd(&fac3.z, &toscr.z);                //  FPADD   FAC3.Z,TOSCR.Z
    reserve[0].rv.x = fac3.x; reserve[0].rv.y = fac3.y; reserve[0].rv.z = fac3.z;
                                            //  MOVMEM  RESERVE.RV,FAC3,12
                                            //  lea     si,RESERVE.RV
    nrmlize(&reserve[0].rv);                //  call    NRMLIZE
    trace();                                //  call    TRACE
    bx = bp_2;                              //  mov     bx,word ptr -2[bp]
    ax = sred;                              //  mov     ax,SRED
    cond = ((ax) < (255));                  //  cmp     ax,255
    if (cond) {goto ma00red;}               //  jb      MA00RED
    ax = 255;                               //  mov     ax,255
  ma00red:
    // pred[bx] = ax;                          // MA00RED:      mov     PRED[bx],al
    subr += ax;
    ax = sgrn;                              //  mov     ax,SGRN
    cond = ((ax) < (255));                  //  cmp     ax,255
    if (cond) {goto ma00grn;}               //  jb      MA00GRN
    ax = 255;                               //  mov     ax,255
  ma00grn:
    // pgrn[bx] = ax;                          // MA00GRN:      mov     PGRN[bx],al
    subg += ax;
    ax = sblu;                              //  mov     ax,SBLU
    cond = ((ax) < (255));                  //  cmp     ax,255
    if (cond) {goto ma00blu;}               //  jb      MA00BLU
    ax = 255;                               //  mov     ax,255
  ma00blu:
    // pblu[bx] = ax;                          // MA00BLU:      mov     PBLU[bx],al
    subb += ax;

    subx += 1;
    goto main10sx;
  main10sxe:
    suby += 1;
    goto main10sy;
  main10sye:
    pred[bx] = subr / (subdiv * subdiv);
    pgrn[bx] = subg / (subdiv * subdiv);
    pblu[bx] = subb / (subdiv * subdiv);

    cond = ((monitor) == (0));              //  cmp     MONITOR,0
    if (cond) {goto main18;}                //  jz      MAIN18
    fprintf(stderr, "pixel value: x, y: (%d, %d):  [%3d, %3d, %3d]\n",
            bp_2, bp_4,
            pred[bx], pgrn[bx], pblu[bx]);
//      push    es                              ;|
//      mov     ax,CVRAM                        ;|
//      mov     es,ax                           ;|
//      lea     si,MONMES                       ;|
//      mov     di,23*160                       ;|  locate (0,23)
//      mov     cx,SIZEofMONMES                 ;|  size of monmess
// MAIN110:lodsb                                        ;|
//      cbw                                     ;|  ah <- 0
//      stosw                                   ;|
//      loop    MAIN110                         ;|
//      mov     di,23*160+8+8*1                 ;|=
//      mov     ax,word ptr (-2*1)[bp]          ;|| display X address
//      call    DECIMAL                         ;|=
//      mov     di,23*160+8+8*2                 ;|=
//      mov     ax,word ptr (-2*2)[bp]          ;|| display Y address
//      call    DECIMAL                         ;|=
//      mov     di,23*160+50                    ;|=  locate (25,23)
//      mov     al,PRED[bx]                     ;||
//      mov     ah,0                            ;||  display red
//      call    DECIMAL                         ;|=
//      mov     di,23*160+58                    ;|=  locate (29,23)
//      mov     al,PGRN[bx]                     ;||
//      mov     ah,0                            ;||  display green
//      call    DECIMAL                         ;|=
//      mov     di,23*160+66                    ;|=  locate (33,23)
//      mov     al,PBLU[bx]                     ;||
//      mov     ah,0                            ;||  display blue
//      call    DECIMAL                         ;|=
//      pop     es                              ;    restore es to tables
//      mov     ah,2                            ;=
//      mov     dl,0dh                          ;|   output CR to check ^C
//      int     21h                             ;=
  main18:
    ax = step;                              // MAIN18:  mov     ax,STEP
    bp_2 += ax;                             //  add     word ptr -2[bp],ax
    goto main10;                            //  jmp     MAIN10
  main19:
                                            // MAIN19:  mov     es,DATASEG
                                            //  push    word ptr -4[bp]
    writescanline(bp_4);                    //  call    WRITESCA
                                            //  pop     ax
    ax = step;                              //  mov     ax,STEP
    bp_4 += ax;                             //  add     word ptr -4[bp],ax
    goto main00;                            //  jmp     MAIN00
  main99:
                                            // MAIN99:  mov     es,DATASEG
                                            //  unlk
    goto endp;                              //  ret
                                            //
                                            //  public  DECIMAL
                                            //
                                            // MAINLOOP ENDP
  endp:;
}




                                            //
//
//
//      TRACE
//
//         trace from RESERVE[0]
//              returns color in S???
//
//      Args  : REAR
//
//      Ret   : S???
//
//
                                            //
                                            //  public  TRACE
/*
 * TRACE
 */

int ura;
real const1 = {0, 0, 0x8000};
real ref;

void trace(void) {
  int ah, al, ax, bx1, reflected, cond, si;
 Environment *bx;
 vector f0, fac0;
 real f1, f2;

    if ((verbose & 8)) {
      fprintf(stderr, "trace: start\n"); fflush(stderr);
    }
                                            // TRACE    PROC
                                            //
                                            //  link    2
    sred = 0;                               //  mov     SRED,0
    sgrn = 0;                               //  mov     SGRN,0
    sblu = 0;                               //  mov     SBLU,0
    first = 1;                              //  mov     FIRST,1
    rear = 0;                               //  mov     REAR,0
    bx1 = 0;                                //  mov     bx,0
    goto tr01;                              //  jmp     short TR01
  tr0:
    first = 0;                              // TR0:     mov     FIRST,0
    bx1 = front;                            //  mov     bx,FRONT
    cond = ((bx1) < (rear));                //  cmp     bx,REAR
    if (cond) {goto tr00;}                  //  jb      TR00
                                            //  unlk
    goto endp;                              //  ret
  tr00:
    bx1 += 1;                               // TR00:    add     bx,type ENVIRONMENT
  tr01:
    if ((verbose & 8)) {
      fprintf(stderr, "tr01: start\n"); fflush(stderr);
    }
    front = bx1;                            // TR01:    mov     FRONT,bx
    reflected = 0;                          //  mov     byte ptr -2[bp],0
    bx = &(reserve[bx1]);                   //  lea     bx,RESERVE[bx]
    v.x = bx->rv.x; v.y = bx->rv.y; v.z = bx->rv.z;
    o.x = bx->ro.x; o.y = bx->ro.y; o.z = bx->ro.z;
                                            //  MOVMEM  V,[bx].RV,24
    col.x = bx->re.x; col.y = bx->re.y; col.z = bx->re.z;
                                            //  MOVMEM  COL,[bx].RE,12
    side = bx->rside;
    rct = bx->rrct;
    dct = bx->rdct;
    prim = bx->rprim;
                                            //  MOVMEM  SIDE,[bx].RSIDE,6
    /* 0 means for ray crossing */          //  mov     si,0
    int r = search(bx, 0);                  //  call    SEARCH
    if (r == 0) {goto tr0;}                 //  jz      TR0
    cond = (!((side) == (-1)));             //  cmp     SIDE,-1
    if (cond) {goto tr1;}                   //  njne    TR1
    if ((verbose & 8)) {
        fprintf(stderr, "in transparent object. compute absorb\n"); fflush(stderr);
    }
    Primitive *pr = prim;                   //  mov     si,PRIM
    f0.x = pr->absorb.x; f0.y = pr->absorb.y; f0.z = pr->absorb.z;
                                            //  MOVMEM  F0,[si].Absorb,12
    fmlt(&f0.x, &t);                        //  FPMLT   F0.X,T
    fmlt(&f0.y, &t);                        //  FPMLT   F0.Y,T
    fmlt(&f0.z, &t);                        //  FPMLT   F0.Z,T
    fexp(&f0.x);                            //  FPEXP   F0.X
    fexp(&f0.y);                            //  FPEXP   F0.Y
    fexp(&f0.z);                            //  FPEXP   F0.Z
    if ((verbose & 8)) {
        fprintvector(stderr, "  absorb factor", &f0); fflush(stderr);
    }
    fmlt(&col.x, &f0.x);                    //  FPMLT   COL.X,F0.X
    fmlt(&col.y, &f0.y);                    //  FPMLT   COL.Y,F0.Y
    fmlt(&col.z, &f0.z);                    //  FPMLT   COL.Z,F0.Z
  tr1:;
    if ((verbose & 8)) {
        fprintf(stderr, "  find object local crspnt\n"); fflush(stderr);
    }
    Primitive *p = isprim;                  // TR1:     mov     bx,ISPRIM
    crspnt.x = pnt.x; crspnt.y = pnt.y; crspnt.z = pnt.z;
                                            //  MOVMEM  CRSPNT,PNT,<type VECTOR>
    vector *pn = &(pnt);                    //  lea     si,PNT
    fsub(&pn->x, &p->offset.x);             //  FPSUB   [si].X,[bx].Offst.X
    fsub(&pn->y, &p->offset.y);             //  FPSUB   [si].Y,[bx].Offst.Y
    fsub(&pn->z, &p->offset.z);             //  FPSUB   [si].Z,[bx].Offst.Z
    rot(p, &pnt);                           //  call    ROT
                                            //  lea     si,V
                                            //  lea     di,NML
    if ((verbose & 8)) {
        fprintvector(stderr, "tr1: object local point", pn); fflush(stderr);
    }
    normal(p, &v, &o, &pnt, &nml);          //  call    NORMAL
    textmap(p, &v, &o, &pnt);               //  call    TEXTMAP
    bumpmap(p, &v, &o, &pnt);               //  call    BUMPMAP
    p = isprim;                             //  mov     bx,ISPRIM
    cond = (!((p->surface) == (5)));        //  cmp     [bx].Surface,5
    if (cond) {goto tr10a;}                 //  jnz     TR10a
    /* beam */
    nml = pnt;                              //  MOVMEM  NML,PNT,<2*(type REAL)>
    nml.z.man = 0x8000; nml.z.exp = 5; nml.z.sign = 1;
                                            //  FPLOD   NML.Z,8000h,5,1
                                            //  lea     si,NML
    nrmlize(&nml);                          //  call    NRMLIZE
  tr10a:
                                            // TR10a:   lea     si,NML
    rot2(p, &nml);                          //  call    ROT2
    fac0.x = nml.x; fac0.y = nml.y; fac0.z = nml.z;
                                            //  MOVMEM  FAC0,NML,<type VECTOR>
    fmlt(&fac0.x, &v.x);                    //  FPMLT   FAC0.X,V.X
    fmlt(&fac0.y, &v.y);                    //  FPMLT   FAC0.Y,V.Y
    fmlt(&fac0.z, &v.z);                    //  FPMLT   FAC0.Z,V.Z
    fadd(&fac0.x, &fac0.y);                 //  FPADD   FAC0.X,FAC0.Y
    fadd(&fac0.x, &fac0.z);                 //  FPADD   FAC0.X,FAC0.Z
    cond = !((fac0.x).sign == 0);           //  FPSGN?  FAC0
    if (cond) {goto tr1000;}                //  jnz     TR1000
    ura = 1;                                //  mov     URA,1
    goto tr1001;                            //  jmp     short TR1001
  tr1000:
    ura = 0;                                // TR1000:  mov     URA,0
  tr1001:
    fac0.x.sign = 0;                        // TR1001:  FPABS   FAC0
    co = fac0.x;                            //  MOVMEM  CO,FAC0
    p = isprim;                             //  mov     bx,ISPRIM
    ref = p->refrate;                       //  MOVMEM  REF,[bx].Refrate
    cond = (!((p->surface) == (3)));        //  cmp     [bx].Surface,3
    if (cond) {goto tr2;}                   //  njne    TR2
    /* when exiting from transparent object, invert refraction index */
    cond = (!((side) == (-1)));             //  cmp     SIDE,-1
    if (cond) {goto tr11;}                  //  jne     TR11
    finv(&ref);                             //  FPINV   REF
  tr11:
    /* compute reflection energy */
    fsqar(&ref);                            // TR11:    FPSQAR  REF
    f2 = co;                                //  MOVMEM  F0,CO
    fsqar(&f2);                             //  FPSQAR  F0
    fadd(&f2, &ref);                        //  FPADD   F0,REF
    fsub(&f2, &const1);                     //  FPSUB   F0,Const1
    cond = ((f2).sign == 0);                //  FPSGN?  F0
    if (cond) {goto tr12;}                  //  jz      TR12
    /* total reflection */
    ref.man = 0x8000; ref.exp = 0; ref.sign = 0;
                                            //  FPLOD   REF,8000h,0,0
    goto tr2;                               //  jmp     TR2
  tr12:
    /* reflection energy */
    fsqrt(&f2);                             // TR12:    FPSQRT  F0
    f1 = f2;                                //  MOVMEM  F1,F0
    fadd(&f2, &co);                         //  FPADD   F0,CO
    fsqar(&f2);                             //  FPSQAR  F0
    fsub(&ref, &const1);                    //  FPSUB   REF,Const1
    fdiv(&ref, &f2);                        //  FPDIV   REF,F0
    fsqar(&ref);                            //  FPSQAR  REF
  tr2:
    diffuse(p);                             // TR2:     call    DIFFUSE
    p = isprim;                             //  mov     bx,ISPRIM
    cond = ((p->surface) == (1));           //  cmp     [bx].Surface,1
    if (cond) {goto tr0;}                   //  nje     TR0
    cond = (!((p->surface) >= (4)));        //  cmp     [bx].Surface,4
    if (cond) {goto tr2000;}                //  jnae    TR2000
                                            //  mov     bx,FRONT
    reserve[front].ro.x = crspnt.x; reserve[front].ro.y = crspnt.y; reserve[front].ro.z = crspnt.z;
                                            //  MOVMEM  RESERVE[bx].RO,CRSPNT,<type VECTOR>
                                            //  mov     ax,ISPRIM
    reserve[front].rprim = isprim;          //  mov     word ptr RESERVE[bx].RPRIM,ax
    first = 0;                              //  mov     FIRST,0
    goto tr01;                              //  jmp     TR01
  tr2000:
    al = dct;                               // TR2000:  mov     al,byte ptr RCT+1
    cond = ((al) < (maxdivct));             //  cmp     al,byte ptr MAXDIVCT
    if (cond) {goto tr20b;}                 //  jb      TR20b
    cond = ((ref.exp) < (-1));              //  cmp     REF.EXP,-1
    if (cond) {goto tr3;}                   //  njl     TR3
  tr20b:
    si = rear;                              // TR20b:   mov     si,REAR
    si += 1;                                //  add     si,type ENVIRONMENT
    al = rct; ah = dct;                     //  mov     ax,RCT
    al += 1;                                //  inc     al
    cond = (!((al) <= (maxrefct)));         //  cmp     al,byte ptr MAXREFCT
    if (cond) {goto tr3;}                   //  njnbe   TR3
    reserve[si].rrct = al;
    reserve[si].rdct = ah;                  //  mov     word ptr RESERVE[si].RRCT,ax
    reserve[si].re.x = col.x; reserve[si].re.y = col.y; reserve[si].re.z = col.z;
                                            //  MOVMEM  RESERVE[si].RE,COL,12
    fmlt(&reserve[si].re.x, &ref);          //  FPMLT   RESERVE[si].RE.X,REF
    fmlt(&reserve[si].re.y, &ref);          //  FPMLT   RESERVE[si].RE.Y,REF
    fmlt(&reserve[si].re.z, &ref);          //  FPMLT   RESERVE[si].RE.Z,REF
    f2 = reserve[si].re.x;                  //  MOVMEM  F0,RESERVE[si].RE.X
    fadd(&f2, &reserve[si].re.y);           //  FPADD   F0,RESERVE[si].RE.Y
    fadd(&f2, &reserve[si].re.z);           //  FPADD   F0,RESERVE[si].RE.Z
    ax = fcomp(&f2, &minenergy); cond = (ax < 0);
                                            //  FPCMP   F0,MINENERG
    if (cond) {goto tr3;}                   //  njl     TR3
    rear = si;                              //  mov     REAR,si
    bx = &(reserve[si]);                    //  lea     si,RESERVE[si]
    reflected = 1;                          //  mov     byte ptr -2[bp],1
    bx->ro.x = crspnt.x; bx->ro.y = crspnt.y; bx->ro.z = crspnt.z;
                                            //  MOVMEM  [si].RO,CRSPNT,12
    bx->rv.x = nml.x; bx->rv.y = nml.y; bx->rv.z = nml.z;
                                            //  MOVMEM  [si].RV,NML,12
    fmlt(&bx->rv.x, &co);                   //  FPMLT   [si].RV.X,CO
    fmlt(&bx->rv.y, &co);                   //  FPMLT   [si].RV.Y,CO
    fmlt(&bx->rv.z, &co);                   //  FPMLT   [si].RV.Z,CO
    fscale(&bx->rv.x, 1);                   //  FPSCALE [si].RV.X,1
    fscale(&bx->rv.y, 1);                   //  FPSCALE [si].RV.Y,1
    fscale(&bx->rv.z, 1);                   //  FPSCALE [si].RV.Z,1
    fadd(&bx->rv.x, &v.x);                  //  FPADD   [si].RV.X,V.X
    fadd(&bx->rv.y, &v.y);                  //  FPADD   [si].RV.Y,V.Y
    fadd(&bx->rv.z, &v.z);                  //  FPADD   [si].RV.Z,V.Z
    ax = side;                              //  mov     ax,SIDE
    bx->rside = ax;                         //  mov     [si].RSIDE,ax
    bx->rprim = p;                          //  mov     [si].RPRIM,bx
    choose(bx, p);                          //  call    CHOOSE
  tr3:
    cond = ((p->surface) == (2));           // TR3:     cmp     [bx].Surface,2
    if (cond) {goto tr0;}                   //  nje     TR0
    fsub(&ref, &const1);                    //  FPSUB   REF,Const1
    cond = (((ref).man & 0x8000) == 0);     //  FP0?    REF
    if (cond) {goto tr0;}                   //  nje     TR0
    ref.sign ^= 1;                          //  FPCHS   REF
    si = rear;                              //  mov     si,REAR
    si += 1;                                //  add     si,type ENVIRONMENT
    al = rct;
    ah = dct;                               //  mov     ax,RCT
    al += 1;                                //  inc     al
    cond = (!((al) <= (maxrefct)));         //  cmp     al,byte ptr MAXREFCT
    if (cond) {goto tr0;}                   //  njnbe   TR0
    cond = ((reflected) == (0));            //  cmp     byte ptr -2[bp],0
    if (cond) {goto tr30;}                  //  je      TR30
    ah += 1;                                //  inc     ah
    cond = (!((ah) <= (maxdivct)));         //  cmp     ah,byte ptr MAXDIVCT
    if (cond) {goto tr0;}                   //  njnbe   TR0
  tr30:
    reserve[si].rrct = al;
    reserve[si].rdct = ah;                  // TR30:    mov     word ptr RESERVE[si].RRCT,ax
    reserve[si].re.x = col.x; reserve[si].re.y = col.y; reserve[si].re.z = col.z;
                                            //  MOVMEM  RESERVE[si].RE,COL,12
    fmlt(&reserve[si].re.x, &ref);          //  FPMLT   RESERVE[si].RE.X,REF
    fmlt(&reserve[si].re.y, &ref);          //  FPMLT   RESERVE[si].RE.Y,REF
    fmlt(&reserve[si].re.z, &ref);          //  FPMLT   RESERVE[si].RE.Z,REF
    f2 = reserve[si].re.x;                  //  MOVMEM  F0,RESERVE[si].RE.X
    fadd(&f2, &reserve[si].re.y);           //  FPADD   F0,RESERVE[si].RE.Y
    fadd(&f2, &reserve[si].re.z);           //  FPADD   F0,RESERVE[si].RE.Z
    ax = fcomp(&f2, &minenergy); cond = (ax < 0);
                                            //  FPCMP   F0,MINENERG
    if (cond) {goto tr0;}                   //  njl     TR0
    rear = si;                              //  mov     REAR,si
    bx = &(reserve[si]);                    //  lea     si,RESERVE[si]
    bx->ro.x = crspnt.x; bx->ro.y = crspnt.y; bx->ro.z = crspnt.z;
                                            //  MOVMEM  [si].RO,CRSPNT,12
    bx->rv.x = nml.x; bx->rv.y = nml.y; bx->rv.z = nml.z;
                                            //  MOVMEM  [si].RV,NML,12
    fsub(&f1, &co);                         //  FPSUB   F1,CO
    f1.sign ^= 1;                           //  FPCHS   F1
    fmlt(&bx->rv.x, &f1);                   //  FPMLT   [si].RV.X,F1
    fmlt(&bx->rv.y, &f1);                   //  FPMLT   [si].RV.Y,F1
    fmlt(&bx->rv.z, &f1);                   //  FPMLT   [si].RV.Z,F1
    fadd(&bx->rv.x, &v.x);                  //  FPADD   [si].RV.X,V.X
    fadd(&bx->rv.y, &v.y);                  //  FPADD   [si].RV.Y,V.Y
    fadd(&bx->rv.z, &v.z);                  //  FPADD   [si].RV.Z,V.Z
    fdiv(&bx->rv.x, &p->n);                 //  FPDIV   [si].RV.X,[bx].N
    fdiv(&bx->rv.y, &p->n);                 //  FPDIV   [si].RV.Y,[bx].N
    fdiv(&bx->rv.z, &p->n);                 //  FPDIV   [si].RV.Z,[bx].N
    ax = side;                              //  mov     ax,SIDE
    ax = - ax;                              //  neg     ax
    bx->rside = ax;                         //  mov     [si].RSIDE,ax
    bx->rprim = p;                          //  mov     [si].RPRIM,bx
    choose(bx, p);                          //  call    CHOOSE
    goto tr0;                               //  jmp     TR0
                                            //
                                            // TRACE    ENDP
  endp:;
}


                                            //
//
//
//      SEARCH
//
//      Args  : bx      (Environment *)
//              si      0 : Search
//                      6 : Shadowing
//              SIDE    inside / outside
//              FIRST   first search or not
//
//      Ret   : ax      number of solution
//
//
                                            //
#define EXPMAR (-3)                         // EXPMAR   equ     -3
                                            //
                                            //  public  SEARCH
/*
 * SEARCH
 */

void begsrch(void), endsrch(void);
void begshdw(void), endshdw(void);
int solsrch(Primitive*, vector*, real*),
    solshdw(Primitive*);

int search(Environment *penv, int shadow) {
int ax, bl, nsol, cond;
real fac0;
vector wv, wo, fac1, fac4;
Andlist *pandlist;                          // Primitive *[MaxPrimInAL]
int i_andlist, i_orlist;                    // index for andlist, orlist
int i_prim, i_inout;                        // index for solve, inout
Primitive *save_solution_primitive;         // primitive where the ray hit
Network *pnetwork;                          // AlNum[MaxALinOL][MaxOrList] never loop

                                            // SEARCH   PROC
                                            //
                                            //  call    cs:SRCHTBL[si]
    if ((verbose & 1)) {
        fprintf(stderr, "search: start. penv=%p shadow=%d\n", (void*)penv, shadow);
        fflush(stderr);
        fprintvector(stderr, "  dir  ", &penv->rv);
        fprintvector(stderr, "  point", &penv->ro);
    }

    if (shadow) {
        begshdw();
    } else {
        begsrch();
    }
                                            //  link    8
    /* bp_10 := shadow */                   //  push    si
    nsol = 0;                               //  mov     word ptr -2[bp],0
    wv.x = penv->rv.x; wv.y = penv->rv.y; wv.z = penv->rv.z;
    wo.x = penv->ro.x; wo.y = penv->ro.y; wo.z = penv->ro.z;
                                            //  MOVMEM  WV,[bx].RV,24
    pnetwork = penv->rnet;                  //  mov     si,[bx].RNET
    /* bp_12 := penv */                     //  push    bx
    i_orlist = 0;                           //  mov     bx,si
  srch00:
                                            // SRCH00:  mov     word ptr -4[bp],bx
    bl = (*pnetwork)[i_orlist][0];  // orlist[0] is ranger number
                                            //  mov     bl,[bx]
    cond = ((bl) == (255)); /* no more orlist */
                                            //  cmp     bl,255
    if (cond) {goto srch0e;}                //  nje     SRCH0E
    if ((verbose & 1)) {
      fprintf(stderr, "srch00: i_orlist=%d\n", i_orlist); fflush(stderr);
    }
    cond = ((bl) == (NORANGE));             //  cmp     bl,NORANGE
    if (cond) {goto srch01;}                //  je      SRCH01
    Ranger *pran = &rang[bl];               //  mov     bh,0
                                            //  mov     cl,5   size of Ranger == 32
                                            //  shl     bx,cl
                                            //  lea     bx,RANG[bx]
                                            //  lea     si,WV
    cond = (!((first) == (0)));             //  cmp     FIRST,0
    if (cond) {goto srch001;}               //  jnz     SRCH001
    cond = (nrange(pran, &wv, &wo) < 0);    //  call    NRANGE
    goto srch002;                           //  jmp     short SRCH002
  srch001:
    cond = (irange(pran, &wv, &wo) < 0);    // SRCH001:call     IRANGE
  srch002:
    if (cond) {goto srch09;} /*skip orlist*/ // SRCH002:njl     SRCH09
  srch01:
                                            // SRCH01:  mov     bx,word ptr -4[bp]
    i_andlist = 0;                          //  mov     word ptr -6[bp],bx
  srch10:
                                            // SRCH10:  mov     bx,word ptr -6[bp]
    i_andlist++;                            //  inc     bx
                                            //  mov     word ptr -6[bp],bx
    bl = (*pnetwork)[i_orlist][i_andlist];  //  mov     bl,[bx]
    cond = (bl == 255);                     //  or      bl,bl
    if (cond) {goto srch09;} /*end of orlist */ //      njs     SRCH09
                                            //  mov     bh,0
                                            //  mov     cl,MaxPrimInAL + 1
                                            //  shl     bx,cl
    int alnum = bl;
    if ((verbose & 1)) {
        fprintf(stderr, "srch10: i_orlilst=%d i_andlist=%d alnum=%d\n", i_orlist, i_andlist, bl);
        fflush(stderr);
    }
    pandlist = &andlists[bl];               //  lea     si,ANDLISTS[bx]
    i_prim = 0;   /* bp_8 := top_and_list */
                                            //  mov     word ptr -8[bp],si
  srch20:;
    Primitive *p = (*pandlist)[i_prim];     // SRCH20:  mov     bx,[si]
    cond = ((p) == 0);                      //  or      bx,bx
    if (cond) {goto srch10;} /* end of andlist */ //    jz      SRCH10
    if ((verbose & 1)) {
        fprintf(stderr, "srch20: alnum=%d i_prim=%d\n", alnum, i_prim); fflush(stderr);
    }
    cond = ((p->surface) >= (4)); /*light source*/ //   cmp     [bx].Surface,4
    if (cond) {goto srch20a;}               //  jae     SRCH20a
    cond = ((shadow) == (0));               //  cmp     byte ptr -10[bp],0
    if (cond) {goto srch20x;}               //  jz      SRCH20X
  srch20a:
    cond = ((p) == (penv->rprim)); /* skip the start point primitive of the ray */
                                            // SRCH20a:mov      di,word ptr -12[bp]
                                            //  cmp     bx,[di].RPRIM
    if (cond) {goto srch10;}                //  jz      SRCH10
  srch20x:
    /* bp_14 := pandlist <0> */             // SRCH20X:push     si
                                            //  lea     si,WV
                                            //  lea     di,FAC0
    cond = solve(p, &wv, &wo, &fac0);       //  call    SOLVE
    /* bp_16 := p <1> */                    //  push    bx
    if (cond) {goto srch21;} /*no sol*/     //  njc     SRCH21
    save_solution_primitive = p;
    if ((verbose & 1)) {
        fprintreal(stderr, "  solution found: ", &fac0);
        fflush(stderr);
    }

    cond = (!((shadow) == (0)));            //  cmp     byte ptr -10[bp],0
    if (cond) {goto srch200;}               //  jnz     SRCH200
    ax = fcomp(&fac0, &t); cond = (!(ax < 0));
    if (verbose & 1) {
        fprintf(stderr, "  solution is %s than previous dist\n", cond ? "farther" : "closer");
        fprintreal(stderr, "    previous", &t);
        fflush(stderr);
    }
    /* if not shadow ray, find the closest solution only */
                                            //  FPCMP   FAC0,T
    if (cond) {goto srch21;} /* too far */  //  njnl    SRCH21
  srch200:  /* closest (so far) solution found */
                                            // SRCH200:lea      bx,FAC1
    linear(&wv, &wo, &fac0, &fac1);         //  call    LINEAR
    /* now fac1 == crossing point*/
    /* compute fac4 := slightly forward point on the ray */
    /*  (slightly backward if refracted ray goes from inside to outside */
    fac4.x = wv.x; fac4.y = wv.y; fac4.z = wv.z;
                                            //  MOVMEM  FAC4,[si],12
    fscale(&fac4.x, EXPMAR);                //  FPSCALE FAC4.X,EXPMAR
    fscale(&fac4.y, EXPMAR);                //  FPSCALE FAC4.Y,EXPMAR
    fscale(&fac4.z, EXPMAR);                //  FPSCALE FAC4.Z,EXPMAR
    if (side > 0) {
        fadd(&fac4.x, &fac1.x);
        fadd(&fac4.y, &fac1.y);
        fadd(&fac4.z, &fac1.z);
    } else {
        fsub(&fac4.x, &fac1.x); fac4.x.sign ^= 1; // fac4.x = fac1.x - fac4.x
        fsub(&fac4.y, &fac1.y); fac4.y.sign ^= 1;
        fsub(&fac4.z, &fac1.z); fac4.z.sign ^= 1;
    }
    if ((verbose & 1)) {
        fprintvector(stderr, "srch200: inout position", &fac4); fflush(stderr);
    }
                                            //  mov     di,offset cs:FSUBR
                                            //  cmp     SIDE,0
                                            //  js      SRCH201
                                            //  mov     di,offset cs:FADD

                                            // SRCH201:
                                            //  lea     si,FAC4.X
                                            //  lea     bx,FAC1.X
                                            //  push    di
                                            //  call    di
                                            //  pop     di
                                            //  lea     si,FAC4.Y
                                            //  lea     bx,FAC1.Y
                                            //  push    di
                                            //  call    di
                                            //  pop     di
                                            //  lea     si,FAC4.Z
                                            //  lea     bx,FAC1.Z
                                            //  push    di
                                            //  call    di
                                            //  pop     di
                                            //  lea     si,FAC4
    i_inout = 0;                            //  mov     bx,word ptr -8[bp]
  srch30:
    if ((verbose & 1)) {
        fprintf(stderr, "  i_inout=%d\n", i_inout);
    }
    cond = (i_inout == i_prim);             // SRCH30:  cmp     bx,word ptr -14[bp]
    if ((verbose & 1) && cond) {
        fprintf(stderr, "  skip inout self\n");
    }
    if (cond) {goto srch39;} /* skip self */ //         jz      SRCH39
                                            //  push    bx
    p = (*pandlist)[i_inout];               //  mov     bx,[bx]
    cond = (p == 0);                        //  or      bx,bx
    if ((verbose & 1) && cond) {
        fprintf(stderr, "  andlist finished.  inout check OK\n");
    }
    if (cond) {goto srch32;} /* sol found */ //         jz      SRCH32
    cond = (inout(p, &fac4) < 0);           //  call    INOUT
                                            //  pop     bx
    if ((verbose & 1)) {
        fprintf(stderr, "  inout(i_inout): %s\n", cond ? "out" : "in");
    }
    if (cond) {goto srch21;} /* out of andlist */
                                            //  jl      SRCH21
  srch39:
    i_inout++;
    goto srch30;                            //  jmp     SRCH30
  srch32: /* end of andlist: ray hit */
                                            // SRCH32:  pop     ax
    p = save_solution_primitive;            //  pop     bx
    nsol += 1; cond = (!((nsol) < 0));      //  inc     word ptr -2[bp]
    if (shadow == 0) {                      //  mov     si,word ptr -10[bp]
        cond = solsrch(p, &fac1, &fac0);
    } else {
        cond = solshdw(p);
    }
                                            //  call    cs:SRCHTBL[si+2]
    if (!cond) {goto srch22;}               //  jnc     SRCH22
                                            //  pop     ax
    goto srch0e;                            //  jmp     SRCH0E
  srch21: /* no solution (inout check fail) */
    if ((verbose & 1)) {
        fprintf(stderr, "  srch21: no solution for inout check\n");
        fflush(stderr);
    }

                                            // SRCH21:  pop     ax
  srch22:
                                            // SRCH22:  pop     si
    i_prim++;                               //  inc     si
                                            //  inc     si
    goto srch20;                            //  jmp     SRCH20
  srch09:
    i_orlist++;                             // SRCH09:  mov     bx,word ptr -4[bp]
                                            //  add     bx,MaxALinOL
    goto srch00;                            //  jmp     SRCH00
  srch0e:
                                            // SRCH0E:  pop     bx
                                            //  pop     si
    if (shadow == 0) {
        endsrch();
    } else {
        endshdw();
    }                                       //  call    cs:SRCHTBL[si+4]
    ax = nsol;                              //  mov     ax,word ptr -2[bp]
                                            //  or      ax,ax
                                            //  unlk
    goto endp;                              //  ret
                                            //
                                            //  even
                                            // SRCHTBL  dw      BEGSRCH
                                            //  dw      SOLSRCH
                                            //  dw      ENDSRCH
                                            //  dw      BEGSHDW
                                            //  dw      SOLSHDW
                                            //  dw      ENDSHDW
                                            //
                                            // SEARCH   ENDP
  endp:
    if ((verbose & 1)) {
        fprintf(stderr, "search result = %d\n", ax); fflush(stderr);
    }
    return ax;
}

void begsrch(void) {
    t.man = 0x0ffff; t.exp = 0x7f; t.sign = 0; /* infinity */
                                            // BEGSRCH:FPLOD    T,0ffffh,7fh,0
                                            //  ret
}
                                            //
int cbx;
int solsrch(Primitive *p, vector *cross_point, real *distance) {
    if (verbose & 1) {
        fprintf(stderr, " solsrch prim=%p\n", (void*)p);
        fprintreal(stderr, "  dist", distance);
        fflush(stderr);
    }
    isprim = p;                             // SOLSRCH:mov      ISPRIM,bx
    pnt.x = cross_point->x; pnt.y = cross_point->y; pnt.z = cross_point->z;
                                            //  MOVMEM  PNT,FAC1,12
    t = *distance;                          //  MOVMEM  T,FAC0
                                            //  mov     ax,CUBEX
    cbx = cubex;  /* remember cube surface */ //        mov     CBX,ax
    return 0;                               //  clc
}                                           //  ret
                                            //
void endsrch(void){                             // ENDSRCH:mov  ax,CBX
    if (verbose & 1) {
        fprintreal(stderr, " endsrch t", &t);
        fflush(stderr);
    }
    cubex = cbx;                            //  mov     CUBEX,ax
                                            //  ret
}

real bright, ref;

void begshdw(void) {
    bright.man = 0x8000; bright.exp = 0; bright.sign = 0;
                                            // BEGSHDW:FPLOD    BRIGHT,8000h,0,0
}
void endshdw(void) {
    /* nop */                               // ENDSHDW:ret
}
                                            //
int solshdw(Primitive *p) {
  int cond;
                                            // SOLSHDW:cmp      [bx].Surface,3
    if (p->surface == 3) {goto slsd2;}      //  jz      SLSD2
    if (p->surface > 3) {goto slsd12;}      //  jnbe    SLSD12
  slsd1:
    /* opaque object */
    bright.man = 0; bright.exp = 0; bright.sign = 0;
                                            // SLSD1:   FPLOD   BRIGHT,0,0,0
  slsd11:
    return 1;                               // SLSD11:  stc
                                            //  ret

  slsd2:
    /* clear object. not really correct */
    fsub(&bright, &grade);                  // SLSD2:   FPSUB   BRIGHT,GRADE
    cond = !(bright.sign == 0);             //  FPSGN?  BRIGHT
    if (cond) {goto slsd1;}  /* dark enough */ //       jne     SLSD1
    cond = ((bright.man & 0x8000) == 0);    //  FP0?    BRIGHT
    if (cond) {goto slsd11;} /* exactly 0 */ //         jz      SLSD11
  slsd12:
    return 0;                               // SLSD12:  clc
                                            //  ret
}



                                            //
                                            // @code    ends
                                            //
                                            // @datau   segment
                                            //
                                            // CBX      dw      ?
                                            // WV       db      (type VECTOR)*2 dup (?)
                                            // BRIGHT   REAL    <?,?,?>
                                            //  public  BRIGHT
                                            // REF      REAL    <?,?,?>
                                            //  public  REF
                                            //
                                            // @datau   ends
                                            //
                                            // @code    segment
                                            //

//
//
//      DIFFUSION
//
//
                                            //
                                            //  public  DIFFUSE
/*
 * DIFFUSE
 */

void diffuse(Primitive *p) {
  int ax, self_shadow, other_shadow, cond, cx;
  real fac11, fac8, fac9;
  vector fac7, fac10;
  Environment env;
    if ((verbose & 4)) {
        fprintf(stderr, "diffuse: prim=%p\n", (void*)p); fflush(stderr);
        fprintf(stderr, "  side=%d\n", side);
        fprintf(stderr, "  prim kind=%d sgn=%d rotation=%d\n", p->kind, p->sgn, p->rotation);
        fprintvector(stderr, "  prim param ", &p->param);
        fprintvector(stderr, "  prim offset", &p->offset);
        fflush(stderr);
    }

                                            // DIFFUSE  PROC
                                            //
                                            // ENV      equ     type ENVIRONMENT + 2
                                            //
                                            //  link    ENV
    self_shadow = 0;
    other_shadow = 0;                       //  mov     word ptr -2[bp],0
    fac7 = light;                           //  MOVMEM  FAC7,LIGHT,12
    fmlt(&fac7.x, &nml.x);                  //  FPMLT   FAC7.X,NML.X
    fmlt(&fac7.y, &nml.y);                  //  FPMLT   FAC7.Y,NML.Y
    fmlt(&fac7.z, &nml.z);                  //  FPMLT   FAC7.Z,NML.Z
    fadd(&fac7.x, &fac7.y);                 //  FPADD   FAC7,FAC8
    fadd(&fac7.x, &fac7.z);                 //  FPADD   FAC7,FAC9
    real lambert = fac7.x;
    if ((verbose & 4)) {
        fprintreal(stderr, "  cos(nml.light)", &lambert); fflush(stderr);
    }
    cond = (lambert.sign == 0);             //  FPSGN?  FAC7
    if (cond) {goto df1;}                   //  jz      DF1
    lambert.sign ^= 1;                      //  FPCHS   FAC7
    self_shadow = 1;                        //  mov     byte ptr -2[bp],1
  df1:
    fac8.man = 0x8000; fac8.exp = 0; fac8.sign = 0;
                                            // DF1:     FPLOD   FAC8,8000h,0,0
    cond = ((shadowing) == (0));            //  cmp     SHADOWIN,0
    if (cond) {goto df2;}                   //  jz      DF2
    cond = ((p->surface) >= (4));           //  cmp     [bx].Surface,4
    if (cond) {goto df2;}                   //  jae     DF2
    cond = ((self_shadow) == (0));          //  cmp     byte ptr -2[bp],0
    if (cond) {goto df11;}                  //  je      DF11
    if ((verbose & 4)) {
        fprintf(stderr, "  in self shadow\n"); fflush(stderr);
    }
    fac8.man = 0; fac8.exp = 0; fac8.sign = 0;
                                            //  FPLOD   FAC8,0,0,0
    goto df2;                               //  jmp     short DF2
  df11:
    env.rv = light;                         // DF11:    MOVMEM  -ENV[bp].RV,LIGHT,12
    env.ro = crspnt;
                                            //  MOVMEM  -ENV[bp].RO,CRSPNT,12
                                            //  mov     bx,ISPRIM
    if ((verbose & 4)) {
        fprintf(stderr, "  shadow raycast\n");
        fprintvector(stderr, "    light dir", &env.rv);
        fprintvector(stderr, "    start pnt", &env.ro); fflush(stderr);
    }
    env.rprim = isprim;                     //  mov     -ENV[bp].RPRIM,bx
                                            //  mov     ax,[bx].SH
    env.rnet = isprim->sh;                  //  mov     -ENV[bp].RNET,ax
                                            //  lea     bx,-ENV[bp]
                                            //  mov     si,6
    first = 0;                              //  mov     FIRST,0
    cond = search(&env, 6); /* 6 for shadow */ //       call    SEARCH
    if ((verbose & 4)) {
        fprintf(stderr, "diffuse: shadow ray %s\n", cond ? "hit" : "none");
        fflush(stderr);
    }
    if (cond == 0) {goto df12;}
                                            //  jz      DF12
    other_shadow = 1;                       //  mov     byte ptr -1[bp],1
  df12:
    fac8 = bright;                          // DF12:    MOVMEM  FAC8,BRIGHT
  df2:
                                            // DF2:     mov     bx,ISPRIM
    cond = (!((isprim->surface) >= (4)));   //  cmp     [bx].Surface,4
    if (cond) {goto df29;}                  //  njnae   DF29
    // light source object
    if ((verbose & 4)) {
        fprintf(stderr, "diffuse: light source object\n");
        fflush(stderr);
    }
    cond = (!((ura) == (0)));               //  cmp     URA,0
    if (cond) {goto df4;}                   //  njnz    DF4
    fac10.x = isprim->solid.x; fac10.y = isprim->solid.y; fac10.z = isprim->solid.z;
                                            //  MOVMEM  FAC10,[bx].Solid,<type VECTOR>
    fac9 = co;                              //  MOVMEM  FAC9,CO
    cx = isprim->diff;                      //  mov     cx,word ptr [bx].Refrate
    if (cx == 0) {goto df201;}              //  jcxz    DF201
  df202:
                                            // DF202:   push    cx
    fsqar(&fac9);                           //  FPSQAR  FAC9
                                            //  pop     cx
    if (--cx != 0) {goto df202;}            //  loop    DF202
  df201:
                                            // DF201:
    fmlt(&fac10.x, &fac9);                  //  FPMLT   FAC10.X,FAC9
    fmlt(&fac10.y, &fac9);                  //  FPMLT   FAC10.Y,FAC9
    fmlt(&fac10.z, &fac9);                  //  FPMLT   FAC10.Z,FAC9
    fmlt(&fac10.x, &col.x);                 //  FPMLT   FAC10.X,COL.X
    fmlt(&fac10.y, &col.y);                 //  FPMLT   FAC10.Y,COL.Y
    fmlt(&fac10.z, &col.z);                 //  FPMLT   FAC10.Z,COL.Z
                                            //  FRTOIF  FAC10.X
    sred += rtoif(&fac10.x);                //  add     SRED[0*2],ax
                                            //  FRTOIF  FAC10.Y
    sblu += rtoif(&fac10.y);                //  add     SRED[1*2],ax
                                            //  FRTOIF  FAC10.Z
    sgrn += rtoif(&fac10.z);                //  add     SRED[2*2],ax
    goto df4;                               //  jmp     DF4
  df29:
                                            // DF29:    mov     ax,word ptr -2[bp]
                                            //  or      al,ah
    if (!(self_shadow == 0 && other_shadow == 0)) {goto df3;}
                                            //  njnz    DF3
    if ((verbose & 4)) {
        fprintf(stderr, "diffuse: Phone specular model\n");
        fflush(stderr);
    }
    fac9 = co;                              //  MOVMEM  FAC9,CO
    fscale(&fac9, 1);                       //  FPSCALE FAC9,1
    fac10 = nml;                            //  MOVMEM  FAC10,NML,12
    fmlt(&fac10.x, &fac9);                  //  FPMLT   FAC10.X,FAC9
    fmlt(&fac10.y, &fac9);                  //  FPMLT   FAC10.Y,FAC9
    fmlt(&fac10.z, &fac9);                  //  FPMLT   FAC10.Z,FAC9
    fadd(&fac10.x, &v.x);                   //  FPADD   FAC10.X,V.X
    fadd(&fac10.y, &v.y);                   //  FPADD   FAC10.Y,V.Y
    fadd(&fac10.z, &v.z);                   //  FPADD   FAC10.Z,V.Z
    fmlt(&fac10.x, &light.x);               //  FPMLT   FAC10.X,LIGHT.X
    fmlt(&fac10.y, &light.y);               //  FPMLT   FAC10.Y,LIGHT.Y
    fmlt(&fac10.z, &light.z);               //  FPMLT   FAC10.Z,LIGHT.Z
    fadd(&fac10.x, &fac10.y);               //  FPADD   FAC10,FAC11
    fadd(&fac10.x, &fac10.z);               //  FPADD   FAC10,FAC12
    real spec = fac10.x; /* specular strength */
    cond = !(spec.sign == 0);               //  FPSGN?  FAC10
    if (cond) {goto df3;}                   //  njnz    DF3
    fsqar(&spec);                           //  FPSQAR  FAC10
    fsqar(&spec);                           //  FPSQAR  FAC10
    fmlt(&spec, &fac8);                     //  FPMLT   FAC10,FAC8
                                            //  mov     bx,ISPRIM
    fmlt(&spec, &isprim->highlight);        //  FPMLT   FAC10,[bx].Highlight
    fac11 = spec;                           //  MOVMEM  FAC11,FAC10
    fmlt(&fac11, &col.x);                   //  FPMLT   FAC11,COL.X
    ax = rtoif(&fac11);                     //  FRTOIF  FAC11
    cond = ((ax) < 0);                      //  or      ax,ax
    if (cond) {goto df3a0;}                 //  js      DF3A0
    sred += ax;                             //  add     SRED[2*0],ax
  df3a0:
                                            // DF3A0:
    fac11 = spec;                           //  MOVMEM  FAC11,FAC10
    fmlt(&fac11, &col.y);                   //  FPMLT   FAC11,COL.Y
    ax = rtoif(&fac11);                     //  FRTOIF  FAC11
    cond = ((ax) < 0);                      //  or      ax,ax
    if (cond) {goto df3a1;}                 //  js      DF3A1
    sgrn += ax;                             //  add     SRED[2*1],ax
  df3a1:
                                            // DF3A1:
    fac11 = spec;                           //  MOVMEM  FAC11,FAC10
    fmlt(&fac11, &col.z);                   //  FPMLT   FAC11,COL.Z
    ax = rtoif(&fac11);                     //  FRTOIF  FAC11
    cond = ((ax) < 0);                      //  or      ax,ax
    if (cond) {goto df3a2;}                 //  js      DF3A2
    sblu += ax;                             //  add     SRED[2*2],ax
  df3a2:
                                            // DF3A2:
  df3:
    if ((verbose & 4)) {
        fprintf(stderr, "diffuse: Lambert object in light\n");
        fflush(stderr);
    }
                                            // DF3:     mov     bx,ISPRIM
    cond = ((isprim->surface) == (3));      //  cmp     [bx].Surface,3
    if (cond) {goto df4;}                   //  nje     DF4
    /* Lambert object */
    {
      real fac7 = lambert;
      if (verbose & 4) {
          fprintreal(stderr, "diffuse: cos(nml.light)", &lambert);
          fprintreal(stderr, "  bright", &fac8);
      }
    fmlt(&fac7, &fac8);                     //  FPMLT   FAC7,FAC8
    fmlt(&fac7, &invbg);                    //  FPMLT   FAC7,INVBG
    fadd(&fac7, &background);               //  FPADD   FAC7,BACKGROU
      if (verbose & 4) {
          fprintreal(stderr, "  incl. ambient", &fac7);
      }

    fac8 = ref;                             //  MOVMEM  FAC8,REF
    fsub(&fac8, &const1);                   //  FPSUB   FAC8,Const1
    fac8.sign ^= 1;                         //  FPCHS   FAC8
      if (verbose & 4) {
          fprintreal(stderr, "  non-ref energy", &fac8);
          fflush(stderr);
      }
    fmlt(&fac7, &fac8);                     //  FPMLT   FAC7,FAC8
    {      vector fac8;
        fac8 = isprim->solid;
                                            //  MOVMEM  FAC8,[bx].Solid,12
    fmlt(&fac8.x, &fac7);                   //  FPMLT   FAC8.X,FAC7
    fmlt(&fac8.y, &fac7);                   //  FPMLT   FAC8.Y,FAC7
    fmlt(&fac8.z, &fac7);                   //  FPMLT   FAC8.Z,FAC7
    fmlt(&fac8.x, &col.x);                  //  FPMLT   FAC8.X,COL.X
    fmlt(&fac8.y, &col.y);                  //  FPMLT   FAC8.Y,COL.Y
    fmlt(&fac8.z, &col.z);                  //  FPMLT   FAC8.Z,COL.Z
    ax = rtoif(&fac8.x);                    //  FRTOIF  FAC8.X
    cond = ((ax) < 0);                      //  or      ax,ax
    if (cond) {goto df30;}                  //  js      DF30
    sred += ax;                             //  add     SRED[0*2],ax
  df30:
                                            // DF30:
    ax = rtoif(&fac8.y);                    //  FRTOIF  FAC8.Y
    cond = ((ax) < 0);                      //  or      ax,ax
    if (cond) {goto df31;}                  //  js      DF31
    sgrn += ax;                             //  add     SRED[1*2],ax
  df31:
                                            // DF31:
    ax = rtoif(&fac8.z);                    //  FRTOIF  FAC8.Z
    cond = ((ax) < 0);                      //  or      ax,ax
    if (cond) {goto df32;}                  //  js      DF32
    sblu += ax;                             //  add     SRED[2*2],ax
  df32:;
    }}
                                            // DF32:
  df4:
                                            // DF4:     unlk
    goto endp;                              //  ret
                                            //
                                            // DIFFUSE  ENDP
  endp:;
}


                                            //
//
//
//      CHOOSE
//
//
                                            //
                                            //  public  CHOOSE
/*
 * CHOOSE
 */

void choose(Environment *penv, Primitive *p) {
int cond, dl;
Network *ax;
                                            // CHOOSE   PROC
                                            //
    dl = 0;                                 //  mov     dx,0
    cond = ((penv->rv.x).sign == 0);        //  FPSGN?  [si].RV.X
    if (cond) {goto ch1;}                   //  je      CH1
    dl = 2;                                 //  mov     dl,4
  ch1:
    cond = ((penv->rv.y).sign == 0);        // CH1:     FPSGN?  [si].RV.Y
    if (cond) {goto ch2;}                   //  je      CH2
    dl += 1;                                //  add     dl,2
  ch2:
                                            // CH2:     xchg    si,dx
    switch (dl) {
    case 0: ax = p->ru; break;
    case 1: ax = p->rd; break;
    case 2: ax = p->lu; break;
    case 3: ax = p->ld; break;
    }                                       //  mov     ax,[bx].RU[si]
                                            //  xchg    si,dx
    penv->rnet = ax;                        //  mov     [si].RNET,ax
    goto endp;                              //  ret
                                            //
                                            // CHOOSE   ENDP
  endp:;
}


                                            //
                                            //  include epilogue.h
                                            //
                                            //  end
