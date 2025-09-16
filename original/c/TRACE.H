/************************************************************************


		  HEADER FILE FOR RAYTRACING C PROGRAMS

			   by    Kaoru MAEDA

			      15-Oct-1985
			      18-Nov-1985


************************************************************************/

/****************************************************************************

NOTICE :
	Before including this header file, please define EXTERN as follows

		#define EXTERN		    for main program
		#define EXTERN extern	    for sub programs


****************************************************************************/

#include "stdio2.h"

/****************************************************
		CONSTANT DEFINITION
****************************************************/

/*   SCREENS   */
#define	HDOTSIZE	640
#define	VDOTSIZE	512
#define	XOFFSET		(HDOTSIZE/2)
#define	YOFFSET		(VDOTSIZE/2)
#define	HSCRSIZE	80.0
#define	VSCRSIZE	64.0

/*   LIMITS   */
#define	MAXREFCT	8		/* 最大反射回数 */
#define	MAXDIVCT	4		/* 最大光線分割回数 */
#define	MINENERGY	0.05		/* 最小エネルギー */

/*   ENVIRONMENT   */
#define	BACKGROUND	.2		/* 環境拡散反射係数 */
#define GRADE		.1875		/* 透明体一つ当たりの減衰率 */
#define	MAXLEVEL	255

/*   KIND OF SURFACE   */
#define	OPAQUE		1		/* 不透明    乱反射	       */
#define	MIRROR		2		/* 鏡面      乱反射 + 鏡面反射 */
#define	CLEAR		3		/* 透明      屈折   + 鏡面反射 */
#define LIGHT		4		/* 自己発光体 */
#define BEAM		5		/* Beam along Z axis */

/*   PRIMITIVE FORM   */
#define	CUBE		1		/* 直方体 */
#define	PLAIN		2		/* 平面 */
#define	SECOND		3		/* 二次曲面 */
#define	CONE		4		/* 円錐 */

/*   KIND OF TEXTURE MAPPING   */
#define	UNFIG		0		/* 無地 */
#define	TILE		1		/* タイル */
#define	STRIPE		2		/* ストライプ */
#define	RING		3		/* 輪 */
#define	DOT		4		/* 斑点 */
#define	CHECK		5		/* チェック */
#define	USER		10		/* User 定義 の Offset */

/*   PROGRAM INTERNAL CONSTANTS   */
#define	INSIDE		-1		/* 現在位置はPrimitiveの中 */
#define	OUTSIDE		 1		/*			外 */
#define	EOL		255		/* End of List */
#define	NORANGE		99		/* Range Check なし */

/*   SIZE OF ARRAYS   */
#define	MAXPRIM		128		/* Primitive の 最大数 */
#define	MaxAndList	128		/* Andlist の 最大数 */
#define	MaxPrimInAL	32		/* １つのAL中 の Prim の 最大数 */
#define	MaxOrList	32		/* １つのNet中 の Orlist の 最大数 */
#define	MaxALinOL	16		/* １つのOL中 の AL の 最大数 */
#define	MAXRANGE	MaxOrList	/* Ranger Primitive の 最大数 */
#define	MAXNET		3		/* Network の 最大数 */
#define	MAXUTEX		10		/* User Texture の 最大数 */
#define	MAXBUMP		10		/* Bump Mapping の 最大数 */
#define MAXRAY		(MAXREFCT*MAXDIVCT*2)

/************************************************
		TYPE DEFINITION
************************************************/

typedef struct {
    char		exp;		/*   exponent   */
    unsigned char	sign;		/*     sign     */
    unsigned short	man;		/*   mantissa   */
}   real;

typedef real vector[3];			/* vector は real の かたまり */

typedef unsigned char	ALNumber;	/* 便宜上の型 */

typedef ALNumber	Orlist[MaxALinOL];	/* Andlist 並びは Orlist */
typedef Orlist		Network[MaxOrList];	/* Orlist 並びは Network */

typedef struct {		/* Primitive */
    short
	kind,				/* Primitive の 形状 */
	texture,			/* Texture Mapping の 種類 */
	bump,				/* Bump Mapping の 種類 */
	surface,			/* 表面特性 */
	sgn;				/* Inside / Outside */
    BOOL
	rotation;			/* Rotation の あり / なし */
    real
	highlight;			/* Highlight 強度 */
    union {
	real
	    refrate,			/* 反射係数 */
	    n;				/* 屈折率 */
	int
	    diff;			/* 発光体の拡散係数 */
    }
	ratio;
    vector
	param,				/* Primitive 形状 Parameter */
	offset;				/* Primitive の World 座標 */
    real
	*matrix;			/* Rotation matrix */
    union {
	vector
	    absorb,			/* 吸収係数 */
	    solid;			/* 乱反射の色 */
    }
	color;
    Network				/* Network への Pointer */
	*sh,				/* Shadowing 用 */
	*ru,				/* Right Up   用 */
	*rd,				/* Right Down 用 */
	*lu,				/* Left  Up   用 */
	*ld;				/* Left  Down 用 */
}   Primitive;

typedef Primitive *Andlist[MaxPrimInAL];	/* PrimへのPointerの並びはAL */

typedef struct {		/* Ray Queue 用 */
    vector	rv,			/* 視線の方向 */
		ro,			/* 視点の位置 */
		re;			/* 減衰率 */
    short	rside,			/* Inside / Outside */
		rrct;			/* 反射の回数 */
    Primitive	*rprim;			/* Primitive */
    Network	*rnet;			/* Network */
}   Environment;

typedef struct {		/* Ranger Primitive */
    vector	direction;		/* 初回Scan用  視点から中心への方向 */
    real	mincos;			/*	       視差半径(?)の余弦    */
    vector	location;		/* 中心の座標 */
    real	radius;			/* 半径の自乗 */
}   Ranger;

/************************************************
		GLOBAL VARIABLES
************************************************/

extern	VOID	(*usertext[MAXUTEX])(),	 /* User Texture 関数アドレスの配列 */
		(*bumping [MAXBUMP])();  /* Bump Mapping 関数アドレスの配列 */

#ifdef _INIT
EXTERN	int			/* 定数Register  マシン語用 */
		xoffset = XOFFSET, yoffset = YOFFSET,
		maxray = MAXRAY,
		maxrefct = MAXREFCT,
		maxdivct = MAXDIVCT;
#endif

EXTERN	real
		background,
		invbg,			/* 1.0 - <環境拡散反射係数> */
		grade,
		minenergy;

EXTERN	Network	networks[MAXNET];		/* Network の 配列 */

EXTERN	Primitive pr[MAXPRIM];
EXTERN	Ranger	  rang[MAXRANGE];
EXTERN	Andlist	  andlists[MaxAndList];

EXTERN	short
		ystart,			/* Ｙ座標の始め */
		ystop,			/* Ｙ座標の終わり */
		xstart,
		xstop,
		step,			/* Ｙ座標の刻み */
		sred, sgrn, sblu;	/* 画素の色 */

EXTERN	int
		cubex,			/* 直方体のどの面で交わったか */
		front, rear;		/* Ray Queue 用 */

EXTERN	BOOL	shadowing,		/* Shadowing の あり / なし */
		monitor;		/* Monitor の あり / なし */

EXTERN	vector
		viewpt,			/* 視点の位置 */
		light,			/* 光源の方向 */
		eh, ev,			/* 単位画素Vector */
		toscr;			/* viewpt - scrorg */

EXTERN	unsigned char
		pred[HDOTSIZE], pgrn[HDOTSIZE], pblu[HDOTSIZE];
					/* 画素Buffer */

EXTERN	Environment reserve[MAXRAY];	/* Ray Queue 用 */

EXTERN	BOOL	first;			/* 最初の探索か */

EXTERN	vector	v,			/* 視線Vector */
		o,			/* 視点の位置 */
		pnt,			/* 交点のローカル座標 */
		nml,			/* 法線Vector */
		col,			/* 色 */
		crspnt;			/* 交点の座標 */

EXTERN	short	side,			/* Inside / Outside flag */
		rct;			/* 反射の回数 */

EXTERN	Primitive *prim,		/* 視点のあるPrimitive */
		  *isprim;		/* 探索すべきPrimitive */

EXTERN	real	co,			/* 法線と視線のなす角 */
		t;			/* 視点から交点までの距離 */

EXTERN	char	desname[255];		/* Destination Filename */
EXTERN	FILE	*dfp;			/* Destination Stream */

/***********************************
	EXTERNAL FUNCTIONS
***********************************/

double	sin(), cos(), sqrt(), rtof();
VOID	fadd(), fsub(), fmlt(), fdiv(), fsqrt(), fsqar(), finv(), fscale(),
	fexp(), flog(), normal(), nrmlize(), linear(), solve();
int	fcomp(), rtoi(), rtoif(), inout();
long int inter();

/**********************************************

		END OF HEADER

**********************************************/

