default: debug
all: build/debug/main build/release/main
debug: build/debug/main
release: build/release/main
run: build/release/main
	./build/release/main

run-debug: build/debug/main
	./build/debug/main

build/debug/main: build/debug/libmpc.a build/debug/libhashmap.a src/main/c/main.c src/main/c/builtin.c src/main/c/printing.c src/main/c/reading.c src/main/c/structures.c src/main/headers/builtin.h src/main/headers/printing.h src/main/headers/reading.h src/main/headers/structures.h
	mkdir -p build/debug
	gcc -std=c99 -Wall -g -Og src/main/c/*.c build/debug/libmpc.a build/debug/libhashmap.a -lreadline -lm -o build/debug/main

build/release/main: build/release/libmpc.a build/release/libhashmap.a src/main/c/main.c src/main/c/builtin.c src/main/c/printing.c src/main/c/reading.c src/main/c/structures.c src/main/headers/builtin.h src/main/headers/printing.h src/main/headers/reading.h src/main/headers/structures.h
	mkdir -p build/release
	gcc -std=c99 -Wall -O3 src/main/c/*.c build/release/libmpc.a build/release/libhashmap.a -lreadline -lm -o build/release/main

build/debug/libmpc.a: src/mpc/c/mpc.c src/mpc/headers/mpc.h
	mkdir -p build/debug/mpc
	gcc -c -std=c99 -Wall -g -Og src/mpc/c/mpc.c -o build/debug/mpc/mpc.o
	ar -cvq build/debug/libmpc.a build/debug/mpc/*.o

build/release/libmpc.a: src/mpc/c/mpc.c src/mpc/headers/mpc.h
	mkdir -p build/release/mpc
	gcc -c -std=c99 -Wall -O3 src/mpc/c/mpc.c -o build/release/mpc/mpc.o
	ar -cvq build/release/libmpc.a build/release/mpc/*.o

build/debug/libhashmap.a: src/hashmap/c/bstrlib.c src/hashmap/c/darray.c src/hashmap/c/hashmap.c src/hashmap/headers/bstrlib.h src/hashmap/headers/darray.h src/hashmap/headers/dbg.h src/hashmap/headers/hashmap.h
	mkdir -p build/debug/hashmap
	cd build/debug/hashmap/ ; \
	gcc -c -std=c99 -Wall -g -Og ../../../src/hashmap/c/*.c; \
	ar -cvq ../libhashmap.a *.o; \
	cd ../../../ ;

build/release/libhashmap.a: src/hashmap/c/bstrlib.c src/hashmap/c/darray.c src/hashmap/c/hashmap.c src/hashmap/headers/bstrlib.h src/hashmap/headers/darray.h src/hashmap/headers/dbg.h src/hashmap/headers/hashmap.h
	mkdir -p build/release/hashmap
	cd build/release/hashmap/ ; \
	gcc -c -std=c99 -Wall -O3 ../../../src/hashmap/c/*.c; \
	ar -cvq ../libhashmap.a *.o; \
	cd ../../../ ;
	