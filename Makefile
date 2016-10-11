UNAME := $(shell uname -s)
cc := gcc

ifeq ($(UNAME), FreeBSD)
	cc = cc
endif
ifeq ($(UNAME), Linux)
	cc = gcc
endif
ifeq ($(UNAME), DARWIN)
	cc = clang
endif

default: all
all: build/debug/lydrige build/release/lydrige
debug: build/debug/lydrige
release: build/release/lydrige
run: build/release/lydrige
	./build/release/lydrige
run-debug: build/debug/lydrige
	./build/debug/lydrige

build/debug/lydrige: build/debug/libmpc.a build/debug/libhashmap.a build/debug/liblinenoise.a src/main/c/main.c src/main/c/builtin.c src/main/c/structure.c src/main/headers/builtin.h src/main/headers/structure.h
	mkdir -p build/debug
	$(cc) -std=c99 -Wall -g src/main/c/*.c build/debug/libmpc.a build/debug/libhashmap.a build/debug/liblinenoise.a -lm -o build/debug/lydrige

build/release/lydrige: build/release/libmpc.a build/release/libhashmap.a build/release/liblinenoise.a src/main/c/main.c src/main/c/builtin.c src/main/c/structure.c src/main/headers/builtin.h src/main/headers/structure.h
	mkdir -p build/release
	$(cc) -std=c99 -Wall -O3 src/main/c/*.c build/release/libmpc.a build/release/libhashmap.a build/release/liblinenoise.a -lm -o build/release/lydrige

build/debug/libmpc.a: src/mpc/c/mpc.c src/mpc/headers/mpc.h
	mkdir -p build/debug/mpc
	$(cc) -c -std=c99 -Wall -g src/mpc/c/mpc.c -o build/debug/mpc/mpc.o
	ar -cvq build/debug/libmpc.a build/debug/mpc/*.o

build/release/libmpc.a: src/mpc/c/mpc.c src/mpc/headers/mpc.h
	mkdir -p build/release/mpc
	$(cc) -c -std=c99 -Wall -O3 src/mpc/c/mpc.c -o build/release/mpc/mpc.o
	ar -cvq build/release/libmpc.a build/release/mpc/*.o

build/debug/libhashmap.a: src/hashmap/c/bstrlib.c src/hashmap/c/darray.c src/hashmap/c/hashmap.c src/hashmap/headers/bstrlib.h src/hashmap/headers/darray.h src/hashmap/headers/dbg.h src/hashmap/headers/hashmap.h
	mkdir -p build/debug/hashmap
	cd build/debug/hashmap/ ; \
	$(cc) -c -std=c99 -Wall -g ../../../src/hashmap/c/*.c; \
	ar -cvq ../libhashmap.a *.o; \
	cd ../../../ ;

build/release/libhashmap.a: src/hashmap/c/bstrlib.c src/hashmap/c/darray.c src/hashmap/c/hashmap.c src/hashmap/headers/bstrlib.h src/hashmap/headers/darray.h src/hashmap/headers/dbg.h src/hashmap/headers/hashmap.h
	mkdir -p build/release/hashmap
	cd build/release/hashmap/ ; \
	$(cc) -c -std=c99 -Wall -O3 ../../../src/hashmap/c/*.c; \
	ar -cvq ../libhashmap.a *.o; \
	cd ../../../ ;

build/debug/liblinenoise.a: src/linenoise/c/linenoise.c src/linenoise/headers/linenoise.h
	mkdir -p build/debug/linenoise
	$(cc) -c -std=c99 -Wall -g src/linenoise/c/linenoise.c -o build/debug/linenoise/linenoise.o
	ar -cvq build/debug/liblinenoise.a build/debug/linenoise/*.o

build/release/liblinenoise.a: src/linenoise/c/linenoise.c src/linenoise/headers/linenoise.h
	mkdir -p build/release/linenoise
	$(cc) -c -std=c99 -Wall -O3 src/linenoise/c/linenoise.c -o build/release/linenoise/linenoise.o
	ar -cvq build/release/liblinenoise.a build/release/linenoise/*.o

clean:
	$(RM) -r build/
