/*
 * REALA.C - 1985年版レイトレーシング用浮動小数点演算ライブラリ
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
#include <math.h>

#include "trace.h"

/* 乗算実装 */
void fmlt(Real *dest, const Real *src) {
    if (is_zero(dest) || is_zero(src)) {
        set_zero(dest);
        return;
    }
    
    /* 符号計算 */
    dest->sign ^= src->sign;
    
    /* 指数計算（オーバーフローチェック付き） */
    int exp_result = (int)dest->exp + (int)src->exp;
    if (exp_result > REAL_MAX_EXP) {
        set_infinity(dest);
        return;
    }
    if (exp_result < -128) {
        set_zero(dest);
        return;
    }
    
    /* 対数テーブルを使用した仮数部計算 */
    /* 左1ビットシフトでMSBを捨て、下位15bitをインデックスに */
    uint16_t dest_idx = dest->man & 0x7FFF;
    uint16_t src_idx = src->man & 0x7FFF;
    
    if (dest_idx < sizeof(logtbl)/sizeof(logtbl[0]) && 
        src_idx < sizeof(logtbl)/sizeof(logtbl[0])) {
        
        uint32_t log_sum = logtbl[dest_idx] + logtbl[src_idx];
	log_sum++; // 四捨五入
        
        if (log_sum >= 0x10000) {
            exp_result++;
	    log_sum -= 0x10000;
            if (exp_result > REAL_MAX_EXP) {
                set_infinity(dest);
                return;
            }
        }
	
	log_sum >>= 1;      /* 2で割ってインデックスに */
	if (log_sum < sizeof(exptbl)/sizeof(exptbl[0])) {
	    dest->man = exptbl[log_sum];
	} else {
	    set_infinity(dest);
	    return;
	}
    }
    
    dest->exp = (uint8_t)exp_result;
}

/* 除算実装 */
void fdiv(Real *dest, const Real *src) {
    if (is_zero(src)) {
        fprintf(stderr, "Division by ZERO !!\n"); fflush(stderr);
        set_infinity(dest);
        return;
    }
    
    if (is_zero(dest)) {
        return;
    }
    
    /* 符号計算 */
    dest->sign ^= src->sign;
    
    /* 指数計算 */
    int exp_result = (int)dest->exp - (int)src->exp;
    
    /* 対数テーブルを使用した仮数部計算 */
    uint16_t dest_idx = dest->man & 0x7FFF;
    uint16_t src_idx = src->man & 0x7FFF;
    
    if (dest_idx < sizeof(logtbl)/sizeof(logtbl[0]) && 
        src_idx < sizeof(logtbl)/sizeof(logtbl[0])) {
        
        int32_t log_diff = (int32_t)logtbl[dest_idx] - (int32_t)logtbl[src_idx];
	log_diff++; // 四捨五入
        
        if (log_diff < 0) {
	    log_diff += 0x10000;
            exp_result--;
            if (exp_result < -128) {
                set_zero(dest);
                return;
            }
        }
        
	log_diff >>= 1;
	if ((uint32_t)log_diff < sizeof(exptbl)/sizeof(exptbl[0])) {
	    dest->man = exptbl[log_diff];
	}
    }
    
    if (exp_result > REAL_MAX_EXP) {
        set_infinity(dest);
        return;
    }
    if (exp_result < -128) {
        set_zero(dest);
        return;
    }
    
    dest->exp = (uint8_t)exp_result;
}

/* 加算実装 */
void fadd(Real *dest, const Real *src) {
    if (is_zero(src)) return;
    if (is_zero(dest)) {
        *dest = *src;
        return;
    }
    
    Real larger, smaller;
    int shift;
    
    /* 指数の大きい方を larger に */
    if (dest->exp >= src->exp) {
        larger = *dest;
        smaller = *src;
        shift = dest->exp - src->exp;
    } else {
        larger = *src;
        smaller = *dest;
        shift = src->exp - dest->exp;
    }
    
    /* シフト量が16以上なら smaller は無視 */
    if (shift >= 16) {
        *dest = larger;
        return;
    }
    
    uint16_t larger_man = larger.man;
    uint16_t smaller_man = smaller.man >> shift;
    int16_t exp_val = larger.exp;
    
    /* 符号が同じなら加算、異なるなら減算 */
    if (larger.sign == smaller.sign) {
        uint32_t result = (uint32_t)larger_man + smaller_man;
        if (result >= 0x10000) {
            /* オーバーフロー：右シフトして指数を増やす */
            result >>= 1;
            exp_val++;
            if (exp_val > REAL_MAX_EXP) {
                set_infinity(dest);
                return;
            }
        }
        dest->man = (uint16_t)result;
        dest->exp = exp_val;
        dest->sign = larger.sign;
    } else {
        int32_t result = (int32_t)larger_man - smaller_man;
        if (result == 0) {
            set_zero(dest);
            return;
        }
        int sign = larger.sign;
        if (result < 0) {
            sign ^= 1;
            result = -result;
        }
        /* 正規化：最上位ビットが1になるまで左シフト */
        while (!(result & 0x8000)) {
            result <<= 1;
            exp_val--;
            if (exp_val < -128) {
                set_zero(dest);
                return;
            }
        }
        dest->man = (uint16_t)result;
        dest->exp = exp_val;
        dest->sign = sign;
    }
}

/* 減算実装 */
void fsub(Real *dest, const Real *src) {
    Real temp = *src;
    temp.sign ^= 1;  /* 符号反転 */
    fadd(dest, &temp);
}

/* 平方根実装 */
void fsqrt(Real *dest) {
    if (is_zero(dest)) return;
    
    uint16_t man_idx = dest->man & 0x7FFF;
    if (man_idx >= sizeof(logtbl)/sizeof(logtbl[0])) return;
    
    /* log(sqrt(x)) = log(x)/2 */
    uint32_t log_val = logtbl[man_idx];
    int exp_val = (int)dest->exp;
    
    /* 指数部を含めて2で割る == 右シフト */
    if (exp_val & 1) {
        log_val += 0x10000;  /* 奇数指数の補正 */
    }
    exp_val >>= 1;  /* 指数を半分に */
    log_val >>= 1;  /* 対数値を半分に */
    
    log_val++; // 四捨五入
    if (log_val == 0) {
        exp_val++;
        if (exp_val > REAL_MAX_EXP) {
            set_infinity(dest);
            return;
        }
    }
    
    log_val >>= 1;
    if (log_val < sizeof(exptbl)/sizeof(exptbl[0])) {
	dest->man = exptbl[log_val];
    }
    
    dest->exp = (uint8_t)exp_val;
    dest->sign = 0;  /* 平方根は常に正 */
}

/* 二乗実装 */
void fsqar(Real *dest) {
    if (is_zero(dest)) return;
    
    uint16_t man_idx = dest->man & 0x7FFF;
    if (man_idx >= sizeof(logtbl)/sizeof(logtbl[0])) return;
    
    /* log(x^2) = 2*log(x) */
    uint32_t log_val = logtbl[man_idx] << 1;
    int exp_val = ((int)dest->exp) << 1;

    if (log_val >= 0x10000) {
	exp_val++;
	log_val -= 0x10000;
    }
    if (exp_val > REAL_MAX_EXP) {
        set_infinity(dest);
        return;
    }
    if (exp_val < -128) {
        set_zero(dest);
        return;
    }
    
    log_val >>= 1;
    if (log_val < sizeof(exptbl)/sizeof(exptbl[0])) {
        dest->man = exptbl[log_val];
    }
    
    dest->exp = (uint8_t)exp_val;
    dest->sign = 0;  /* 二乗は常に正 */
}

/* スケール（2の累乗倍）実装 */
void fscale(Real *dest, int8_t scale) {
    if (is_zero(dest)) return;
    
    int new_exp = (int)dest->exp + scale;
    if (new_exp > REAL_MAX_EXP) {
        set_infinity(dest);
    } else if (new_exp < REAL_MIN_EXP) {
        set_zero(dest);
    } else {
        dest->exp = (int8_t)new_exp;
    }
}

/* 逆数実装 */
void finv(Real *dest) {
    if (is_zero(dest)) {
        fprintf(stderr, "Division by ZERO !!\n");
        set_infinity(dest);
        return;
    }
    
    int16_t exp_val = -dest->exp;
    if (exp_val > REAL_MAX_EXP) {
        set_infinity(dest);
        return;
    }
    
    uint16_t man_idx = dest->man & 0x7FFF;
    if (man_idx < sizeof(logtbl)/sizeof(logtbl[0])) {
        int32_t log_val = -(int32_t)logtbl[man_idx];
        log_val++; // 四捨五入
        
        if (log_val < 0) {
	    log_val += 0x10000;
	    exp_val--;
        }

	if (exp_val < -128) {
	    set_zero(dest);
	    return;
	}

	log_val >>= 1;
	if ((uint32_t)log_val < sizeof(exptbl)/sizeof(exptbl[0])) {
	    dest->man = exptbl[log_val];
	}
	dest->exp = exp_val;
    }
}

/* 比較実装 */
int fcomp(const Real *a, const Real *b) {
    if (is_zero(a) && is_zero(b)) return 0;
    if (is_zero(a)) return b->sign ? 1 : -1;
    if (is_zero(b)) return a->sign ? -1 : 1;
    
    if (a->sign != b->sign) {
        return a->sign ? -1 : 1;
    }
    
    int result;
    if (a->exp != b->exp) {
        result = (a->exp > b->exp) ? 1 : -1;
    } else if (a->man != b->man) {
        result = (a->man > b->man) ? 1 : -1;
    } else {
        return 0;
    }
    
    return a->sign ? -result : result;
}

/* 整数→実数変換 */
void itor(Real *dest, int16_t value) {
    if (value == 0) {
        set_zero(dest);
        return;
    }
    
    if (value < 0) {
        dest->sign = 1;
        value = -value;
    } else {
        dest->sign = 0;
    }
    
    /* 正規化：最上位ビットを探す */
    uint16_t exp = 15;
    while (!(value & 0x8000)) {
        value <<= 1;
        exp--;
    }
    
    dest->man = (uint16_t)value;
    dest->exp = exp;
}

/* 実数→整数変換 */
int16_t rtoi(const Real *src) {
    if (is_zero(src)) return 0;
    
    uint16_t value = src->man;
    int shift = (int)src->exp - 15;
    
    if (shift == 0) {
        value &= 0x7FFF;
    } else if (shift > 0) {
        value <<= shift;
    } else {
        value >>= (-shift);
        value &= 0x7FFF;
    }
    
    return src->sign ? -(int16_t)value : (int16_t)value;
}

/* 実数→整数変換（端数処理付き） */
// floor (-2.5 -> -3)
int16_t rtoif(const Real *src) {
    if (is_zero(src)) return 0;
    
    uint16_t value = src->man;
    uint16_t remainder = 0;
    int shift = (int)src->exp - 15;
    
    if (shift >= 0) {
        return rtoi(src);  /* シフトが正の場合は通常変換 */
    }
    
    /* 右シフトでの端数処理 */
    shift = -shift;
    for (int i = 0; i < shift; i++) {
        remainder = (value & 1) ? 1 : remainder;
        value >>= 1;
    }
    
    value &= 0x7FFF;
    int16_t result = src->sign ? -(int16_t)value - remainder : (int16_t)value;
    
    return result;
}

/* 対数実装 */
void flog(Real *dest) {
    /* ゼロまたはMSBが0の場合 */
    if (is_zero(dest)) {
        /* FLOGMI: log(0) = -∞ */
        dest->man = 0xFFFF;
        dest->exp = 0x7F;
        dest->sign = 1;
        return;
    }
    
    /* 負数の場合 */
    if (dest->sign) {
        /* FLOG0: log(負数) = 0 */
        set_zero(dest);
        return;
    }
    
    /* 対数テーブルから仮数部の対数を取得 */
    uint16_t man_idx = dest->man & 0x7FFF;
    if (man_idx >= sizeof(logtbl)/sizeof(logtbl[0])) {
        set_zero(dest);
        return;
    }
    
    uint8_t sign;
    uint16_t exp;
    uint16_t log_man = logtbl[man_idx];
    
    /*
     * 指数が負の場合の処理
     * logの絶対値をexpとmanにセットし、signを1にする
     */
    if (dest->exp >= 0) {
	exp = dest->exp;
	sign = 0;
    } else {
	exp = -dest->exp;
	sign = 1;
	if (log_man != 0) {
	    log_man = 0x10000 - log_man;
	}
    }

    /* expとlog_manをつなげて24ビットレジスタを作成 */
    uint32_t reg = (exp << 16) + log_man;
    
    /* ゼロチェック */
    if (reg == 0) {
        set_zero(dest);
        return;
    }
    
    /* 正規化：MSBが1になるまで左シフト */
    uint8_t al = 7;  /* 指数部の初期値 */
    while (al > 0 && !(reg & 0x800000)) {
	reg <<= 1;
        al--;
    }
    
    /* 結果を格納 */
    dest->man = reg >> 8;
    dest->exp = al;
    dest->sign = sign;
}

/* 指数実装 */
void fexp(Real *dest) {
    /* ゼロの場合 */
    if (is_zero(dest)) {
        /* FEXPR1: exp(0) = 1.0 */
        dest->man = REAL_MAN_MSB;
        dest->exp = 0;
        dest->sign = 0;
        return;
    }
    
    int16_t exp = (int8_t)dest->exp;
    
    /* 指数が大きすぎる場合 */
    if (exp >= 7) {
        if (dest->sign) {
            /* FEXPI負数: exp(-大きい値) = 0 */
            set_zero(dest);
        } else {
            /* FEXPIP: exp(大きい値) = ∞ */
            dest->man = 0xFFFF;
            dest->exp = REAL_MAX_EXP;
            dest->sign = 0;
        }
        return;
    }
    
    /* 指数が小さすぎる場合 */
    if (exp <= -16) {
        /* FEXPR1: exp(-小さい値) = 1.0 */
        dest->man = REAL_MAN_MSB;
        dest->exp = 0;
        dest->sign = 0;
        return;
    }
    
    /* 32bitレジスタのシフト準備 */
    uint32_t reg = dest->man << 16;
    
    /* シフト回数計算 */
    int8_t shift_count = 15 - exp;
    if (shift_count < 0) shift_count = 0;
    
    /* 右シフトでregの下16ビットに対数値の絶対値の小数以下、regの上16ビットに整数部 */
    reg >>= shift_count;
    int16_t exp_val = (int16_t)(reg >> 16);
    uint16_t frac_val = reg & 0xffff;
    
    /* 符号処理 */
    if (dest->sign) {
	exp_val = -exp_val;
        if (frac_val != 0) {
	    exp_val -= 1;
	    frac_val = 0x10000 - frac_val;
        }
	if (exp_val < -128) {
	    set_zero(dest);
	    return;
	}
    }
    
    /* 指数テーブル参照用インデックス準備 */
    frac_val >>= 1;
    dest->man = exptbl[frac_val & 0x7fff];
    dest->exp = exp_val;
    dest->sign = 0;
}

/* Realからdoubleへの変換 */
double rtof(const Real *src) {
    if (is_zero(src)) {
	return 0.0;
    }
    double r = (double)src->man / 32768.0;
    r *= exp2((double)src->exp);
    if (src->sign) {
	r = -r;
    }
    return r;
}

void ftor(Real *dest, const double value) {
    if (value == 0.0) {
	set_zero(dest);
	return;
    }
    int exp;
    double m = frexp(fabs(value), &exp);
    int sign = value > 0 ? 0 : 1;
    if (exp < -127) {
	// too small
	set_zero(dest);
	return;
    } else if (exp > 128) {
	// too big
	set_infinity(dest);
	return;
    }
    dest->exp = exp - 1;
    dest->sign = sign;
    dest->man = (uint16_t)floor(m * 65536.0);
}

void fprintreal(FILE *fp, const char *message, const real *v) {
  fprintf(fp, "%s: %.6lg\n", message, rtof(v));
}

void fprintvector(FILE *fp, const char *message, const vector *v) {
  double x, y, z;
  x = rtof(&v->x);
  y = rtof(&v->y);
  z = rtof(&v->z);
  fprintf(fp, "%s: [%.6lg, %.6lg, %.6lg]\n", message, x, y, z);
}

