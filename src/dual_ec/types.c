#include <stdio.h>
#include "dual_ec.h"

u256 u256_from_hex(const char *s) {
    u256 x = 0;

    /* optional 0x prefix */
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
    }

    while (*s) {
        char c = *s++;
        unsigned digit;

        if (c >= '0' && c <= '9')
            digit = c - '0';
        else if (c >= 'a' && c <= 'f')
            digit = 10 + (c - 'a');
        else if (c >= 'A' && c <= 'F')
            digit = 10 + (c - 'A');
        else
            break; /* invalid character */

        x = (x << 4) | digit;
    }

    return x;
}

void print_hex_u256(u256 x) {
    for (int i = 252; i >= 0; i -= 4) {
        unsigned digit = (x >> i) & 0xF;
        putchar("0123456789abcdef"[digit]);
    }
}