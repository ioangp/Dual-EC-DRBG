#include "dual_ec.h"
#include <assert.h>
#include <string.h>

DualECDRBG
DualECDRBG_new(Curve *curve, Point P, Point Q, u256 seed)
{
    assert(Point_on_curve(curve, P));
    assert(Point_on_curve(curve, Q));
    assert(seed != 0);

    DualECDRBG state = {
        .curve = curve,
        .P = P,
        .Q = Q,
        .s = seed,
    };

    return state;
}

DECDRBG_RESULT
DualECDRBG_next(DualECDRBG *state, u256 *out_bits)
{
    assert(state->s != 0);
    Point sP;
    u256 r;

    sP = Point_multiply(state->curve, state->P, state->s);

    state->s = sP.x;

    r = Point_multiply(state->curve, state->Q, state->s).x;
    *out_bits = r & u256_from_hex("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

    return SUCCESS;
}