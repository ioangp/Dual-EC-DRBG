#include "dual_ec.h"
#include <stdio.h>

u256 mod_add(u256 a, u256 b, u256 p)
{
    a %= p;
    b %= p;
    u256 r = a + b;
    if (r >= p || r < a)
        r -= p;
    return r;
}

u256 mod_sub(u256 a, u256 b, u256 p)
{
    a %= p;
    b %= p;
    return (a >= b) ? (a - b) : (p - (b - a));
}

inline u256
mod_mul(u256 a, u256 b, u256 p)
{
    return (u256)((u512)a * b % p);
}

u256 mod_pow(u256 a, u256 e, u256 p)
{
    u256 r = 1 % p;
    a %= p;
    while (e) {
        if (e & 1)
            r = mod_mul(r, a, p);
        a = mod_mul(a, a, p);
        e >>= 1;
    }
    return r;
}

u256 mod_inv(u256 a, u256 p)
{
    /* valid only if p is prime and a != 0 mod p */
    return mod_pow(a, p - 2, p);
}

Curve Curve_new(int a, const char *b, const char *p, const char *n)
{
    u256 _p = u256_from_hex(p);
    u256 _a;
    if (a >= 0)
        _a = (u256)a;
    else
        _a = mod_sub(0, (u256)(a * -1), _p);

    Curve c = {
        .a = _a,
        .b = u256_from_hex(b),
        .p = _p,
        .n = u256_from_hex(n)
    };

    return c;
}

Point Point_add(Curve *curve, Point a, Point b)
{
    if (a.identity)
        return b;
    if (b.identity)
        return a;

    if (a.x == b.x) {
        if (a.y == b.y)
            return Point_double(curve, a); // P + P = 2P
        else {
            Point inf = { .identity = 1 };
            return inf; // P + (-P) = O
        }
    }

    u256 p = curve->p;

    /* s = (y2 - y1).(x2 - x1)^-1 mod p */
    u256 s = mod_mul(
        mod_sub(b.y, a.y, p),
        mod_inv(mod_sub(b.x, a.x, p), p),
        p);

    /* x3 = s^2 - x2 - x1 mod p */
    u256 ss = mod_mul(s, s, p);
    u256 x = mod_sub(mod_sub(ss, a.x, p), b.x, p);

    /* y3 =  s.(x2 - x3) - y2 mod p */
    u256 y = mod_sub(mod_mul(s, mod_sub(b.x, x, p), p), b.y, p);

    Point point = { .x = x, .y = y, .identity = 0 };
    return point;
}

Point Point_double(Curve *curve, Point point)
{
    if (point.identity)
        return point;

    u256 p = curve->p;

    /* s = (3x1^2 + a).(2y1)^-1 mod p */
    u256 s = mod_mul(
        mod_add(mod_mul(3, mod_mul(point.x, point.x, p), p), curve->a, p),
        mod_inv(mod_mul(2, point.y, p), p),
        p);

    /* x2 = s^2 - 2x1 mod p */
    u256 x = mod_sub(mod_pow(s, 2, p), mod_mul(2, point.x, p), p);

    /* y2 = s.(x1 - x2) - y1 mod p */
    u256 y = mod_sub(
        mod_mul(s, mod_sub(point.x, x, p), p),
        point.y,
        p);

    Point pnt = { .x = x, .y = y, .identity = 0 };
    return pnt;
}

Point Point_multiply(Curve *curve, Point point, u256 n)
{
    Point P = { .identity = 1 };
    Point G = point;

    while (n != 0) {
        if (n & (u256)1) {
            P = Point_add(curve, P, G);
        }
        G = Point_double(curve, G);
        n >>= 1;
    }

    return P;
}

#define Point_multiply Point_multiply_windowed;

Point Point_multiply_windowed(Curve *curve, Point P, u256 n)
{
    Point inf = { .identity = 1 };

    if (n == 0 || P.identity)
        return inf;

    /* precomp[i] = i * P for i in [0, 15] */
    Point precomp[16];
    precomp[0] = inf;
    precomp[1] = P;
    for (int i = 2; i < 16; i++) {
        precomp[i] = Point_add(curve, precomp[i - 1], P);
    }

    Point R = inf;
    int started = 0;

    /* 256 bits / 4 bits per window = 64 nibbles */
    for (int nib = 63; nib >= 0; nib--) {
        u256 chunk = (n >> (nib * 4)) & (u256)0xF;

        if (!started) {
            if (chunk != 0) {
                R = precomp[(int)chunk];
                started = 1;
            }
            continue;
        }

        for (int i = 0; i < 4; i++) {
            R = Point_double(curve, R);
        }

        if (chunk != 0) {
            R = Point_add(curve, R, precomp[(int)chunk]);
        }
    }

    return started ? R : inf;
}

int Point_on_curve(Curve *curve, Point point)
{
    /* rhs = x^3 + ax + b mod p */
    u256 x3 = mod_pow(point.x, 3, curve->p);
    u256 ax = mod_mul(curve->a, point.x, curve->p);
    u256 rhs = mod_add(mod_add(x3, ax, curve->p), curve->b, curve->p);

    /* lhs = (y^2) mod p */
    u256 lhs = mod_pow(point.y, 2, curve->p);

    return lhs == rhs;
}

void Point_print(Point p)
{
    printf("{\n\tx: ");
    print_hex_u256(p.x);
    printf("\n\ty: ");
    print_hex_u256(p.y);
    printf("\n\tidentity?: %d\n}\n", p.identity);
}