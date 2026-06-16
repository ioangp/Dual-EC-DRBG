# Dual-EC-DRBG
Demonstration of the vulnerability in the Dual_EC_DRBG pseudorandom number generator that allows the internal state to be reconstructed, given you know the relationship `e` between points `P` and `Q` such that `P = eQ`. A simplified version of the algorithm is implemented fixed at a 240 bit output and only using the curve P-256.

[NIST algorithm specification](https://csrc.nist.gov/pubs/sp/800/90/a/final)

## Run
Compile with Makefile (on Windows). Run `rng.exe` and get some random numbers, then run `exploit.exe` and give it two values. You will then get a prediction for the next number.