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

build/debug/lydrige: src/main.c src/builtin.c src/structure.c src/headers/builtin.h src/headers/structure.h src/libs/bstrlib.c src/darray.c src/hashmap.c src/libs/mpc.c src/structure.c src/libs/bstrlib.h src/headers/darray.h src/libs/dbg.h src/headers/hashmap.h src/headers/linenoise.h src/libs/mpc.h src/unix/linenoise.c
	mkdir -p build/debug
	$(cc) -std=c99 -Wall -g src/*.c src/unix/*.c src/libs/*.c -Isrc/libs/ -Isrc/headers/ -lm -o build/debug/lydrige

build/release/lydrige: src/main.c src/builtin.c src/structure.c src/headers/builtin.h src/headers/structure.h src/libs/bstrlib.c src/darray.c src/hashmap.c src/libs/mpc.c src/structure.c src/libs/bstrlib.h src/headers/darray.h src/libs/dbg.h src/headers/hashmap.h src/headers/linenoise.h src/libs/mpc.h src/unix/linenoise.c
	mkdir -p build/release
	$(cc) -std=c99 -Wall -O3 src/*.c src/unix/*.c src/libs/*.c -Isrc/libs/ -Isrc/headers/ -lm -o build/release/lydrige

clean:
	$(RM) -r build/
