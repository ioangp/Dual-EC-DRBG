#include <stdio.h>
#include <limits.h>
#include <stdio.h>
#include "dual_ec/dual_ec.h"

static int rand_num(DualECDRBG* s);

int main (int argc, char *argv[]) {
    Curve c = Curve_new(
        -3,
        "5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b",
        "ffffffff00000001000000000000000000000000ffffffffffffffffffffffff",
        "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551"
    );
    Point P = {
        .x = u256_from_hex("6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296"),
        .y = u256_from_hex("4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5")
    };
    /*
    Point Q = {
        .x = u256_from_hex("c97445f45cdef9f0d3e05e1e585fc297235b82b5be8ff3efca67c59852018192"),
        .y = u256_from_hex("b28ef557ba31dfcbdd21ac46e2a91e3c304f44cb87058ada2cb815151e610046")
        };
    */
   
    /* Hmm, I could use the above Q from NIST, but my mate Evil John gave me this super cool alternative Q. */
    Point Q = {
        .x = u256_from_hex("b2c2c64e5e6025c537015eaf036ca13291c5c558e58637b22909f1abaa1b21f2"),
        .y = u256_from_hex("c56e031551ccd3a90467d975a5acc699054d0aafdb17cc2469cdcb72acaf0ed8")
    };

    DualECDRBG rng = DualECDRBG_new(&c, P, Q, u256_from_hex("9876bdfb765aa49f32343f88523877dc24d129cc2717714fc050cfd252863057"));
    DualECDRBG *state = &rng;
    
    printf("\rPress `enter` to generate the next random number, `q + enter` to quit.\n");
    for (;;)
    {
        if (getchar() == 'q')
            break;

        u256 out;
        DualECDRBG_next(state, &out);
        print_hex_u256(out); printf(" (%03d)", (int)(out & 0xFF));
    }
}

static int rand_num(DualECDRBG *s) {
    u256 output;
    int result = DualECDRBG_next(s, &output);

    return (int)(output & 0xFF);
}