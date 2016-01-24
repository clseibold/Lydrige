default: build/main
all: clean main dev
all-dev: clean dev run-dev
all-main: clean main run
main: build/main
dev: build/dev

build/main: src/main/c/main.c src/main/c/builtin.c src/main/c/hashtable.c src/main/c/printing.c src/main/c/reading.c src/main/c/structures.c build/libmpc.a
	mkdir -p build
	gcc -std=c99 -Wall src/main/c/*.c build/libmpc.a -lreadline -lm -o build/main

build/dev: src/dev/c/main.c build/libmpc.a
	mkdir -p build
	gcc -std=c99 -Wall src/dev/c/*.c build/libmpc.a -lreadline -lm -o build/dev

build/libmpc.a: src/mpc/c/mpc.c src/mpc/headers/mpc.h
	mkdir -p build/mpc
	gcc -c -std=c99 -Wall src/mpc/c/mpc.c -lm -o build/mpc/mpc.o
	ar -cvq build/libmpc.a build/mpc/*.o
	cp copy/*.* build/

run: build/main
	./build/main

run-dev: build/dev
	./build/dev

clean:
	rm -rf build
