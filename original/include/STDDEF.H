#ifdef ___regval

struct	regval {
	int	ax, bx, cx, dx, si, di, ds, es;
};

#define	CarryFlag		0x001
#define ParityFlag		0x004
#define	AuxCarryFlag		0x010
#define	ZeroFlag		0x040
#define	SignFlag		0x080
#define TrapFlag		0x100
#define	InterruptEnableFlag	0x200
#define	DirectionFlag		0x400
#define	OverflowFlag		0x800

#endif

#ifdef ___segreg

struct	segreg {
	int	scs, sss, sds, ses;
};

#endif

#ifdef ___fileio

#include <fileio2.h>

#endif

#ifdef ___seek

#define BEGINofFILE	0
#define CURRENT		1
#define ENDofFILE	2

#endif

#undef ___regval
#undef ___segreg
#undef ___fileio
#undef ___seek
