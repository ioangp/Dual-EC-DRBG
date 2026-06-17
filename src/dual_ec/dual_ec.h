#ifndef DUAL_EC_H
#define DUAL_EC_H

#ifdef __INTELLISENSE__
    #define _BitInt(x) int
#endif

#define BYTES_TO_BITINT(bits, arr, len, out) \
    do {                                     \
        out = 0;                             \
        for (size_t i = 0; i < (len); ++i) { \
            out <<= 8;                       \
            out |= (arr)[i];                 \
        }                                    \
    } while (0)

typedef signed _BitInt(256) i256;
typedef signed _BitInt(512) i512;
typedef unsigned _BitInt(256) u256;
typedef unsigned _BitInt(512) u512;

u256 u256_from_hex(const char *s);
void print_hex_u256(u256 x);

typedef struct Curve {
    u256 p; /* Field prime */
    u256 a; /* a coefficient */
    u256 b; /* b coefficient */
    u256 n; /* Curve order */
} Curve;

typedef struct Point {
    u256 x;
    u256 y;
    int identity; /* 1 if this point is O, anything else otherwise*/
} Point;

Curve Curve_new(int a, const char *b, const char *p, const char *n);

u256 mod_mul(u256 a, u256 b, u256 p);
u256 mod_add(u256 a, u256 b, u256 p);
u256 mod_sub(u256 a, u256 b, u256 p);
u256 mod_pow(u256 a, u256 e, u256 p);
u256 mod_inv(u256 a, u256 p);

Point Point_add(Curve *curve, Point a, Point b);
Point Point_double(Curve *curve, Point point);
Point Point_multiply(Curve *curve, Point point, u256 n);
int   Point_on_curve(Curve *curve, Point point);
void  Point_print(Point p);

typedef struct state {
    Curve *curve;
    Point P;
    Point Q;
    u256 s;
} DualECDRBG;

typedef enum DECDRBG_RESULT {
    SUCCESS,
    ERROR,
} DECDRBG_RESULT;

DualECDRBG     DualECDRBG_new(Curve *curve, Point P, Point Q, u256 seed);
DECDRBG_RESULT DualECDRBG_next(DualECDRBG *state, u256 *out_bits);

#endif