/*
 * REALA.H - 1985年版レイトレーシング用浮動小数点演算ライブラリ
 * 
 * REALA.ASMのC言語移植版
 * 独自REAL型（4バイト）による高精度演算を実装
 * 
 * REAL型仮数部: 16bit全てが有効（hidden 1なし）
 * - 通常時は最上位ビット（MSB）が1
 * - ゼロの場合のみMSBが0  
 * - 対数テーブル参照: 左1ビットシフトでMSB除去、下位15bitをインデックス使用
 *
 * Original: by K.Maeda (1985)
 * C Port: 2025
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* REAL型構造体定義 */
typedef struct {
    int8_t exp;    /* 指数部 */
    uint8_t sign;   /* 符号部 */
    uint16_t man;   /* 仮数部 */
} Real;

#define real Real // 最近のCはrealが予約語?

/* 定数定義 */
#define REAL_ZERO_EXP 0
#define REAL_MAX_EXP 0x7F
#define REAL_MIN_EXP -0x80
#define REAL_SIGN_MASK 0x01
#define REAL_MAN_MSB 0x8000

/* 対数テーブル（logtbl1.asmから抽出） */
extern const uint16_t logtbl[32768];

/* 指数テーブル（exptbl1.asmから抽出） */
extern const uint16_t exptbl[32768];

/* 関数プロトタイプ */
void fadd(Real *dest, const Real *src);
void fsub(Real *dest, const Real *src);
void fmlt(Real *dest, const Real *src);
void fdiv(Real *dest, const Real *src);
void fsqrt(Real *dest);
void fsqar(Real *dest);
void fscale(Real *dest, int8_t scale);
void finv(Real *dest);
void flog(Real *dest);
void fexp(Real *dest);
int fcomp(const Real *a, const Real *b);
void itor(Real *dest, int16_t value);
int16_t rtoi(const Real *src);
int16_t rtoif(const Real *src);
double rtof(const Real *src);
void ftor(Real *dest, double value);

/* ユーティリティ関数 */
inline static int is_zero(const Real *r) {
    return (r->man & REAL_MAN_MSB) == 0;
}

inline static void set_zero(Real *r) {
    r->exp = 0;
    r->sign = 0;
    r->man = 0;
}

inline static void set_infinity(Real *r) {
    r->exp = REAL_MAX_EXP;
    r->sign = 0;
    r->man = 0xFFFF;
}
