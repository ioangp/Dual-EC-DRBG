SRC=$(wildcard src/dual_ec/*.c)

all: rng exploit

remake: clean all
.NOTPARALLEL: remake

clean:
	del rng.exe exploit.exe

rng: src/rng.c $(SRC)
	gcc -std=c23 src/rng.c $(SRC) -o rng

exploit: src/exploit.c $(SRC)
	gcc -std=c23 -fopenmp src/exploit.c $(SRC) -o exploit

debug: src/exploit.c $(SRC)
	gcc -std=c23 -g -O0 -fopenmp src/exploit.c $(SRC) -o exploit